#include "MyDB.h"

#include <sqlite3.h>

namespace sge {

class MySQLite3_Conn : public Conn {
public:
	MySQLite3_Conn(StrView filename);
	~MySQLite3_Conn() { destroy(); }

	virtual void directExec(StrView sql) override;
	virtual void destroy() override;

	sqlite3* _conn = nullptr;

protected:
	virtual UPtr<Stmt> onCreateStmt(StrView sql) override;
	virtual UPtr<Stmt> onCreateStmt(StrViewW sql) override;
};

SPtr<Conn> connectSQLite3(StrView filename) {
	return new MySQLite3_Conn(filename);
}

class MySQLite3_Stmt : public Stmt {
public:
	MySQLite3_Stmt(MySQLite3_Conn* conn, StrView sql);
	MySQLite3_Stmt(MySQLite3_Conn* conn, StrViewW sql);
	~MySQLite3_Stmt() { destroy(); }

	void destroy();

protected:
	virtual void onExec(ExecParam* params, size_t n) override;

	void _step();

private:
	SPtr<MySQLite3_Conn> _conn;
	sqlite3_stmt* _stmt = nullptr;
};

MySQLite3_Conn::MySQLite3_Conn(StrView filename) {
	TempString db(filename);
	if (SQLITE_OK != sqlite3_open_v2(db.c_str(), &_conn, SQLITE_OPEN_READWRITE, nullptr)) {
		throw SGE_ERROR(sqlite3_errmsg(_conn));
	}
}

void MySQLite3_Conn::destroy() {
	if (_conn) {
		if (SQLITE_OK != sqlite3_close(_conn)) {
			throw SGE_ERROR(sqlite3_errmsg(_conn));
		}
		_conn = nullptr;
	}
}

UPtr<Stmt> MySQLite3_Conn::onCreateStmt(StrView sql) { return UPtr<MySQLite3_Stmt>(new MySQLite3_Stmt(this, sql)); }
UPtr<Stmt> MySQLite3_Conn::onCreateStmt(StrViewW sql) { return UPtr<MySQLite3_Stmt>(new MySQLite3_Stmt(this, sql)); }

void MySQLite3_Conn::directExec(StrView sql) {
	TempString sql_(sql);
	char* zErrMsg = nullptr;
	if (SQLITE_OK != sqlite3_exec(_conn, sql_.c_str(), nullptr, nullptr, &zErrMsg)) {
		TempString errMsg(zErrMsg);
		sqlite3_free(zErrMsg);
		throw SGE_ERROR(errMsg.c_str());
	}
}

MySQLite3_Stmt::MySQLite3_Stmt(MySQLite3_Conn* conn, StrView sql)
	: _conn(conn)
{
	if (!_conn)
		throw SGE_ERROR("_conn is null");

	TempString sql_(sql);
	const char* unusedSql = nullptr;
	if (SQLITE_OK != sqlite3_prepare_v3(_conn->_conn, sql_.c_str(), static_cast<int>(sql_.size()), SQLITE_PREPARE_PERSISTENT, &_stmt, &unusedSql)) {
		throw SGE_ERROR("sqlite3_prepare_v3: out of memory");
	}
}

MySQLite3_Stmt::MySQLite3_Stmt(MySQLite3_Conn* conn, StrViewW sql)
	: _conn(conn)
{
	if (!_conn)
		throw SGE_ERROR("_conn is null");

	TempStringW sql_(sql);
	const void* unusedSql = nullptr;
	if (SQLITE_OK != sqlite3_prepare16_v3(_conn->_conn, sql_.c_str(), static_cast<int>(sql_.size()), SQLITE_PREPARE_PERSISTENT, &_stmt, &unusedSql)) {
		throw SGE_ERROR("sqlite3_prepare16_v3");
	}
}

void MySQLite3_Stmt::destroy() {
	if (_stmt) {
//		sqlite3_reset(_stmt);
		sqlite3_finalize(_stmt);
		_stmt = nullptr;
	}
}

void MySQLite3_Stmt::onExec(ExecParam* params, size_t n) {
	if (!_stmt)
		throw SGE_ERROR("_stmt is null");

	if (n != sqlite3_bind_parameter_count(_stmt))
		throw SGE_ERROR("incorrect parameter count");

	using Type = ExecParam::Type;
	for (int i = 0; i < n; ++i) {
		const auto& p = params[i];
		int idx = i + 1;
		switch (p.type) {

			case Type::Null : sqlite3_bind_null(_stmt, idx); break;

			case Type::Int8 : sqlite3_bind_int	(_stmt, idx, *reinterpret_cast<const i8 *>(p.value)); break;
			case Type::Int16: sqlite3_bind_int	(_stmt, idx, *reinterpret_cast<const i16*>(p.value)); break;
			case Type::Int32: sqlite3_bind_int	(_stmt, idx, *reinterpret_cast<const i32*>(p.value)); break;
			case Type::Int64: sqlite3_bind_int64(_stmt, idx, *reinterpret_cast<const i64*>(p.value)); break;

			case Type::UInt8 : sqlite3_bind_int	 (_stmt, idx, *reinterpret_cast<const u8 *>(p.value)); break;
			case Type::UInt16: sqlite3_bind_int	 (_stmt, idx, *reinterpret_cast<const u16*>(p.value)); break;
			case Type::UInt32: sqlite3_bind_int64(_stmt, idx, *reinterpret_cast<const u32*>(p.value)); break; // int64 can handle uint32

//			case Type::UInt64: sqlite3_bind_int64(_stmt, idx, *reinterpret_cast<const u64*>(p.value)); break; // int64 might not able to handle uint64

			case Type::Float:  sqlite3_bind_double(_stmt, idx, *reinterpret_cast<const float*>(p.value)); break;
			case Type::Double: sqlite3_bind_double(_stmt, idx, *reinterpret_cast<const double*>(p.value)); break;

			case Type::c_str: {
				auto* c_str = reinterpret_cast<const char*>(p.value);
				sqlite3_bind_text(_stmt, idx, c_str, static_cast<int>(strlen(c_str)), nullptr);
			} break;

			default: throw SGE_ERROR("unsupported params type");
		}
	}

	_step();
}

void MySQLite3_Stmt::_step() {
	int ret = sqlite3_step(_stmt);
	if (SQLITE_DONE == ret) {
		return;
	}
}

}