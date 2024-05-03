#include "MyDB.h"

#include <mysql.h>
#pragma comment(lib, "libmysql.lib")

namespace sge {

class MySQL_Conn : public Conn {
public:
	MySQL_Conn(StrView host, StrView db, StrView user, StrView password);
	~MySQL_Conn() { destroy(); }

	virtual void directExec(StrView sql) override;
	virtual void destroy() override;

	MYSQL* _conn = nullptr;

protected:
	virtual UPtr<Stmt> onCreateStmt(StrView sql) override;
	virtual UPtr<Stmt> onCreateStmt(StrViewW sql) override;
};

SPtr<Conn> connectMySQL(StrView host, StrView db, StrView user, StrView password) { return new MySQL_Conn(host, db, user, password); }

class MySQL_Stmt : public Stmt {
public:
	MySQL_Stmt(MySQL_Conn* conn, StrView sql);
	MySQL_Stmt(MySQL_Conn* conn, StrViewW sql);
	~MySQL_Stmt() { destroy(); }

	void destroy();

protected:
	virtual void onExec(ExecParam* params, size_t n) override;

private:
	void _step();

	SPtr<MySQL_Conn> _conn;
	MYSQL_STMT* _stmt = nullptr;
};

UPtr<Stmt> MySQL_Conn::onCreateStmt(StrView sql)  { return UPtr<MySQL_Stmt>(new MySQL_Stmt(this, sql)); }
UPtr<Stmt> MySQL_Conn::onCreateStmt(StrViewW sql) { return UPtr<MySQL_Stmt>(new MySQL_Stmt(this, sql)); }

MySQL_Stmt::MySQL_Stmt(MySQL_Conn* conn, StrView sql)
	: _conn(conn)
{
	if (!_conn)
		throw SGE_ERROR("_conn is null");

	_stmt = mysql_stmt_init(_conn->_conn);
	if (!_stmt)
		throw SGE_ERROR("mysql_stmt_init");

	TempString sql_(sql);
	if (0 != mysql_stmt_prepare(_stmt, sql_.c_str(), static_cast<unsigned long>(sql.size()))) {
		throw SGE_ERROR(mysql_stmt_error(_stmt));
	}
}

MySQL_Stmt::MySQL_Stmt(MySQL_Conn* conn, StrViewW sql)
	: _conn(conn)
{
	if (!_conn)
		throw SGE_ERROR("_conn is null");

	_stmt = mysql_stmt_init(_conn->_conn);
	if (!_stmt)
		throw SGE_ERROR("mysql_stmt_init: out of memory");

	TempStringW s(sql);
	TempString sql_;
	UtfUtil::convert(sql_, s);
	if (0 != mysql_stmt_prepare(_stmt, sql_.c_str(), static_cast<unsigned long>(sql.size()))) {
		throw SGE_ERROR(mysql_stmt_error(_stmt));
	}
}

void MySQL_Stmt::destroy() {
	if (_stmt) {
		if (0 != mysql_stmt_close(_stmt)) {
			throw SGE_ERROR(mysql_stmt_error(_stmt));
		}
		_stmt = nullptr;
	}
}

void MySQL_Stmt::onExec(ExecParam* params, size_t n) {
	if (!_stmt)
		throw SGE_ERROR("_stmt is null");

	if (n != mysql_stmt_param_count(_stmt))
		throw SGE_ERROR("incorrect parameter count");

	Vector<MYSQL_BIND> binds;
	binds.resize(n);
	
	using Type = ExecParam::Type;
	for (int i = 0; i < n; ++i) {
		const auto& p = params[i];
		auto& bind = binds[i];
		memset(&bind, 0, sizeof(bind));

		switch (p.type) {
			case Type::Null: bind.buffer_type = MYSQL_TYPE_NULL; break;

			case Type::Int8 : bind.buffer_type = MYSQL_TYPE_TINY;		bind.buffer = constCast(p.value); break;
			case Type::Int16: bind.buffer_type = MYSQL_TYPE_SHORT;		bind.buffer = constCast(p.value); break;
			case Type::Int32: bind.buffer_type = MYSQL_TYPE_LONG;		bind.buffer = constCast(p.value); break;
			case Type::Int64: bind.buffer_type = MYSQL_TYPE_LONGLONG;	bind.buffer = constCast(p.value); break;

			case Type::UInt8 : bind.buffer_type = MYSQL_TYPE_TINY;		bind.buffer = constCast(p.value); bind.is_unsigned = true; break;
			case Type::UInt16: bind.buffer_type = MYSQL_TYPE_SHORT;		bind.buffer = constCast(p.value); bind.is_unsigned = true; break;
			case Type::UInt32: bind.buffer_type = MYSQL_TYPE_LONG;		bind.buffer = constCast(p.value); bind.is_unsigned = true; break;
			case Type::UInt64: bind.buffer_type = MYSQL_TYPE_LONGLONG;	bind.buffer = constCast(p.value); bind.is_unsigned = true; break;

			case Type::Float : bind.buffer_type = MYSQL_TYPE_FLOAT;		bind.buffer = constCast(p.value); break;
			case Type::Double: bind.buffer_type = MYSQL_TYPE_DOUBLE;	bind.buffer = constCast(p.value); break;

			case Type::c_str: {
				auto* c_str = reinterpret_cast<const char*>(p.value);
				bind.buffer_type	= MYSQL_TYPE_STRING;
				bind.buffer			= constCast(p.value);
				bind.buffer_length	= static_cast<unsigned long>(strlen(c_str));
			} break;

			default: throw SGE_ERROR("unsupported params type");
		}
	}
	
	if (0 != mysql_stmt_bind_param(_stmt, binds.data())) {
		throw SGE_ERROR(mysql_stmt_error(_stmt));
	}

	_step();
}

void MySQL_Stmt::_step() {
	if (0 != mysql_stmt_execute(_stmt)) {
		throw SGE_ERROR(mysql_stmt_error(_stmt));
	}
}

MySQL_Conn::MySQL_Conn(StrView host, StrView db, StrView user, StrView password) {
	_conn = mysql_init(nullptr);
	if (!_conn)
		throw SGE_ERROR("mysql_init: In case of insufficient memory, NULL is returned");

	TempString szHost(host);
	TempString szDB(db);
	TempString szUser(user);
	TempString szPassword(password);

	if (nullptr == mysql_real_connect(_conn, szHost.c_str(), szUser.c_str(), szPassword.c_str(), szDB.c_str(), 0, nullptr, 0)) {
		throw SGE_ERROR(mysql_error(_conn));
	}
}

void MySQL_Conn::destroy() {
	if (_conn) {
		mysql_close(_conn);
		_conn = nullptr;
	}
}
 
void MySQL_Conn::directExec(StrView sql) {
	if (!_conn)
		throw SGE_ERROR("_conn is null");

	TempString s(sql);
	if (mysql_query(_conn, s.c_str())) {
		throw SGE_ERROR(mysql_error(_conn));
	}
}

}