#pragma once
#include<string>
#include "mySqlPool.h"
#include "mySqlDao.h"
class mySqlMgr
{

public:
	mySqlMgr();
	~mySqlMgr();

	////int RegUser(const std::string& name, const std::string& email, const std::string& pwd);
	//////int RegUserTransaction(const std::string& name, const std::string& email, const std::string& pwd, const std::string& icon);
	////bool CheckEmail(const std::string& name, const std::string& email);
	////bool UpdatePwd(const std::string& name, const std::string& newpwd);
	////bool CheckPwd(const std::string& email, const std::string& pwd, userInfo& userInfo);
	int RegUser(const std::string& name, const std::string& email, const std::string& pwd);

	int RegUserTransaction(const std::string& username, const std::string& email, const std::string& pwd, const std::string& icon);
	bool CheckEmail(const std::string& username, const std::string& email);
	bool UpdatePwd(const std::string & username, const std::string & newpwd);
	bool CheckPwd(const std::string& email, const std::string& pwd, userInfo& userInfo);
private:


	mySqlDao Dao_;



};

