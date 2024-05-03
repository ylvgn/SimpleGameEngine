#pragma once

#include "MyCommon.h"

#if sge_sita_db_EXPORTS
	#define MyDB_API __declspec(dllexport)
#else
	#define MyDB_API __declspec(dllimport)
	#pragma comment(lib, "sge_sita_db.lib")
#endif

namespace sge {

enum class DBDataType {
	Null,
	Int8,
	Int16,
	Int32,
	Int64,
	UInt8,
	UInt16,
	UInt32,
	UInt64,
	Float,
	Double,
	c_str,
};

class Stmt : public NonCopyable {
public:
	virtual ~Stmt() = default;

	struct ExecParam {
		using Type = DBDataType;

		ExecParam(const nullptr_t*) : value(nullptr), type(Type::Null) {}

		ExecParam(i8&  v)		{ value = &v; type = Type::Int8;   }
		ExecParam(i16& v)		{ value = &v; type = Type::Int16;  }
		ExecParam(i32& v)		{ value = &v; type = Type::Int32;  }
		ExecParam(i64& v)		{ value = &v; type = Type::Int64;  }

		ExecParam(u8&  v)		{ value = &v; type = Type::UInt8;  }
		ExecParam(u16& v)		{ value = &v; type = Type::UInt16; }
		ExecParam(u32& v)		{ value = &v; type = Type::UInt32; }
		ExecParam(u64& v)		{ value = &v; type = Type::UInt64; }

		ExecParam(float&  v)	{ value = &v; type = Type::Float;  }
		ExecParam(double& v)	{ value = &v; type = Type::Double; }

		ExecParam(const char* v) { value = reinterpret_cast<const void*>(v); type = Type::c_str; }

		const void* value	= nullptr;
		Type		type	= Type::Null;
	};

	template<class... Args>
	void exec(Args&&... args) {
		ExecParam p[] = { args... };
		size_t n = sizeof...(args);
		onExec(p, n);
	};

	void exec() { onExec(nullptr, 0); };

protected:
	virtual void onExec(ExecParam* params, size_t n) = 0;
};

class Conn : public RefCountBase {
public:
	virtual ~Conn() = default;

	virtual void directExec(StrView sql) = 0;
	virtual void destroy() = 0;

	UPtr<Stmt> createStmt(StrView sql)  { return onCreateStmt(sql); }
	UPtr<Stmt> createStmt(StrViewW sql) { return onCreateStmt(sql); }

protected:
	virtual UPtr<Stmt> onCreateStmt(StrView sql)  = 0;
	virtual UPtr<Stmt> onCreateStmt(StrViewW sql) = 0;
};

MyDB_API SPtr<Conn> connectMySQL(StrView host, StrView db, StrView user, StrView password);
MyDB_API SPtr<Conn> connectSQLite3(StrView filename);

}
