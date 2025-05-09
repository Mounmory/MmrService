/**
 * @file MakeShared.hpp
 * @brief 使用内存池生成智能指针对象
 * @author Mounmory (237628106@qq.com) https://github.com/Mounmory
 * @date 
 *
 * 
 */

#ifndef MMR_UTILE_MAKE_SHARED_HPP
#define MMR_UTILE_MAKE_SHARED_HPP
#include <common/include/util/SamllAllocator.h>

BEGINE_NAMESPACE(mmrUtil)

namespace
{
	template<typename Type, typename... Args, typename std::enable_if<sizeof(Type) <= MAX_OBJECT_SIZE>::type* = nullptr>
	std::shared_ptr<Type> Make_Shared(Args&&... args)
	{
		return mmrComm::Singleton<mmrUtil::SmallAllocator>::getInstance()->Make_Shared<Type>(std::forward<Args>(args)...);
	}




}

END_NAMESPACE(mmrUtil)

#endif
