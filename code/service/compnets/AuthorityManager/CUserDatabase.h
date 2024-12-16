#ifndef CUSERDATABASE_H
#define CUSERDATABASE_H

#include "Common_def.h"
#include "SqliteCom.h"

BEGINE_NAMESPACE(mmrService)
BEGINE_NAMESPACE(mmrComp)

class CUserDatabase 
{
public:
	CUserDatabase();
	~CUserDatabase();

	bool init();

private:
	sqlite3* m_db;
};

END_NAMESPACE(mmrService)
END_NAMESPACE(mmrComp)
#endif