#ifndef MMR_UTILE_MAKE_SHARED_HPP
#define MMR_UTILE_MAKE_SHARED_HPP
#include <common/include/util/SamllAllocator.h>

BEGINE_NAMESPACE(mmrUtil)

namespace
{
	template<typename Type, typename... Args>
	std::shared_ptr<Type> Make_Shared(Args&&... args)
	{
		return mmrComm::Singleton<mmrUtil::SmallAllocator>::getInstance()->Make_Shared<Type>(std::forward<Args>(args)...);
	}




}

END_NAMESPACE(mmrUtil)

#endif