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
	string,
	sge_String,
};

class Stmt : public NonCopyable {
public:
	virtual ~Stmt() = default;

	virtual void reset() { onReset(); }

	struct ExecParam {
		using Type = DBDataType;

		ExecParam(nullptr_t) : value(nullptr), type(Type::Null) {}

		ExecParam(i8&  v)			{ value = &v; type = Type::Int8;   }
		ExecParam(i16& v)			{ value = &v; type = Type::Int16;  }
		ExecParam(i32& v)			{ value = &v; type = Type::Int32;  }
		ExecParam(i64& v)			{ value = &v; type = Type::Int64;  }

		ExecParam(u8&  v)			{ value = &v; type = Type::UInt8;  }
		ExecParam(u16& v)			{ value = &v; type = Type::UInt16; }
		ExecParam(u32& v)			{ value = &v; type = Type::UInt32; }
		ExecParam(u64& v)			{ value = &v; type = Type::UInt64; }

		ExecParam(float&  v)		{ value = &v; type = Type::Float;  }
		ExecParam(double& v)		{ value = &v; type = Type::Double; }

		ExecParam(const char* v)	{ value = reinterpret_cast<const void*>(v); type = Type::c_str; }
		ExecParam(std::string& v)	{ value = &v; type = Type::string; }
		ExecParam(String& v)		{ value = &v; type = Type::sge_String; }

		const void* value	= nullptr;
		Type		type	= Type::Null;
	};

	template<class... Args>
	void exec(Args&&... args) {
		reset();
		ExecParam p[] = { args... };
		size_t n = sizeof...(args);
		onExec(p, n);
	};

	void exec() {
		reset();
		onExec(nullptr, 0);
	};

	struct ResultField {
		using Type = DBDataType;

		ResultField(nullptr_t) : value(nullptr), type(Type::Null) {}

		ResultField(i8&  v)			{ value = &v; type = Type::Int8;   }
		ResultField(i16& v)			{ value = &v; type = Type::Int16;  }
		ResultField(i64& v)			{ value = &v; type = Type::Int64;  }
		ResultField(i32& v)			{ value = &v; type = Type::Int32;  }

		ResultField(u8&  v)			{ value = &v; type = Type::UInt8;  }
		ResultField(u16& v)			{ value = &v; type = Type::UInt16; }
		ResultField(u32& v)			{ value = &v; type = Type::UInt32; }
		ResultField(u64& v)			{ value = &v; type = Type::UInt64; }

		ResultField(float&  v)		{ value = &v; type = Type::Float;  }
		ResultField(double& v)		{ value = &v; type = Type::Double; }

		ResultField(std::string& v) { value = &v; type = Type::string; }
		ResultField(String& v)		{ value = &v; type = Type::sge_String; }

		void* value	= nullptr;
		Type  type	= Type::Null;
	};

	template<class... Args>
	bool fetch(Args&&... args) {
		ResultField fields[] {args...};
		size_t n = sizeof...(args);
		return onFetch(fields, n);
	}
	void fetch() { onFetch(nullptr, 0); };

	virtual int resultFieldCount() = 0;
	virtual const char* resultFieldName(int i) = 0;
	virtual bool isFieldNull(int i) = 0;

protected:
	virtual void onReset() = 0;
	virtual void onExec(ExecParam* params, size_t n) = 0;
	virtual bool onFetch(ResultField* fields, size_t n) = 0;
};

class Conn : public RefCountBase {
public:
	virtual ~Conn() noexcept = default;

	virtual void directExec(StrView sql) = 0;
	virtual void destroy() = 0;

	UPtr<Stmt> createStmt(StrView sql) { return onCreateStmt(sql); }

protected:
	virtual UPtr<Stmt> onCreateStmt(StrView sql)  = 0;
};

MyDB_API SPtr<Conn> connectMySQL(StrView host, StrView db, StrView user, StrView password);
MyDB_API SPtr<Conn> connectSQLite3(StrView filename);

}
