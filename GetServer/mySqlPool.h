#pragma once

#include "Global.h"
#include <queue>
class sqlConnection
{

public:

	sqlConnection(sql::Connection* con, int64_t  last_oper_timer) : sql_ptr_(con), last_oper_timer_(last_oper_timer)
	{

	}

	std::unique_ptr<sql::Connection>  sql_ptr_;

	/*
	 * 记录连接最后一次被使用的时间
	 * 用于管理连接，判断连接是否长时间未使用
	 * 连接超时后可以回收这些超时的空闲连接
	 * 通常表示最后一次操作的时间戳
	 */
	int64_t last_oper_timer_;
};

class mySqlPool
{
	mySqlPool(size_t poolSize, const std::string url, const std::string  user, std::string  password, const std::string scheme) :poolSize_(poolSize),
		url_(url),
		user_(user),
		passWord_(passWord_),
		scheam_(scheme)

	{
		try
		{
			// 首先创建连接池，创建一个连接池
			for (int i = 0; i < poolSize_; i++)
			{
				sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
				// 创建连接
				auto* connection_ = driver->connect(url_, user_, passWord_);
				// 设置连接到的数据库名称
				connection_->setSchema(scheam_);

				// 获得当前的时间

				auto currenTimer = std::chrono::system_clock::now().time_since_epoch();

				//（时间间隔）转换为以秒为单位 
				long long timestamp = std::chrono::duration_cast<std::chrono::seconds>(currenTimer).count();
				//新的连接加入连接池中
				pool_.push(std::make_unique<sqlConnection>(connection_, timestamp));

			}

			check_thread_ = std::thread([this]() {
				while (isStop_)
				{	
					// 每60 要检查连接池中的连接状态得得得
					checkConnection();
					std::this_thread::sleep_for(std::chrono::seconds(60));
					//添加新的内容
				}


				});


		}
		catch (sql::SQLException& epc)
		{

		}




	}
	// 检查连接池中的连接状态

	void  checkConnection()
	{
		size_t targetCount;
		{
			std::lock_guard<std::mutex> guard(mutex_);
			//获得当前连接数量的大小
			targetCount = pool_.size();
		}

		size_t processed = 0;
		//获取当前的时间
		auto now_Time = std::chrono::system_clock::now().time_since_epoch();
		//转化为秒
		long long tiemstamo = std::chrono::duration_cast<std::chrono::seconds>(now_Time).count();


	}



private:
	// 连接池的大小
	size_t		 poolSize_;
	std::string  url_;
	std::string  passWord_;
	std::string  user_;
	std::string  scheam_;
	// 数据库的连接池，管理池中的所有连接
	std::queue<std::unique_ptr<sqlConnection>>  pool_;
	std::thread check_thread_;
	std::atomic<bool>  isStop_;
	std::mutex mutex_;
};
