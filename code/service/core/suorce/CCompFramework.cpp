#include "service/core/include/CCompFramework.h"
#include "service/core/include/CLoggerCtrl.h"
#include "service/core/include/CLiceseCtrl.h"

#include "common/include/util/json.hpp"
#include "common/include/util/UtilFunc.h"
#include <common/include/util/ChunkAllocator.h>

#include <sstream>
#include <fstream>
#include <iostream>
#include <set>
#include <mutex>
#include <queue>
#include <atomic>
#include <condition_variable>

using namespace mmrService::mmrCore;

const char g_detail_options[] = R"(
  -h |--help                 Print this information
  -lg|--log                  Component logger settings and viewing
  -lc|--license              License info setting and viewing
  -q |--quit                 quit
)";

//定义配置文件关键字
#define STR_CFG_KEY_LOGGER		"Logger"
#define STR_CFG_KEY_ALLOCATOR	"Allocator"
#define STR_CFG_KEY_THREAD_POOL	"ThreadPool"

void print_help()
{
	printf("Options:%s", g_detail_options);
}

template<typename... TPolicies>
struct CCompFramework<TPolicies...>::CFrameData
{
	CFrameData() 
		: ptrLoggerCtrl(std::make_unique<CLoggerCtrl>())
		, ptrLicenseCtrl(nullptr)
		, bRunflag(false)
		, ptrFunc(nullptr)
	{
	}
	~CFrameData() 
	{
	}

	void clear()
	{
		ptrLoggerCtrl->getMapCtrollerPtr().clear();
		//ptrLicenseCtrl.reset();
	}

	std::unique_ptr<CLoggerCtrl> ptrLoggerCtrl;//组件日志控制类,使用了组件中创建的变量，要在组件卸载前释放
	std::unique_ptr<CLicenseCtrl> ptrLicenseCtrl;//权限控制类

	std::atomic_bool		bRunflag;
	std::mutex				mutex;
	std::condition_variable	cv;
	std::shared_ptr<CallbackFunc> ptrFunc;
};

template<typename... TPolicies>
CCompFramework<TPolicies...>::CCompFramework()
	: m_upServPolicy(std::make_unique<ServiceCtrl>())
	, m_ptrEventCtrl(std::make_unique<EventCtrl>())
	, m_ptrData(std::make_unique<CFrameData>())
{
	
}

template<typename... TPolicies>
CCompFramework<TPolicies...>::~CCompFramework()
{

}

template<typename... TPolicies>
void CCompFramework<TPolicies...>::handleEvent(const mmrUtil::CVarDatas& varData) 
{
	if (varData.getName() == "stop")
	{
		if (m_ptrData->bRunflag.load() == false)//不是通过run函数运行的，直接终止
		{
			return std::terminate();
		}

		const std::string& strMsg = varData.getVar("message").getStringData();
		std::cout << "stop app message : " << strMsg << std::endl;

		m_ptrData->bRunflag.store(false, std::memory_order_relaxed);
		m_ptrData->cv.notify_one();
	}
}

template<typename... TPolicies>
void CCompFramework<TPolicies...>::run(const std::string& strCfg)
{
	if (this->start(strCfg))
	{
		m_ptrData->bRunflag.store(true);
		std::thread(&CCompFramework<TPolicies...>::dealCmd, this).detach();
		//std::future<void> ft = std::async(std::launch::async, &CAppControler::dealCmd, this);
		std::unique_lock<std::mutex> locker(m_ptrData->mutex);
		m_ptrData->cv.wait(locker, [&]() {return !m_ptrData->bRunflag.load(); });
		//ft.get();
		this->stop();
	}
	else
	{
		std::cout << "Core framework start failed!\n";
	}
}

template<typename... TPolicies>
bool CCompFramework<TPolicies...>::start(const std::string& strCfg)
{
	try
	{
		//读配置文件，加载所有组件
		std::string strAppPath, strAppName;
		Json::Value jsonRoot;

		mmrUtil::getAppPathAndName(strAppPath, strAppName);
		std::string strConfigPath = strAppPath + "config/" + strCfg + ".json";
		std::string strErr = Json::load_from_file(strConfigPath, jsonRoot);
		if (jsonRoot.IsNull() || !strErr.empty())
		{
			printf("parse json file [%s] failed! error message is: %s.", strConfigPath.c_str(), strErr.c_str());
			return false;
		}

		//读取日志配置
		if (!jsonRoot.hasKey(STR_CFG_KEY_LOGGER)
			|| !jsonRoot[STR_CFG_KEY_LOGGER].hasKey("LogFileNum")
			|| !jsonRoot[STR_CFG_KEY_LOGGER].hasKey("LogFileSize")
			|| !jsonRoot[STR_CFG_KEY_LOGGER].hasKey("LogAsyn"))
		{
			printf("config file [%s] about key [%s] Inccrecct, use default parameters.", strConfigPath.c_str(), STR_CFG_KEY_LOGGER);
			mmrComm::Singleton<mmrUtil::CLogger>::initInstance();
		}
		else 
		{
			const auto& cfgLogger = jsonRoot[STR_CFG_KEY_LOGGER];

#if __cplusplus >= 201402L//需要c++14标准
			auto dictLogPara = mmrUtil::LoggerParas::Create().
				template Set<mmrUtil::TagLogFileNum>(cfgLogger.at("LogFileNum").ToInt()).
				template Set<mmrUtil::TagLogFileSize>(cfgLogger.at("LogFileSize").ToInt()).
				template Set<mmrUtil::TagAsyn>(cfgLogger.at("LogAsyn").ToBool());

			mmrComm::Singleton<mmrUtil::CLogger>::initInstance(dictLogPara);
#else
			mmrComm::Singleton<mmrUtil::CLogger>::initInstance(cfgLogger.at("LogFileNum").ToInt(),
				cfgLogger.at("LogFileSize").ToInt(),
				cfgLogger.at("LogAsyn").ToBool());
#endif
		}

		//读取内存管理配置
		if (!jsonRoot.hasKey(STR_CFG_KEY_ALLOCATOR)
			|| !jsonRoot[STR_CFG_KEY_ALLOCATOR].hasKey("CacheExpiredTime")
			|| !jsonRoot[STR_CFG_KEY_ALLOCATOR].hasKey("CacheMaxSize"))
		{
			printf("config file [%s] about key [%s] Inccrecct, use default parameters.", strConfigPath.c_str(), STR_CFG_KEY_ALLOCATOR);
			mmrComm::Singleton<mmrUtil::ChunkAllocator>::initInstance();
		}
		else
		{
			const auto& cfgAlloc = jsonRoot[STR_CFG_KEY_ALLOCATOR];
#if __cplusplus >= 201402L//需要c++14标准
			auto dictAllocPara = mmrUtil::ChunkAllocParas::Create().
				template Set<mmrUtil::TagExpiredTime>(cfgAlloc.at("CacheExpiredTime").ToInt()).
				template Set<mmrUtil::TagMaxCache>(cfgAlloc.at("CacheMaxSize").ToInt());

			mmrComm::Singleton<mmrUtil::ChunkAllocator>::initInstance(dictAllocPara);
			//mmrComm::Singleton<mmrUtil::ChunkAllocator>::initInstance(mmrUtil::ChunkAllocParas::Create().template Set<mmrUtil::TagExpiredTime>(cfgAlloc.at("CacheExpiredTime").ToInt()).template Set<mmrUtil::TagMaxCache>(cfgAlloc.at("CacheMaxSize").ToInt()));
#else
			mmrComm::Singleton<mmrUtil::ChunkAllocator>::initInstance(cfgAlloc.at("CacheExpiredTime").ToInt(), cfgAlloc.at("CacheMaxSize").ToInt());
#endif
		}


		LOG_INFO("framework start! processID[%d]", Process_ID);
		//启动订阅管理
		m_ptrEventCtrl->start();
		//加载组件
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
			LOG_INFO("Component %s init success!", iterComp.first.c_str());
		}

		//执行启动接口
		for (const auto& iterComp : m_mapComponents)
		{
			if (!iterComp.second->start())
			{
				LOG_ERROR_PRINT("component [%s] start failed!", iterComp.second->getName());
			}
		}

		//权限校验
		if (is_Server && nullptr == m_ptrData->ptrLicenseCtrl)
		{
			std::string strLicFilePath = strAppPath + "config/";
			m_ptrData->ptrLicenseCtrl = std::make_unique<CLicenseCtrl>(std::move(strLicFilePath), strAppName);
			m_ptrData->ptrLicenseCtrl->initLicense();
		}

		//注册回调
		m_ptrData->ptrFunc = std::make_shared<CallbackFunc>(std::bind(&CCompFramework<TPolicies...>::handleEvent, this, std::placeholders::_1));
		this->addFunc("stop", m_ptrData->ptrFunc);

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

template<typename... TPolicies>
void CCompFramework<TPolicies...>::stop()
{
	//停止事件回调
	m_ptrEventCtrl->stop();

	//停止所有组件
	for (const auto& iterComp : m_mapComponents)
	{
		iterComp.second->stop();
	}

	//在卸载库前，清空从动态库获取的指针数据
	m_mapComponents.clear();

	m_upServPolicy->clear();

	m_ptrData->clear();

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

	LOG_INFO("framework stoped! processID[%d]", Process_ID);
	mmrComm::Singleton<mmrUtil::ChunkAllocator>::destroyInstance();//清理内存分配器
	mmrComm::Singleton<mmrUtil::CLogger>::destroyInstance();//清理内存分配器
}

template<typename... TPolicies>
bool CCompFramework<TPolicies...>::addComponent(std::unique_ptr<IComponent> pComp)
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

template<typename... TPolicies>
void CCompFramework<TPolicies...>::addComponetLogWrapper(std::string strCompName, std::weak_ptr<mmrUtil::LogWrapper> logWrap)
{
	uint16_t sIndex = m_ptrData->ptrLoggerCtrl->getMapCtrollerPtr().size() + 1;
	m_ptrData->ptrLoggerCtrl->getMapCtrollerPtr()[sIndex] = std::make_pair(std::move(strCompName), logWrap);
}

template<typename... TPolicies>
void CCompFramework<TPolicies...>::dealCmd()
{
	if (!is_Server)
	{
		std::cout << "framework type is client!" << std::endl;
		return;
	}
	print_help();
	std::atomic_bool& rfRunFlag(m_ptrData->bRunflag);

	while (rfRunFlag.load(std::memory_order_relaxed))
	{
		std::string strCmd;
		printf("> ");
		std::getline(std::cin, strCmd);

		if (strCmd == "-h")
		{
			print_help();

			////测试通过handler停止
			//mmrUtil::CVarDatas vars;
			//vars.setName("stop");
			//vars.addVar("message", "stop by event test!");
			//CoreFrameworkIns->addEvenVartData(std::move(vars));
		}
		else if (strCmd == "-lg")
		{
			m_ptrData->ptrLoggerCtrl->loop(rfRunFlag);
		}
		else if (strCmd == "-lc")
		{
			if (m_ptrData->ptrLicenseCtrl)
			{
				m_ptrData->ptrLicenseCtrl->loop(rfRunFlag);
			}
			else 
			{
				printf("license controller invalid!\n");
				print_help();
			}
		}
		else if (strCmd == "-q")
		{
			printf("quit the application?[Y/N]\n");
			printf("> ");
			std::getline(std::cin, strCmd);

			if ("Y" == strCmd || "y" == strCmd)
			{
				printf("app stopped!\n");
				break;
			}
			else
			{
				print_help();
			}
		}
		else
		{
			printf("invalid command[%s]!\n", strCmd.c_str());
			print_help();
		}
	}
	rfRunFlag.store(false, std::memory_order_relaxed);
	m_ptrData->cv.notify_one();
}
