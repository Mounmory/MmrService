#include "CUserDatabase.h"
#include "util/FileOperator.h"
#include "util/UtilFunc.h"
#include "util/Clogger.h"

using namespace mmrService::mmrComp;

static const char* g_usrDBName = "service_usr_info.db";

CUserDatabase::CUserDatabase()
	: m_db(nullptr)
{

}

CUserDatabase::~CUserDatabase()
{
	if (m_db)
	{
		sqlite3_close(m_db);
	}
}

bool CUserDatabase::init()
{
	bool bRet = false;
	do 
	{
		std::string strFilePath, strName;
		mmrUtil::getAppPathAndName(strFilePath, strName);
		strFilePath += "data/";
		if (!mmrUtil::creatDirIfNotExist(strFilePath.c_str()))
		{
			LOG_ERROR("create databse folder %s failed.", strFilePath.c_str());
			break;
		}
		strFilePath += g_usrDBName;

		int statu = sqlite3_open_v2(strFilePath.c_str(), &m_db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, 0);

		if (SQLITE_OK != statu)
		{
			LOG_ERROR("open databse file %s failed.", strFilePath.c_str());
			break;
		}

		bRet = true;
	} while (false);
	return bRet;
}
