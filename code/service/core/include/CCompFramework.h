#ifndef CCOMPFRAMEWORK_H
#define CCOMPFRAMEWORK_H
#include <set>
#include <mutex>
#include <queue>
#include <atomic>
#include <memory>
#include <condition_variable>



#include "service/interface/ComponentExport.h"
//#include "IEventHandler.h"
#include "service/interface/IComponent.h"
#include "CLoggerCtrl.h"
#include "CLiceseCtrl.h"

#include "service/core/include/ServiceCtrlPolicies.hpp"
#include "EventCallbacks.h"

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

template<typename ServiceCtrl/*服务管理策略*/
	, typename HandlerCtrl>
class COMPO_CORE_CLASS_API CCompFramework
{
	CCompFramework();
	~CCompFramework();
public:
	static CCompFramework<ServiceCtrl, HandlerCtrl>* getInstance()
	{
		static CCompFramework<ServiceCtrl, HandlerCtrl>* instalce = new CCompFramework<ServiceCtrl, HandlerCtrl>;
		return instalce;
	}

	bool start(const std::string& strCfgFile = "");//配置文件路径

	void stop();

	//处理组件相关
	bool addComponent(std::unique_ptr<IComponent> pComp);

	//void removeComponet(uint16_t usIndex);


	//日志相关
	void addComponetLogWrapper(std::string strCompName, std::weak_ptr<mmrUtil::LogWrapper> logWrap);

	void loggerCtrlLoop(std::atomic_bool& bRunFlag);

	void licenseCtrlLoop(std::atomic_bool& bRunFlag);

	//服务相关
#if GCC_VER_OVER_5
	//在gcc7.5.0及9.4.0中，模板函数A内调用模板函数B，显式指定模板类型会编译失败，智能靠参数推导确定模板参数
	template<typename _T>
	void registService(std::shared_ptr<_T>&& pSer)
	{
		m_upServPolicy->registService(std::forward<std::shared_ptr<_T>>(pSer));
	}

	template<typename _T>
	std::shared_ptr<_T> getService()
	{
		std::shared_ptr<_T> retPtr = nullptr;
		m_upServPolicy->getService(retPtr);
		return retPtr;
	}
#else
	//服务相关
	template<typename _T>
	void registService(std::shared_ptr<_T>&& pSer)
	{
		m_upServPolicy->registService<_T>(std::forward<std::shared_ptr<_T>>(pSer));
	}

	template<typename _T>
	std::shared_ptr<_T> getService() 
	{
		return m_upServPolicy->getService<_T>();
	}
#endif




	//处理事件相关接口
	void addFunc(const std::string& strTopcs, const std::shared_ptr<CallbackFunc>& ptrFunc)
	{
		m_ptrHandlerCtrl->addFunc(strTopcs, ptrFunc);
	}

	void addEvenVartData(mmrUtil::CVarDatas&& varData)
	{
		m_ptrHandlerCtrl->addEvenVartData(std::forward<mmrUtil::CVarDatas>(varData));
	}

	template<typename CompType>
	class CCompRegister//组件自动注册类
	{
	public:
		CCompRegister()
		{
			std::unique_ptr<IComponent> compPtr = std::make_unique<CompType>();
			CCompFramework<ServiceCtrl, HandlerCtrl>::getInstance()->addComponent(std::move(compPtr));
		}
		~CCompRegister() = default;
	};

private:
	std::unique_ptr<CLoggerCtrl> m_loggerCtrl;//组件日志控制类

	std::unique_ptr<CLicenseCtrl> m_licenseCtrl;//权限控制类

	std::unique_ptr<ServiceCtrl> m_upServPolicy;//服务管理策略

	std::unique_ptr<HandlerCtrl> m_ptrHandlerCtrl;//订阅事件管理

	//处理组件相关
	std::unordered_map<std::string, std::unique_ptr<IComponent>> m_mapComponents;
	std::set<libHandle> m_libHandl;
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

using FramHandlerPolicy = mmrService::mmrCore::CEventDealWithLock;//使用带锁的观察者

template class COMPO_CORE_CLASS_API mmrService::mmrCore::CCompFramework<FramServicePolicy, FramHandlerPolicy>;//到处模板类实例

using ServiceType = mmrService::mmrCore::CCompFramework<FramServicePolicy, FramHandlerPolicy>;

#define CoreFrameworkIns ServiceType::getInstance()//服务框架单实例指针
#define REGIST_COMPONENT(_Component) ServiceType::CCompRegister<_Component> g_Comp//定义组件全局实例

#endif