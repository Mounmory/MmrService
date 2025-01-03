#include "CCompFramework.h"

#include <list>
#include <functional>
#include <memory>
#include <iostream>

#include <chrono>
#include <mutex>
#include <condition_variable>


//framework框架事件注册与回调示例
class CEventDealler 
{
public:
	CEventDealler() 
	{
		//先进行数据初始化，确保回调在注册后，能够正常执行


		//注册第一个函数，事件回调函数中保存的是弱引用，因此一定要将回调指针保存到list中
		{
			auto func1 = std::make_shared<CallbackFunc>(std::bind(&CEventDealler::dealEvent1, this, std::placeholders::_1));
			CoreFrameworkIns->addFunc("strTopic", func1);
			m_listFuncs.emplace_back(std::move(func1));
		}

		//注册第二个函数
		{
			auto func2 = std::make_shared<CallbackFunc>(std::bind(&CEventDealler::dealEvent2, this, std::placeholders::_1));
			CoreFrameworkIns->addFunc("intTopic", func2);
			m_listFuncs.emplace_back(std::move(func2));
		}

	}

	~CEventDealler() 
	{
		//先清理回调函数
		m_listFuncs.clear();

		//释放其它资源


	}

	void dealEvent1(const mmrUtil::CVarDatas& varData)
	{
		if (varData.getName() == "strTopic")
		{
			if (varData.isContain("strValue"))
			{
				std::cout << "receive string event :" << varData.getVar("strValue").getStringData() << std::endl;
			}
		}
		cv.notify_all();
	}

	void dealEvent2(const mmrUtil::CVarDatas& varData)
	{
		if (varData.getName() == "intTopic")
		{
			if (varData.isContain("intValue"))
			{
				std::cout << "receive int event :" << varData.getVar("intValue").getInt32Data() << std::endl;
			}
		}
		cv.notify_all();
	}

	void waitForDealEvent() 
	{
		std::unique_lock<std::mutex> lock(m_mutex);
		cv.wait_for(lock, std::chrono::seconds(10));

	}
private:
	std::list<ptrCallbackFunc> m_listFuncs;
	std::condition_variable cv;
	std::mutex m_mutex;
};

int main(int argc ,char** argv)
{
	do 
	{
		//启动框架
		if (!CoreFrameworkIns->start("fmServTest.json"))
		{
			std::cout << "framework started failed!" << std::endl;
			break;
		}

		{
			CEventDealler evDealler;

			{//添加ComponentDemo模块中注册的事件
				mmrUtil::CVarDatas evDatas;
				evDatas.setName("info1");
				std::cout << "add component demon handler event!" << std::endl;;
				CoreFrameworkIns->addEvenVartData(std::move(evDatas));
			}

			{//添加string事件
				mmrUtil::CVarDatas evDatas;
				evDatas.setName("strTopic");
				evDatas.addVar("strValue", "This is an string event!");
				std::cout << "add string event!" << std::endl;;
				CoreFrameworkIns->addEvenVartData(std::move(evDatas));
				evDealler.waitForDealEvent();
			}

			{//添加int事件
				mmrUtil::CVarDatas evDatas;
				evDatas.setName("intTopic");
				evDatas.addVar("intValue", int(100));
				std::cout << "add int event!" << std::endl;;
				CoreFrameworkIns->addEvenVartData(std::move(evDatas));
				evDealler.waitForDealEvent();
			}
		}

		{//CEventDealler析构了，添加string事件，不会进行处理
			mmrUtil::CVarDatas evDatas;
			evDatas.setName("strTopic");
			evDatas.addVar("strValue", "This is an string event out scope!");
			std::cout << "add string event out scope!" << std::endl;;
			CoreFrameworkIns->addEvenVartData(std::move(evDatas));
		}

		CoreFrameworkIns->stop();
	} while (false);


	std::cout << "输入任意字符继续..." << std::endl;
	std::cin.get();
	return 0;
}



