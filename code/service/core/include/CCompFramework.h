#ifndef CCOMPFRAMEWORK_H
#define CCOMPFRAMEWORK_H
#include <set>
#include <mutex>
#include <queue>
#include <atomic>
#include <memory>
#include <condition_variable>



#include "ComponentExport.h"
#include "IEventHandler.h"
#include "IComponent.h"
#include "CLoggerCtrl.h"
#include "CLiceseCtrl.h"

#include "ServiceCtrlPolicies.hpp"

#ifdef OS_MMR_WIN
#include <Windows.h>
static const char* strLibExtension = ".dll";
#define libHandle HINSTANCE


#elif defined OS_MMR_LINUX
#include <dirent.h>
#include <dlfcn.h>
static const char* strLibExtension = ".so";
#define libHandle void*


#endif

BEGINE_NAMESPACE(mmrService)
BEGINE_NAMESPACE(mmrCore)

template<typename ServiceCtrl>/*服务管理策略*/
class COMPO_CORE_CLASS_API CCompFramework
{
	CCompFramework();
	~CCompFramework();
public:
	static CCompFramework<ServiceCtrl>* getInstance()
	{
		static CCompFramework<ServiceCtrl>* instalce = new CCompFramework<ServiceCtrl>;
		return instalce;
	}

	bool start();//配置文件路径

	void stop();

	//处理事件相关接口
	void addHandler(std::string strTopic, IEventHandler* pHandler);

	void removeHandler(std::string strTopic, IEventHandler* pHandler);

	void addEvenVartData(mmrUtil::CVarDatas varData);

	//处理组件相关
	bool addComponent(std::unique_ptr<IComponent> pComp);

	//void removeComponet(uint16_t usIndex);

	void addComponetLogWrapper(std::string strCompName, std::weak_ptr<mmrUtil::LogWrapper> logWrap);

	void loggerCtrlLoop(std::atomic_bool& bRunFlag);

	void licenseCtrlLoop(std::atomic_bool& bRunFlag);

	template<typename _T>
	void registService(std::shared_ptr<_T>&& pSer) 
	{
		m_upSerVPolicy->registService<_T>(std::forward<std::shared_ptr<_T>>(pSer));
	}

	template<typename _T>
	std::shared_ptr<_T> getService() 
	{
		return m_upSerVPolicy->getService<_T>();
	}

	template<typename CompType>
	class CCompRegister//组件自动注册类
	{
	public:
		CCompRegister()
		{
			std::unique_ptr<IComponent> compPtr = std::make_unique<CompType>();
			CCompFramework<ServiceCtrl>::getInstance()->addComponent(std::move(compPtr));
		}
		~CCompRegister() = default;
	};

private:
	void dealThread();

private:
	std::unique_ptr<std::thread> m_threadDeal;
	std::atomic_bool m_bRunning;

	std::unique_ptr<ServiceCtrl> m_upSerVPolicy;//服务管理策略

	//处理事件相关成员
	std::unordered_map<std::string, std::set<IEventHandler*>> m_mapHandlers;//所有事件处理者
	std::mutex m_mutexHander;//事件处理集合互斥量

	std::queue<std::pair<std::string, mmrUtil::CVarDatas>> m_queueAddData;
	std::queue<std::pair<std::string, mmrUtil::CVarDatas>> m_queueDealData;
	std::mutex m_mutexData;
	std::condition_variable m_cvData;

	//处理组件相关
	std::unordered_map<std::string, std::unique_ptr<IComponent>> m_mapComponents;
	std::set<libHandle> m_libHandl;
	
	std::unique_ptr<CLoggerCtrl> m_loggerCtrl;//组件日志控制类
	std::unique_ptr<CLicenseCtrl> m_licenseCtrl;//权限控制类
};


END_NAMESPACE(mmrCore)
END_NAMESPACE(mmrService)

//定义服务管理策略
#ifdef OS_MMR_WIN
//using FramServicePolicy = mmrService::mmrCore::MapServKeyByTypeID;//使用map管理服务指针，typeID作为键
using FramServicePolicy = mmrService::mmrCore::UnMapServKeyByGUID;//使用unodered_map管理服务指针，GUID作为键
#else
//using FramServicePolicy = mmrService::mmrCore::MapServKeyByTypeID;//使用map管理服务指针，typeID作为键
//using FramServicePolicy = mmrService::mmrCore::UnMapServKeyByGUID;//使用unodered_map管理服务指针，GUID作为键
using FramServicePolicy = mmrService::mmrCore::VecServByIndex;//使用静态索引管理作为服务指针地址，vs编译存在类导出问题，不同动态库中对基类中静态成员值不一致，这个策略无法使用
#endif // OS_MMR_LINUX


template class COMPO_CORE_CLASS_API mmrService::mmrCore::CCompFramework<FramServicePolicy>;//到处模板类实例

using ServiceType = mmrService::mmrCore::CCompFramework<FramServicePolicy>;

#define CoreFrameworkIns ServiceType::getInstance()//服务框架单实例指针
#define REGIST_COMPONENT(_Component) ServiceType::CCompRegister<_Component> g_Comp//定义组件全局实例

#endif