#ifndef IEVENTHANDLER
#define IEVENTHANDLER
#include "common/include/util/CVarDatas.hpp"
//#include <memory>

/*
* 头文件废弃，不再使用了 2025.02.11
*/

/*
class IEventHandler : public std::enable_shared_from_this<IEventHandler>
{
public:
	IEventHandler() = default;
	virtual ~IEventHandler() = default;

	virtual void handleEvent(const mmrUtil::CVarDatas& varData) = 0;

	virtual void registTopic(const std::initializer_list<std::string>& listTopcs) = 0;

};

#define  HANDLER_REGIST_TOPICS(vecStr)\
{\
	std::vector<std::string> vecTopic = vecStr;\
	for (const auto& iterTopic : vecTopic)\
	{\
		CoreFrameworkIns->addHandler(iterTopic, this);\
	}\
}

#define  HANDLER_REMOVE_TOPICS(topics)\
{\
	std::vector<std::string> vecTopic({ topics });\
	for (const auto& iterTopic : vecTopic)\
	{\
		CoreFrameworkIns->removeHandler(iterTopic, this);\
	}\
}

*/
#endif // !IEVENTHANDLER


