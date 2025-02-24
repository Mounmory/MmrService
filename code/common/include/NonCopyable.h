#ifndef COMMON_NOCOPYABLE_H
#define COMMON_NOCOPYABLE_H

#include "common/include/Common_def.h"

BEGINE_NAMESPACE(mmrComm)

class NonCopyable
{
public:
	NonCopyable(const NonCopyable&) = delete;
	NonCopyable operator=(const NonCopyable&) = delete;

protected:
	NonCopyable() = default;
	~NonCopyable() = default;
};

END_NAMESPACE(mmrComm)
#endif  // COMMON_NOCOPYABLE_H
