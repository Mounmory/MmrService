#include <service/core/include/CCompFramework.h>
#include <service/core/include/CLoggerCtrl.h>
#include <service/core/include/CLiceseCtrl.h>

#include <common/include/util/json.hpp>
#include <common/include/util/UtilFunc.h>
#include <common/include/util/MemoryPool.hpp>
#include <common/include/util/CThreadpool.h>

#include <sstream>
#include <fstream>
#include <iostream>
#include <set>
#include <mutex>
#include <queue>
#include <atomic>
#include <iomanip>
#include <cmath>
#include <condition_variable>

using namespace mmrService::mmrCore;

//定义配置文件关键字
#define STR_CFG_KEY_LOGGER			"Logger"
#define STR_CFG_KEY_CHUNK_ALLOCATOR	"ChunkAllocator"
#define STR_CFG_KEY_SMALL_ALLOCATOR	"SmallAllocator"
#define STR_CFG_KEY_THREAD_POOL		"ThreadPool"

template<typename... TPolicies>
struct CCompFramework<TPolicies...>::DataFrame
{
	DataFrame()
		: ptrLoggerCtrl(std::make_unique<CLoggerCtrl>())
		, ptrLicenseCtrl(nullptr)
		, bRunflag(false)
		, ptrFunc(nullptr)
	{
	}
	~DataFrame()
	{
	}

	void clear()
	{
		ptrLoggerCtrl->getMapCtrollerPtr().clear();
		//ptrLicenseCtrl.reset();
		m_funcTimer.clear();
		m_wkFuncTimer.clear();
		m_mapCmdFunc.clear();
	}

	std::unique_ptr<CLoggerCtrl> ptrLoggerCtrl;//组件日志控制类,使用了组件中创建的变量，要在组件卸载前释放
	std::unique_ptr<CLicenseCtrl> ptrLicenseCtrl;//权限控制类

	std::atomic_bool		bRunflag;
	std::mutex				mutex;
	std::condition_variable	cv;
	std::shared_ptr<CallbackFunc> ptrFunc;//framework用于处理订阅回调的函数


	std::list<std::shared_ptr<TimerFunc>> m_funcTimer;//时钟回调函数
	std::list<std::weak_ptr<TimerFunc>> m_wkFuncTimer;//时钟回调函数

	//程序控制函数
	std::map<std::string, std::pair<std::string, std::shared_ptr<CmdFunc>>> m_mapCmdFunc;
};

template<typename... TPolicies>
CCompFramework<TPolicies...>::CCompFramework()
	: m_upServPolicy(std::make_unique<ServiceCtrl>())
	, m_ptrEventCtrl(std::make_unique<EventCtrl>())
	, m_ptrData(std::make_unique<DataFrame>())
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
		//std::future<void> ft = std::async(std::launch::async, &CAppControler::dealCmd, this);
		waitAndDealTimer();
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
			printf("parse json file [%s] failed! error message is: %s.\n", strConfigPath.c_str(), strErr.c_str());
			return false;
		}

		//读取日志配置
		if (!jsonRoot.hasKey(STR_CFG_KEY_LOGGER)
			|| !jsonRoot[STR_CFG_KEY_LOGGER].hasKey("LogFileNum")
			|| !jsonRoot[STR_CFG_KEY_LOGGER].hasKey("LogFileSize")
			|| !jsonRoot[STR_CFG_KEY_LOGGER].hasKey("LogAsyn"))
		{
			printf("config file [%s] about key [%s] Inccrecct, use default parameters.\n", strConfigPath.c_str(), STR_CFG_KEY_LOGGER);
			mmrComm::Singleton<mmrUtil::CLogger>::initInstance();
		}
		else 
		{
			const auto& cfgLogger = jsonRoot[STR_CFG_KEY_LOGGER];

#if __cplusplus >= 201402L//需要c++14标准
			auto dictPara = mmrUtil::LoggerParas::Create().
				template Set<mmrUtil::TagLogFileNum>(cfgLogger.at("LogFileNum").ToInt()).
				template Set<mmrUtil::TagLogFileSize>(cfgLogger.at("LogFileSize").ToInt()).
				template Set<mmrUtil::TagAsyn>(cfgLogger.at("LogAsyn").ToBool());

			mmrComm::Singleton<mmrUtil::CLogger>::initInstance(dictPara);
#else
			mmrComm::Singleton<mmrUtil::CLogger>::initInstance(cfgLogger.at("LogFileNum").ToInt(),
				cfgLogger.at("LogFileSize").ToInt(),
				cfgLogger.at("LogAsyn").ToBool());
#endif
		}
		
		//读取大块内存分配器管理配置
		if (!jsonRoot.hasKey(STR_CFG_KEY_CHUNK_ALLOCATOR)
			|| !jsonRoot[STR_CFG_KEY_CHUNK_ALLOCATOR].hasKey("CacheExpiredTime")
			|| !jsonRoot[STR_CFG_KEY_CHUNK_ALLOCATOR].hasKey("CacheMaxSize"))
		{
			printf("config file [%s] about key [%s] Inccrecct, use default parameters.\n", strConfigPath.c_str(), STR_CFG_KEY_CHUNK_ALLOCATOR);
			mmrComm::Singleton<mmrUtil::ChunkAllocator<>>::initInstance();
		}
		else
		{
			const auto& cfgAlloc = jsonRoot[STR_CFG_KEY_CHUNK_ALLOCATOR];
#if __cplusplus >= 201402L//需要c++14标准
			auto dictPara = mmrUtil::ChunkAllocParas::Create().
				template Set<mmrUtil::TagCAExpiredTime>(cfgAlloc.at("CacheExpiredTime").ToInt()).
				template Set<mmrUtil::TagCAMaxCache>(cfgAlloc.at("CacheMaxSize").ToInt());

			mmrComm::Singleton<mmrUtil::ChunkAllocator<>>::initInstance(dictPara);
#else
			mmrComm::Singleton<mmrUtil::ChunkAllocator<>>::initInstance(cfgAlloc.at("CacheExpiredTime").ToInt(), cfgAlloc.at("CacheMaxSize").ToInt());
#endif
		}

		//设置小块内存分配
		if (!jsonRoot.hasKey(STR_CFG_KEY_SMALL_ALLOCATOR)
			|| !jsonRoot[STR_CFG_KEY_SMALL_ALLOCATOR].hasKey("CacheExpiredTime")
			|| !jsonRoot[STR_CFG_KEY_SMALL_ALLOCATOR].hasKey("CacheMaxSize"))
		{
			printf("config file [%s] about key [%s] Inccrecct, use default parameters.\n", strConfigPath.c_str(), STR_CFG_KEY_SMALL_ALLOCATOR);
			mmrComm::Singleton<mmrUtil::SmallAllocator>::initInstance();
		}
		else
		{
			const auto& cfgAlloc = jsonRoot[STR_CFG_KEY_SMALL_ALLOCATOR];
#if __cplusplus >= 201402L//需要c++14标准
			auto dictPara = mmrUtil::SmallAllocParas::Create().
				template Set<mmrUtil::TagSAExpiredTime>(cfgAlloc.at("CacheExpiredTime").ToInt()).
				template Set<mmrUtil::TagSAMaxCache>(cfgAlloc.at("CacheMaxSize").ToInt());

			mmrComm::Singleton<mmrUtil::SmallAllocator>::initInstance(dictPara);
#else
			mmrComm::Singleton<mmrUtil::SmallAllocator>::initInstance(cfgAlloc.at("CacheExpiredTime").ToInt(), cfgAlloc.at("CacheMaxSize").ToInt());
#endif
		}
		
		//设置线程池
		if (!jsonRoot.hasKey(STR_CFG_KEY_THREAD_POOL)
			|| !jsonRoot[STR_CFG_KEY_THREAD_POOL].hasKey("Mode")
			|| !jsonRoot[STR_CFG_KEY_THREAD_POOL].hasKey("FixThreadSize")
			|| !jsonRoot[STR_CFG_KEY_THREAD_POOL].hasKey("TaskQueueMaxSize"))
		{
			printf("config file [%s] about key [%s] Inccrecct, use default parameters.\n", strConfigPath.c_str(), STR_CFG_KEY_THREAD_POOL);
			mmrComm::Singleton<mmrUtil::ThreadPool>::initInstance();
		}
		else
		{
			const auto& cfgAlloc = jsonRoot[STR_CFG_KEY_THREAD_POOL];
			uint16_t usMaxThreadSize(1024)//最大线程数
				, usIdleTime(120);//线程回收时间
			if (cfgAlloc.hasKey("MaxThreadSize"))
				usMaxThreadSize = cfgAlloc.at("MaxThreadSize").ToInt();
			if (cfgAlloc.hasKey("ThreadIdleTime"))
				usIdleTime = cfgAlloc.at("ThreadIdleTime").ToInt();

#if __cplusplus >= 201402L//需要c++14标准
			auto dictPara = mmrUtil::ThreadpoolParas::Create().
				template Set<mmrUtil::TagMode>(static_cast<mmrUtil::emPoolMode>(cfgAlloc.at("Mode").ToInt())).
				template Set<mmrUtil::TagFixThreadSize>(cfgAlloc.at("FixThreadSize").ToInt()).
				template Set<mmrUtil::TagTaskQueueMaxSize>(cfgAlloc.at("TaskQueueMaxSize").ToInt()).
				template Set<mmrUtil::TagMaxThreadSize>(usMaxThreadSize).
				template Set<mmrUtil::TagThreadIdleTime>(usIdleTime);

			mmrComm::Singleton<mmrUtil::ThreadPool>::initInstance(dictPara);
#else
			mmrComm::Singleton<mmrUtil::ThreadPool>::initInstance(static_cast<mmrUtil::emPoolMode>(cfgAlloc.at("Mode").ToInt()), cfgAlloc.at("FixThreadSize").ToInt()
				, cfgAlloc.at("TaskQueueMaxSize").ToInt(), usMaxThreadSize, usIdleTime);
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

		//注册回调
		m_ptrData->ptrFunc = std::make_shared<CallbackFunc>(std::bind(&CCompFramework<TPolicies...>::handleEvent, this, std::placeholders::_1));
		this->addFunc("stop", m_ptrData->ptrFunc);

		LOG_INFO("framework start success ....");
		std::cout << "framework start success ...." << std::endl;
		std::cout << "********************************************" << std::endl;

		//服务器权限校验相关
		if (is_Server && nullptr == m_ptrData->ptrLicenseCtrl)
		{
			std::string strLicFilePath = strAppPath + "config/";
			m_ptrData->ptrLicenseCtrl = std::make_unique<CLicenseCtrl>(std::move(strLicFilePath), strAppName);
			m_ptrData->ptrLicenseCtrl->initLicense();

			/*注册命令控制函数*/
			//日志控制函数
			{
				auto ptrCmd = std::make_shared<CmdFunc>(std::bind(&CLoggerCtrl::loop,
					m_ptrData->ptrLoggerCtrl.get(), std::placeholders::_1));
				addCmdCallback("-lg", "--log, component logger settings and viewing", std::move(ptrCmd));
			}

			//授权控制组件
			{
				auto ptrCmd = std::make_shared<CmdFunc>(std::bind(&CLicenseCtrl::loop,
					m_ptrData->ptrLicenseCtrl.get(), std::placeholders::_1));
				addCmdCallback("-lc", "--license, license info setting and viewing", std::move(ptrCmd));
			}

			//小内存分配器
			{
				auto ptrCmd = std::make_shared<CmdFunc>(std::bind(&mmrUtil::SmallAllocator::loop,
					mmrComm::Singleton<mmrUtil::SmallAllocator>::getInstance(), std::placeholders::_1));
				addCmdCallback("-sa", "--small allocator, small allocator control and viewing", std::move(ptrCmd));
			}

			//大块内存分配器
			{
				auto ptrCmd = std::make_shared<CmdFunc>(std::bind(&mmrUtil::ChunkAllocator<>::loop,
					mmrComm::Singleton<mmrUtil::ChunkAllocator<>>::getInstance(), std::placeholders::_1));
				addCmdCallback("-ca", "--chunk allocator, chunck allocator control and viewing", std::move(ptrCmd));
			}

			{
				auto ptrCmd = std::make_shared<CmdFunc>(std::bind(&mmrUtil::ThreadPool::loop,
					mmrComm::Singleton<mmrUtil::ThreadPool>::getInstance(), std::placeholders::_1));
				addCmdCallback("-tp", "--thread pool, thread pool control and viewing", std::move(ptrCmd));
			}


			//运行命令处理线程
			m_ptrData->bRunflag.store(true);
			std::thread(&CCompFramework<TPolicies...>::dealCmd, this).detach();
		}
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

	auto pairSmallAllocMemInfo = mmrComm::Singleton<mmrUtil::SmallAllocator>::getInstance()->getAvailableMemoryInfo();
	auto pairChunkAllocMemInfo = mmrComm::Singleton<mmrUtil::ChunkAllocator<>>::getInstance()->getAvailableMemoryInfo();
	LOG_INFO("memory info[free/total]\nsmall allocator [%lld/%lld]\nchunk allocator [%lld/%lld]"
		, pairSmallAllocMemInfo.first, pairSmallAllocMemInfo.second
		, pairChunkAllocMemInfo.first, pairChunkAllocMemInfo.second);

	//停掉线程池
	mmrComm::Singleton<mmrUtil::ThreadPool>::destroyInstance();
	//停掉小内存分配器
	mmrComm::Singleton<mmrUtil::SmallAllocator>::destroyInstance();
	//停掉大内存分配器
	mmrComm::Singleton<mmrUtil::ChunkAllocator<>>::destroyInstance();//清理内存分配器
	//停掉Log
	LOG_INFO("framework stoped! processID[%d]", Process_ID);
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
void CCompFramework<TPolicies...>::addTimerCallback(const std::shared_ptr<TimerFunc>& func)
{
	m_ptrData->m_funcTimer.emplace_back(func);
}

template<typename... TPolicies>
void CCompFramework<TPolicies...>::addTimerCallbackWk(const std::shared_ptr<TimerFunc>& func)
{
	m_ptrData->m_wkFuncTimer.emplace_back(func);
}

template<typename... TPolicies>
void CCompFramework<TPolicies...>::addCmdCallback(std::string strCmd, std::string strDesc, std::shared_ptr<CmdFunc> func) 
{
	auto iterCmd = m_ptrData->m_mapCmdFunc.find(strCmd);
	if (iterCmd != m_ptrData->m_mapCmdFunc.end())
	{
		LOG_WARN_PRINT("cmd[%s] have already exit!, description is %s",
			iterCmd->first.c_str(), iterCmd->second.first.c_str());
	}
	m_ptrData->m_mapCmdFunc[strCmd] = std::make_pair(strDesc, std::move(func));
}

template<typename... TPolicies>
void CCompFramework<TPolicies...>::waitAndDealTimer()
{
	//绑定小内存分配器时间回调函数
	addTimerCallback(std::make_shared<TimerFunc>(std::bind(&mmrUtil::SmallAllocator::onTimer, mmrComm::Singleton<mmrUtil::SmallAllocator>::getInstance(), std::placeholders::_1)));

	//绑定大块内存分配器时间回调函数
	addTimerCallback(std::make_shared<TimerFunc>(std::bind(&mmrUtil::ChunkAllocator<>::onTimer, mmrComm::Singleton<mmrUtil::ChunkAllocator<>>::getInstance(), std::placeholders::_1)));

	auto begineTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	while (m_ptrData->bRunflag.load(std::memory_order_relaxed))
	{
		int64_t currTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) - begineTime;
		
		//调用注册的函数(弱引用),如果在程序中有动态增加回调，则需要加锁或者使用无锁队列
		for (auto iterFuncWk = m_ptrData->m_wkFuncTimer.begin(); iterFuncWk != m_ptrData->m_wkFuncTimer.end();)
		{
			auto ptrFun = (*iterFuncWk).lock();
			if (ptrFun) 
			{
				(*ptrFun)(currTime);
				++iterFuncWk;
			}
			else 
			{
				iterFuncWk = m_ptrData->m_wkFuncTimer.erase(iterFuncWk);
			}
		}

		//调用智能指针函数
		for (auto&  iterFunc : m_ptrData->m_funcTimer)
		{
			(*iterFunc)(currTime);
		}

		{
			std::unique_lock<std::mutex> lock(m_ptrData->mutex);
			m_ptrData->cv.wait_for(lock, std::chrono::milliseconds(1000));//异步日志5秒写一次
		}
	}
	//由DataFrame类清空
	//m_ptrData->m_wkFuncTimer.clear();
	//m_ptrData->m_funcTimer.clear();
}

template<typename... TPolicies>
void CCompFramework<TPolicies...>::dealCmd()
{
	auto print_help = [&]() 
	{
		printf("Options:\n"
			"  -h |--help, Print this information\n"
			"  -q |--quit, quit application\n");
		for (const auto& iterCmd : m_ptrData->m_mapCmdFunc)
		{
			printf("  %s|%s\n", iterCmd.first.c_str(), iterCmd.second.first.c_str());
		}
	};

	print_help();
	std::atomic_bool& rfRunFlag(m_ptrData->bRunflag);

	while (rfRunFlag.load(std::memory_order_relaxed))
	{
		std::string strCmd;
		printf("> ");
		std::getline(std::cin, strCmd);

		auto iterCmd = m_ptrData->m_mapCmdFunc.find(strCmd);
		if (iterCmd != m_ptrData->m_mapCmdFunc.end())
		{
			auto& funPtr = iterCmd->second.second;
			(*funPtr)(rfRunFlag);
		}
		else if (strCmd == "-h")
		{
			print_help();

			////测试通过handler停止
			//mmrUtil::CVarDatas vars;
			//vars.setName("stop");
			//vars.addVar("message", "stop by event test!");
			//CoreFrameworkIns->addEvenVartData(std::move(vars));
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

	//由DataFrame实例去清空
	//m_ptrData->m_mapCmdFunc.clear();
}
