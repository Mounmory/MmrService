#include "common/include/Common_def.h"
#include "common/include/util/UtilFunc.h"
#include "common/include/util/CLicenseObj.h"
#include "common/include/util/CThreadpool.h"
#include <common/include/util/Clogger.h>
#include <thread>
#include <chrono>
#include <vector>
#include <iostream>


int addNum(std::string tag, int num1,uint8_t num2);

//测试任务队列满后抛异常
void TestTaskQueueException();

//测试线程数量动态变化
void TestChaceMode();

int main(int argc, char **argv)
{
	//TestTaskQueueException();

	TestChaceMode();

	std::cout << "输入任意字符继续..." << std::endl;
	std::cin.get();
	return 0;
}

int addNum(std::string tag, int num1, uint8_t num2)
{
	LOG_INFO("Tag %s begine", tag.c_str());
	for (uint8_t i = 0 ; i < num2 ; ++i)
	{
		num1 += i;
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		//std::cout << tag << "num1 value now is " << num1 << std::endl;
	}
	LOG_INFO("Tag %s end", tag.c_str());
	return num1;
}

void TestTaskQueueException() 
{
	std::cout << "-----------测试线程队列满后抛出异常：开始-----------" << std::endl;
	mmrComm::Singleton<mmrUtil::CLogger>::initInstance(10, 16, true);
	auto ptrPool = mmrComm::Singleton<mmrUtil::ThreadPool>::initInstance(mmrUtil::emPoolMode::MODE_FIXED, 4, 100, 10);

	std::vector<std::future<int>> vecFu;
	for (int i = 0; i < 100; ++i)
	{
		try
		{
			vecFu.emplace_back(ptrPool->submit(addNum, std::to_string(i), i, 10));
		}
		catch (const std::exception& e)
		{
			std::cout << "exception info " << e.what() << " i value is " << i << std::endl;
			break;
		}
	}
	for (int i = 0 ;i < vecFu.size(); ++i) 
	{
		LOG_INFO("index %d future %ld", i, vecFu[i].get());
	}

	mmrComm::Singleton<mmrUtil::ThreadPool>::destroyInstance();
	mmrComm::Singleton<mmrUtil::CLogger>::destroyInstance();
	std::cout << "-----------测试线程队列满后抛出异常：结束-----------" << std::endl;
}

void TestChaceMode() 
{
	std::cout << "-----------测试线程自动增长后回收线程：开始-----------" << std::endl;
	mmrComm::Singleton<mmrUtil::CLogger>::initInstance(10, 16, true);
	auto ptrPool = mmrComm::Singleton<mmrUtil::ThreadPool>::initInstance(mmrUtil::emPoolMode::MODE_CACHED//线程数量动态变化
		, 4	//固定线程数
		, 10000 //任务队列最大任务数
		, 40 //最大线程数量
		,10);//线程闲置回收时间

	std::vector<std::future<int>> vecFu;
	for (int i = 0; i < 80; ++i)//线程池中加入80个任务，每个任务执行时间5秒，最多40个线程
	{
		try
		{
			vecFu.emplace_back(ptrPool->submit(addNum, std::to_string(i), i, 10));
		}
		catch (const std::exception& e)
		{
			std::cout << "exception info " << e.what() << " i value is " << i << std::endl;
			break;
		}
	}
	for (int i = 0; i < vecFu.size(); ++i)
	{
		LOG_INFO("index %d future %ld", i, vecFu[i].get());
	}
	LOG_INFO("before sleep");
	std::this_thread::sleep_for(std::chrono::seconds(200));
	LOG_INFO("end sleep");//睡眠后看后台日志，线程数应该为4个
	//std::cin.get();
	mmrComm::Singleton<mmrUtil::ThreadPool>::destroyInstance();
	mmrComm::Singleton<mmrUtil::CLogger>::destroyInstance();
	std::cout << "-----------测试线程自动增长后回收线程：结束-----------" << std::endl;
}
