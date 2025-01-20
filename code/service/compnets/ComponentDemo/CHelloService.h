#ifndef CHELLOSERVICE_H
#define CHELLOSERVICE_H
#include "service/interface/iservice/ComponentDemo/IHelloService.h"
#include <iostream>


BEGINE_NAMESPACE(mmrService)
BEGINE_NAMESPACE(mmrComp)

class CHelloService :public IHelloService
{
public:
	CHelloService()
	{
		//std::cout << "CHelloService construct!" << std::endl;
	}
	~CHelloService() 
	{
		//std::cout << "CHelloService destruct!" << std::endl;
	}

	virtual void sayHello() override;
private:

};

END_NAMESPACE(mmrService)
END_NAMESPACE(mmrComp)
#endif