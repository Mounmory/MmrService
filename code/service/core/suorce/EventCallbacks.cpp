#include "EventCallbacks.h"

#include <string>
#include <unordered_map>
#include <list>
#include <mutex>
#include <queue>
#include <future>

using namespace mmrService::mmrCore;

struct CEventDealWithLock::CImpData
{
	CImpData()
		: m_bDataAdded(false)
	{
	}
	~CImpData() 
	{
		clear();
	}

	void clear()
	{
		{
			std::unique_lock<std::mutex> lock(m_mutexData);
			m_bDataAdded.store(false);
			while (!m_queueAddData.empty())
				m_queueAddData.pop();
			while (!m_queueDealData.empty())
				m_queueDealData.pop();
		}
		{
			std::lock_guard<std::mutex> lock(m_mutexFunc);
			m_mapFuncs.clear();
		}
	}
	//处理事件相关成员
	std::unordered_map<std::string, std::list<std::weak_ptr<CallbackFunc>>> m_mapFuncs;//所有事件处理者
	std::mutex m_mutexFunc;//事件处理集合互斥量

	//事件数据，异步处理
	std::queue<std::pair<std::string, mmrUtil::CVarDatas>> m_queueAddData;
	std::queue<std::pair<std::string, mmrUtil::CVarDatas>> m_queueDealData;
	std::atomic_bool m_bDataAdded;
	std::mutex m_mutexData;
	std::condition_variable m_cvData;
};

CEventDealWithLock::CEventDealWithLock()
	: m_data(std::make_unique<CImpData>())
	, m_bTheadRun(false)
{

}

CEventDealWithLock::~CEventDealWithLock()
{
	stop();
}

void CEventDealWithLock::start()
{
	m_bTheadRun.store(true);
	m_future = std::async(std::launch::async, &CEventDealWithLock::dealEventData, this);
}

void CEventDealWithLock::stop()
{
	if (m_bTheadRun.load() && m_future.wait_for(std::chrono::milliseconds(0)) != std::future_status::deferred)
	{
		m_bTheadRun.store(false);
		m_data->m_cvData.notify_all();
		m_future.get();
	}
}

void CEventDealWithLock::addFunc(const std::string& strTopcs, const std::shared_ptr<CallbackFunc>& ptrFunc)
{
	std::lock_guard<std::mutex> lock(m_data->m_mutexFunc);
	m_data->m_mapFuncs[strTopcs].emplace_back(ptrFunc);
}

void CEventDealWithLock::addEvenVartData(mmrUtil::CVarDatas&& varData)
{
	std::string strEventName = varData.getName();
	std::unique_lock<std::mutex> lock(m_data->m_mutexData);
	//m_queueAddData.emplace(std::make_pair(std::move(strEventName), std::forward<mmrUtil::CVarDatas>(varData)));
	m_data->m_queueAddData.emplace(std::make_pair(strEventName, std::forward<mmrUtil::CVarDatas>(varData)));//对于小字符串优化，string不需要move
	m_data->m_bDataAdded.store(true);
	m_data->m_cvData.notify_all();
}

void CEventDealWithLock::dealEventData()
{
	while (m_bTheadRun.load(std::memory_order_relaxed))
	{
		if (m_data->m_bDataAdded.load())
		{
			{//交换数据
				std::lock_guard<std::mutex> lock(m_data->m_mutexData);
				std::swap(m_data->m_queueAddData, m_data->m_queueDealData);
				m_data->m_bDataAdded.store(false);
			}

			{//处理数据
				std::lock_guard<std::mutex> lock(m_data->m_mutexFunc);
				while (m_data->m_queueDealData.size() > 0)
				{
					const auto& data = m_data->m_queueDealData.front();

					auto iterMap = m_data->m_mapFuncs.find(data.first);
					if (iterMap != m_data->m_mapFuncs.end())
					{
						for (auto iterFunc = iterMap->second.begin(); iterFunc != iterMap->second.end();)
						{
							auto ptrFunc = iterFunc->lock();
							if (ptrFunc)
							{
								(*ptrFunc)(data.second);
								++iterFunc;
							}
							else 
							{
								iterFunc = iterMap->second.erase(iterFunc);
							}
						}
						//主题的处理函数为空了，清空这个主题
						if (iterMap->second.size() == 0)
						{
							m_data->m_mapFuncs.erase(iterMap);
						}
					}
					m_data->m_queueDealData.pop();
				}
			}
		}

		{
			std::unique_lock<std::mutex> lock(m_data->m_mutexData);
			while (!m_data->m_bDataAdded.load() && m_bTheadRun.load(std::memory_order_relaxed))
			{
				m_data->m_cvData.wait_for(lock, std::chrono::seconds(10));
			}
		}
	}
}
