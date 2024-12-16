#ifndef CHANDLERDEMO_H
#define CHANDLERDEMO_H
#include "IEventHandler.h"
#include "CCompFramework.h"
#include <iostream>
#include <vector>
#include <string>


BEGINE_NAMESPACE(mmrService)
BEGINE_NAMESPACE(mmrComp)

class CHandlerDemo : public IEventHandler
{
public:
	CHandlerDemo()
	:IEventHandler()
	{

		HANDLER_REGIST_TOPICS(std::vector<std::string>({"info1", "info2"}));
		//std::cout << "CHandlerDemo construct " << std::endl;
	}

	~CHandlerDemo() {
		HANDLER_REMOVE_TOPICS(std::vector<std::string>({ "info1", "info2" }));
		//std::cout << "CHandlerDemo destruct " << std::endl;
	}

	virtual void handleEvent(const mmrUtil::CVarDatas& varData) override 
	{
		if (varData.getName() == "info1")
		{
			//std::cout << "CHandlerDemo deal data " << varData.getName() << std::endl;
		}
		else if (varData.getName() == "info2")
		{
			//std::cout << "CHandlerDemo deal data " << varData.getName() << std::endl;
		}
	}
private:
	char m_char[1024];
};

END_NAMESPACE(mmrService)
END_NAMESPACE(mmrComp)
#endif
