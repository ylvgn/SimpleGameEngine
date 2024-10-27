#include "MyDB.h"

#include <mysql.h>
#pragma comment(lib, "libmysql.lib")

namespace sge {

class MySQL_Conn : public Conn {
public:
	MySQL_Conn(StrView host, StrView db, StrView user, StrView password);
	~MySQL_Conn() noexcept { destroy(); }

	virtual void directExec(StrView sql) override;
	virtual void destroy() override;

	MYSQL* _conn = nullptr;

protected:
	virtual UPtr<Stmt> onCreateStmt(StrView sql) override;
}; // MySQL_Conn

SPtr<Conn> connectMySQL(StrView host, StrView db, StrView user, StrView password) {
	return new MySQL_Conn(host, db, user, password);
}

class MySQL_Stmt : public Stmt {
public:
	MySQL_Stmt(MySQL_Conn* conn, StrView sql)
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

		_meta = mysql_stmt_result_metadata(_stmt);
		if (!_meta) {
			throw SGE_ERROR(mysql_stmt_error(_stmt));
		}
	}

	void destroy() {
		if (_meta) {
			mysql_free_result(_meta);
			_meta = nullptr;
		}

		if (_stmt) {
			mysql_stmt_close(_stmt);
			_stmt = nullptr;
		}
	}

	~MySQL_Stmt() noexcept { destroy(); }

	virtual void onReset() override {
		if (!_stmt)
			throw SGE_ERROR("_stmt is null");

		mysql_stmt_reset(_stmt);
	}

	virtual int resultFieldCount() override {
		if (!_stmt)
			throw SGE_ERROR("_stmt is null");

		return static_cast<int>(mysql_stmt_field_count(_stmt));
	}

	void _throwIfOutOfRange(int i) {
		if (i < 0 || i >= resultFieldCount())
			throw SGE_ERROR("out of range");
	}

	virtual const char* resultFieldName(int i) override {
		if (!_stmt)
			throw SGE_ERROR("_stmt is null");

		_throwIfOutOfRange(i);

		mysql_field_seek(_meta, static_cast<MYSQL_FIELD_OFFSET>(i));
		auto* field = mysql_fetch_field(_meta);
		return field->name;
	}

	virtual bool isFieldNull(int i) override {
		if (!_stmt)
			throw SGE_ERROR("_stmt is null");

		_throwIfOutOfRange(i);

		return _bindOutResults[i].is_null;
	}

	virtual void onExec(ExecParam* params, size_t n) override {
		if (!_stmt)
			throw SGE_ERROR("_stmt is null");

		if (n != mysql_stmt_param_count(_stmt))
			throw SGE_ERROR("incorrect parameter count");

		_binds.resize(n);
	
		using Type = ExecParam::Type;
		for (int i = 0; i < n; ++i) {
			const auto& p = params[i];
			auto& bind = _binds[i];
			memset(&bind, 0, sizeof(bind));

			switch (p.type) {
				case Type::Null : bind.buffer_type = MYSQL_TYPE_NULL; break;

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
	
		if (0 != mysql_stmt_bind_param(_stmt, _binds.data())) {
			throw SGE_ERROR(mysql_stmt_error(_stmt));
		}

		if (0 != mysql_stmt_execute(_stmt)) {
			throw SGE_ERROR(mysql_stmt_error(_stmt));
		}
	}

	template<typename T>
	void _bind_number_result(MYSQL_BIND& b, enum enum_field_types type, ResultField& f) {
		b.buffer_type = type;
		b.buffer = f.value;
		b.is_unsigned = std::is_unsigned<T>::value;

		auto* dst = (T*)f.value;
		*dst = 0;
	}

	virtual bool onFetch(ResultField* fields, size_t n) override {
		if (!_stmt)
			throw SGE_ERROR("_stmt is null");

		if (n != mysql_stmt_field_count(_stmt))
			throw SGE_ERROR("incorrect parameter count");

		_binds.resize(n);
		_bindOutResults.resize(n);

		using Type = ResultField::Type;
		for (int i = 0; i < n; ++i) {
			auto& f			= fields[i];
			auto& bind		= _binds[i];
			auto& result	= _bindOutResults[i];

			memset(&bind, 0, sizeof(bind));
			memset(&result, 0, sizeof(result));

			bind.is_null = &result.is_null;
			bind.length  = &result.length;
			bind.error   = &result.error;

			switch (f.type) {

				case Type::Int8 : _bind_number_result<i8 >(bind, MYSQL_TYPE_TINY,		f); break;
				case Type::Int16: _bind_number_result<i16>(bind, MYSQL_TYPE_SHORT,		f); break;
				case Type::Int32: _bind_number_result<i32>(bind, MYSQL_TYPE_LONG,		f); break;
				case Type::Int64: _bind_number_result<i64>(bind, MYSQL_TYPE_LONGLONG,	f); break;

				case Type::UInt8 : _bind_number_result<u8 >(bind, MYSQL_TYPE_TINY,		f);	break;
				case Type::UInt16: _bind_number_result<u16>(bind, MYSQL_TYPE_SHORT,		f);	break;
				case Type::UInt32: _bind_number_result<u32>(bind, MYSQL_TYPE_LONG,		f);	break;
				case Type::UInt64: _bind_number_result<u64>(bind, MYSQL_TYPE_LONGLONG,	f);	break;

				case Type::Float : _bind_number_result<float >(bind, MYSQL_TYPE_FLOAT,	f); break;
				case Type::Double: _bind_number_result<double>(bind, MYSQL_TYPE_DOUBLE, f); break;

				case Type::string:		bind.buffer_type = MYSQL_TYPE_STRING; break; // mysql_stmt_fetch_column later
				case Type::sge_String:	bind.buffer_type = MYSQL_TYPE_STRING; break; // mysql_stmt_fetch_column later

				default: throw SGE_ERROR("unsupported params type");
			}
		}
		
		if (0 != mysql_stmt_bind_result(_stmt, _binds.data())) {
			throw SGE_ERROR(mysql_stmt_error(_stmt));
		}

		int ret = mysql_stmt_fetch(_stmt);
		if (ret == 1) throw SGE_ERROR(mysql_stmt_error(_stmt));
		else if (ret == MYSQL_NO_DATA) return false;
		else if (ret == 0 || ret == MYSQL_DATA_TRUNCATED) {
			for (int i = 0; i < n; ++i) {
				auto& p				= fields[i];
				auto& bind			= _binds[i];
				const auto& result	= _bindOutResults[i];
				switch (p.type) {

					case Type::string: {
						auto* s = reinterpret_cast<std::string*>(p.value);
						size_t len = result.length;
						if (len > 0) {
							_tmp.resize(len);
							bind.buffer_type	= MYSQL_TYPE_STRING;
							bind.buffer			= _tmp.data();
							bind.buffer_length	= static_cast<unsigned long>(_tmp.size());

							mysql_stmt_fetch_column(_stmt, &bind, i, 0);
							s->assign(_tmp.data(), _tmp.size());
						} else {
							s->clear();
						}
					} break;

					case Type::sge_String: {
						auto* s = reinterpret_cast<String*>(p.value);
						size_t len = result.length;
						if (len > 0) {
							_tmp.resize(len);
							bind.buffer_type = MYSQL_TYPE_STRING;
							bind.buffer = _tmp.data();
							bind.buffer_length = static_cast<unsigned long>(_tmp.size());

							mysql_stmt_fetch_column(_stmt, &bind, i, 0);
							s->assign(_tmp.data(), _tmp.size());
						}
						else {
							s->clear();
						}
					} break;
				}
			}

			return true;
		}
		
		throw SGE_ERROR("fetch error");
	}

	SPtr<MySQL_Conn>	_conn;
	Vector<MYSQL_BIND>	_binds;
	
	MYSQL_STMT* _stmt = nullptr;
	MYSQL_RES*	_meta = nullptr;

	Vector<char, 45>	_tmp;

	struct MySQLBindOutResult {
		my_bool is_null = false;
		my_bool error	= false;
		unsigned long length = 0;
	};
	Vector<MySQLBindOutResult>	_bindOutResults;
}; // MySQL_Stmt

UPtr<Stmt> MySQL_Conn::onCreateStmt(StrView sql) {
	return UPtr<MySQL_Stmt>(new MySQL_Stmt(this, sql));
}

MySQL_Conn::MySQL_Conn(StrView host, StrView db, StrView user, StrView password) {
	_conn = mysql_init(nullptr);
	if (!_conn)
		throw SGE_ERROR("mysql_init");

	TempString host_(host);
	TempString db_(db);
	TempString user_(user);
	TempString password_(password);

	if (nullptr == mysql_real_connect(_conn, host_.c_str(), user_.c_str(), password_.c_str(), db_.c_str(), 0, nullptr, 0)) {
		TempString e = Fmt("{}\n\thost={}\n\tdb={}\n\tuser={}\n\tpw={}", mysql_error(_conn), host_, db_, user_, password_);
		throw SGE_ERROR(e);
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