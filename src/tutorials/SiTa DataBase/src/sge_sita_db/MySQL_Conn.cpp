#include "MyDB.h"

#include <mysql.h>
#pragma comment(lib, "libmysql.lib")

namespace sge {

class MySQL_Conn : public Conn {
public:
	MySQL_Conn(StrView host, StrView db, StrView user, StrView password);
	~MySQL_Conn();

	virtual void directExec(StrView sql) override;

private:
	MYSQL* _conn = nullptr;
};

MySQL_Conn::MySQL_Conn(StrView host, StrView db, StrView user, StrView password) {
	_conn = mysql_init(nullptr);
	if (!_conn)
		throw SGE_ERROR("mysql_init _conn is null");

	TempString szHost(host);
	TempString szDB(db);
	TempString szUser(user);
	TempString szPassword(password);

	if (nullptr == mysql_real_connect(_conn, szHost.c_str(), szUser.c_str(), szPassword.c_str(), szDB.c_str(), 0, nullptr, 0)) {
		throw SGE_ERROR(mysql_error(_conn));
	}
}

MySQL_Conn::~MySQL_Conn() {
	if (_conn) {
		mysql_close(_conn);
		_conn = nullptr;
	}
}
 
void MySQL_Conn::directExec(StrView sql) {
	TempString s(sql);
	if (mysql_query(_conn, s.c_str())) {
		throw SGE_ERROR(mysql_error(_conn));
	}
}

Conn* connectMySQL(StrView host, StrView db, StrView user, StrView password) {
	return new MySQL_Conn(host, db, user, password);
}

}