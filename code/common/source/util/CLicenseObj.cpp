#include "util/CLicenseObj.h"
#include "util/BufConvert.h"
#include "util/Clogger.h"
#include "util/UtilFunc.h"
#include <iostream>

#define LICENSE_CODE_MIN_LEN 32//授权码最小长度

using namespace mmrUtil;

CLicenseObj::CLicenseObj(std::string strFilePath, std::string strModule /*= "license"*/)
	: m_licState(emLicenseState::LICENSE_NONE)
	, m_strFile(std::move(strFilePath))
	, m_strModule(std::move(strModule))
{
	m_strFile = m_strFile + m_strModule + ".lic";
}

CLicenseObj::~CLicenseObj()
{

}

bool CLicenseObj::parseLicenFile(std::string strFilePath /*= ""*/)
{
	bool bRet = false;
	do 
	{
		const std::string& strRef = strFilePath.empty() ? m_strFile : strFilePath;

		Json::Value jsonRoot;
		std::string strErr = Json::json_from_file(strRef, jsonRoot);
		//parse the license file and get license info here!







		printLicData();

		//m_jvLicData = std::move(jsonRoot);
		//bRet = true;
	} while (false);
	return bRet;
}

bool CLicenseObj::checkLicense()
{
	//检查权限状态，并更新授权文件
	bool bRet = false;
	do
	{
		//check license code here!!





		//m_licState = emLicenseState::LICENSE_OK;
		//bRet = true;
	} while (false);
	return bRet;
}

bool CLicenseObj::parseLicenseCode(const std::string& strLicCode)
{
	bool bRet = false;
	std::string strBuf /*= mmrUtil::base64DecodeToByte(strLicCode.data(), strLicCode.size())*/;

	do 
	{
		if (strBuf.size() < LICENSE_CODE_MIN_LEN)
		{
			STD_CERROR << "授权码[" << strLicCode << "]不是有效长度！" << std::endl;
			break;
		}
		//parse you license code here!








		//bRet = true;
	} while (false);

	return bRet;
}

const Json::Value& CLicenseObj::generateJsonCode()
{
	m_jvLicData.clear();
	//construct json here!




	
	return m_jvLicData;
}

void CLicenseObj::printLicData(bool bToLog /*= true*/)
{
	//print your license info here!
	std::stringstream ss;
	ss << "License info:\n";

	







	if (true == bToLog)
	{
		LOG_INFO("%s", ss.str().c_str());
	}
	else
	{
		std::cout << ss.str() << std::endl;
	}
}

std::string CLicenseObj::generateLicenseCode()
{




	return "";
}

bool CLicenseObj::updateLicense()
{
	bool bRet = false;
	do 
	{
		if (m_jvLicData.IsNull())//正常不会出现
		{
			STD_CERROR << "error! license file jv value is null!" << std::endl;
			break;
		}

		m_jvLicData[m_strModule][LICENSE_CODE_LIC_CODE] = generateLicenseCode();

		//写入文件
		std::ofstream output;
		output.open(m_strFile, std::ofstream::out);
		if (output.is_open())
		{
			output << m_jvLicData.dumpStyle() << std::endl;
			output.close();
		}

		printLicData();

		bRet = true;
	} while (false);

	return bRet;
}

