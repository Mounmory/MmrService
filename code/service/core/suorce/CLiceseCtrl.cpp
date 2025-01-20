#include "service/core/include/CLiceseCtrl.h"
#include "service/core/include/CCompFramework.h"
#include "common/include/util/json.hpp"
#include "common/include/util/CVarDatas.hpp"

#include <sstream>

using namespace mmrService::mmrCore;

#define TRAIL_PERIO_MIN 10 //试用时间，分钟

CLicenseCtrl::CLicenseCtrl(std::string strFilePath, std::string strModule)
	: m_licObjPtr(std::make_unique<mmrUtil::CLicenseObj>(std::move(strFilePath),std::move(strModule)))
{

}

CLicenseCtrl::~CLicenseCtrl()
{
	releaseFuture();
}

void CLicenseCtrl::initLicense()
{
	if (!m_licObjPtr->parseLicenFile() || !m_licObjPtr->checkLicense())
	{
		STD_CERROR << "无授权或授权文件解析失败，程序将在" << TRAIL_PERIO_MIN << "分钟后退出。" << std::endl;
	}

	m_futureThread = std::async(std::launch::async, &CLicenseCtrl::licenseThread, this);
}

static const char license_options[] = R"(
  -vl|--view license        Viewing license info
  -rs|--reset license       Reset license info after update license file
  -q |--quit                quit
)";

void print_lic_opt()
{
	printf("Options:%s", license_options);
}


void CLicenseCtrl::loop(std::atomic_bool& bRunFlag)
{
	print_lic_opt();
	while (bRunFlag.load(std::memory_order_relaxed))
	{
		std::string strCmd;
		printf("license> ");
		std::getline(std::cin, strCmd);

		if (strCmd.find("-vl") == 0)
		{
			m_licObjPtr->printLicData(false);
		}
		else if (strCmd.find("-rs") == 0)
		{
			printf("license>-rs>Input license file path\n");
			printf("license>-rs>");
			std::getline(std::cin, strCmd);
			printf("reset license %s \n", reloadLicense(std::move(strCmd)) ? "success!" : "failed!");
		}
		else if (strCmd == "-q")
		{
			printf("quit license setting \n");
			break;
		}
		else
		{
			printf("invalid command[%s]!\n", strCmd.c_str());
			print_lic_opt();
		}
		
	}
}

bool CLicenseCtrl::reloadLicense(std::string strFilePath)
{
	std::unique_lock<std::mutex> lock(m_mutex);

	return m_licObjPtr->parseLicenFile(std::move(strFilePath))
		&& m_licObjPtr->checkLicense();
}

void CLicenseCtrl::licenseThread()
{
	std::unique_lock<std::mutex> lock(m_mutex);
	while (std::cv_status::timeout == m_cv.wait_for(lock, std::chrono::seconds(TRAIL_PERIO_MIN * 60)))
	{
		//LOG_INFO("license wait time out!");
		if (mmrUtil::emLicenseState::LICENSE_OK != m_licObjPtr->getLicenseState())
			break;

		if (!m_licObjPtr->checkLicense())
			break;
	}

	LOG_WARN("license thread quit whith %d", m_licObjPtr->getLicenseState());

	if (mmrUtil::emLicenseState::LICENSE_OK != m_licObjPtr->getLicenseState())
	{
		mmrUtil::CVarDatas vars;

		vars.setName("stop");
		std::stringstream ss;
		ss << "invalid license! license state " << static_cast<int>(m_licObjPtr->getLicenseState());
		vars.addVar("message", ss.str());
		LOG_WARN("send license ctrl info: %s", vars.getVar("message").getStringData().c_str());
		CoreFrameworkIns->addEvenVartData(std::move(vars));
	}
}

void CLicenseCtrl::releaseFuture() 
{
	if (m_futureThread.wait_for(std::chrono::milliseconds(0)) == std::future_status::timeout)
	{
		m_cv.notify_one();
		m_futureThread.get();
	}
}
