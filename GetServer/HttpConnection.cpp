#include "HttpConnection.h"

void HttpConnection::sendResponse()
{
	// 获取 shared_from_this 指针，确保在异步操作完成时，当前的对象不会被释放
	auto self = shared_from_this();
	response_.content_length(response_.body().size());
	http::async_write(_socket, response_,[self](beast::error_code errcode, std::size_t  bytes_transferred)
	{
			self->_socket.shutdown(tcp::socket::shutdown_send, errcode);
			// 取消超时定时器
			self->deadLine.cancel();
	});




}
