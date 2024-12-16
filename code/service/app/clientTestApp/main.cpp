#include "Common_def.h"

#include <chrono>
#include <iostream>
#include "Common_def.h"
#include "util/UtilFunc.h"
#include "util/CLicenseObj.h"
#include "util/Clogger.h"
#include "ComponentDemo/IHelloService.h"
#include "AppController/ICmdService.h"
#include "core/include/CCompFramework.h"

#include "Login.pb.h"

#include "CProtoRpcClient.h"
#include "util/Clogger.h"

#define MAX_STR_LEN 1024

using MessageLoginRequstPtr = std::shared_ptr<mmrService::LoginRequest>;

using MessageLoginResponsePtr = std::shared_ptr<mmrService::LoginResponse>;

int main(int argc, char **argv)
{
	
	logInstancePtr->start();

	CProtoRpcClient protoClient;
	//protoClient.connect("127.0.0.1", 30010);
	protoClient.connect("192.168.43.23", 30010);

	std::this_thread::sleep_for(std::chrono::microseconds(100));

	std::cout <<"after connect" << std::endl;

	if (protoClient.getConnectState() == CProtoRpcClient::kConnected)
	{
		std::cout << "connect success!" << std::endl;

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

	std::cout << "输入任意字符继续..." << std::endl;
	std::cin.get();
	protoClient.stop();
	logInstancePtr->stop();
	return 0;
}

