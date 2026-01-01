#pragma once
#include "Global.h"
#include <map>
#include<unordered_map>
#include "LogicSystem.h"
class HttpConnection:std:: enable_shared_from_this<HttpConnection>
{
public:

    friend class LogicSystem;

        HttpConnection();
        ~HttpConnection();
        void sendResponse();
        // 超时检测
        void CheckDeadLine();
        // 处理请求函数
        void HandleReq();
        // 写响应函数
        void WriteResponse(); 

        void PerParseGetParam(); // 解析请求的参数
		// 发送响应
private:
   tcp::socket _socket;
   http::request<http::dynamic_body> request_;
   http::response<http::dynamic_body> response_;
   std::string url_; 
   std::unordered_map<std::string ,std::string> urlMap_;
   // 定义一个超时定时器
   net::steady_timer  deadLine{
    _socket.get_executor(),
    std::chrono::seconds(60)
   };
   net::steady_timer  deedline_timer_
   {
       _socket.get_executor(),std::chrono::seconds(60)
   };

};
