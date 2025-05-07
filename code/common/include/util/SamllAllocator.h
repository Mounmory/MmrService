#ifndef MMR_UTIL_SMALL_ALLOCATOR_H
#define MMR_UTIL_SMALL_ALLOCATOR_H
#include <common/include/util/UtilExport.h>
#include <common/include/general/Singleton.hpp>
#include <common/include/general/Noncopyable.hpp>
#include <common/include/general/VarTypeDict.hpp>

#include <memory>
#include <atomic>
#include <cstddef>
#include <cassert> 
#include <stdexcept>
#include <iostream>
#include <vector>
#include <climits>
#include <mutex>
#include <type_traits>

#ifndef DEFAULT_CHUNK_SIZE
#define DEFAULT_CHUNK_SIZE 4096
#endif

BEGINE_NAMESPACE(mmrUtil)

template <typename T>
constexpr size_t aligned_size() {
	// 计算对齐后的总大小（向上取整到对齐值的倍数）
	return ((sizeof(T) + alignof(T) - 1) / alignof(T)) * alignof(T);
}

class ChunkLockFree //无锁的内存分配器
{

public:

	ChunkLockFree()
		: m_pData(nullptr)
		, m_availaBlockCurr(0)
		, m_availaBlockSize(0)
		, m_endBlock(0)
	{ }

	ChunkLockFree(ChunkLockFree&& rhs)
		: m_pData(std::exchange(rhs.m_pData,nullptr))
		, m_availaBlockCurr(rhs.m_availaBlockCurr.exchange(0))
		, m_availaBlockSize(rhs.m_availaBlockSize.exchange(0))
		, m_endBlock(std::exchange(rhs.m_endBlock, 0))
	{ }

	ChunkLockFree& operator = (ChunkLockFree&& rhs)
	{
		m_pData = std::exchange(rhs.m_pData, nullptr);
		m_availaBlockCurr.store(rhs.m_availaBlockCurr.exchange(0));
		m_availaBlockSize.store(rhs.m_availaBlockSize.exchange(0));
		m_endBlock = std::exchange(rhs.m_endBlock, 0);
	}
	ChunkLockFree(const ChunkLockFree&) = delete;

	ChunkLockFree& operator = (const ChunkLockFree&) = delete;

	~ChunkLockFree()
	{
		Release();
	}

	void Init(std::size_t blockSize, uint8_t blocks)//要分配的内存块大小，块数量
	{
		assert(blockSize > 0);
		assert(blocks > 0);
		assert((blockSize * blocks) / blockSize == blocks);// 溢出检查

		m_pData = new uint8_t[blockSize * blocks];
		m_availaBlockCurr = 0;
		m_availaBlockSize.store(blocks, std::memory_order_relaxed);
		m_endBlock = blocks;//最后一个节点指向位置值
		uint8_t* p = m_pData;
		for (uint8_t i = 0; i != blocks; p += blockSize)
		{
			*p = ++i;//初始化下一个节点位置值
		}
	}

	void* Allocate(std::size_t blockSize) 
	{
		uint8_t current_avail = m_availaBlockCurr.load(std::memory_order_acquire);
		uint8_t next_avail;
		uint8_t* ptrRet = nullptr;

		do {
			if (current_avail == m_endBlock)//当前
			{
				ptrRet = nullptr;
				break;
			}

			// 计算新值
			ptrRet = m_pData + (current_avail * blockSize);
			next_avail = *ptrRet;
			// 尝试原子更新
		} while (!m_availaBlockCurr.compare_exchange_weak(current_avail, next_avail,
				std::memory_order_release,
				std::memory_order_relaxed));

		if (nullptr != ptrRet)
		{
			m_availaBlockSize.fetch_sub(1, std::memory_order_relaxed);//可用
		}

		return ptrRet;
	}

	void Deallocate(void* p, std::size_t blockSize) 
	{
		assert(p >= m_pData);
		uint8_t* toRelease = static_cast<uint8_t*>(p);
		assert((toRelease - m_pData) % blockSize == 0);
		uint8_t block_index = static_cast<uint8_t>((toRelease - m_pData) / blockSize);//内存块索引
		*toRelease = m_availaBlockCurr.load(std::memory_order_acquire);

		 while (!m_availaBlockCurr.compare_exchange_weak(
			*toRelease, block_index,
				std::memory_order_release,
				std::memory_order_relaxed));

		m_availaBlockSize.fetch_add(1, std::memory_order_relaxed);
	}

	void Release() 
	{
		if (nullptr != m_pData) 
		{
			if (m_availaBlockSize.load(std::memory_order_relaxed) != m_endBlock)
			{
				throw std::runtime_error("unreturned memory exist!");//有待归还的内存，清掉很危险
			}
			m_availaBlockSize.store(0, std::memory_order_relaxed);
			m_availaBlockCurr.store(0, std::memory_order_relaxed);
			m_endBlock = 0;
			delete[] m_pData;
			m_pData = nullptr;
			
		}
	}

	const uint8_t AvailadeBlockSize() const { return m_availaBlockSize.load(std::memory_order_relaxed); }
private:
	uint8_t* m_pData;//大块内存首地址
	std::atomic<uint8_t> m_availaBlockCurr;//当前可用内存索引
	std::atomic<uint8_t> m_availaBlockSize;//剩余可用内存块数量
	uint8_t m_endBlock;//结束点指向索引
};

//template<typename TChunk>//模板参数，大块内存类型
class FixedAllocator 
{
	//using ChunkType = TChunk;
	using ChunkType = ChunkLockFree;
public:
	explicit FixedAllocator(std::size_t blockSize) //负责分配的内存大小
		: m_blockSize(blockSize)
		, m_ptrAllocChunk(nullptr)
	{
		assert(m_blockSize > 0);
		std::size_t numBlocks = DEFAULT_CHUNK_SIZE / blockSize;

		if (numBlocks > UCHAR_MAX) 
			numBlocks = UCHAR_MAX;
		else if (numBlocks == 0) 
			numBlocks = 8 * blockSize;

		m_numBlocks = static_cast<unsigned char>(numBlocks);
		assert(m_numBlocks == numBlocks);
	}
	FixedAllocator(const FixedAllocator&) = delete;

	FixedAllocator(FixedAllocator&& rhs) 
		: m_blockSize(rhs.m_blockSize)
		, m_numBlocks(std::exchange(rhs.m_numBlocks, 0))
		, m_vecChunks(std::move(rhs.m_vecChunks))
		, m_ptrAllocChunk(std::exchange(rhs.m_ptrAllocChunk, nullptr))
	{
	}

	~FixedAllocator() 
	{
	}

	template<typename Type, typename... Args>
	std::shared_ptr<Type> AllocateAndContructData(Args&&... args)
	{
		assert(sizeof(Type) <= m_blockSize);

		auto pairData = Allocate();
		new (pairData.first)Type(std::forward<Args>(args)...);//调用构造函数
		return std::static_pointer_cast<Type>(std::shared_ptr<void>(pairData.first,
			[=](void* ptr)
		{
			reinterpret_cast<Type*>(ptr)->~Type();//调用析构
			pairData.second->Deallocate(ptr, m_blockSize);
		}));
	}

	//用于分配的块大小
	std::size_t AllocBlockSize() const { return m_blockSize; }

	//FixAllocator总大小
	std::size_t GetTotleMemorySize() const { return m_vecChunks.size() * m_blockSize * m_numBlocks; }

	//获取可用内存大小
	const std::size_t GetAvailableMemorySize()
	{ 
		std::size_t memCount = 0;
		std::lock_guard<std::mutex> lock(m_mutexChunks);
		for (const auto& iterChunk : m_vecChunks) 
		{
			memCount += m_blockSize * iterChunk.AvailadeBlockSize();
		}
		return memCount;
	}

	//释放闲置内存
	void ReleaseFreeMemory() 
	{
		//这里有删除chunk，锁住后在Allocate里就不会改变m_ptrAllocChunk了，能保证m_ptrAllocChunk指针一直有效
		std::lock_guard<std::mutex> lock(m_mutexChunks);
		for (auto iterChunk = m_vecChunks.begin(); iterChunk != m_vecChunks.end();)
		{
			if (iterChunk->AvailadeBlockSize() == m_numBlocks && &(*iterChunk) != m_ptrAllocChunk)
			{
				iterChunk = m_vecChunks.erase(iterChunk);
			}
			else 
			{
				++iterChunk;
			}
		}
	}

	// 用于比较块大小
	bool operator<(std::size_t rhs) const { return AllocBlockSize() < rhs; }

private:
	std::pair<void*, ChunkType*> Allocate()//返回分配的内存地址，属于哪个chunk
	{
		std::pair<void*, ChunkType*> pairRet = { nullptr, nullptr };
		do
		{
			if (nullptr == m_ptrAllocChunk || m_ptrAllocChunk->AvailadeBlockSize() == 0)
			{
				std::lock_guard<std::mutex> lock(m_mutexChunks);//这个锁下面只添加chunk，不会导致m_ptrAllocChunk失效

				auto iterChunk = m_vecChunks.begin();//先行查找一个可用的chunk
				for (; ; ++iterChunk)
				{
					if (iterChunk == m_vecChunks.end())
					{
						ChunkType newChunk;
						newChunk.Init(m_blockSize, m_numBlocks);
						m_vecChunks.emplace_back(std::move(newChunk));
						m_ptrAllocChunk = &m_vecChunks.back();
						break;
					}
					if (iterChunk->AvailadeBlockSize() > 0)
					{
						m_ptrAllocChunk = &(*iterChunk);
						break;
					}
				}
			}
			pairRet.second = m_ptrAllocChunk;
			pairRet.first = pairRet.second->Allocate(m_blockSize);

		} while (nullptr == pairRet.first);

		return pairRet;
	}

private:
	const std::size_t m_blockSize;//用于每次分配m_blockSize大小的内存
	uint8_t m_numBlocks;//每个chunk中的内存数量

	std::vector<ChunkType> m_vecChunks;//所有的内存块
	std::mutex m_mutexChunks;
	ChunkType* m_ptrAllocChunk;//当前正在分配内存的chunk
};


class SmallAllocator : public mmrComm::NonCopyable
{
	friend class mmrComm::Singleton<mmrUtil::SmallAllocator>;

	 static constexpr size_t _MaxObjectSize = 1024;//所能分配的最大内存大小
	 static constexpr size_t _StepSize = sizeof(void*);//内存池分配步长，考虑为对象分配的内存地址对齐
	 static constexpr size_t _MaxIndex = _MaxObjectSize / _StepSize;
	//缓存清理过期时间（Min），最大缓存数（Mb）
	SmallAllocator() 
	{
		assert((_MaxObjectSize %_StepSize) == 0);
		m_vecPool.reserve(_MaxIndex);
		for (size_t i = 1; i <= _MaxIndex; i++)
		{
			m_vecPool.emplace_back(FixedAllocator(i*_StepSize));
		}
	}

public:
	~SmallAllocator() 
	{
	}

	template<typename Type, typename... Args>
	std::shared_ptr<Type> Make_Shared(Args&&... args)
	{
		static constexpr size_t allocIndex = ((sizeof(Type) + _StepSize - 1) / _StepSize) -1;
		static_assert(allocIndex <= _MaxIndex, "Data type should less than max object size.");

		std::cout << "size of " << sizeof(Type) << std::endl;
		std::cout << "alloc index of " << allocIndex << std::endl;
		return m_vecPool[allocIndex].AllocateAndContructData<Type>(std::forward<Args>(args)...);
	}

private:
	std::vector<FixedAllocator> m_vecPool;
};

END_NAMESPACE(mmrUtil)

#endif