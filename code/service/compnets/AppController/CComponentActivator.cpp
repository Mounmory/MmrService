#include "CComponentActivator.h"
#include "CCompFramework.h"
#include "CCmdService.h"
//#include "util/Clogger.h"


REGIST_COMPONENT(CComponentActivator);

using namespace mmrService::mmrComp;

std::shared_ptr<mmrUtil::LogWrapper> g_LoggerPtr = nullptr;

CComponentActivator::CComponentActivator()
{
	
}

const char* CComponentActivator::getName()
{
	static const char* szName = "AppController";
	return szName;
}

bool CComponentActivator::initialise(const Json::Value& jsonConfig)
{
	//日志设置
	g_LoggerPtr = std::make_shared<mmrUtil::LogWrapper>();
	CoreFrameworkIns->addComponetLogWrapper(getName(), g_LoggerPtr);
	if (jsonConfig.hasKey("LogLevel"))
	{
		g_LoggerPtr->logLevel = static_cast<mmrUtil::emLogLevel>(jsonConfig.at("LogLevel").ToInt());
	}

	//注册服务
	std::shared_ptr<ICmdService> serPtr = std::make_shared<CCmdService>();
	CoreFrameworkIns->registService<ICmdService>(std::move(serPtr));

	LOG_INFO("%s initialise success!", getName());
	return true;
}

bool CComponentActivator::start()
{
	return true;
}

void CComponentActivator::stop()
{
	g_LoggerPtr.reset();
}
