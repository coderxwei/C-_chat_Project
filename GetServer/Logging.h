#pragma once
#include "Sington.h"
#include <fstream>
#include <sstream>
#include <string>
#include <mutex>
#include <iomanip>
#include <chrono>

// 日志级别枚举
enum class LogLevel
{
	DEBUG = 0,
	INFO = 1,
	WARN = 2,
	ERROR = 3
};

class Logging : public Sington<Logging>
{
public:
	friend class Sington<Logging>;

	// 初始化日志系统
	// logFilePath: 日志文件路径，如果为空则只输出到控制台
	void init(const std::string& logFilePath = "");

	// 打印日志 - DEBUG 级别
	void DEBUG(const std::string& message);

	// 打印日志 - INFO 级别
	void INFO(const std::string& message);

	// 打印日志 - WARN 级别
	void WARN(const std::string& message);

	// 打印日志 - ERROR 级别
	void ERROR(const std::string& message);

	// 设置日志级别（低于此级别的日志将不输出）
	void setLogLevel(LogLevel level);

	// 设置是否输出到控制台
	void setConsoleOutput(bool enable);

	// 设置是否输出到文件
	void setFileOutput(bool enable);

protected:
	Logging() = default;
	Logging(const Logging&) = delete;
	Logging& operator=(const Logging&) = delete;
	~Logging();

private:
	// 内部日志输出函数
	void log(LogLevel level, const std::string& message);

	// 获取日志级别字符串
	std::string getLevelString(LogLevel level) const;

	// 获取当前时间字符串
	std::string getCurrentTime() const;

	// 格式化日志消息
	std::string formatMessage(LogLevel level, const std::string& message) const;

	std::mutex mutex_;                    // 互斥锁，保证线程安全
	std::ofstream logFile_;               // 日志文件流
	std::string logFilePath_;             // 日志文件路径
	LogLevel currentLogLevel_ = LogLevel::DEBUG;  // 当前日志级别
	bool consoleOutput_ = true;           // 是否输出到控制台
	bool fileOutput_ = false;             // 是否输出到文件
};
