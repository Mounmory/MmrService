#include "CComponentActivator.h"
#include "service/core/include/CCompFramework.h"
#include "CProtoServer.h"


REGIST_COMPONENT(CComponentActivator);

using namespace mmrService::mmrComp;

std::shared_ptr<mmrUtil::LogWrapper> g_LoggerPtr = nullptr;

std::shared_ptr<CProtoServer> g_rpcServer = nullptr;

CComponentActivator::CComponentActivator()
{
	
}

const char* CComponentActivator::getName()
{
	static const char* szName = "ProtoRpcServer";
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

	//服务设置
	g_rpcServer = std::make_shared<CProtoServer>();
	
	std::string strIP = "0.0.0.0";
	uint16_t usPort = 30010;
	uint32_t ulTreadNum = std::thread::hardware_concurrency();

	if (jsonConfig.hasKey("ServThreadNum") && jsonConfig.at("ServThreadNum").ToInt() <= ulTreadNum)
	{
		ulTreadNum = jsonConfig.at("ServThreadNum").ToInt();
	}
	g_rpcServer->setConnectInfo(strIP, usPort);
	g_rpcServer->setThreadNum(std::thread::hardware_concurrency());

	LOG_INFO("set proto rpc server,ip %s port %d, thread num %d!", strIP.c_str(), usPort, ulTreadNum);

	//注册服务
	//std::shared_ptr<IProtoRpcServer> serPtr = std::make_shared<CProtoServer>();
	//CoreFrameworkIns->registService<IProtoRpcServer>(std::move(serPtr));
	CoreFrameworkIns->registService<IProtoRpcServer>(g_rpcServer);

	LOG_INFO("%s initialise success!", getName());
	return true;
}

bool CComponentActivator::start()
{

	//建立网络连接
	int32_t sockeFd = g_rpcServer->createSocket();
	if (sockeFd < 0)
	{
		LOG_ERROR("create socke failed! socke fd %ld", sockeFd);
		return false;
	}
	g_rpcServer->start();
	return true;
}

void CComponentActivator::stop()
{
	g_rpcServer.reset();
	g_LoggerPtr.reset();
}
