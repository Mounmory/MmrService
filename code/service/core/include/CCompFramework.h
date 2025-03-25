#ifndef CCOMPFRAMEWORK_H
#define CCOMPFRAMEWORK_H
#include "common/include/util/Clogger.h"
#include "service/interface/ComponentExport.h"
#include "service/interface/IComponent.h"
#include "service/core/include/ServiceCtrlPolicies.hpp"
#include "service/core/include/EventCtrlPolicies.h"

#include <set>

BEGINE_NAMESPACE(mmrService)
BEGINE_NAMESPACE(mmrCore)


template<typename ServiceCtrl/*服务管理策略*/
	, typename EventCtrl>/*事件管理策略*/
class COMPO_CORE_CLASS_API CCompFramework
{
	CCompFramework();
	~CCompFramework();
public:
	static CCompFramework<ServiceCtrl, EventCtrl>* getInstance()
	{
		static CCompFramework<ServiceCtrl, EventCtrl>* instalce = new CCompFramework<ServiceCtrl, EventCtrl>;
		return instalce;
	}

	void handleEvent(const mmrUtil::CVarDatas& varData);//处理回调信息

	void run(const std::string& strCfg);//运行程序，自动控制start和stop

	bool start(const std::string& strCfg);//配置文件路径

	void stop();

	//处理组件相关
	bool addComponent(std::unique_ptr<IComponent> pComp);

	//日志相关
	void addComponetLogWrapper(std::string strCompName, std::weak_ptr<mmrUtil::LogWrapper> logWrap);

	//程序控制相关
	void dealCmd();

	//服务相关
	template<typename _T>
	void registService(std::shared_ptr<_T>&& pSer)
	{
		m_upServPolicy->registService(std::forward<std::shared_ptr<_T>>(pSer));//自动推导模板参数
	}

	template<typename _T>
	auto getService() 
	{
		return m_upServPolicy->template getService<_T>();//显示标记为模板成员函数
	}

	//处理事件相关接口
	void addFunc(const std::string& strTopcs, const std::shared_ptr<CallbackFunc>& ptrFunc)
	{
		m_ptrEventCtrl->addFunc(strTopcs, ptrFunc);
	}

	void addEvenVartData(mmrUtil::CVarDatas&& varData)
	{
		m_ptrEventCtrl->addEvenVartData(std::forward<mmrUtil::CVarDatas>(varData));
	}

	template<typename CompType>
	class CCompRegister//组件自动注册类
	{
	public:
		CCompRegister()
		{
			std::unique_ptr<IComponent> compPtr = std::make_unique<CompType>();
			CCompFramework<ServiceCtrl, EventCtrl>::getInstance()->addComponent(std::move(compPtr));
		}
		~CCompRegister() = default;
	};

private:
	std::unique_ptr<ServiceCtrl> m_upServPolicy;//服务管理策略

	std::unique_ptr<EventCtrl> m_ptrEventCtrl;//订阅事件管理

	//处理组件相关
	std::unordered_map<std::string, std::unique_ptr<IComponent>> m_mapComponents;//所有组件模块
	std::set<libHandle> m_libHandl;//组件动态库handle

	struct CFrameData;//framework中的其它数据
	std::unique_ptr<CFrameData> m_ptrData;
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