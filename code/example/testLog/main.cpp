#include "common/include/Common_def.h"
#include "common/include/util/Clogger.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <thread>
#include <atomic>	

std::atomic_bool g_atoStart;

std::shared_ptr<mmrUtil::LogWrapper> g_LoggerPtr;

void logThread() 
{
	while (!g_atoStart.load());

	std::cout << "write log start!" << std::endl;

	for (int i = 0; i < 10000000; ++i)
	{
		LOG_FORCE("multi thread log test. a looooooooooooooooooooooooooooooooooooooooooooooooooooooooog string, i value is %d!", i);
	}
}

int main()
{
	g_LoggerPtr = std::make_shared<mmrUtil::LogWrapper>();
	
	//测试同步写日志
	{
		std::cout << "syn log test..." << std::endl;
		g_LoggerPtr->loger->start(false);

		for (int i = 0; i < 100; ++i)
		{
			LOG_FORCE("syn log test. a looooooooooooooooooooooooooooooooooooooooooooooooooooooooog string, i value is %d!", i);
			//std::cout << "i value is" << i << std::endl;
			//std::this_thread::sleep_for(std::chrono::microseconds(10));
		}
		g_LoggerPtr->loger->stop();
	}

	//异步写日志
	{
		std::cout << "asyn log test..." << std::endl;
		g_LoggerPtr->loger->start();

		for (int i = 0; i < 100; ++i)
		{
			LOG_FORCE("asyn log test. a looooooooooooooooooooooooooooooooooooooooooooooooooooooooog string, i value is %d!", i);
			//std::cout << "i value is" << i << std::endl;
			//std::this_thread::sleep_for(std::chrono::microseconds(10));
		}
		g_LoggerPtr->loger->stop();
	}

	//异步多线程写日志
	{
		std::cout << "multi thread log test..." << std::endl;
		g_atoStart.store(false);
		std::vector<std::thread> vecThread;
		for (int i = 0; i < 4; ++i)
		{
			vecThread.emplace_back(std::thread(logThread));
		}
		g_atoStart.store(true);
		for (auto& iterThread : vecThread)
		{
			iterThread.join();
		}
	}


	std::cout << "输入任意字符继续..." << std::endl;
	std::cin.get();

	return 0;
}
