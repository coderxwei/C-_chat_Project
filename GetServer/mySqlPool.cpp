#include "mySqlPool.h"

#include "Logging.h"

mySqlPool::mySqlPool(size_t poolSize, const std::string url, const std::string  user, std::string  password, const std::string scheme) :poolSize_(poolSize),
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
void mySqlPool::checkConnection()
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
	long long timestamp = std::chrono::duration_cast<std::chrono::seconds>(now_Time).count();
	//当前的运行的数量 ，小于池子的大小
	while (processed < targetCount)
	{
		std::unique_ptr<sqlConnection> con_;
		{

			std::lock_guard<std::mutex> guard(mutex_);
			if (pool_.empty())
			{
				break;
			}
			//不是空 就从池中取出连接
			con_ = std::move(pool_.front());
			pool_.pop();
		}

		bool  hrealthy = true;
		if (timestamp - con_->last_oper_timer_ >= 5)
		{
			try
			{
				//sql::Statement :一个“可以在某个数据库连接上执行 SQL 的对象”
				std::unique_ptr<sql::Statement>stmt(con_->sql_ptr_->createStatement());
				//executeQuery ：表示执行查询语句
				// SELECT 1：表示“让数据库返回一个常量结果，用来确认数据库能正常执行查询”。
				stmt->executeQuery("SELECT 1");
				con_->last_oper_timer_ = timestamp;  //更新最后操作的时间
			}
			catch (const std::exception&)
			{
				//表示 执行查询有异常
				///TODO: 打印出具体的异常细节
				hrealthy = false;
				failer_count++;
			}
		}
		if (hrealthy)
		{
			std::lock_guard<std::mutex>guard(mutex_);
			//当前连接是正常的 
			pool_.push(std::move(con_));
			condition_.notify_all();

		}
		++processed;

	}

	while (failer_count > -0)
	{

		auto b_res = reConnection(timestamp);
		if (b_res)
		{

			failer_count--;
		}
		else
		{

			break;
		}

	}
}
// 这个是表示重新连接对吗？
bool mySqlPool::reConnection(long long timestamp)
{
	try
	{
		//获取 MySQL 驱动实例
		sql::mysql::MySQL_Driver* Diver_ = sql::mysql::get_mysql_driver_instance();
		// 建立连接
		auto con_ = Diver_->connect(url_, user_, passWord_);
		//③ 选择要操作的数据库（schema）
		con_->setSchema(scheam_);

		auto newContec_ = std::make_unique< sqlConnection>(con_, timestamp);

		{
			std::lock_guard<std::mutex>gurad(mutex_);
			// 需要一个指针对象
			pool_.push(std::move(newContec_));

		}
		//TPDO 连接成功
		return true;

	}
	catch (sql::SQLException& e)
	{
		

		Logging::Instance().LOG_ERROR("sql返回失败");
		return false;
	}
}


std::unique_ptr<sqlConnection> mySqlPool::getConnection()
{
	std::unique_ptr<sqlConnection>  newCon;
	//// 判断连接池是是否为空
	//if (!pool_.empty())
	//{
	//	//通过加锁保证共享区资源的安全。
	//	std::lock_guard<std::mutex> guard(mutex_);
	//	{
	//		
	//		
	//	}

	//}

	std::unique_lock<std::mutex> guard_(mutex_);
	{
		condition_.wait(guard_, [this]() {
			if (isStop_)
				return true;

			//连接池没有停止
			return !pool_.empty();

			});
		if (isStop_)
		{
			return nullptr;
		}

		newCon = std::move(pool_.front());
		pool_.pop();
		return newCon;
	}
}

void  mySqlPool::close()
{
	isStop_ = true;
	condition_.notify_all();

}
mySqlPool::~mySqlPool()
{
	std::lock_guard<std::mutex> guard(mutex_);
	{
		while (!pool_.empty())
		{
			pool_.pop();
		}
	}

}
