#include <common/include/util/ChunkAllocator.h>
#include <common/include/util/Clogger.h>

#include <mutex>
#include <unordered_map>
#include <map>
#include <deque>
#include <chrono>
#include <iostream>

using TimeType = std::chrono::system_clock::time_point;

template<size_t Align>
struct mmrUtil::ChunkAllocator<Align>::DataImp
{
	std::map<uint32_t, std::deque<std::pair<TimeType, void*>> > memBuffer;
	std::mutex mt;

	void clear() 
	{
		for (auto& p : memBuffer)
		{
			auto& refVec = p.second;
			for (auto& p1 : refVec)
			{
				char* buf = (char*)(p1.second);
				delete[]buf;
			}
			refVec.clear();
		}
	}
};

template<size_t Align>
mmrUtil::ChunkAllocator<Align>::ChunkAllocator(uint32_t ulExpiredTime, uint32_t ulMaxCache)
	: m_ulCacheSize(0)
	, m_ulExpireTime(ulExpiredTime)
	, m_usMaxCacheSize(ulMaxCache * 1024 * 1024)//4M
	, m_ptrData(std::make_unique<DataImp>())
{
	std::cout << "ChunkAllocator instance construct: expired time[" << m_ulExpireTime << " Min] Max Cache size [" << ulMaxCache << " Mb]" << std::endl;
}

template<size_t Align>
mmrUtil::ChunkAllocator<Align>::~ChunkAllocator()
{
	m_ptrData->clear();
	m_ulCacheSize = 0;
	std::cout << "Allocator instance destruct." << std::endl;
}

template<size_t Align>
void mmrUtil::ChunkAllocator<Align>::FreeExpiredMemory() 
{
	//清理过期的缓存
	auto timeNow = std::chrono::system_clock::now();
	std::lock_guard<std::mutex> guard(m_ptrData->mt);
	for (auto iterMemQueue = m_ptrData->memBuffer.begin(); iterMemQueue != m_ptrData->memBuffer.end();)
	{
		auto& queMem = iterMemQueue->second;
		for (auto iterBlock = queMem.begin(); iterBlock != queMem.end(); )
		{
			if (std::chrono::duration_cast<std::chrono::minutes>(timeNow - iterBlock->first).count() > m_ulExpireTime)
			{
				char* buf = (char*)(iterBlock->second);
				delete[]buf;

				m_ulCacheSize -= iterMemQueue->first;
				iterBlock = iterMemQueue->second.erase(iterBlock);
			}
			else
			{
				++iterBlock;
			}
		}
		if (queMem.size() == 0)
		{
			iterMemQueue = m_ptrData->memBuffer.erase(iterMemQueue);
		}
		else
		{
			++iterMemQueue;
		}
	}
}

template<size_t Align>
std::shared_ptr<void> mmrUtil::ChunkAllocator<Align>::alloMemory(uint32_t ulElemSize)
{
	void* ptrVoid = alloRowMemory(ulElemSize);//将锁操作封装独立接口，避免new内存时延长锁时长
	if (nullptr == ptrVoid) 
	{
		ptrVoid = new char[ulElemSize];
	}
	return std::shared_ptr<void>(ptrVoid, [=](void* pBuf) {deallocate(ulElemSize, pBuf); });
}

template<size_t Align>
void * mmrUtil::ChunkAllocator<Align>::alloRowMemory(uint32_t ulElemSize)
{
	void* ptrVoid = nullptr;
	std::lock_guard<std::mutex> guard(m_ptrData->mt);
	auto iterBuf = m_ptrData->memBuffer.find(ulElemSize);
	if (iterBuf != m_ptrData->memBuffer.end())
	{
		ptrVoid = iterBuf->second.back().second;
		iterBuf->second.pop_back();
		m_ulCacheSize -= ulElemSize;
	}
	return ptrVoid;
}

template<size_t Align>
void mmrUtil::ChunkAllocator<Align>::deallocate(uint32_t ulBufSize, void* pBuf)
{
	m_ulCacheSize += ulBufSize;
	std::lock_guard<std::mutex> guard(m_ptrData->mt);
	m_ptrData->memBuffer[ulBufSize].push_back(std::make_pair(std::chrono::system_clock::now(), pBuf));
	//std::cout << "deallocate size " << ulBufSize << std::endl;
	if (m_ulCacheSize > m_usMaxCacheSize)
	{
		if (logInstancePtr) 
		{
			LOG_WARN("Before clear cache, cache size[%ld],MaxCacheSize[%ld].", m_ulCacheSize, m_usMaxCacheSize);
		}
		//清理过期的缓存
		auto timeNow = std::chrono::system_clock::now();
		for (auto iterMemQueue = m_ptrData->memBuffer.begin(); iterMemQueue != m_ptrData->memBuffer.end();)
		{
			auto& queMem = iterMemQueue->second;
			for (auto iterBlock = queMem.begin();iterBlock != queMem.end(); )
			{
				if (std::chrono::duration_cast<std::chrono::minutes>(timeNow - iterBlock->first).count() > m_ulExpireTime) 
				{
					char* buf = (char*)(iterBlock->second);
					delete[]buf;

					m_ulCacheSize -= iterMemQueue->first;
					iterBlock = iterMemQueue->second.erase(iterBlock);
				}
				else 
				{
					++iterBlock;
				}
			}
			if (queMem.size() == 0)
			{
				iterMemQueue = m_ptrData->memBuffer.erase(iterMemQueue);
			}
			else 
			{
				++iterMemQueue;
			}
		}
		//如果仍然超过最大值，把内存最大的一块清理，肯定小于最大限了
		if (m_ulCacheSize > m_usMaxCacheSize)
		{
			auto iterRMem = m_ptrData->memBuffer.rbegin();
			if (iterRMem != m_ptrData->memBuffer.rend() && iterRMem->second.size() > 0)//肯定是成立的
			{
				auto& pairBack = iterRMem->second.front();
				char* buf = (char*)(pairBack.second);
				delete[]buf;
				iterRMem->second.pop_front();
				m_ulCacheSize -= iterRMem->first;
				if (iterRMem->second.size() == 0)
				{
					m_ptrData->memBuffer.erase(std::next(iterRMem).base());
				}
			}
		}
		if (logInstancePtr)
		{
			LOG_WARN("After clear cache, cache size[%ld].", m_ulCacheSize);
		}
	}
}


