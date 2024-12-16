#ifndef SQLITECOM_H
#define SQLITECOM_H

#include "sqlite3/sqlite3.h"

class StmLocker 
{
public:
	StmLocker(sqlite3_stmt*& stmt)
		: m_stmt(stmt)
	{
	}

	~StmLocker() 
	{
		sqlite3_finalize(m_stmt);
	}

private:
	sqlite3_stmt*& m_stmt;
};

#endif