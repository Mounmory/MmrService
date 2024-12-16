#include "Common_def.h"

#include <iostream>
#include "Common_def.h"
#include "util/UtilFunc.h"
#include "util/CLicenseObj.h"
#include "util/Clogger.h"
#include "ComponentDemo/IHelloService.h"
#include "AppController/ICmdService.h"
#include "core/include/CCompFramework.h"
#include "CAppControler.h"

using namespace mmrService::mmrApp;

int main(int argc, char **argv)
{

	//启动框架
	std::cout << "********************************************" << std::endl;
	std::cout << "\t" << mmrUtil::getFileName(argv[0]) << " start...." << std::endl;
	std::cout << "\t" << "complied time: " << mmrUtil::getComplieTime() << std::endl;
	std::cout << "********************************************" << std::endl;

	CAppControler appCtl;

	appCtl.run();
	
	std::cout << "输入任意字符继续..." << std::endl;
	std::cin.get();
	return 0;
}

