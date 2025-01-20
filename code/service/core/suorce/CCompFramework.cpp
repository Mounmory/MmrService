#include "service/core/include/CCompFramework.h"

#include "common/include/util/json.hpp"
#include "common/include/util/UtilFunc.h"

#include <sstream>
#include <fstream>
#include <iostream>

using namespace mmrService::mmrCore;


template<typename ServiceCtrl, typename HandlerCtrl>
CCompFramework<ServiceCtrl, HandlerCtrl>::CCompFramework()
	: m_loggerCtrl(std::make_unique<CLoggerCtrl>())
	, m_licenseCtrl(nullptr)
	, m_upServPolicy(std::make_unique<ServiceCtrl>())
	, m_ptrHandlerCtrl(std::make_unique<HandlerCtrl>())
{
	
}

template<typename ServiceCtrl, typename HandlerCtrl>
CCompFramework<ServiceCtrl, HandlerCtrl>::~CCompFramework()
{

}

template<typename ServiceCtrl, typename HandlerCtrl>
bool CCompFramework<ServiceCtrl, HandlerCtrl>::start(const std::string& strCfgFile)
{
	try
	{
		//启动日志
		logInstancePtr->start();

		LOG_INFO("framework start! processID[%d]", Process_ID);

		//启动订阅管理
		m_ptrHandlerCtrl->start();

		//读配置文件，加载所有组件
		std::string strAppPath, strAppName;
		mmrUtil::getAppPathAndName(strAppPath, strAppName);

		std::string strConfigPath = strAppPath + "config/"; 
		if (strCfgFile.empty())
		{
			strConfigPath += strAppName + ".json";
		}
		else 
		{
			strConfigPath += strCfgFile;
		}

		Json::Value jsonRoot;
		std::string strErr = Json::json_from_file(strConfigPath, jsonRoot);

		if (jsonRoot.IsNull() || !strErr.empty())
		{
			LOG_ERROR_PRINT("parse json file [%s] failed! error message is: %s.", strConfigPath.c_str(), strErr.c_str());
			return false;
		}

		auto components = jsonRoot["Components"];
		if (components.IsNull())
		{
			LOG_ERROR_PRINT("config file [%s] do not has key [Components] !", strConfigPath.c_str());
			return false;
		}

		for (const auto& iterComp : components.ObjectRange())
		{
			if (!iterComp.second.hasKey("Enable"))
			{
				LOG_ERROR_PRINT("component [%s] config do not has key [Enable] !", iterComp.first.c_str());
				continue;
			}
			if (iterComp.second.at("Enable").ToBool() == false)
			{
				continue;
			}
#ifdef OS_MMR_WIN
			std::string strLibPath = strAppPath + "component/" + iterComp.first + ".dll";
			HINSTANCE handle = LoadLibrary(strLibPath.c_str());
			if (handle == nullptr)
			{
				LOG_ERROR_PRINT("load lib %s failed.", strLibPath.c_str());
				//FreeLibrary(handle);
				continue;
			}
#elif defined OS_MMR_LINUX
			std::string strLibPath = strAppPath + "component/lib" + iterComp.first + ".so";
			void* handle = dlopen(strLibPath.c_str(), RTLD_LAZY);
			if (handle == nullptr)
			{
				LOG_ERROR_PRINT("load lib %s failed.", strLibPath.c_str());
				//dlclose(handle);
				continue;
			}
#endif
			else
			{
				m_libHandl.insert(handle);
			}
		}
		//初始化所有组件
		for (const auto& iterComp : m_mapComponents)
		{
			std::string strComoName = iterComp.second->getName();
			if (!components.hasKey(strComoName))
			{
				LOG_ERROR_PRINT("component name [%s] do not find in config file!", strComoName.c_str());
				continue;

			}
			if (!iterComp.second->initialise(components[strComoName]))
			{
				LOG_ERROR_PRINT("init component name [%s] failed!", strComoName.c_str());
				continue;
			}
		}

		for (const auto& iterComp : m_mapComponents)
		{
			if (!iterComp.second->start())
			{
				LOG_ERROR_PRINT("component [%s] start failed!", iterComp.second->getName());
			}
		}

		//权限校验
		if (nullptr == m_licenseCtrl)
		{
			std::string strLicFilePath = strAppPath + "config/";
			m_licenseCtrl = std::make_unique<CLicenseCtrl>(std::move(strLicFilePath), strAppName);
			m_licenseCtrl->initLicense();
		}

		LOG_INFO("framework start success ....");
		std::cout << "framework start success ...." << std::endl;
		std::cout << "********************************************" << std::endl;
	}
	catch (std::exception& e)
	{
		LOG_FATAL("exception info is %s.", e.what());
		return false;
	}
	catch (...)
	{
		LOG_FATAL("unknow exception.");
		return false;
	}

	return true;
}

template<typename ServiceCtrl, typename HandlerCtrl>
void CCompFramework<ServiceCtrl, HandlerCtrl>::stop()
{

	//停止
	m_ptrHandlerCtrl->stop();

	//停止所有组件
	for (const auto& iterComp : m_mapComponents)
	{
		iterComp.second->stop();
	}

	//在卸载库前，清空从动态库获取的指针数据
	m_mapComponents.clear();

	m_upServPolicy->clear();

	m_loggerCtrl->getMapCtrollerPtr().clear();

	//卸载所有动态库
	for (const auto& iterHandl:m_libHandl)
	{
#ifdef OS_MMR_WIN
		FreeLibrary(iterHandl);
#elif defined OS_MMR_LINUX
		dlclose(iterHandl);
#endif
	}
	m_libHandl.clear();

	m_licenseCtrl.reset();

	LOG_INFO("framework stoped! processID[%d]", Process_ID);
}

template<typename ServiceCtrl, typename HandlerCtrl>
bool CCompFramework<ServiceCtrl, HandlerCtrl>::addComponent(std::unique_ptr<IComponent> pComp)
{
	std::string strCompName = pComp->getName();
	auto iterComp = m_mapComponents.find(strCompName);
	if (iterComp != m_mapComponents.end())
	{
		LOG_ERROR("Component [%s] load failed! There is already a component with the same name.",
			pComp->getName());
		return false;
	}

	LOG_INFO("Component [%s] load succesfully! commponent num is %ld.", pComp->getName(), m_mapComponents.size() + 1);
	m_mapComponents.insert(std::make_pair(std::move(strCompName), std::move(pComp)));
	return true;
}

//void mmrCore::CCompFramework::removeComponet(uint16_t usIndex)
//{
//	//暂不实现
//}
template<typename ServiceCtrl, typename HandlerCtrl>
void CCompFramework<ServiceCtrl, HandlerCtrl>::addComponetLogWrapper(std::string strCompName, std::weak_ptr<mmrUtil::LogWrapper> logWrap)
{
	uint16_t sIndex = m_loggerCtrl->getMapCtrollerPtr().size() + 1;
	m_loggerCtrl->getMapCtrollerPtr()[sIndex] = std::make_pair(std::move(strCompName), logWrap);
}

template<typename ServiceCtrl, typename HandlerCtrl>
void CCompFramework<ServiceCtrl, HandlerCtrl>::loggerCtrlLoop(std::atomic_bool& bRunFlag)
{
	m_loggerCtrl->loop(bRunFlag);
}

template<typename ServiceCtrl, typename HandlerCtrl>
void CCompFramework<ServiceCtrl, HandlerCtrl>::licenseCtrlLoop(std::atomic_bool& bRunFlag)
{
	m_licenseCtrl->loop(bRunFlag);
}


