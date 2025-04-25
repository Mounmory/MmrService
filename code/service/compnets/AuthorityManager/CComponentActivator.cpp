#include "CComponentActivator.h"
#include "service/core/include/CCompFramework.h"
#include "common/include/util/Clogger.h"

#include "service/interface/iservice/ProtoRpcServer/IProtoRpcServer.h"
#include "common/include/protoBase/ProtoMsgCallback.h"

#include "CAlthorityProcessor.h"

#include "Login.pb.h"

REGIST_COMPONENT(CComponentActivator);

using namespace mmrService::mmrComp;

std::shared_ptr<mmrUtil::LogWrapper> g_LoggerPtr = nullptr;

std::unique_ptr<CAlthorityProcessor> g_althorProcesser = nullptr;

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
	g_LoggerPtr = std::make_shared<mmrUtil::LogWrapper>();
	CoreFrameworkIns->addComponetLogWrapper(getName(), g_LoggerPtr);
	if (jsonConfig.hasKey("LogLevel"))
	{
		g_LoggerPtr->logLevel = static_cast<mmrUtil::emLogLevel>(jsonConfig.at("LogLevel").ToInt());
	}

	////注册服务
	//std::shared_ptr<ICmdService> serPtr = std::make_shared<CCmdService>();
	//CoreFrameworkIns->registService<ICmdService>(std::move(serPtr));

	LOG_INFO("%s initialise success!", getName());
	return true;
}


bool CComponentActivator::start()
{
	g_althorProcesser = std::make_unique<CAlthorityProcessor>();

	auto rpcDispatcher = CoreFrameworkIns->getService<IProtoRpcServer>();
	if (!rpcDispatcher)
	{
		LOG_ERROR("Get rpcDispatcher service failed!");
		return false;
	}

	//std::function<std::shared_ptr<mmrService::LoginResponse>(const std::shared_ptr<mmrService::LoginRequest>&)>
	//	fun = std::bind(&CAlthorityProcessor::onLogin, g_althorProcesser.get(), std::placeholders::_1);
	//rpcDispatcher->registerMessageCallback<mmrService::LoginRequest, mmrService::LoginResponse>(std::move(fun));

	//注册接口
	rpcDispatcher->registerMessageCallback<mmrService::LoginRequest, mmrService::LoginResponse>(std::bind(&CAlthorityProcessor::onLogin, g_althorProcesser.get(), std::placeholders::_1));

	
	return true;
}

void CComponentActivator::stop()
{
	g_LoggerPtr.reset();
	g_althorProcesser.reset();
}
