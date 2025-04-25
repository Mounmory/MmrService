#ifndef CUSERDATABASE_H
#define CUSERDATABASE_H

#include "common/include/Common_def.h"
#include "common/include/general/SqliteCom.h"

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