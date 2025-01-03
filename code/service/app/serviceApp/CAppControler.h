#ifndef CAPPCONTROLER_H
#define CAPPCONTROLER_H
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <list>
#include <functional>

//#include "interface/IEventHandler.h"

#include "util/CVarDatas.hpp"

BEGINE_NAMESPACE(mmrService)
BEGINE_NAMESPACE(mmrApp)

class CAppControler// :public IEventHandler
{
public:
	CAppControler();
	~CAppControler();

	void run();

	void dealCmd();

	void handleEvent(const mmrUtil::CVarDatas& varData);

private:
	std::atomic_bool		m_bRunflag;
	std::mutex              m_mutex;
	std::condition_variable m_cond;

	using CallbackFunc = std::function<void(const mmrUtil::CVarDatas&)>;
	std::list<std::shared_ptr<CallbackFunc>> m_listCallbackFun;
};

END_NAMESPACE(mmrService)
END_NAMESPACE(mmrApp)
#endif