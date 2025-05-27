#include "CComponentActivator.h"
#include "service/core/include/CCompFramework.h"
#include "CHelloService.h"

REGIST_COMPONENT(CComponentActivator);

using namespace mmrService::mmrComp;

std::shared_ptr<mmrUtil::LogWrapper> g_LoggerPtr = nullptr;

CComponentActivator::CComponentActivator()
{
	
}

const char* CComponentActivator::getName()
{
	static const char* szName = COMPONENT_NAME;
	return szName;
}

bool CComponentActivator::initialise(const Json::Value& jsonConfig)
{
	//日志设置
	g_LoggerPtr = mmrUtil::Make_Shared<mmrUtil::LogWrapper>();
	CoreFrameworkIns->addComponetLogWrapper(getName(), g_LoggerPtr);
	if (jsonConfig.hasKey("LogLevel"))
	{
		g_LoggerPtr->logLevel = static_cast<mmrUtil::emLogLevel>(jsonConfig.at("LogLevel").ToInt());
	}

	//测试崩溃
	//int* lPtr = nullptr;
	//g_LoggerPtr->logLevel = static_cast<mmrUtil::emLogLevel>(*lPtr);

	//注册服务
	std::shared_ptr<IHelloService> serPtr = mmrUtil::Make_Shared<CHelloService>();
	CoreFrameworkIns->registService<IHelloService>(std::move(serPtr));

	//注册事件回调
	m_pDemoHandler = mmrUtil::Make_Shared<CHandlerDemo>();

	auto pDemoHandler = mmrUtil::Make_Shared<CHandlerDemo>();
	LOG_INFO("%s initialise success!", getName());
	return true;
}

bool CComponentActivator::start()
{
	return true;
}

void CComponentActivator::stop()
{
	g_LoggerPtr.reset();//linux系统全局数据在动态库卸载时不自动调用析构函数
}
