#include "service/core/include/CCompFramework.h"

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


		//在构造函数中添加回调，事件回调函数中保存的是弱引用，因此一定要将回调指针保存到list中
		{
			auto func1 = std::make_shared<CallbackFunc>(std::bind(&CEventDealler::dealEvenOne, this, std::placeholders::_1));
			CoreFrameworkIns->addFunc("strTopic", func1);
			m_listFuncs.emplace_back(std::move(func1));
		}
	}

	~CEventDealler() 
	{
		//先清理回调函数
		m_listFuncs.clear();

		//释放其它资源


	}

	void dealEvenOne(const mmrUtil::CVarDatas& varData)
	{
		if (varData.getName() == "strTopic")
		{
			if (varData.isContain("strValue"))
			{
				std::cout << "deal event one receive string event :" << varData.getVar("strValue").getStringData() << std::endl;
			}
		}
		else if (varData.getName() == "intTopic")
		{
			if (varData.isContain("intValue"))
			{
				std::cout << "deal event one receive int event :" << varData.getVar("intValue").getInt32Data() << std::endl;
			}
		}
		cv.notify_all();
	}

	void dealEventTwo(const mmrUtil::CVarDatas& varData)
	{
		if (varData.getName() == "strTopic")
		{
			if (varData.isContain("strValue"))
			{
				std::cout << "deal event two receive string event :" << varData.getVar("strValue").getStringData() << std::endl;
			}
		}
		else if (varData.getName() == "intTopic")
		{
			if (varData.isContain("intValue"))
			{
				std::cout << "deal event two receive int event :" << varData.getVar("intValue").getInt32Data() << std::endl;
			}
		}
		cv.notify_all();
	}

	void waitForDealEvent() 
	{
		//std::unique_lock<std::mutex> lock(m_mutex);
		//cv.wait_for(lock, std::chrono::seconds(10));
		std::this_thread::sleep_for(std::chrono::seconds(1));

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
		if (!CoreFrameworkIns->start("fmServTest"))
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

			//添加成员函数到回调
			auto funcClassMemberOne = std::make_shared<CallbackFunc>(std::bind(&CEventDealler::dealEventTwo, &evDealler, std::placeholders::_1));
			CoreFrameworkIns->addFunc("intTopic", funcClassMemberOne);
			CoreFrameworkIns->addFunc("strTopic", funcClassMemberOne);


			{//添加int事件进入 CEventDealler::dealEvenOne
				mmrUtil::CVarDatas evDatas;
				evDatas.setName("intTopic");
				evDatas.addVar("intValue", int(100));
				std::cout << "add int event!" << std::endl;;
				CoreFrameworkIns->addEvenVartData(std::move(evDatas));
				evDealler.waitForDealEvent();
			}

			{//添加string事件 CEventDealler::dealEvenOne和CEventDealler::dealEvenTwo同时处理
				mmrUtil::CVarDatas evDatas;
				evDatas.setName("strTopic");
				evDatas.addVar("strValue", "This is an string event!");
				std::cout << "add string event!" << std::endl;;
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



