#include "common/include/Common_def.h"
#include "common/include/util/Clogger.h"
#include "common/include/util/UtilFunc.h"
#include "common/include/util/CLicenseObj.h"
//#include "service/interface/iservice/ComponentDemo/IHelloService.h"
//#include "service/interface/iservice/AppController/ICmdService.h"

#include "service/app/clientTestApp/CProtoRpcClient.h"

#include "Login.pb.h"

#include <chrono>
#include <iostream>


#define MAX_STR_LEN 1024

using MessageLoginRequstPtr = std::shared_ptr<mmrService::LoginRequest>;

using MessageLoginResponsePtr = std::shared_ptr<mmrService::LoginResponse>;

int main(int argc, char **argv)
{
	
	if (argc == 3)
	{
		logInstancePtr->start();

		CProtoRpcClient protoClient;
		std::string strIP = argv[1];
		uint16_t usPort = std::atoi(argv[2]);

		protoClient.connect(strIP.c_str(), usPort);
		//protoClient.connect("192.168.43.23", 30010);

		protoClient.waitConnecting();

		//std::cout <<"after connect" << std::endl;

		if (protoClient.getConnectState() == CProtoRpcClient::kConnected)
		{
			std::cout << "连接成功" << std::endl;

			MessageLoginRequstPtr requetPtr = nullptr;
			MessageLoginResponsePtr responsePtr = nullptr;


			requetPtr = std::make_shared<mmrService::LoginRequest>();
			requetPtr->set_username("admin");
			requetPtr->set_password("123");

			auto rcvPtr = protoClient.call(requetPtr);
			responsePtr = std::dynamic_pointer_cast<mmrService::LoginResponse>(rcvPtr);
			if (responsePtr)
			{
				std::cout << "response is" << std::endl;
				std::cout << responsePtr->DebugString() << std::endl;
				std::string strMsg;
				mmrUtil::utf8ToLocalString(responsePtr->message(), strMsg);
				std::cout << "message " << strMsg << std::endl;
			}
			else
			{
				std::cout << "error response." << std::endl;
			}
		}
		else
		{
			std::cout << "连接失败" << std::endl;
		}
		protoClient.stop();
		logInstancePtr->stop();
	}
	else 
	{
		std::cout << "使用命令行输入IP和端口，如：127.0.0.1 30010" << std::endl;
	}

	std::cout << "输入任意字符继续..." << std::endl;
	std::cin.get();

	return 0;
}

