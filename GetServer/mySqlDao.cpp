#include "mySqlDao.h"
#include "configMgr.h"
#include "Logging.h"
mySqlDao::mySqlDao()
{
	// 创建dao 时 必须要与数据库建立连接
	auto& config_ = configMgr::inst();
	const auto& host = config_["MYSQL"]["Host"];
	const auto& port = config_["MYSQL"]["Port"];
	const auto& pwd = config_["MYSQL"]["Password"];
	const auto& schema = config_["MYSQL"]["schema"];
	const auto& user = config_["MYSQL"]["User"];
	pool_.reset(new mySqlPool(5, host+':'+port, user, pwd, schema));
}
mySqlDao::~mySqlDao()
{
	pool_->close();
}
bool mySqlDao::CheckEmail(const std::string& username, const std::string& email)
{

	//先查询数据库
	/*
	
	select  emila   form userInfo  where  email=email, name='name';
	
	*/
	//先获得连接
	auto   con_= pool_->getConnection();
	try {
		if (con_!=nullptr)
		{

			//创建sql语句并绑定
			std::unique_ptr<sql::PreparedStatement> psmt(con_->sql_ptr_->prepareStatement("select email FROM users WHERE username=?"));
			psmt->setString(1, username);
			std::unique_ptr<sql::ResultSet> result(psmt->executeQuery());
			// 检查返回的结果
			while (result->next())
			{
				if (email != result->getString("email"))
				{
					pool_->returnConnection(std::move(con_));
					return false;

				}
				pool_->returnConnection(std::move(con_));
				return true;
			}
		}
		else
		{
			return false;
		}

	}
	catch (sql::SQLException &e)
	{
		pool_->returnConnection(std::move(con_));

		Logging::Instance().DEBUG("获取连接失败");
		return false;
	}
}
int  mySqlDao::RegUser(const std::string& username, const std::string& email, const std::string& password)
{
	auto con_ = pool_->getConnection();
	try
	{
		if (con_ != nullptr)
		{
			std::unique_ptr < sql::PreparedStatement>pstm(con_->sql_ptr_->prepareStatement("CALL reg_user(?,?,?,@result)"));
			pstm->setString(1, username);
			pstm->setString(2, email);
			pstm->setString(3, password);
			//执行sql语句
			pstm->execute();
			std::unique_ptr<sql::Statement>pstmResult(con_->sql_ptr_->createStatement());
			std::unique_ptr<sql::ResultSet> res(pstmResult->executeQuery("select @result as result"));
			if (res->next())
			{	
				int result = res->getInt("result");

				Logging::Instance().INFO("数据库返回注册的结果");
				pool_->returnConnection( std::move(con_));
				return result;
			}
			pool_->returnConnection(std::move(con_));
			//执行错误
			return -1;
			
		}

	}
	catch (sql::SQLException e)

	{	
		//就算创建连接失败也要返回失败的连接
		pool_->returnConnection(std::move(con_));
		//连接是空的
		Logging::Instance().LOG_ERROR("获取sql连接失败");
		return -1;
	}
	//注册用户应该是add 添加
	
}
bool mySqlDao::UpdatePwd(const std::string& username, const std::string& newpasswrod)
{
	try {
		auto con_ = pool_->getConnection();
		if (con_ != nullptr)
		{
			std::unique_ptr<sql::PreparedStatement>pstm(con_->sql_ptr_->prepareStatement("update users set password=? from  where username=?"));

			pstm->setString(1, newpasswrod);
			pstm->setString(2, username);

			int updataCount = pstm->execute();

			Logging::Instance().INFO("更新密码的信息个数{updataCount} ");

			pool_->returnConnection(std::move(con_));
			return true;
		}
	}
	catch (sql::SQLException& e)
	{

		Logging::Instance().INFO("更新的对象失败");
		return false;

	}


}
bool mySqlDao::CheckPwd(const std::string& email, const std::string& password, userInfo& userInfo)
{

	//检查密码是否正确

	auto con_ = pool_->getConnection();

	if (con_ != nullptr)
	{
		//进行数据库的查询
		try
		{
			std::unique_ptr<sql::PreparedStatement>psmt(con_->sql_ptr_->prepareStatement("select * from user where email=?"));
			psmt->setString(1, email);
			std::unique_ptr<sql::ResultSet>res(psmt->executeQuery());
			std::string  dataBase_password = "";
			while (res->next())
			{
				dataBase_password = res->getString("password");
				break;

			}
			if (password != dataBase_password)
			{
				Logging::Instance().LOG_ERROR("输出的密码是错误的");
				pool_->returnConnection(std::move(con_));
				return false;
			}
			userInfo.name_ = res->getString("username");
			userInfo.email_ = res->getString("email");
			userInfo.u_id = res->getInt("uid");
			userInfo.password_ = dataBase_password;
			pool_->returnConnection(std::move(con_));
			return true;
		}
		catch (sql::SQLException& e)
		{
			Logging::Instance().DEBUG("发生了错误再检查密码的时候");
		}
	}

}