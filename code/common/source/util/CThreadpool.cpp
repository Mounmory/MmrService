#include "common/include/util/CThreadpool.h"
#include <common/include/util/Clogger.h>

const int THREAD_MAX_IDLE_TIME = 60;	//单位：秒

using namespace mmrUtil;

uint32_t mmrUtil::ThreadPool::Thread::m_generateId = 0;

ThreadPool::ThreadPool(emPoolMode mode, uint16_t treadSize, uint32_t ulMaxTaskSize, uint16_t usMaxThreadSize, uint16_t usMaxIdleTime)
	: m_poolMode(mode)
	, m_usThreadSize(treadSize)
	, m_usMaxTreadSize(usMaxThreadSize)
	, m_ulTaskqueMaxSize(ulMaxTaskSize)
	, m_usMaxIdleTime(usMaxIdleTime)
	, m_usCurThreadSize(0)
	, m_usIdleThreadSize(0)
	, m_isPoolRunning(false)
{
	if (0 == m_usThreadSize)
		m_usThreadSize = std::thread::hardware_concurrency();

	start();

	std::cout << "ThreadPool instance construct: "
		"pool mode [" << static_cast<int32_t>(m_poolMode) << "] "
		"fix thread size [" << m_usThreadSize << " ] " <<
		"max task queue size [" << m_ulTaskqueMaxSize <<"] "<<
		"max thread size ["<< m_usMaxTreadSize <<"] " <<
		"thread idle time ["<< m_usMaxIdleTime << "]."<<std::endl;
}

ThreadPool::~ThreadPool()
{
	stop(); 
	std::cout << "ThreadPool instance destruct." << std::endl;
}

void ThreadPool::loop(std::atomic_bool& bRunFlag) 
{
	printf("Thead pool info:\n"
		"\tmode[%s], fix thread size[%ld], max task queue size[%ld], current task queue size[%ld], current thread size[%ld], idle thread size[%ld]\n"
		, m_poolMode == emPoolMode::MODE_FIXED ? "fix" : "cache"
		, m_usThreadSize
		, m_ulTaskqueMaxSize
		, m_queueTask.size()
		, m_usCurThreadSize.load(std::memory_order_relaxed)
		, m_usIdleThreadSize.load(std::memory_order_relaxed));

}

void ThreadPool::start()
{
	m_isPoolRunning.store(true, std::memory_order_relaxed);
	//创建线程对象
	for (int i = 0; i < m_usThreadSize; i++)
	{
		createThreadInner();
	}
}

void ThreadPool::stop()
{
	m_isPoolRunning.store(false, std::memory_order_relaxed);

	//等待线程池里面所有的线程返回 线程有两种状态 阻塞和正在执行中
	std::unique_lock<std::mutex> lock(m_mutex);
	m_cv.notify_all();
	m_cv.wait(lock, [&]()->bool 
	{
		LOG_INFO("Remaind thread num is %ld", m_mapThreads.size());
		return m_mapThreads.size() == 0; 
	});//等待所有线程退出
}

//创建线程
void ThreadPool::createThreadInner() 
{
	//创建新的线程对象
	auto ptr = std::make_unique<Thread>(std::bind(&ThreadPool::threadFunc, this, std::placeholders::_1));//c++14才有
	auto threadId = ptr->getId();
	m_mapThreads.emplace(threadId, std::move(ptr));
	//启动线程
	m_mapThreads[threadId]->start();
	//修改线程个数相关的变量
	m_usCurThreadSize.fetch_add(1, std::memory_order_relaxed);
	m_usIdleThreadSize.fetch_add(1, std::memory_order_relaxed);

	LOG_INFO("Thread ID [%ld] created! thread num now is %ld, current thread size %d, idle thead size %d."
		, threadId, m_mapThreads.size(), m_usCurThreadSize.load(std::memory_order_relaxed), m_usIdleThreadSize.load(std::memory_order_relaxed));
}

void ThreadPool::threadFunc(uint32_t threadId)
{
	auto lastTime = std::chrono::high_resolution_clock().now();

	//所有任务必须执行完成，线程池才能回收所有线程资源
	while (m_isPoolRunning.load(std::memory_order_relaxed))
	{
		TaskFunc task;
		{
			//获取锁
			std::unique_lock<std::mutex> lock(m_mutex);
			if (m_queueTask.size() == 0)
			{
				if (m_poolMode == emPoolMode::MODE_CACHED 
					&& std::cv_status::timeout == m_cv.wait_for(lock, std::chrono::seconds(1))//没有任务时释放锁等待1秒，避免线程空运行
					&& m_usCurThreadSize.load(std::memory_order_relaxed) > m_usThreadSize)
				{
					auto now = std::chrono::high_resolution_clock().now();
					auto dur = std::chrono::duration_cast<std::chrono::seconds>(now - lastTime);
					if (dur.count() >= m_usMaxIdleTime)
					{
						LOG_INFO("Thread ID [%ld] stoped by expired idle time %d!", threadId, dur.count());
						break;;
					}
				}
				else 
				{
					m_cv.wait_for(lock, std::chrono::seconds(5));
				}
			}
			else 
			{
				m_usIdleThreadSize.fetch_sub(1, std::memory_order_relaxed);//空闲线程少一个
				task = std::move(m_queueTask.front());
				m_queueTask.pop();
			}
		}//把锁放掉

		 //当前线程负责执行这个任务
		if (task != nullptr)
		{
			task();
			lastTime = std::chrono::high_resolution_clock().now();
			m_usIdleThreadSize.fetch_add(1, std::memory_order_relaxed);
		}
	}

	{
		m_usCurThreadSize.fetch_sub(1, std::memory_order_relaxed);
		m_usIdleThreadSize.fetch_sub(1, std::memory_order_relaxed);
		std::unique_lock<std::mutex> lock(m_mutex);
		m_mapThreads.erase(threadId); // std::this_thread::getid()
		LOG_INFO("Thread ID [%ld] stoped! thread num now is %ld, current thread size %d, idle thead size %d."
			, threadId, m_mapThreads.size(), m_usCurThreadSize.load(std::memory_order_relaxed), m_usIdleThreadSize.load(std::memory_order_relaxed));
	}
	m_cv.notify_all();

}
