#include <common/include/util/SmallAllocator.h>
#include <common/include/util/Clogger.h>
#include <common/include/util/UtilFunc.h>
#include <cstddef>
#include <stdexcept>
#include <iostream>
#include <climits>

using namespace mmrUtil;


FixedAllocator::ChunkLockFree::ChunkLockFree()
	: m_pData(nullptr)
, m_blockSize(0)
, m_availaBlockCurr(0)
, m_availaBlockNum(0)
, m_blockNum(0)
{ 
}

//FixedAllocator::ChunkLockFree::ChunkLockFree(FixedAllocator::ChunkLockFree&& rhs)
//	: m_pData(std::exchange(rhs.m_pData, nullptr))
//	, m_blockSize(std::exchange(rhs.m_blockSize, 0))
//	, m_availaBlockCurr(rhs.m_availaBlockCurr.exchange(0))
//	, m_availaBlockNum(rhs.m_availaBlockNum.exchange(0))
//	, m_blockNum(std::exchange(rhs.m_blockNum, 0))
//{ 
//}

FixedAllocator::ChunkLockFree::~ChunkLockFree()
{
	release();
}

//FixedAllocator::ChunkLockFree& FixedAllocator::ChunkLockFree::operator = (FixedAllocator::ChunkLockFree&& rhs)
//{
//	if (this != &rhs)
//	{
//		m_pData = std::exchange(rhs.m_pData, nullptr);
//		m_blockSize = std::exchange(rhs.m_blockSize, 0);
//		m_availaBlockCurr.store(rhs.m_availaBlockCurr.exchange(0));
//		m_availaBlockNum.store(rhs.m_availaBlockNum.exchange(0));
//		m_blockNum = std::exchange(rhs.m_blockNum, 0);
//	}
//	return *this;
//}

void  FixedAllocator::ChunkLockFree::init(std::size_t blockSize, uint8_t blockNum)
{
	assert(blockSize > 0);
	assert(blockNum > 0);
	assert((blockSize * blockNum) / blockSize == blockNum);// 溢出检查

	release();

	m_blockSize = blockSize;
	m_pData = new uint8_t[m_blockSize * blockNum];
	m_availaBlockCurr = 0;
	m_availaBlockNum.store(blockNum, std::memory_order_relaxed);
	m_blockNum = blockNum;//最后一个节点指向位置值
	uint8_t* p = m_pData;
	for (uint8_t i = 0; i != blockNum; p += m_blockSize)
	{
		*p = ++i;//初始化下一个节点位置值
	}
}

void* FixedAllocator::ChunkLockFree::allocate()
{
	uint8_t current_avail = m_availaBlockCurr.load(std::memory_order_acquire);
	uint8_t next_avail;
	uint8_t* ptrRet = nullptr;

	do {
		if (current_avail == m_blockNum)//没有可用内存块
		{
			ptrRet = nullptr;
			break;
		}

		// 计算新值
		ptrRet = m_pData + (current_avail * m_blockSize);
		next_avail = *ptrRet;
		// 尝试原子更新
	} while (!m_availaBlockCurr.compare_exchange_weak(current_avail, next_avail,
		std::memory_order_release,
		std::memory_order_relaxed));

	if (nullptr != ptrRet)
	{
		m_availaBlockNum.fetch_sub(1, std::memory_order_relaxed);//可用
	}

	return ptrRet;
}

void  FixedAllocator::ChunkLockFree::deallocate(void* ptr)
{
	assert(ptr >= m_pData);
	uint8_t* toRelease = static_cast<uint8_t*>(ptr);
	assert((toRelease - m_pData) % m_blockSize == 0);
	uint8_t block_index = static_cast<uint8_t>((toRelease - m_pData) / m_blockSize);//内存块索引
	assert(m_blockNum >= block_index);
	*toRelease = m_availaBlockCurr.load(std::memory_order_acquire);

	while (!m_availaBlockCurr.compare_exchange_weak(
		*toRelease, block_index,
		std::memory_order_release,
		std::memory_order_relaxed));

	m_availaBlockNum.fetch_add(1, std::memory_order_relaxed);
}

void FixedAllocator::ChunkLockFree::release()
{
	if (nullptr != m_pData)
	{
		if (m_availaBlockNum.load(std::memory_order_relaxed) != m_blockNum)
		{
			LOG_FATAL_PRINT("Chunk has unreturned memory, block size %d available num %d,total num %d",
				m_blockSize, m_availaBlockNum.load(std::memory_order_relaxed), m_blockNum);
			//throw std::runtime_error("unreturned memory exist!");//有待归还的内存，清掉很危险
		}
		m_availaBlockNum.store(0, std::memory_order_relaxed);
		m_availaBlockCurr.store(0, std::memory_order_relaxed);
		m_blockNum = 0;
		delete[] m_pData;
		m_pData = nullptr;
	}
}

//固定内存分配器定义
FixedAllocator::FixedAllocator(std::size_t blockSize) //负责分配的内存大小
	: m_blockSize(blockSize)
	, m_ptrAllocChunk(nullptr)
{
	assert(m_blockSize > 0);
	std::size_t numBlocks = ALLOC_CHUNK_SIZE / blockSize;

	if (numBlocks > UCHAR_MAX)
		numBlocks = UCHAR_MAX;
	else if (numBlocks == 0)
		numBlocks = 8 * blockSize;

	m_numBlocks = static_cast<unsigned char>(numBlocks);
	assert(m_numBlocks == numBlocks);
}

FixedAllocator::FixedAllocator(FixedAllocator&& rhs)
	: m_blockSize(rhs.m_blockSize)
	, m_numBlocks(std::exchange(rhs.m_numBlocks, 0))
	, m_listChunks(std::move(rhs.m_listChunks))
	, m_ptrAllocChunk(std::exchange(rhs.m_ptrAllocChunk, nullptr))
{
}

std::size_t FixedAllocator::getAvailableMemorySize()
{
	std::size_t memCount = 0;
	std::lock_guard<std::mutex> lock(m_mutexChunks);
	for (const auto& iterChunk : m_listChunks)
	{
		memCount += m_blockSize * iterChunk->availadeBlockNum();
	}
	return memCount;
}

void FixedAllocator::releaseFreeMemory()
{
	//这里有删除chunk，锁住后在allocate里就不会改变m_ptrAllocChunk了，能保证m_ptrAllocChunk指针一直有效
	std::lock_guard<std::mutex> lock(m_mutexChunks);
	for (auto iterChunk = m_listChunks.begin(); iterChunk != m_listChunks.end();)
	{
		if ((*iterChunk)->availadeBlockNum() == m_numBlocks && (*iterChunk).get() != m_ptrAllocChunk)
		{
			iterChunk = m_listChunks.erase(iterChunk);
		}
		else
		{
			++iterChunk;
		}
	}
}

std::pair<void*, FixedAllocator::ChunkType*> FixedAllocator::allocate()//返回分配的内存地址，属于哪个chunk
{
	std::pair<void*, ChunkType*> pairRet = { nullptr, nullptr };
	do
	{
		if (nullptr == m_ptrAllocChunk || m_ptrAllocChunk->availadeBlockNum() == 0)
		{
			std::lock_guard<std::mutex> lock(m_mutexChunks);//这个锁下面只添加chunk，不会导致m_ptrAllocChunk失效

			auto iterChunk = m_listChunks.begin();//先行查找一个可用的chunk
			for (; ; ++iterChunk)
			{
				if (iterChunk == m_listChunks.end())
				{
					auto newChunk = std::make_unique<ChunkType>();
					newChunk->init(m_blockSize, m_numBlocks);
					m_listChunks.emplace_back(std::move(newChunk));
					m_ptrAllocChunk = m_listChunks.back().get();
					break;
				}
				if ((*iterChunk)->availadeBlockNum() > 0)
				{
					m_ptrAllocChunk = (*iterChunk).get();
					break;
				}
			}
		}
		pairRet.second = m_ptrAllocChunk;
		pairRet.first = pairRet.second->allocate();

	} while (nullptr == pairRet.first);

	return pairRet;
}

SmallAllocator::SmallAllocator(uint32_t ulExpiredTime, uint32_t ulMaxCache)
	: m_ulExpireTime(ulExpiredTime)
	, m_usMaxFreeCacheSize(ulMaxCache * 1024 * 1024)//4M
{
	assert((_MaxObjectSize %_StepSize) == 0);
	m_vecPool.reserve(_MaxIndex);
	for (size_t i = 1; i <= _MaxIndex; i++)
	{
		m_vecPool.emplace_back(FixedAllocator(i*_StepSize));
	}
	std::cout << "SmallAllocator instance construct: _MaxObjectSize[" << _MaxObjectSize << " byte] _StepSize size [" << _StepSize << " byte]" 
	<< ", expired time[" << m_ulExpireTime << " Min] Max Cache size [" << ulMaxCache << " Mb]" << std::endl;
}

SmallAllocator::~SmallAllocator() 
{
	std::cout << "SmallAllocator instance destruct." << std::endl;
}

void SmallAllocator::onTimer(int64_t framTime)
{
	static int64_t timeLast = framTime;
	static constexpr int64_t timeInter = 10;
	if (framTime - timeLast >= m_ulExpireTime * 60)
	{
		auto pairMemInfoOld = getAvailableMemoryInfo();
		if (pairMemInfoOld.first > m_usMaxFreeCacheSize)
		{
			releaseFreeMemory();
			auto pairMemInfo = getAvailableMemoryInfo();
			LOG_INFO("memory info (free/total) before clear is  %lld/%lld Byte, after clear is  %lld/%lld Byte"
				, pairMemInfoOld.first, pairMemInfoOld.second
				, pairMemInfo.first, pairMemInfo.second);
		}
		timeLast = framTime;
	}
}

void SmallAllocator::loop(std::atomic_bool& bRunFlag) 
{
	auto pairAllocMemInfo = getAvailableMemoryInfo();
	printf("Small allocator memory pool info:\n"
			"\tSmall allocator memory(free/total): %lld/%lld Byte (%s/%s)\n"
				, pairAllocMemInfo.first
				, pairAllocMemInfo.second
				, formatMemorySize(pairAllocMemInfo.first).c_str()
				, formatMemorySize(pairAllocMemInfo.second).c_str());
}

std::pair<size_t, size_t> SmallAllocator::getAvailableMemoryInfo()
{
	std::pair<size_t, size_t> pairRet = { 0,0 };
	for (auto& iterVec : m_vecPool)
	{
		pairRet.first += iterVec.getAvailableMemorySize();
		pairRet.second += iterVec.getTotleMemorySize();
	}

	std::lock_guard<std::mutex> lock(m_mutexMapPool);
	for (auto& itermap : m_mapPool)
	{
		pairRet.first += itermap.second->getAvailableMemorySize();
		pairRet.second += itermap.second->getTotleMemorySize();
	}
	return pairRet;
}

void SmallAllocator::releaseFreeMemory()
{
	for (auto& iterVec : m_vecPool)
	{
		iterVec.releaseFreeMemory();
	}

	std::lock_guard<std::mutex> lock(m_mutexMapPool);
	for (auto& itermap : m_mapPool)
	{
		itermap.second->releaseFreeMemory();
	}
}

FixedAllocator* SmallAllocator::getBigFixAllocator(size_t allocSize)
{
	FixedAllocator* ptrRet = nullptr;

	std::lock_guard<std::mutex> lock(m_mutexMapPool);
	auto iterPool = m_mapPool.find(allocSize);
	if (iterPool != m_mapPool.end())
	{
		ptrRet = iterPool->second.get();
	}
	else
	{
		auto ptrNew = std::make_unique<FixedAllocator>(allocSize);
		ptrRet = ptrNew.get();
		m_mapPool[allocSize] = std::move(ptrNew);
	}
	return ptrRet;
}