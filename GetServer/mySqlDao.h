#pragma once
#include <string>
#include <memory>
#include "mySqlPool.h"


// 用户信息结构提
struct userInfo
{	
	int u_id;
	std::string  name_;
	std::string  email_;
	std::string  password_;

};
class mySqlDao
{
public:
	mySqlDao();
	~mySqlDao();
	int RegUser(const std::string& name, const std::string& email, const std::string& pwd);
	//int RegUserTransaction(const std::string& name, const std::string& email, const std::string& pwd, const std::string& icon);
	bool CheckEmail(const std::string& name, const std::string& email);
	bool UpdatePwd(const std::string& name, const std::string& newpwd);
	bool CheckPwd(const std::string& email, const std::string& pwd, userInfo& userInfo);
	//bool TestProcedure(const std::string& email, int& uid, string& name);
private:
	std::unique_ptr<mySqlPool> pool_;
};