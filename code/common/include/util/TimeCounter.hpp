/**
 * @file TimeCounter.hpp
 * @brief 
 * @author Mounmory (237628106@qq.com) https://github.com/Mounmory
 * @date 
 *
 * 
 */

#ifndef MMR_UTIL_TIME_COUNTER_HPP
#define MMR_UTIL_TIME_COUNTER_HPP
#include <common/include/Common_def.h>

#include<chrono>

BEGINE_NAMESPACE(mmrUtil)

class TimeCounter
{
public:
	TimeCounter() : m_begin(std::chrono::high_resolution_clock::now()) {}

	void reset()
	{ 
		m_begin = std::chrono::high_resolution_clock::now(); 
	}

	//ƒ¨»œ ‰≥ˆ∫¡√Î
	int64_t elapsed() const
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::high_resolution_clock::now() - m_begin).count();
	}

	//Œ¢√Î
	int64_t elapsed_micro() const
	{
		return std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::high_resolution_clock::now() - m_begin).count();
	}

	//ƒ…√Î
	int64_t elapsed_nano() const
	{
		return std::chrono::duration_cast<std::chrono::nanoseconds>(
			std::chrono::high_resolution_clock::now() - m_begin).count();
	}

	//√Î
	int64_t elapsed_seconds() const
	{
		return std::chrono::duration_cast<std::chrono::seconds>(
			std::chrono::high_resolution_clock::now() - m_begin).count();
	}

	//∑÷
	int64_t elapsed_minutes() const
	{
		return std::chrono::duration_cast<std::chrono::minutes>(
			std::chrono::high_resolution_clock::now() - m_begin).count();
	}

	// ±
	int64_t elapsed_hours() const
	{
		return std::chrono::duration_cast<std::chrono::hours>(
			std::chrono::high_resolution_clock::now() - m_begin).count();
	}

private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_begin;
};

END_NAMESPACE(mmrUtil)
#endif  //MMR_UTIL_TIME_COUNTER_HPP
