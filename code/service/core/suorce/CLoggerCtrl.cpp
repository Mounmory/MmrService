#include <iostream>
#include <string>

#include "CLoggerCtrl.h"

using namespace mmrService::mmrCore;

CLoggerCtrl::CLoggerCtrl()
{

}

CLoggerCtrl::~CLoggerCtrl()
{

}

static const char logger_options[] = R"(
  -vl|--view logger         Viewing all component logger settings
  -sl|--set logger          set component logger
  -q|--quit                 quit
)";

void print_log_opt()
{
	printf("Options:%s", logger_options);
}

void CLoggerCtrl::loop(std::atomic_bool& bRunFlag)
{
	print_log_opt();
	while (bRunFlag.load(std::memory_order_relaxed))
	{
		std::string strCmd;
		printf("logger> ");
		std::getline(std::cin, strCmd);

		auto pos = strCmd.find("-vl");
		if (strCmd.find("-vl") == 0)
		{
			printf("Index\tLogLevel\tComponentName\n");
			for (const auto& iterLogger : m_mapCompLogger)
			{
				auto LoggerPtr = iterLogger.second.second.lock();
				if (LoggerPtr)
					printf("%d\t%d\t\t%s\n",iterLogger.first, static_cast<int>(LoggerPtr->logLevel), iterLogger.second.first.c_str());
				//else
				//	printf("-1\t-1\t%s\\n", iterLogger.second.first.c_str());
			}
		}
		else if (strCmd.find("-sl") == 0)
		{
			printf("Index\tLogLevel\tComponentName\n");
			for (const auto& iterLogger : m_mapCompLogger)
			{
				auto LoggerPtr = iterLogger.second.second.lock();
				if (LoggerPtr)
					printf("%d\t%d\t\t%s\n", iterLogger.first, static_cast<int>(LoggerPtr->logLevel), iterLogger.second.first.c_str());
				//else
				//	printf("-1\t-1\t%s\\n", iterLogger.second.first.c_str());
			}

			printf("set component log level like <index>:<level>. level -1 for log off.\n");
			printf("this modification is temporary.\n");
			
			printf("logger>-sl>");
			std::getline(std::cin, strCmd);
			auto pos = strCmd.find(':');
			if (pos > 0 && pos == (strCmd.size() - 2))//暂时不支持设置关闭
			{
				std::string compName(strCmd.begin(), strCmd.begin() + pos);
				int16_t sIndex = std::atoi(compName.c_str());
				auto iterLogger = m_mapCompLogger.find(sIndex);
				if (iterLogger != m_mapCompLogger.end())
				{
					auto LoggerPtr = iterLogger->second.second.lock();
					if (LoggerPtr)
					{
						int32_t level = static_cast<int8_t>(strCmd[pos + 1]) - 0x30 ;
						if (level >= -1 && level <= 5) //暂时不支持设置关闭
						{
							LoggerPtr->logLevel = static_cast<mmrUtil::emLogLevel>(level);
							printf("%s log level set %d success.\n", iterLogger->second.first.c_str(), level);
							continue;
						}
					}
				}
			}
			printf("unknown command! you can input [-vl] to view all component info.\n");
		}
		else if (strCmd == "-q")
		{
			printf("quit log setting \n");
			break;
		}
		else
		{
			printf("invalid command[%s]!\n", strCmd.c_str());
			print_log_opt();
		}
	}
}

