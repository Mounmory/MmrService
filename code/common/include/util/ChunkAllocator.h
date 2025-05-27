/**
 * @file ChunkAllocator.h
 * @brief 大块内存分配器类
 * @author Mounmory (237628106@qq.com) https://github.com/Mounmory
 * @date 
 *
 * 
 */

#ifndef MMR_UTIL_ALLOCATOR_H
#define MMR_UTIL_ALLOCATOR_H
#include <common/include/util/UtilExport.h>
#include <common/include/general/Singleton.hpp>
#include <common/include/general/Noncopyable.hpp>
#include <common/include/general/VarTypeDict.hpp>

#include <memory>
#include <atomic>

BEGINE_NAMESPACE(mmrUtil)

struct TagCAExpiredTime; struct TagCAMaxCache;

using ChunkAllocParas = mmrComm::VarTypeDict<struct TagCAExpiredTime, struct TagCAMaxCache>;

template<size_t Align = 10>//分配对齐，可分配最小的内存为2^Align
class COMMON_CLASS_API ChunkAllocator : public mmrComm::NonCopyable
{
	static_assert(Align <= 16,"min allocate memory size should larger than 2 ^16 byte.");

	static constexpr uint32_t minBlockSize = (uint32_t(1) << Align) -1;

	friend class mmrComm::Singleton<mmrUtil::ChunkAllocator<Align>>;

	//缓存清理过期时间（Min），最大缓存数（Mb）
	ChunkAllocator(uint32_t ulExpiredTime = 60, uint32_t ulMaxCache = 64);

	template <typename TIn>
	ChunkAllocator(TIn&& in) : ChunkAllocator(in.template Get<TagCAExpiredTime>(), in.template Get<TagCAMaxCache>())
	{
	}
public:
	~ChunkAllocator();

	//获取最大缓存
	uint32_t getMaxCacheSize() const { return m_usMaxFreeCacheSize; }

	//分配指向void指针内存
	template<typename T>
	std::pair<uint32_t, std::shared_ptr<T>> allocate(uint32_t ulElemSize)
	{
		static_assert(std::is_same<mmrComm::RemConstRef<T>, T>::value, "type should not be with const or reference!");
		static_assert(std::is_arithmetic<T>::value, "type should be arithmetic!");

		std::pair<uint32_t, std::shared_ptr<T>> pairRet = { 0,nullptr };
		pairRet.first = (ulElemSize * sizeof(T) + minBlockSize) & (uint32_t(-1) ^ minBlockSize);//最少分配1024Byte内存，且为1024整数倍
		pairRet.second = std::static_pointer_cast<T>(alloMemory(pairRet.first));
		//std::cout << "allocate use count " << pairRet.second.use_count() << std::endl;
		return pairRet;
	}

	//释放过期内存
	void freeExpiredMemory();

	//处理定时器
	void onTimer(int64_t framTime);

	//处理命令行控制命令
	void loop(std::atomic_bool& bRunFlag);

	//获取内存使用情况<free/total>
	std::pair<size_t, size_t> getAvailableMemoryInfo() const 
	{ 
		return { m_sizeFree.load(std::memory_order_relaxed),m_sizeTotal.load(std::memory_order_relaxed) };
	}
private:
	//分配指向void智能指针内存
	std::shared_ptr<void> alloMemory(uint32_t ulElemSize);

	//分配原始指针内存
	void* alloRowMemory(uint32_t ulElemSize);

	//归还内存
	void deallocate(uint32_t ulBufSize, void* pBuf);

private:
	const uint32_t m_ulExpireTime;//过期时间，单位min,默认60min
	const uint32_t m_usMaxFreeCacheSize;//最大内存大小,默认4M

	std::atomic<uint32_t> m_sizeFree;//空闲内存，单位byte
	std::atomic<uint32_t> m_sizeTotal;//总的内存，单位byte
private:
	struct DataImp;
	std::unique_ptr<DataImp> m_ptrData;
};
template class COMMON_CLASS_API mmrUtil::ChunkAllocator<10>;
template class COMMON_CLASS_API mmrUtil::ChunkAllocator<9>;
template class COMMON_CLASS_API mmrUtil::ChunkAllocator<8>;

template class COMMON_CLASS_API mmrComm::Singleton<mmrUtil::ChunkAllocator<10>>;//最小分配内存1024
template class COMMON_CLASS_API mmrComm::Singleton<mmrUtil::ChunkAllocator<9>>;
template class COMMON_CLASS_API mmrComm::Singleton<mmrUtil::ChunkAllocator<8>>;
END_NAMESPACE(mmrUtil)


#endif
