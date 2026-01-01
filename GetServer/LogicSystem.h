#pragma once
#include <functional>
#include <map>
#include <string>
#include <memory>
#include  "Global.h"
#include "HttpConnection.h"
// 主要是处理连接的逻辑

class HttpConnection;

using Handler_ = std::function<void(std::shared_ptr<HttpConnection> Conn)>;
class LogicSystem
{

	friend class HttpConnection;
public:
	// 构造函数
	LogicSystem();
	~LogicSystem();	

    	// 注册回调
	void regGet(std:: string url, Handler_ handler_);
	void regPost(std::string url, Handler_ handler_);
	
	// 要在构造函数中初始化注册的回调
private:
	void initHandler_();
	std::map<std::string, Handler_>	post_handlers;
	std::map<std::string, Handler_>  get_Handlers ;

};
