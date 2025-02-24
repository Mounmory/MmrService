#ifndef CLICENSEOBJ_H
#define CLICENSEOBJ_H

#include <fstream>
#include <string>
#include <vector>

#include "common/include/Common_def.h"
#include "common/include/util/UtilExport.h"
#include "common/include/util/CDataStream.hpp"
#include "common/include/util/json.hpp"

BEGINE_NAMESPACE(mmrUtil)

#define LICENSE_VER_10		"V1.0"//权限版本

//#define LICENSE_CODE_APP_NAME	"AppName"//权限数据字段
#define STR_LICENSE_CODE_VALID_TIME	"ValidTime"//权限可用时间
#define STR_LICENSE_CODE_LIC_CODE	"LicenseCode"//权限数据字段
#define STR_LICENSE_CODE_PC_ID	"PCId"//权限数据字段
#define STR_LICENSE_VER	"LicVer"//权限数据字段

#define STR_LICENSE_FILE_NAME "license.lic"
enum class emLicenseState : uint8_t
{
	LICENSE_OK = 0,
	LICENSE_NONE,//没有授权文件
	LICENSE_UPDATE,//更新授权文件
	LICENSE_OVERDUE//授权过期
};

class COMMON_CLASS_API CLicData 
{
public:
	CLicData()
		: m_strVer(LICENSE_VER_10)
		, m_strModule()
		, m_strPCID()
		, m_llValidTime(0)
		, m_llcurTime(0)
	{}

	~CLicData() = default;
	
	CLicData& operator = (const CLicData& rhs) 
	{
		if (this != &rhs) 
		{
			m_strModule = rhs.m_strModule;
			m_strPCID = rhs.m_strPCID;
			m_llValidTime = rhs.m_llValidTime;
			m_llcurTime = rhs.m_llcurTime;
		}
		return *this;
	}

	const std::string m_strVer;//版本
	std::string m_strModule;//授权模块
	std::string m_strPCID;//计算机唯一ID
	int64_t m_llValidTime;//有效日期
	int64_t m_llcurTime;//当前时间
};

class COMMON_CLASS_API CLicenseObj
{
public:
	CLicenseObj(std::string strFilePath, std::string strModule = "");

	~CLicenseObj();

	bool parseLicenFile(std::string strFilePath = "");

	bool checkLicense();

	const emLicenseState getLicenseState() const { return m_licState; }

	CLicData& getLicenseData() { return m_licData; }

	std::string& getModeleName() { return m_strModule; }

	bool parseLicenseCode(const std::string& strLicCode);

	const Json::Value& generateJsonCode();

	//打印授权信息，true写到日志，false打印到屏幕
	void printLicData(bool bToLog = true);

private:
	std::string generateLicenseCode();

	bool updateLicense();

private:
	emLicenseState	m_licState;
	CLicData		m_licData;

	std::string m_strFile;
	std::string m_strModule;
	Json::Value m_jvLicData;//授权文件中的Json内容
};


END_NAMESPACE(mmrUtil)

#endif