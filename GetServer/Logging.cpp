#include "Logging.h"
#include <iostream>

// 初始化日志系统
void Logging::init(const std::string& logFilePath)
{
	std::lock_guard<std::mutex> lock(mutex_);
	
	logFilePath_ = logFilePath;
	
	if (!logFilePath.empty())
	{
		// 以追加模式打开文件
		logFile_.open(logFilePath, std::ios::app);
		if (logFile_.is_open())
		{
			fileOutput_ = true;
		}
		else
		{
			std::cerr << "Failed to open log file: " << logFilePath << std::endl;
			fileOutput_ = false;
		}
	}
}

// 设置日志级别
void Logging::setLogLevel(LogLevel level)
{
	std::lock_guard<std::mutex> lock(mutex_);
	currentLogLevel_ = level;
}

// 设置是否输出到控制台
void Logging::setConsoleOutput(bool enable)
{
	std::lock_guard<std::mutex> lock(mutex_);
	consoleOutput_ = enable;
}

// 设置是否输出到文件
void Logging::setFileOutput(bool enable)
{
	std::lock_guard<std::mutex> lock(mutex_);
	fileOutput_ = enable;
}

// DEBUG 级别日志
void Logging::DEBUG(const std::string& message)
{
	log(LogLevel::DEBUG, message);
}

// INFO 级别日志
void Logging::INFO(const std::string& message)
{
	log(LogLevel::INFO, message);
}

// WARN 级别日志
void Logging::WARN(const std::string& message)
{
	log(LogLevel::WARN, message);
}

// ERROR 级别日志
void Logging::ERROR(const std::string& message)
{
	log(LogLevel::ERROR, message);
}

// 内部日志输出函数
void Logging::log(LogLevel level, const std::string& message)
{
	// 如果日志级别低于当前设置的最低级别，则不输出
	if (level < currentLogLevel_)
	{
		return;
	}

	std::lock_guard<std::mutex> lock(mutex_);
	
	std::string formattedMessage = formatMessage(level, message);

	// 输出到控制台
	if (consoleOutput_)
	{
		std::cout << formattedMessage << std::endl;
	}

	// 输出到文件
	if (fileOutput_ && logFile_.is_open())
	{
		logFile_ << formattedMessage << std::endl;
		logFile_.flush();  // 立即刷新到文件
	}
}

// 获取日志级别字符串
std::string Logging::getLevelString(LogLevel level) const
{
	switch (level)
	{
	case LogLevel::DEBUG:
		return "DEBUG";
	case LogLevel::INFO:
		return "INFO";
	case LogLevel::WARN:
		return "WARN";
	case LogLevel::ERROR:
		return "ERROR";
	default:
		return "UNKNOWN";
	}
}

// 获取当前时间字符串
std::string Logging::getCurrentTime() const
{
	auto now = std::chrono::system_clock::now();
	auto time_t = std::chrono::system_clock::to_time_t(now);
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
		now.time_since_epoch()) % 1000;

	std::stringstream ss;
	ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
	ss << '.' << std::setfill('0') << std::setw(3) << ms.count();
	return ss.str();
}

// 格式化日志消息
std::string Logging::formatMessage(LogLevel level, const std::string& message) const
{
	std::stringstream ss;
	ss << "[" << getCurrentTime() << "] "
		<< "[" << getLevelString(level) << "] "
		<< message;
	return ss.str();
}

// 析构函数
Logging::~Logging()
{
	std::lock_guard<std::mutex> lock(mutex_);
	if (logFile_.is_open())
	{
		logFile_.close();
	}
}

