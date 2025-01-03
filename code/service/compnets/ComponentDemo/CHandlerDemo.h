#ifndef CHANDLERDEMO_H
#define CHANDLERDEMO_H
//#include "IEventHandler.h"
#include "CCompFramework.h"
#include <iostream>
#include <vector>
#include <string>
#include <list>


BEGINE_NAMESPACE(mmrService)
BEGINE_NAMESPACE(mmrComp)

class CHandlerDemo
{
public:
	CHandlerDemo()
	{
		//先进行其他初始化


		//注册第一个函数
		auto func1 = std::make_shared<CallbackFunc>(std::bind(&CHandlerDemo::dealInfo1Func, this, std::placeholders::_1));
		CoreFrameworkIns->addFunc("info1", func1);
		m_listCallbackFun.emplace_back(std::move(func1));
		//注册第二个函数
		auto func2 = std::make_shared<CallbackFunc>(std::bind(&CHandlerDemo::dealInfo2Func, this, std::placeholders::_1));
		CoreFrameworkIns->addFunc("info1", func2);
		m_listCallbackFun.emplace_back(std::move(func1));
	}

	~CHandlerDemo() 
	{
		//在析构函数中，首先清空回调，让回调失效
		m_listCallbackFun.clear();

		//释放其他资源
	}

	void dealInfo1Func(const mmrUtil::CVarDatas& varData) 
	{
		if (varData.getName() == "info1")
		{
			std::cout << "CHandlerDemo deal data " << varData.getName() << std::endl;
		}
	}
	void dealInfo2Func(const mmrUtil::CVarDatas& varData)
	{
		if (varData.getName() == "info2")
		{
			std::cout << "CHandlerDemo deal data " << varData.getName() << std::endl;
		}
	}

private:
	std::list<std::shared_ptr<CallbackFunc>> m_listCallbackFun;
	char m_char[1024];
};

END_NAMESPACE(mmrService)
END_NAMESPACE(mmrComp)
#endif
