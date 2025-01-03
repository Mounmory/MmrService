#ifndef EVENTCALLBACKS_H
#define EVENTCALLBACKS_H

#include "Common_def.h"
#include "util/CVarDatas.hpp"

#include <atomic>
#include <future>
#include <string>
#include <functional>

/*
* 处理全局事件的回调
*/

//回调函数
using CallbackFunc = std::function<void(const mmrUtil::CVarDatas&)>;

using ptrCallbackFunc = std::shared_ptr<CallbackFunc>;

BEGINE_NAMESPACE(mmrService)
BEGINE_NAMESPACE(mmrCore)

class CEventDealWithLock
{
public:
	CEventDealWithLock();

	~CEventDealWithLock();

	CEventDealWithLock(const CEventDealWithLock& rhs) = delete;

	void start();

	void stop();

	//处理事件相关接口
	void addFunc(const std::string& strTopcs, const std::shared_ptr<CallbackFunc>& ptrFunc);

	void addEvenVartData(mmrUtil::CVarDatas&& varData);
private:
	void dealEventData();

private:
	struct CImpData;
	std::unique_ptr<CImpData> m_data;

	std::atomic_bool m_bTheadRun;
	std::future<void> m_future;
};

END_NAMESPACE(mmrCore)
END_NAMESPACE(mmrService)

#endif