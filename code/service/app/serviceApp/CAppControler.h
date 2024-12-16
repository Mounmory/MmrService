#ifndef CAPPCONTROLER_H
#define CAPPCONTROLER_H
#include <mutex>
#include <condition_variable>
#include <atomic>

#include "interface/IEventHandler.h"

BEGINE_NAMESPACE(mmrService)
BEGINE_NAMESPACE(mmrApp)

class CAppControler :public IEventHandler
{
public:
	CAppControler();
	~CAppControler();

	void run();

	void dealCmd();

	virtual void handleEvent(const mmrUtil::CVarDatas& varData) override;
private:
	std::atomic_bool		m_bRunflag;
	std::mutex              m_mutex;
	std::condition_variable m_cond;
};

END_NAMESPACE(mmrService)
END_NAMESPACE(mmrApp)
#endif