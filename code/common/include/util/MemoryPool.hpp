/**
 * @file MemoryPool.hpp
 * @brief 定义大块内存和小块内存分配器使用接口
 * @author Mounmory (237628106@qq.com) https://github.com/Mounmory
 * @date 
 *
 * 
 */
 
#ifndef MMR_UTIL_MEMORY_POOL_HPP
#define MMR_UTIL_MEMORY_POOL_HPP
#include <common/include/util/ChunkAllocator.h>
#include <common/include/util/SmallAllocator.h>
 
BEGINE_NAMESPACE(mmrUtil)


namespace
{
	template<typename Type, typename... Args>
	std::shared_ptr<Type> Make_Shared(Args&&... args)
	{
		return mmrComm::Singleton<mmrUtil::SmallAllocator>::getInstance()->Make_Shared<Type>(std::forward<Args>(args)...);
	}

	template<typename Type, typename... Args>
	std::unique_ptr<Type, std::function<void(Type*)>> Make_Unique(Args&&... args)
	{
		return mmrComm::Singleton<mmrUtil::SmallAllocator>::getInstance()->Make_Unique<Type>(std::forward<Args>(args)...);
	}
}

END_NAMESPACE(mmrUtil)
 #endif //MMR_UTIL_MEMORY_POOL_HPP