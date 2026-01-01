#pragma once
#include <map>
#include <string>

#include "Sington.h"

struct paramInfo
{
	paramInfo()
	{
		paramInfo_data_.clear();
	}
	// 拷贝构造函数
	paramInfo(const paramInfo& src)
	{
		paramInfo_data_ = src.paramInfo_data_;
	}
	/**
	 * 	Port=8080
		Host=127.0.0.1
		通过 port，获取具体的值
	 */

	 // 重载[] 操作符
	std::string operator [](const std::string key)
	{
		// 表示没有找到对应的key 所以返回空
		if (paramInfo_data_.find(key) == paramInfo_data_.end())
		{
			return "";
		}

		return paramInfo_data_[key];

	}

	// 需要一个map 存储配置信息
	std::map<std::string, std::string >  paramInfo_data_;
};


// 配置信息管理器
class configMgr
{
public:
	// 通过该函数来获取，只用该函数来获取配置信息
	/*
	 * [GetServer]:配置信息
		Port=8080
		Host=127.0.0.1
	 *
	 *
	 *
	 *
	 *
	 */

	static 	configMgr& inst()
	{
		static   configMgr  cnfMgr;
		return   cnfMgr;
	}


	paramInfo operator[](const  std::string& configName)
	{

		if (conFigMessage_.find(configName) == conFigMessage_.end())
		{
			return  paramInfo();
		}
		return  conFigMessage_[configName];
	}
	//赋值运算符
	configMgr& operator=(const configMgr& other)
	{
		if (&other == this)
		{
			return  *this;
		}
		this->conFigMessage_ = other.conFigMessage_;

	}






	//拷贝构造
	configMgr(const configMgr& other)
	{

		this->conFigMessage_ = other.conFigMessage_;
		//conFigMessage_ = other.conFigMessage_;


	}

	// 声明友元，允许 Sington 模板访问私有构造函数
	//friend class Sington<configMgr>;

private:
	// 构造函数私有化（通过友元允许 Sington 访问）
	configMgr();
	std::map<std::string, paramInfo>  conFigMessage_;


};
