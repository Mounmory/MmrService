/**
 * @file CCompFramework.h
 * @brief 服务框架
 * @author Mounmory (237628106@qq.com) https://github.com/Mounmory
 * @date 
 *
 * 
 */
 
#ifndef MMR_CORE_COMP_FRAMWORK_H
#define MMR_CORE_COMP_FRAMWORK_H
#include <common/include/util/Clogger.h>
#include <service/interface/ComponentExport.h>
#include <service/interface/IComponent.h>
#include <service/core/include/ServiceCtrlPolicies.hpp>
#include <service/core/include/EventCtrlPolicies.h>
#include <common/include/general/PolicySelector.hpp>

#include <set>

BEGINE_NAMESPACE(mmrService)
BEGINE_NAMESPACE(mmrCore)

struct FramworkPolicy 
{
	using MajorClass = FramworkPolicy;

	//服务控制策略
	struct ServTypeCate
	{
		using PTypeID = MapServKeyByTypeID;
		using PIndex = VecServByIndex;
		using PGuid = UnMapServKeyByGUID;
	};
#ifdef OS_MMR_WIN
	using Serv = ServTypeCate::PGuid;
#else
	using Serv = ServTypeCate::PIndex;
#endif
	//事件控制策略
	struct EventTypeCate
	{
		using  PWithLock = CEventDealWithLock;
	};
	using Event = EventTypeCate::PWithLock;

	struct IsSerValueCate;
	static constexpr bool IsSer = true;
};

#include <common/include/general/PolicyMicroBegin.h>
TypePolicyObj(PServTypeid, FramworkPolicy, Serv, PTypeID);
TypePolicyObj(PServIndex, FramworkPolicy, Serv, PIndex);
TypePolicyObj(PServGuid, FramworkPolicy, Serv, PGuid);
ValuePolicyObj(PNotSer, FramworkPolicy, IsSer, false);
#include <common/include/general/PolicyMicroEnd.h>

template<typename... TPolicies>
class COMPO_CORE_CLASS_API CCompFramework
{
	using TPoliCont = mmrComm::PolicyContainer<TPolicies...>;
	using TPolicyRes = mmrComm::PolicySelect<FramworkPolicy, TPoliCont>;


	using ServiceCtrl = typename TPolicyRes::Serv;
	using EventCtrl = typename TPolicyRes::Event;
	static constexpr bool is_Server = TPolicyRes::IsSer;

	CCompFramework();
	~CCompFramework();
public:
	static CCompFramework<TPolicies...>* getInstance()
	{
		static CCompFramework<TPolicies... >* instalce = new CCompFramework<TPolicies...>;
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
			CCompFramework<TPolicies...>::getInstance()->addComponent(std::move(compPtr));
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

//导出模板类实例
template class COMPO_CORE_CLASS_API mmrService::mmrCore::CCompFramework<>;
template class COMPO_CORE_CLASS_API mmrService::mmrCore::CCompFramework<mmrService::mmrCore::PNotSer>;

using ServiceType = mmrService::mmrCore::CCompFramework<>;

#define CoreFrameworkIns ServiceType::getInstance()//服务框架单实例指针
#define REGIST_COMPONENT(_Component) ServiceType::CCompRegister<_Component> g_Comp//定义组件全局实例

#endif
