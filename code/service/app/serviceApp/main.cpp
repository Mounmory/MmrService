#include "common/include/Common_def.h"
#include "common/include/util/UtilFunc.h"
#include "common/include/util/CLicenseObj.h"
#include "common/include/util/Clogger.h"
#include "service/interface/iservice/ComponentDemo/IHelloService.h"
#include "service/interface/iservice/AppController/ICmdService.h"
#include "service/core/include/CCompFramework.h"
#include "service/app/serviceApp/CAppControler.h"

#include <iostream>

using namespace mmrService::mmrApp;

int main(int argc, char** argv)
{
	std::cout << "\t" << mmrUtil::getFileName(argv[0]) << " start...." << std::endl;
	//启动框架
	std::cout << "********************************************" << std::endl;
	std::cout << "Complied time: \t" << mmrUtil::getComplieTime() << std::endl;
	std::cout << "Build type: \t" << BUILD_TYPE << std::endl;
	std::cout << "System type: \t" << OS_TYPE << std::endl;
	std::cout << "********************************************" << std::endl;

#ifdef OS_MMR_LINUX
	////修改core文件格式和位置
	//int result = system(R"(ulimit -c unlimited)");
	//if (result != 0)
	//	std::cout << "set core dump file size failed!" << std::endl;
	////修改core文件名称
	//result = system(R"(echo "core-%h-%e-%p-%t" > /proc/sys/kernel/core_pattern)");
	//if (result != 0)
	//	std::cout << "set core dump file name failed!" << std::endl;
#endif

	CAppControler appCtl;

	appCtl.run();

	std::cout << "输入任意字符继续..." << std::endl;
	std::cin.get();
	return 0;
}

