#pragma once
#include <memory>
#include <mutex>
#include <boost/smart_ptr/shared_ptr.hpp>


template <typename T>
class Sington
{

public:
	static std::shared_ptr<T> getInstance()
	{
		if (Instance_ == nullptr)
		{
			std::lock_guard<std::mutex>lock(mutex_);
			{
				if (Instance_ == nullptr)

				{
					Instance_ = std::shared_ptr<T>(new T);
				}
			}

		}
		return  Instance_;

	}

protected:
	Sington() = default;
	Sington(const Sington<T>&) = delete;
	Sington operator=(const Sington<T>& st) = delete;

private:
	static std::mutex mutex_;
	static 	std::shared_ptr<T>   Instance_;
};
// 静态的成员变量需要进行初始化，因为是模板类，所以需要在头文件中直接进行初始化
template <typename  T>
std::mutex Sington<T>::mutex_;
template <typename  T>
std::shared_ptr<T> Sington<T>::Instance_ = nullptr;
