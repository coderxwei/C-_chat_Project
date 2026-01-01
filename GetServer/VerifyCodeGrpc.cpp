#pragma once
#include "VerifyCodeGrpc.h"

#include "configMgr.h"
//实现GRpc 连接池的实现

// 验证码服务的Grpc 客户端进行初始化。  对于端口号和主机地址进行初始化。
VerifyCodeGrpc::VerifyCodeGrpc()
{
	auto& configMgr = configMgr::inst();

	std::string   host = configMgr["VarifyServer"]["Host"];
	std::string   port = configMgr["VarifyServer"]["Prot"];

}
