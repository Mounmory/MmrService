/**
 * @file SamllAllocator.h
 * @brief 内存分配器,用于构造智能指针对象
 * @author Mounmory (237628106@qq.com) https://github.com/Mounmory
 * @date 
 *
 * 
 */

#ifndef MMR_UTIL_SMALL_ALLOCATOR_H
#define MMR_UTIL_SMALL_ALLOCATOR_H

#include <common/include/util/UtilExport.h>
#include <common/include/general/Singleton.hpp>
#include <common/include/general/Noncopyable.hpp>
#include <common/include/general/VarTypeDict.hpp>

#include <type_traits>
#include <memory>
#include <atomic>
#include <vector>
#include <list>
#include <mutex>
#include <unordered_map>
#include <cassert> 


#define ALLOC_CHUNK_SIZE 8192

#define MAX_OBJECT_SIZE 1024

BEGINE_NAMESPACE(mmrUtil)

/*
	固定内存分配器，用于分配指定大小的内存
*/
class COMMON_CLASS_API FixedAllocator
{
	class COMMON_CLASS_API ChunkLockFree : public mmrComm::NonCopyable //无锁的内存分配块
	{
		/*
			一大块内存，用于分配小块内存，每个实例可分配指定大小的内存，在init时设置参数
			每个块可分配最多255块内存
		*/
	public:
		ChunkLockFree();
		~ChunkLockFree();

		//要分配的内存块大小，块数量
		void init(std::size_t blockSize, uint8_t blockNum);

		//分配内存
		void* allocate();

		//归还内存
		void deallocate(void* ptr);

		//释放内存，请确保执行前所有分配内存已归还，否则会抛出异常
		void release();

		//可用的内存块数量
		const uint8_t availadeBlockNum() const { return m_availaBlockNum.load(std::memory_order_relaxed); }

		//总内存块数量
		const uint8_t blockNum() const { return m_blockNum; }
	private:
		uint8_t* m_pData;//大块内存首地址
		size_t m_blockSize; //每次分配的内存大小
		std::atomic<uint8_t> m_availaBlockCurr;//当前可用内存索引
		std::atomic<uint8_t> m_availaBlockNum;//剩余可用内存块数量
		uint8_t m_blockNum;//结束点指向索引
	};

	using ChunkType = ChunkLockFree;
public:
	explicit FixedAllocator(std::size_t blockSize);//blockSize：负责分配的内存的大小
	FixedAllocator(const FixedAllocator&) = delete;
	FixedAllocator(FixedAllocator&& rhs);
	~FixedAllocator() = default;

	FixedAllocator& operator = (const FixedAllocator&) = delete;
	FixedAllocator& operator = (FixedAllocator&&) = delete;

	//分配内存并构造共享智能指针对象
	template<typename Type, typename... Args>
	std::shared_ptr<Type> Make_Shared(Args&&... args)
	{
		assert(sizeof(Type) <= m_blockSize);

		auto pairData = allocate();
		new (pairData.first)Type(std::forward<Args>(args)...);//调用构造函数
		return std::shared_ptr<Type>(reinterpret_cast<Type*>(pairData.first),
			[=](Type* ptr)
		{	ptr->~Type();//调用析构
			pairData.second->deallocate(ptr);
		});
	}

	//分配内存并构造独享智能指针对象
	template<typename Type, typename... Args>
	std::unique_ptr<Type, std::function<void(Type*)>> Make_Unique(Args&&... args)
	{
		assert(sizeof(Type) <= m_blockSize);
		auto pairData = allocate();
		new (pairData.first)Type(std::forward<Args>(args)...);//调用构造函数
		return std::unique_ptr<Type, std::function<void(Type*)>>(reinterpret_cast<Type*>(pairData.first), 
			[=](Type* ptr)
		{
			ptr->~Type();//调用析构
			pairData.second->deallocate(ptr);
		});
	}

	//FixAllocator总大小
	std::size_t getTotleMemorySize() const { return m_listChunks.size() * m_blockSize * m_numBlocks; }

	//获取可用内存大小
	std::size_t getAvailableMemorySize();

	//释放闲置内存块
	void releaseFreeMemory();

private:
	//使用内部内存块分配内存，返回<分配的内存地址，属于哪个chunk指针>
	std::pair<void*, ChunkType*> allocate();
	
private:
	const std::size_t m_blockSize;//用于每次分配m_blockSize大小的内存
	uint8_t m_numBlocks;//每个chunk中的内存数量
	ChunkType* m_ptrAllocChunk;//当前正在分配内存的chunk

	std::mutex m_mutexChunks;
	std::list<std::unique_ptr<ChunkType>> m_listChunks;//所有的chunk内存块
};

struct TagSAExpiredTime; struct TagSAMaxCache;

using SmallAllocParas = mmrComm::VarTypeDict<struct TagSAExpiredTime, struct TagSAMaxCache>;

class COMMON_CLASS_API SmallAllocator : public mmrComm::NonCopyable
{
	friend class mmrComm::Singleton<mmrUtil::SmallAllocator>;

	 static constexpr size_t _MaxObjectSize = MAX_OBJECT_SIZE;//所能分配的最大内存大小
	 static constexpr size_t _StepSize = sizeof(void*);//内存池分配步长，考虑为对象分配的内存地址对齐
	 static constexpr size_t _MaxIndex = _MaxObjectSize / _StepSize;//内存池中FixAllocator最大数量
	//缓存清理过期时间（Min），最大缓存数（Mb）
	 SmallAllocator(uint32_t ulExpiredTime = 60, uint32_t ulMaxCache = 512);

	 template <typename TIn>
	 SmallAllocator(TIn&& in) : SmallAllocator(in.template Get<TagSAExpiredTime>(), in.template Get<TagSAMaxCache>())
	 {
	 }
public:
	~SmallAllocator();

	//为小于_MaxObjectSize的类分配内存，返回std::shared_ptr
	template<typename Type, typename... Args, typename std::enable_if<(sizeof(Type) <= _MaxObjectSize) >::type* = nullptr>
	std::shared_ptr<Type> Make_Shared(Args&&... args)
	{
		static constexpr size_t allocIndex = ((sizeof(Type) + _StepSize - 1) / _StepSize) -1;
		static_assert(allocIndex < _MaxIndex, "Data type should less than max object index.");
		return m_vecPool[allocIndex].Make_Shared<Type>(std::forward<Args>(args)...);
	}

	//为大于_MaxObjectSize的类分配内存
	template<typename Type, typename... Args, typename std::enable_if<(sizeof(Type) > _MaxObjectSize) >::type* = nullptr>
	std::shared_ptr<Type> Make_Shared(Args&&... args)
	{
		static_assert(sizeof(Type) <= ALLOC_CHUNK_SIZE, "Type size is too big!");
		//使用标准库中make_shared
		//return std::make_shared<Type>(std::forward<Args>(args)...);

		//对于用的较少的使用内存较大的类型，使用map保存内存分配器
		static constexpr size_t allocSize = (sizeof(Type) + _StepSize - 1) / _StepSize * _StepSize;//为了内存对齐
		static FixedAllocator* ptrFixAlloc = nullptr;
		if (nullptr == ptrFixAlloc)
		{
			ptrFixAlloc = getBigFixAllocator(allocSize);
		}
		return ptrFixAlloc->Make_Shared<Type>(std::forward<Args>(args)...);
	}

	//为小于_MaxObjectSize的类分配内存，返回std::unique_ptr
	template<typename Type, typename... Args, typename std::enable_if<(sizeof(Type) <= _MaxObjectSize) >::type* = nullptr>
	std::unique_ptr<Type, std::function<void(Type*)>> Make_Unique(Args&&... args)
	{
		static constexpr size_t allocIndex = ((sizeof(Type) + _StepSize - 1) / _StepSize) - 1;
		static_assert(allocIndex < _MaxIndex, "Data type should less than max object index.");
		return m_vecPool[allocIndex].Make_Unique<Type>(std::forward<Args>(args)...);
	}

	//大于_MaxObjectSize
	template<typename Type, typename... Args, typename std::enable_if<(sizeof(Type) > _MaxObjectSize) >::type* = nullptr>
	std::unique_ptr<Type, std::function<void(Type*)>> Make_Unique(Args&&... args)
	{
		static_assert(sizeof(Type) <= ALLOC_CHUNK_SIZE, "Type size is too big!");
		//如果确定使用标准库中为大对象分配内存，屏蔽下面的断言代码即可
		//return std::unique_ptr<Type, std::function<void(Type*)>>(new Type(std::forward<Args>(args)...), [](Type* ptr) {delete ptr; });
	
		//对于用的较少的使用内存较大的类型，使用map保存内存分配器
		static constexpr size_t allocSize = (sizeof(Type) + _StepSize - 1) / _StepSize * _StepSize;//为了内存对齐
		static FixedAllocator* ptrFixAlloc = nullptr;
		if (nullptr == ptrFixAlloc)
		{
			ptrFixAlloc = getBigFixAllocator(allocSize);
		}
		return ptrFixAlloc->Make_Unique<Type>(std::forward<Args>(args)...);
	}

	//处理定时器
	void onTimer(int64_t framTime);

	//处理命令行控制命令
	void loop(std::atomic_bool& bRunFlag);

	//获取内存使用情况<free/total>
	std::pair<size_t, size_t> getAvailableMemoryInfo();

private:
	void releaseFreeMemory();

	FixedAllocator* getBigFixAllocator(size_t allocSize);
private:
	const uint32_t m_ulExpireTime;//过期时间，单位min,默认60min
	const uint32_t m_usMaxFreeCacheSize;//最大内存大小,默认4M

	std::vector<FixedAllocator> m_vecPool;

	//对于大于1024KB的对象分配内存用
	std::mutex m_mutexMapPool;//大对象内存分配互斥锁
	std::unordered_map<size_t, std::unique_ptr<FixedAllocator>> m_mapPool;
};

//导出SmallAllocator模板实例
template class COMMON_CLASS_API mmrComm::Singleton<mmrUtil::SmallAllocator>;

END_NAMESPACE(mmrUtil)

#endif
