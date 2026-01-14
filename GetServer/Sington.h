#pragma once
#include <memory>
#include <mutex>


template <typename T>
class Sington
{
public:
	// 获取单例实例（返回 shared_ptr）
	static T& Instance()
	{
		static T instance;
		return  instance;
	}

	
	// 构造函数设为 protected，只有派生类可以访问
	Sington(const Sington&) = delete;
	Sington& operator=(const Sington&) = delete;

protected:
	Sington() = default;
	~Sington() = default;

};


