#include "common/include/util/UtilFunc.h"
#include "service/core/include/CCompFramework.h"
#include <iostream>


int main(int argc, char** argv)
{
	std::cout << "\t" << mmrUtil::getFileName(argv[0]) << " start...." << std::endl;
	//启动框架
	std::cout << "********************************************" << std::endl;
	std::cout << "Complied time: \t" << mmrUtil::getComplieTime() << std::endl;
	std::cout << "Complier: \t" << mmrUtil::getComplierInfo() << std::endl;
	std::cout << "Build type: \t" << STR_BUILD_TYPE << std::endl;
	std::cout << "System type: \t" << STR_OS_TYPE << std::endl;
	std::cout << "********************************************" << std::endl;

	CoreFrameworkIns->run("serviceApp");

	std::cout << "输入任意字符继续..." << std::endl;
	std::cin.get();
	return 0;
}

