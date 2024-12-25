#include <iostream>
#include "CAppControler.h"
#include "core/include/CCompFramework.h"
#include <future>

using namespace mmrService::mmrApp;

static const char detail_options[] = R"(
  -h |--help                 Print this information
  -lg|--log                  Component logger settings and viewing
  -lc|--license              License info setting and viewing
  -q |--quit                 quit
)";

void print_help() 
{
	printf("Options:%s", detail_options);
}

CAppControler::CAppControler()
{
	CoreFrameworkIns->addHandler("stop", this);
}

CAppControler::~CAppControler()
{
	CoreFrameworkIns->removeHandler("stop", this);
}

void CAppControler::run()
{
	if (CoreFrameworkIns->start())
	{
		m_bRunflag.store(true);
		std::thread(&CAppControler::dealCmd, this).detach();
		//std::future<void> ft = std::async(std::launch::async, &CAppControler::dealCmd, this);
		std::unique_lock<std::mutex> locker(m_mutex);
		m_cond.wait(locker, [&]() {return !m_bRunflag.load(); });
		//ft.get();
		CoreFrameworkIns->stop();
	}
	else 
	{
		std::cout << "Core framework start failed!\n";
	}
	//m_bRunflag.store(false,std::memory_order_relaxed);
}

void CAppControler::dealCmd()
{
	print_help();

	while (m_bRunflag.load(std::memory_order_relaxed))
	{
		std::string strCmd;
		printf("> ");
		std::getline(std::cin, strCmd);

		if (strCmd == "-h")
		{
			print_help();

			////测试通过handler停止
			//mmrUtil::CVarDatas vars;
			//vars.setName("stop");
			//vars.addVar("message", "stop by event test!");
			//CoreFrameworkIns->addEvenVartData(std::move(vars));
		}
		else if (strCmd == "-lg")
		{
			CoreFrameworkIns->loggerCtrlLoop(m_bRunflag);
		}
		else if (strCmd == "-lc")
		{
			CoreFrameworkIns->licenseCtrlLoop(m_bRunflag);
		}
		else if (strCmd == "-q")
		{
			printf("qiut the application?[Y/N]\n");
			printf("> ");
			std::getline(std::cin, strCmd);

			if ("Y" == strCmd || "y" == strCmd)
			{
				printf("app stopped!\n");
				break;
			}
			else 
			{
				print_help();
			}
		}
		else
		{
			printf("invalid command[%s]!\n", strCmd.c_str());
			print_help();
		}
	}
	m_bRunflag.store(false, std::memory_order_relaxed);
	m_cond.notify_one();
}

void CAppControler::handleEvent(const mmrUtil::CVarDatas& varData)
{
	if (varData.getName() == "stop")
	{
		const std::string& strMsg = varData.getVar("message").getStringData();
		std::cout << "stop app message : " << strMsg << std::endl;

		m_bRunflag.store(false, std::memory_order_relaxed);
		m_cond.notify_one();
	}
}
