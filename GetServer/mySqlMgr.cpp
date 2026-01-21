#include "mySqlMgr.h"
//构造函数:
mySqlMgr::mySqlMgr()
{

}
bool mySqlMgr::CheckEmail(const std::string& username, const std::string& email)
{

	return Dao_.CheckEmail(username, email);

}
int mySqlMgr::RegUser(const std::string& username, const std::string& email, const std::string& pwd)
{
	return  Dao_.RegUser(username, email, pwd);
}
bool mySqlMgr::UpdatePwd(const std::string& username, const std::string& newpwd)
{
	return  Dao_.UpdatePwd(username, newpwd);
}
bool mySqlMgr::CheckPwd(const std::string& email, const std::string& pwd, userInfo& userInfo)
{
	return  Dao_.CheckPwd(email, pwd, userInfo);
}
//析构函数释放所有的资源
mySqlMgr::~mySqlMgr()
{
	//Dao_.~mySqlDao();

}