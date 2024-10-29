#include "MyDB.h"

#include <sqlite3.h>

namespace sge {

class MySQLite3_Conn : public Conn {
public:
	MySQLite3_Conn(StrView filename);
	~MySQLite3_Conn() noexcept { destroy(); }

	virtual void directExec(StrView sql) override;
	virtual void destroy() override;

	sqlite3* _conn = nullptr;

protected:
	virtual UPtr<Stmt> onCreateStmt(StrView sql) override;
}; // MySQLite3_Conn

SPtr<Conn> connectSQLite3(StrView filename) {
	return new MySQLite3_Conn(filename);
}

class MySQLite3_Stmt : public Stmt {
public:
	void destroy() {
		if (_stmt) {
//			sqlite3_reset(_stmt);
			sqlite3_finalize(_stmt);
			_stmt = nullptr;
		}
	}
	~MySQLite3_Stmt() noexcept { destroy(); }

	MySQLite3_Stmt::MySQLite3_Stmt(MySQLite3_Conn* conn, StrView sql)
		: _conn(conn)
	{
		if (!_conn)
			throw SGE_ERROR("_conn is null");

		TempString sql_(sql);
		const char* unusedSql = nullptr;
		if (SQLITE_OK != sqlite3_prepare_v3(_conn->_conn, sql_.c_str(), static_cast<int>(sql_.size()), SQLITE_PREPARE_PERSISTENT, &_stmt, &unusedSql)) {
			throw SGE_ERROR("sqlite3_prepare_v3");
		}

		if (unusedSql) {
			const char* c = unusedSql;
			for (; *c ; c ++) {
				if (*c == '\t') continue;
				if (*c == '\r') continue;
				if (*c == '\n') continue;
				if (*c == ' ')  continue;

				throw SGE_ERROR("SQLite3 doesn't support multiple SQL statement");
			}
		}
	}

	virtual void onReset() override {
		if (!_stmt)
			throw SGE_ERROR("_stmt is null");

		_endOfRows = false;
		sqlite3_reset(_stmt);
	}

	virtual void onExec(ExecParam* params, size_t n) override {
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

//				case Type::UInt64: sqlite3_bind_int64(_stmt, idx, *reinterpret_cast<const u64*>(p.value)); break; // int64 might not able to handle uint64

				case Type::Float:  sqlite3_bind_double(_stmt, idx, *reinterpret_cast<const float *>(p.value)); break;
				case Type::Double: sqlite3_bind_double(_stmt, idx, *reinterpret_cast<const double*>(p.value)); break;

				case Type::c_str: {
					auto* c_str = reinterpret_cast<const char*>(p.value);
					sqlite3_bind_text(_stmt, idx, c_str, static_cast<int>(strlen(c_str)), nullptr);
				} break;

				case Type::string: {
					auto* s = reinterpret_cast<const std::string*>(p.value);
					sqlite3_bind_text(_stmt, idx, s->c_str(), static_cast<int>(s->size()), nullptr);
				} break;

				case Type::sge_String: {
					auto* s = reinterpret_cast<const String*>(p.value);
					sqlite3_bind_text(_stmt, idx, s->c_str(), static_cast<int>(s->size()), nullptr);
				} break;

				default: throw SGE_ERROR("unsupported params type");
			}
		}
		_step();
	}

	void _throwIfOutOfRange(int i) {
		if (i < 0 || i >= sqlite3_column_count(_stmt))
			throw SGE_ERROR("out of range");
	}

	virtual bool MySQLite3_Stmt::onFetch(ResultField* fields, size_t n) override {
		if (_endOfRows)
			return false;

		if (!_stmt)
			throw SGE_ERROR("_stmt is null");

		if (n != sqlite3_column_count(_stmt))
			throw SGE_ERROR("incorrect parameter count");

		using Type = ResultField::Type;
		for (int i = 0; i < n; ++i) {
			auto& f = fields[i];

			switch (f.type) {
				case Type::Int8:	_column_int<i8 >(f, i); break;
				case Type::Int16:	_column_int<i16>(f, i); break;
				case Type::Int32:	_column_int<i32>(f, i); break;
				case Type::Int64:	_column_int<i64>(f, i); break;

				case Type::UInt8:	_column_int<u8 >(f, i); break;
				case Type::UInt16:	_column_int<u16>(f, i); break;
				case Type::UInt32:	_column_int<u32>(f, i); break;
				case Type::UInt64:	_column_int<u64>(f, i); break; // sqlite3 never saved u64 data, so allow to fetch int data cast it as u64 value.

				case Type::Float:	_column_double<float >(f, i); break;
				case Type::Double:	_column_double<double>(f, i); break;

				case Type::string: {
					auto* s = reinterpret_cast<std::string*>(f.value);
					int t = sqlite3_column_type(_stmt, i);
					if (SQLITE_NULL == t) {
						s->clear();
					}
					if (SQLITE3_TEXT != t) {
						throw SGE_ERROR("invalid field type");
					}
					const auto* src = sqlite3_column_text(_stmt, i); // return value are always zero-terminated.
					s->assign(reinterpret_cast<const char*>(src));
				} break;

				case Type::sge_String: {
					auto* s = reinterpret_cast<String*>(f.value);
					int t = sqlite3_column_type(_stmt, i);
					if (SQLITE_NULL == t) {
						s->clear();
					}
					if (SQLITE3_TEXT != t) {
						throw SGE_ERROR("invalid field type");
					}
					const auto* src = sqlite3_column_text(_stmt, i); // return value are always zero-terminated.
					s->assign(reinterpret_cast<const char*>(src));
				} break;

				default: throw SGE_ERROR("unsupported field type");
			}
		}

		_step();
		return true;
	}

	void _step() {
		_endOfRows = true;

		int ret = sqlite3_step(_stmt);
		if (SQLITE_DONE == ret) {
			return;
		}
		if (SQLITE_ROW == ret) {
			_endOfRows = false;
			return;
		}
	}

	template<class T> inline
	void _column_int(ResultField& f, int col) {
		auto& dst = *reinterpret_cast<T*>(f.value);

		int t = sqlite3_column_type(_stmt, col);
		if (SQLITE_NULL == t) {
			dst = 0;
			return;
		}

		if (SQLITE_INTEGER != t)
			throw SGE_ERROR("invalid field type");

		sqlite_int64 src = sqlite3_column_int64(_stmt, col);
		dst = static_cast<T>(src);
		if (static_cast<sqlite_int64>(dst) != src)
			throw SGE_ERROR("field overflow");

//		SGE_DUMP_VAR(col, resultFieldName(col), dst);
	}

	template<class T> inline
	void _column_double(ResultField& f, int col) {
		auto& dst = *reinterpret_cast<T*>(f.value);

		int t = sqlite3_column_type(_stmt, col);
		if (SQLITE_NULL == t) {
			dst = 0;
			return;
		}

		if (SQLITE_FLOAT != t)
			throw SGE_ERROR("invalid field type");

		double src = sqlite3_column_double(_stmt, col);
		dst = static_cast<T>(src);
		if (static_cast<double>(dst) != src)
			throw SGE_ERROR("field overflow");

//		SGE_DUMP_VAR(col, resultFieldName(col), dst);
	}

	virtual int MySQLite3_Stmt::resultFieldCount() override {
		if (!_stmt)
			throw SGE_ERROR("_stmt is null");
		return sqlite3_column_count(_stmt);
	}

	virtual const char* MySQLite3_Stmt::resultFieldName(int i) override {
		if (!_stmt)
			throw SGE_ERROR("_stmt is null");

		_throwIfOutOfRange(i);

		return sqlite3_column_name(_stmt, i);
	}

	virtual bool MySQLite3_Stmt::isFieldNull(int i) override {
		if (!_stmt)
			throw SGE_ERROR("_stmt is null");

		_throwIfOutOfRange(i);

		int t = sqlite3_column_type(_stmt, i);
		return t == SQLITE_NULL;
	}

	SPtr<MySQLite3_Conn> _conn;
	sqlite3_stmt* _stmt = nullptr;
	bool _endOfRows = false;
}; // MySQLite3_Stmt

MySQLite3_Conn::MySQLite3_Conn(StrView filename) {
	TempString db(filename);
	if (SQLITE_OK != sqlite3_open_v2(db.c_str(), &_conn, SQLITE_OPEN_READWRITE, nullptr)) {
		throw SGE_ERROR(sqlite3_errmsg(_conn));
	}
}

void MySQLite3_Conn::destroy() {
	if (_conn) {
		sqlite3_close(_conn);
		_conn = nullptr;
	}
}

void MySQLite3_Conn::directExec(StrView sql) {
	TempString sql_(sql);
	char* outErrMsg = nullptr;
	if (SQLITE_OK != sqlite3_exec(_conn, sql_.c_str(), nullptr, nullptr, &outErrMsg)) {
		auto e = SGE_ERROR(outErrMsg);
		sqlite3_free(outErrMsg);
		throw e;
	}
}

UPtr<Stmt> MySQLite3_Conn::onCreateStmt(StrView sql) {
	return UPtr_make<MySQLite3_Stmt>(this, sql);
}

} // namespace sge