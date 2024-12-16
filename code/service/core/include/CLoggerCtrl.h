#ifndef CLOGGERCTRL_H
#define CLOGGERCTRL_H
#include <memory>
#include <atomic>
#include <map>
#include "ComponentExport.h"
#include "util/Clogger.h"

BEGINE_NAMESPACE(mmrService)
BEGINE_NAMESPACE(mmrCore)

/*
	组件日志等级控制类
	保存各个组件日志实例指针，动态修改实例中的日志等级
*/

class CLoggerCtrl
{
	using MapLogCtrl = std::map<int16_t, std::pair<std::string, std::weak_ptr<mmrUtil::LogWrapper>>>;
public:
	CLoggerCtrl();
	~CLoggerCtrl();

	MapLogCtrl& getMapCtrollerPtr() { return m_mapCompLogger; }

	void loop(std::atomic_bool& bRunFlag);
private:
	MapLogCtrl m_mapCompLogger;//所有控件日志组件
};

END_NAMESPACE(mmrCore)
END_NAMESPACE(mmrService)
#endif