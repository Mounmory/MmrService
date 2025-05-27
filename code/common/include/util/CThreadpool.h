/**
 * @file CThreadpool.h
 * @brief 线程池
 * @author Mounmory (237628106@qq.com) https://github.com/Mounmory
 * @date 
 *
 * 
 */

#ifndef MMR_UTIL_THREADPOOL_H
#define MMR_UTIL_THREADPOOL_H
#include <common/include/Common_def.h>
#include <common/include/util/UtilExport.h>
#include <common/include/general/Singleton.hpp>
#include <common/include/general/Noncopyable.hpp>
#include <common/include/general/VarTypeDict.hpp>

#include <iostream>
#include <vector>
#include <queue>
#include <memory>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <unordered_map>
#include <thread>
#include <future>
#include <exception>

BEGINE_NAMESPACE(mmrUtil)

//线程池支持的模式
enum class emPoolMode : uint8_t
{
	MODE_FIXED,		//固定数量的线程
	MODE_CACHED,	//线程数量可以动态增长
};

struct TagMode; 
struct TagFixThreadSize; 
struct TagTaskQueueMaxSize; 
struct TagMaxThreadSize; 
struct TagThreadIdleTime;

using ThreadpoolParas = mmrComm::VarTypeDict<struct TagMode, struct TagFixThreadSize, 
	struct TagTaskQueueMaxSize, struct TagMaxThreadSize, struct TagThreadIdleTime>;

//线程池类型
class COMMON_CLASS_API ThreadPool : public mmrComm::NonCopyable
{
	//Task任务 函数对象
	using TaskFunc = std::function<void()>;

	using ThreadFunc = std::function<void(int)>;

	class COMMON_CLASS_API Thread//线程类型
	{
	public:
		Thread(ThreadFunc func)
			:m_func(func)
			, m_threadId(++m_generateId) {}

		~Thread() = default;

		void start() { std::thread(m_func, m_threadId).detach(); }

		uint32_t getId() const { return m_threadId; }
	private:
		ThreadFunc m_func;
		static uint32_t m_generateId;
		uint32_t m_threadId;		//保存线程id
	};

	friend class mmrComm::Singleton<mmrUtil::ThreadPool>;

	ThreadPool(emPoolMode mode = emPoolMode::MODE_FIXED, uint16_t treadSize = std::thread::hardware_concurrency(), uint32_t ulMaxTaskSize = 10000, uint16_t usMaxThreadSize = 1024, uint16_t usMaxIdleTime = 60);

	template <typename TIn>
	ThreadPool(TIn&& in) : ThreadPool(in.template Get<TagMode>(), in.template Get<TagFixThreadSize>()
		, in.template Get<TagTaskQueueMaxSize>(), in.template Get<TagMaxThreadSize>(), in.template Get<TagThreadIdleTime>())
	{
	}

public:
	//线程池析构
	~ThreadPool();

	//给线程池提交任务
	//使用可变参模板变参 让submitTask可以接受任意任务函数和任意数量的参数
	template<typename Func, typename... Args>
	auto submit(Func&& func, Args&&... args)->std::future<decltype(func(args...))>
	{
		using RType = decltype(func(args...));//返回类型
		auto task = std::make_shared<std::packaged_task<RType(void)>>(std::bind(std::forward<Func>(func),
			std::forward<Args>(args)...));

		std::future<RType> result = task->get_future();
		std::unique_lock<std::mutex> lock(m_mutex);//获取锁
		if (!m_cv.wait_for(lock, std::chrono::seconds(1),//队列已经满了，等待1秒，仍是满的状态
			[&]()->bool {return m_queueTask.size() < m_ulTaskqueMaxSize; }))
		{
			throw std::runtime_error("task queue is full!");
			//(*task)();//直接在当前线程执行任务
		}
		else //如果有空余， 把任务放入任务队列中
		{
			m_queueTask.emplace([task]() {(*task)(); });
			
			if (m_poolMode == emPoolMode::MODE_CACHED &&  m_queueTask.size() > m_usIdleThreadSize.load(std::memory_order_relaxed)
				&& m_usCurThreadSize.load(std::memory_order_relaxed) < m_usMaxTreadSize)
			{
				//如果线程池数量动态变化的，且队列任务数大于线程数，新建一个线程
				createThreadInner();
			}
			m_cv.notify_all();
		}

		return result;
	}

	//处理命令行控制命令
	void loop(std::atomic_bool& bRunFlag);
private:
	//开启线程池
	void start();

	//停止线程池
	void stop();

	//创建线程
	void createThreadInner();

	//定义线程函数
	void threadFunc(uint32_t threadId);
private:
	emPoolMode m_poolMode;//线程池工作模式

	uint16_t m_usThreadSize;//初始的线程数量
	uint32_t m_ulTaskqueMaxSize;//任务队列数量上限的阈值

	uint16_t m_usMaxTreadSize;//用于在MODE_CACHED，控制最大线程数量
	uint16_t m_usMaxIdleTime;//用于在MODE_CACHED，线程闲置回时间

	std::atomic<uint16_t> m_usCurThreadSize;//用于在MODE_CACHED，动态线程数量是，记录当前线程池里面线程的总数量
	std::atomic<uint16_t> m_usIdleThreadSize;//记录空闲线程的数量

	std::mutex m_mutex;//任务队列和线程互斥量
	std::condition_variable m_cv;//任务队列及线程管理变化条件变量
	std::queue<TaskFunc> m_queueTask;//任务队列
	std::unordered_map <uint32_t, std::unique_ptr<Thread>> m_mapThreads;

	std::atomic_bool m_isPoolRunning;//表示当前线程池的启动状态
};

template class COMMON_CLASS_API mmrComm::Singleton<mmrUtil::ThreadPool>;

END_NAMESPACE(mmrUtil)

#define threadPooLIntance mmrComm::Singleton<mmrUtil::ThreadPool>::getInstance()

#endif // !MMR_UTIL_THREADPOOL_H
