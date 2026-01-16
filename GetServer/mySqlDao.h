#pragma once
#include <string>
#include <memory>
#include "mySqlPool.h"
class mySqlDao
{
public:
	mySqlDao();
	~mySqlDao();
	int RegUser(const std::string& name, const std::string& email, const std::string& pwd);
	//int RegUserTransaction(const std::string& name, const std::string& email, const std::string& pwd, const std::string& icon);
	bool CheckEmail(const std::string& name, const std::string& email);
	bool UpdatePwd(const std::string& name, const std::string& newpwd);
	bool CheckPwd(const std::string& email, const std::string& pwd, UserInfo& userInfo);
	//bool TestProcedure(const std::string& email, int& uid, string& name);
private:
	std::unique_ptr<mySqlPool> pool_;
};