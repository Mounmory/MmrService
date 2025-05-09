/**
 * @file CLoggerCtrl.h
 * @brief 各模块日志处理类，用于日志模块控制级别
 * @author Mounmory (237628106@qq.com) https://github.com/Mounmory
 * @date 
 *
 * 
 */

#ifndef CLOGGERCTRL_H
#define CLOGGERCTRL_H
#include <service/interface/ComponentExport.h>
#include <common/include/util/Clogger.h>

#include <memory>
#include <atomic>
#include <map>


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
