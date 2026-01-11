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
	int64_t last_oper_timer_;  // 记录最近操作的时间。
};

class mySqlPool
{
	mySqlPool(size_t poolSize, const std::string url, const std::string  user, std::string  password, const std::string scheme)
		
	{
		
	}
	// 检查连接池中的连接状态

	void   checkConnection();

	//返回新的连接
	bool  retConnection(long long conn);


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
	//记录发生错误的数量
	int  failer_count;
	//还需要一个信号进行通知
	std::condition_variable condition_;
};
