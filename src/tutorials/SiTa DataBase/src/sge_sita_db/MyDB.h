#pragma once

#include "MyCommon.h"

#if sge_sita_db_EXPORTS
	#define MyDB_API __declspec(dllexport)
#else
	#define MyDB_API __declspec(dllimport)
	#pragma comment(lib, "sge_sita_db.lib")
#endif

namespace sge {

class Conn : public NonCopyable {
public:
	virtual ~Conn() = default;

	virtual void directExec(StrView sql) = 0;
	virtual void destroy() = 0;
};


MyDB_API UPtr<Conn> connectMySQL(StrView host, StrView db, StrView user, StrView password);
MyDB_API UPtr<Conn> connectSQLite3(StrView filename);

}
