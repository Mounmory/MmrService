#ifndef MMR_UTIL_ALLOCATOR_H
#define MMR_UTIL_ALLOCATOR_H
#include <common/include/util/UtilExport.h>
#include <common/include/general/Singleton.hpp>
#include <common/include/general/Noncopyable.hpp>
#include <common/include/general/VarTypeDict.hpp>

#include <memory>
//#include <iostream>

/*
	大块内存分配器类
*/


BEGINE_NAMESPACE(mmrUtil)

struct TagExpiredTime; struct TagMaxCache;

using ChunkAllocParas = mmrComm::VarTypeDict<struct TagExpiredTime, struct TagMaxCache>;

class COMMON_CLASS_API ChunkAllocator : public mmrComm::NonCopyable
{
	friend class mmrComm::Singleton<ChunkAllocator>;

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
		pairRet.first = (ulElemSize * sizeof(T) + 1023) & (uint32_t(-1) ^ 1023);//最少分配1024Byte内存，且为1024整数倍
		pairRet.second = std::static_pointer_cast<T>(alloMemory(pairRet.first));
		//std::cout << "allocate use count " << pairRet.second.use_count() << std::endl;
		return pairRet;
	}

private:
	std::shared_ptr<void> alloMemory(uint32_t ulElemSize);

	void deallocate(uint32_t ulBufSize, void* pBuf);

private:
	uint32_t m_ulCacheSize;//内存大小
	uint32_t m_ulExpireTime;//过期时间，单位min,默认60min
	uint32_t m_usMaxCacheSize;//最大内存大小,默认4M

private:
	struct DataImp;
	std::unique_ptr<DataImp> m_ptrData;
};

template class COMMON_CLASS_API ::mmrComm::Singleton<mmrUtil::ChunkAllocator>;

END_NAMESPACE(mmrUtil)


#endif
