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
bool mySqlDao::CheckEmail(const std::string& name, const std::string& email)
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
			std::unique_ptr<sql::PreparedStatement> psmt(con_->sql_ptr_->prepareStatement("select email FROM user WHERE name=?"));
			psmt->setString(1, name);
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
bool mySqlDao::UpdatePwd(const std::string& name, const std::string& newpwd)
{

}
bool mySqlDao::CheckPwd(const std::string& email, const std::string& pwd, userInfo& userInfo)
{

}