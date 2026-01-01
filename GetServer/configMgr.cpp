#include  "configMgr.h"
//配置文件读取配置
#include <iostream>

#include  "Global.h"
#include  <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
configMgr::configMgr()
{
	//获得当前的工作路径
	boost::filesystem::path  curentPath = boost::filesystem::current_path();

	//构建Config.ini的完成路径
	boost::filesystem::path config_ini_path = curentPath / "资源文件" / "Config.ini";
	std::cout << "config_ini 的路径是什么" << config_ini_path << std::endl;
	//使 用boost.propertyTree 来读取文件--👉 从一个 ini 文件中读取配置，并解析到 pt 这个 property_tree 对象里。
	boost::property_tree::ptree pt;
	boost::property_tree::read_ini(config_ini_path.string(), pt);
	// 遍历这个对象树---用于存储信息
	for (auto& section_pair : pt)
	{
		// 获得服务名称比如 【varifycode】中的varifycode
		const std::string& section_name = section_pair.first;
		// 获得服务具体的配置信息
		const boost::property_tree::ptree& section_tree = section_pair.second;
		// 遍历section_tree
		std::map<std::string, std::string> section_config;
		for (auto& key_value : section_tree)
		{
			const std::string key = key_value.first;
			const std::string value = key_value.second.get_value<std::string>();
			// 存储到一个map 中
			section_config[key] = value;
		}
		paramInfo paramInfo_;
		paramInfo_.paramInfo_data_ = section_config;
		// 再把具体的配置信息存储到服务名称对应的map中
		conFigMessage_[section_name] = paramInfo_;
	}
	// 打印已经存储的信息
	for (const  auto& section_Entry: conFigMessage_)
	{
		const std::string  cof_service_name = section_Entry.first;
		paramInfo cof_service_content = section_Entry.second;

		std::cout << "[" << cof_service_name << "]" << std::endl;
		for ( auto& key_value_pair: cof_service_content.paramInfo_data_)
		{
			std::cout << key_value_pair.first << ":" << key_value_pair.second << std::endl;

		}
		



	}









}
