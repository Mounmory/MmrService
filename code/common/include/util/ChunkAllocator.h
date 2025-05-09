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

BEGINE_NAMESPACE(mmrUtil)

struct TagExpiredTime; struct TagMaxCache;

using ChunkAllocParas = mmrComm::VarTypeDict<struct TagExpiredTime, struct TagMaxCache>;

template<size_t Align = 10>//分配对齐，可分配最小的内存为2^Align
class COMMON_CLASS_API ChunkAllocator : public mmrComm::NonCopyable
{
	static_assert(Align <= 16,"min allocate memory size should larger than 2 ^16 byte.");

	static constexpr uint32_t minBlockSize = (uint32_t(1) << Align) -1;

	friend class mmrComm::Singleton<mmrUtil::ChunkAllocator<Align>>;

	//缓存清理过期时间（Min），最大缓存数（Mb）
	ChunkAllocator(uint32_t ulExpiredTime = 60, uint32_t ulMaxCache = 64);

	template <typename TIn>
	ChunkAllocator(TIn&& in) : ChunkAllocator(in.template Get<TagExpiredTime>(), in.template Get<TagMaxCache>())
	{
	}
public:
	~ChunkAllocator();

	//获取最大缓存
	uint32_t getMaxCacheSize() const { return m_usMaxCacheSize; }

	//获取已缓存大小
	uint32_t getCacheSize() const { return m_ulCacheSize; }

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
	void FreeExpiredMemory();
private:
	std::shared_ptr<void> alloMemory(uint32_t ulElemSize);

	void * alloRowMemory(uint32_t ulElemSize);

	void deallocate(uint32_t ulBufSize, void* pBuf);

private:
	uint32_t m_ulCacheSize;//内存大小
	uint32_t m_ulExpireTime;//过期时间，单位min,默认60min
	uint32_t m_usMaxCacheSize;//最大内存大小,默认4M

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
