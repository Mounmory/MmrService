#include "service/interface/iservice/ComponentDemo/IHelloService.h"
#include "service/core/include/CCompFramework.h"

#include <iostream>

int main(int argc, char **argv)
{
	for (uint32_t index = 0 ;index < 1; ++index) 
	{
		CoreFrameworkIns->start("fmServTest");

		{
			auto helloSer = CoreFrameworkIns->getService<mmrService::mmrComp::IHelloService>();

			if (helloSer)
			{
				helloSer->sayHello();
				std::cout << "test index " << index << "..." << std::endl;
			}
			else
			{
				std::cout << "IHelloService is null..." << std::endl;
			}
		}

		CoreFrameworkIns->stop();
	}

	std::cout << "输入任意字符继续..." << std::endl;
	std::cin.get();
	return 0;
}

