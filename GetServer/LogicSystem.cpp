#include "LogicSystem.h"
#include <iostream>
#include <json/json.h>
#include  "Global.h"
#include  "VerifyCodeGrpc.h"
// 构造函数
//直接构造函数中初始化接口
	/*
	 * 注册所有 HTTP 请求处理函数
	 * get 系列：处理 GET 请求
	 * post 系列：处理 POST 请求
	 */
LogicSystem::LogicSystem()
{
	regGet("get/verify", [](std::shared_ptr<HttpConnection> connection) {
		// 测试用 GET 接口，简单回一段文本
		//beast::ostream(conn->response_.body()) << "receive test request";
		beast::ostream(connection->request_.body()) << "receive test request";
		connection->response_.set(http::field::content_type, "text/plain");
		connection->sendResponse();
		});

	// 获取验证码的 POST 接口
	regPost("get/verifycode", [](std::shared_ptr<HttpConnection> connection)
		{
			// 读取请求体
			// 将 HTTP POST 请求中的 body 内容转换为字符串
			auto reqbody = boost::beast::buffers_to_string(connection->request_.body().data());

			// 设置响应的 Content-Type 为 JSON
			connection->response_.set(http::field::content_type, "application/json");

			// 解析 / 构造 JSON 数据

			Json::Value retbody;  // 返回给客户端的 JSON 数据
			Json::Reader reader;
			Json::Value srcbody;  // 从请求中解析出来的 JSON 数据
			Json::CharReaderBuilder builder;
			std::string err;
			std::istringstream srm(reqbody);
			// 将字符串形式的 JSON 解析为 Json::Value
			if
				(!Json::parseFromStream(builder, srm, &srcbody, &err))
			{
				// JSON 解析失败
				retbody["error"] = ErrorCodes::Error_Json;
				std::string  jsonstr = retbody.toStyledString();
				beast::ostream(connection->response_.body()) << jsonstr;
				connection->sendResponse();
				return;
			}
			// 解析成功 - 判断是否包含 email 字段
			if (!srcbody.isMember("email"))
			{
				// 缺少 email 字段，返回参数错误
				retbody["error"] = ErrorCodes::Error_Json;
				std::string  jsonstr = retbody.toStyledString();
				beast::ostream(connection->response_.body()) << jsonstr;
				connection->sendResponse();
				return;
			}
			// 获取邮箱地址
			auto email_ = srcbody["email"].asString();
			// TODO: 通过 Grpc 调用验证码服务，发送验证码到指定邮箱
			GetVarifyRsp response = VerifyCodeGrpc::getInstance()->GetVarifyCode(email_);
			std::cout << "email is" << email_ << std::endl;
			retbody["error"] = response.error();
			retbody["email"] = srcbody["email"];
			const  std::string jsonstr = retbody.toStyledString();	
			beast::ostream(connection->response_.body()) << jsonstr;
			return;



		});
}

// 析构函数
LogicSystem::~LogicSystem()
{
}

void LogicSystem::regGet(std::string url, Handler_ handler_)
{
	get_Handlers.insert(make_pair(url, handler_));
}
void LogicSystem::regPost(std::string url, Handler_ handler)
{

	post_handlers.insert(make_pair(url, handler));
}
void LogicSystem::initHandler_()
{

}
	