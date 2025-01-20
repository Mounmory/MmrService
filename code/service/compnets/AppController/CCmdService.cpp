#include "CCmdService.h"
#include "service/core/include/CCompFramework.h"

using namespace mmrService::mmrComp;

static const char detail_options[] = R"(
  -h|--help                 Print this information
  -v|--version              Print version
  -c|--confile <confile>    Set configure file, default etc/{program}.conf
  -t|--test                 Test configure file and exit
  -s|--signal <signal>      Send <signal> to process,
                            <signal>=[start,stop,restart,status,reload]
  -p|--port <port>          Set listen port
  -q|--quit                 quit
)";

void print_help()
{
	printf("Options:\n%s\n", detail_options);
}

CCmdService::CCmdService()
{

}

CCmdService::~CCmdService()
{

}

void CCmdService::cmdLoop()
{
	print_help();

	//while (true)
	//{
	//	std::string strCmd;
	//	printf("> ");
	//	std::getline(std::cin, strCmd);

	//	if (strCmd == "-q")
	//	{
	//		CoreFrameworkIns->stop();
	//		printf("app stopped!\n");
	//		break;
	//	}
	//	else if (strCmd == "-h")
	//	{
	//		print_help();
	//	}
	//	else
	//	{
	//		printf("invalid command[%s]!\n", strCmd.c_str());
	//		print_help();
	//	}
	//}
}
