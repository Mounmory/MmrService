#include "CCompFramework.h"

#include "util/json.hpp"
#include "util/UtilFunc.h"

#include <sstream>
#include <fstream>
#include <iostream>

using namespace mmrService::mmrCore;


template<typename ServiceCtrl>
CCompFramework<ServiceCtrl>::CCompFramework()
	: m_upSerVPolicy(std::make_unique<ServiceCtrl>())
	, m_loggerCtrl(std::make_unique<CLoggerCtrl>())
	, m_licenseCtrl(nullptr)
{
	m_bRunning.store(false, std::memory_order_relaxed);
}

template<typename ServiceCtrl>
CCompFramework<ServiceCtrl>::~CCompFramework()
{

}

template<typename ServiceCtrl>
bool CCompFramework<ServiceCtrl>::start()
{
	if (m_bRunning.load(std::memory_order_relaxed))
	{
		LOG_WARN("component has been started!");
		return false;
	}

	try
	{
		//启动日志
		logInstancePtr->start();

		LOG_INFO("framework start! processID[%d]", Process_ID);

		//读配置文件，加载所有组件
		std::string strAppPath, strAppName;
		mmrUtil::getAppPathAndName(strAppPath, strAppName);

		//启动处理线程
		m_bRunning.store(true, std::memory_order_relaxed);
		m_threadDeal = std::make_unique<std::thread>(&CCompFramework::dealThread, this);

		std::string strConfigPath = strAppPath + "config/"+ strAppName +".json";

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

template<typename ServiceCtrl>
void CCompFramework<ServiceCtrl>::stop()
{
	//停掉线程
	if (true == m_bRunning)
	{
		m_bRunning.store(false, std::memory_order_relaxed);//退出线程
		m_cvData.notify_all();
		if (m_threadDeal->joinable())
		{
			m_threadDeal->join();//等待线程结束
		}
	}

	//停止所有组件
	for (const auto& iterComp : m_mapComponents)
	{
		iterComp.second->stop();
	}

	//在卸载库前，清空从动态库获取的指针数据
	m_mapComponents.clear();

	m_upSerVPolicy->clear();

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

	//清空数据
	//m_mapHandlers.clear();不用清空，析构函数自动移除

	while (m_queueDealData.size() > 0)
		m_queueDealData.pop();

	while (m_queueDealData.size() > 0)
		m_queueDealData.pop();

	m_libHandl.clear();

	m_licenseCtrl.reset();

	LOG_INFO("framework stoped! processID[%d]", Process_ID);
}

template<typename ServiceCtrl>
void CCompFramework<ServiceCtrl>::addHandler(std::string strTopic,IEventHandler* pHandler)
{
	std::lock_guard<std::mutex> lock(m_mutexHander);
	m_mapHandlers[strTopic].insert(pHandler);
}

template<typename ServiceCtrl>
void CCompFramework<ServiceCtrl>::removeHandler(std::string strTopic, IEventHandler* pHandler)
{
	std::lock_guard<std::mutex> lock(m_mutexHander);
	auto iterMap = m_mapHandlers.find(strTopic);
	if (iterMap != m_mapHandlers.end())
	{
		iterMap->second.erase(pHandler);
		if (iterMap->second.size() == 0)
		{
			m_mapHandlers.erase(iterMap);
		}
	}
}

template<typename ServiceCtrl>
void CCompFramework<ServiceCtrl>::addEvenVartData(mmrUtil::CVarDatas varData)
{
	std::unique_lock<std::mutex> lock(m_mutexData);
	m_queueAddData.emplace(std::make_pair(varData.getName(), std::move(varData)));
	m_cvData.notify_all();
}

template<typename ServiceCtrl>
void CCompFramework<ServiceCtrl>::dealThread()
{
	while (m_bRunning.load(std::memory_order_relaxed))
	{
		if (m_queueAddData.size() > 0)
		{
			LOG_INFO("Begine to deal event ,size[%d]", m_queueAddData.size());

			{//交换数据
				std::lock_guard<std::mutex> lock(m_mutexData);
				std::swap(m_queueAddData, m_queueDealData);
			}

			{//处理数据
				std::lock_guard<std::mutex> lock(m_mutexHander);
				while (m_queueDealData.size() > 0)
				{
					const auto& data = m_queueDealData.front();

					auto iterMap = m_mapHandlers.find(data.first);
					if (iterMap != m_mapHandlers.end())
					{
						for (const auto iterHandler : iterMap->second)
						{
							iterHandler->handleEvent(data.second);
						}
					}
					m_queueDealData.pop();
				}
			}
		}

		{
			std::unique_lock<std::mutex> lock(m_mutexData);
			while (m_queueAddData.empty() && m_bRunning.load(std::memory_order_relaxed))
			{
				m_cvData.wait_for(lock, std::chrono::seconds(100));
			}
		}
	}

	LOG_INFO("frame work deal thread exit!", m_queueAddData.size());
}

template<typename ServiceCtrl>
bool CCompFramework<ServiceCtrl>::addComponent(std::unique_ptr<IComponent> pComp)
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
template<typename ServiceCtrl>
void CCompFramework<ServiceCtrl>::addComponetLogWrapper(std::string strCompName, std::weak_ptr<mmrUtil::LogWrapper> logWrap)
{
	uint16_t sIndex = m_loggerCtrl->getMapCtrollerPtr().size() + 1;
	m_loggerCtrl->getMapCtrollerPtr()[sIndex] = std::make_pair(std::move(strCompName), logWrap);
}

template<typename ServiceCtrl>
void CCompFramework<ServiceCtrl>::loggerCtrlLoop(std::atomic_bool& bRunFlag)
{
	m_loggerCtrl->loop(bRunFlag);
}

template<typename ServiceCtrl>
void CCompFramework<ServiceCtrl>::licenseCtrlLoop(std::atomic_bool& bRunFlag)
{
	m_licenseCtrl->loop(bRunFlag);
}


