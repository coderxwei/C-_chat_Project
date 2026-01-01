#pragma once
#include <queue>
#include <grpcpp/grpcpp.h>
#include "Message.grpc.pb.h"
#include  "Sington.h"
#include  "Global.h"
using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;
using  Message::GetVarifyreq;
using  Message::GetVarifyRsp;
using  Message::VarifyService;

// GRPC 连接池：维护一组到验证码服务的 Stub 连接
class RpcConnPool
{
public:
	// 构造函数
	/*
	 *  host: grpc 服务端地址（IP 或域名）
	 *  prot: grpc 服务端端口
	 */
	RpcConnPool(size_t poolSize, std::string host, std::string port) :poolSize_(poolSize),
		host_(host), prot_(port), b_stop_(false)
	{
		// 根据连接池大小提前创建若干个 Stub 连接
		for (int i = 0; i < poolSize_; i++)
		{
			// grpc::InsecureChannelCredentials: 表示使用非加密通道（明文传输）
			// 创建到指定地址的 Channel
			std::shared_ptr<Channel>  channel_ptr = grpc::CreateChannel(host_ + ":" + prot_, grpc::InsecureChannelCredentials());
			// 创建一个新的 Stub 并放入连接池队列中
			Conect_.push(VarifyService::NewStub(channel_ptr));
		}
	};
	// 析构函数：回收所有连接资源
	~RpcConnPool()
	{
		// 为了安全释放，先加锁再清空队列
		std::unique_lock<std::mutex> lock(mutex_);
		close();
		while (!Conect_.empty())
		{
			Conect_.pop();
		}
	}
	void close()
	{
		// 标记连接池已经停止，不再对外提供连接
		b_stop_ = true;
		// 唤醒所有等待中的线程，让它们及时退出
		cod_v_.notify_all();

	}
	// 获取一个 grpc 连接（Stub），可能会阻塞等待可用连接
	std::unique_ptr < VarifyService::Stub> grpcConnection()
	{
		std::unique_lock<std::mutex>lock(mutex_);
		cod_v_.wait(lock, [this]()
			{
				if (b_stop_)
					return true;
				return !Conect_.empty();
			});
		if (b_stop_)
		{
			return  nullptr;
		}
		// 从队列头部取出一个 Stub，使用移动语义转移所有权
		auto Connon_ = std::move(Conect_.front());
		Conect_.pop();
		return  Connon_;
	}
	// 使用完连接之后需要归还到连接池
	void  retConnection(std::unique_ptr<VarifyService::Stub> Conn_)
	{
		std::lock_guard<std::mutex>lock(mutex_);
		if (b_stop_)
		{
			return;
		}
		// 将唯一指针使用移动语义重新放回队列
		Conect_.push(std::move(Conn_));
		// 唤醒一个正在等待可用连接的线程
		cod_v_.notify_one();
	}

private:
	std::string host_;
	std::string prot_;
	size_t poolSize_;
	std::atomic<bool> b_stop_;
	std::queue<std::unique_ptr<VarifyService::Stub>> Conect_;
	std::mutex mutex_;
	std::condition_variable cod_v_;

};



class VerifyCodeGrpc :public Sington<VerifyCodeGrpc>
{

	friend  class Sington<VerifyCodeGrpc>;

public:
	// 识别验证码服务，通过邮箱获取验证码
	GetVarifyRsp GetVarifyCode(std::string email)
	{
		ClientContext context;
		GetVarifyRsp respone;
		GetVarifyreq request;
		auto stub = rpcPool_->grpcConnection();
		Status status = stub->GetVarifyCode(&context, request, &respone);
		//判断请求是否成功
		if (status.ok())
		{
			rpcPool_->retConnection(std::move(stub));
			return respone;
		}
		else
		{
			//rpc请求失败要设置错误码。
			rpcPool_->retConnection(std::move(stub));
			respone.set_error(ErrorCodes::RPCFAILED);
			return  respone;
		}


	}

protected:
	VerifyCodeGrpc();
	std::unique_ptr<RpcConnPool>  rpcPool_;








};

