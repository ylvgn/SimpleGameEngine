#include <sge_core/base/UnitTest.h>
#include <sge_core/log/Log.h>
#include <sge_core/pointer/SPtr.h>
#include <sge_core/string/StringUtil.h>
#include "MyVector.h"

namespace sge {
class Test_Vector : public UnitTestBase {
public:

	class MyClass : RefCountBase {
	public:
		int test = 0;
		int _refCount = 0;

		MyClass() {}
		MyClass(int t) : test(t) {}
		MyClass(int t, int refCount) : test(t), _refCount(refCount) {}

		MyClass(MyClass&& r) noexcept : test(r.test), _refCount(r._refCount) {
			test = r.test;
			_refCount = r._refCount;

			r.test = 0;
			r._refCount = 0;
		}

		//MyClass(const MyClass&& r) : test(r.test), _refCount(r._refCount) {}

		auto onFormat(fmt::format_context& ctx) const {
			return fmt::format_to(ctx.out(), "test={}\n", test);
		}

	}; // MyClass

	struct H {
		static void init(MyVector<int>& v) {
			for (int i = 0; i < v.size(); i++) {
				v[i] = i * 10;
			}
		}

		static void dump(MyVector<int>& v) {
			for (int i = 0; i < v.size(); i++) {
				SGE_LOG("dump[{}]={}\n", i, v[i]);
			}
		}
	}; // H

	void test_resize() {
		MyVector<int> v;
		v.resize(10);
		H::init(v);
		SGE_LOG("{}\ncapacity={}", v, v.capacity());
	}

	void test_capacity() {
		MyVector<int> v;
		v.resize(10);
		SGE_DUMP_VAR(v.capacity());

		v.resize(11);
		SGE_DUMP_VAR(v.capacity());
	}

	void test_back() {
		MyVector<int> v;
		v.resize(5);
		
		v.back(v.size() - 1) = 1000;
		SGE_DUMP_VAR(v.back(v.size() - 1));

		v[v.size() - 1] = 999;
		SGE_DUMP_VAR(v.back());
	}

	void test_unsafeAt() {
		MyVector<int> v;
		v.resize(5);
		H::init(v);
		v.unsafeAt(4) = 444;
		v.unsafeAt(100) = 666;
		SGE_LOG("{}", v);
	}

	void test_at() {
		MyVector<int> v;
		v.resize(5);
		H::init(v);
		v.at(4) = 444;
		try {
			v.at(100) = 666;
		} catch (...) {
			SGE_LOG("{}", v);
		}
		
		SGE_LOG("never print");
	}

	void test_append() {
		MyVector<int> v;
		for (int i = 0; i < 5; i++) {
			v.append(i);
			v += i;
		}
		SGE_LOG("{}\nsize={}\ncapacity={}", v, v.size(), v.capacity());
	}

	void test_push_back() {
		MyClass a;
		a.test = 100;
		MyVector<MyClass> v;
		// v.push_back(a);			// -> copy constructor not allow
		v.push_back(std::move(a));	// -> move constructor, test=100
		v += std::move(a);			// -> move constructor, test=0
		v.push_back(MyClass(300));	// -> function return value auto std::move
		SGE_LOG("{}\nsize={}\ncapacity={}", v, v.size(), v.capacity());
	}

	void test_emplace_back() {
		
		MyVector<MyClass> v;
		v.emplace_back(100);
		v.emplace_back(MyClass(200));
		
		MyClass a(300);
		v.emplace_back(std::move(a));
		SGE_LOG("{}\nsize={}\ncapacity={}", v, v.size(), v.capacity());
	}

	void main()
	{
		Vector<SPtr<MyClass>> a;
		MyClass* b = new MyClass();

		b->test = 1;
		a.push_back(b);

		b->test = 2;
		a.emplace_back(b);

		a.emplace_back(b);
		b->test = 3;

		//SGE_DUMP_VAR(*a[0], *a[1], *a[2]);
		SGE_LOG("{}", a);

		Vector_<int, 1> c;
		c.push_back(100);
		c.push_back(200);
		c.push_back(300);
		SGE_LOG("{}", c);
	}

}; // Test_Vector

SGE_FORMATTER(Test_Vector::MyClass)

} // namespace

template<class T>
struct fmt::formatter<sge::Vector<sge::SPtr<T>>> {
	auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }
	auto format(const sge::Vector<sge::SPtr<T>>& v, fmt::format_context& ctx) {
		auto it = *ctx.out();
		for (const auto& c : v) {
			it = c->onFormat(ctx);
		}
		return ctx.out();
	}
};

template<>
struct fmt::formatter<sge::MyVector<int>> {
	auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }
	auto format(const sge::MyVector<int>& v, fmt::format_context& ctx) {
		sge::TempString tmp;
		for (int i = 0; i < v.size(); i++) {
			if (tmp.size() > 0) tmp += ',';
			FmtTo(tmp, "{}", v[i]);
		}
		fmt::format_to(ctx.out(), "[{}]", tmp);
		return ctx.out();
	}
};

template<>
struct fmt::formatter<sge::MyVector<sge::Test_Vector::MyClass>> {
	using MyType = sge::MyVector<sge::Test_Vector::MyClass>;
	auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }
	auto format(const MyType& v, fmt::format_context& ctx) {
		auto it = *ctx.out();
		for (int i = 0; i < v.size(); i++) {
			it = v[i].onFormat(ctx);
		}
		return ctx.out();
	}
};

template<class T, size_t N>
struct fmt::formatter<sge::Vector_<T, N>> {
	using MyType = sge::Vector_<T, N>;
	auto parse(fmt::format_parse_context& ctx) { return ctx.begin(); }
	auto format(const MyType& v, fmt::format_context& ctx) {
		sge::TempString tmp;
		for (int i = 0; i < v.size(); ++i) {
			sge::FmtTo(tmp, "{}\n", v[i]);
		}
		return fmt::format_to(ctx.out(), "{}", tmp);
	}
};

void test_Vector() {
	using namespace sge;
	SGE_TEST_CASE(Test_Vector, main());

#if 0
	SGE_TEST_CASE(Test_Vector, test_resize());
	SGE_TEST_CASE(Test_Vector, test_capacity());
	SGE_TEST_CASE(Test_Vector, test_back());
	SGE_TEST_CASE(Test_Vector, test_unsafeAt());
	//SGE_TEST_CASE(Test_Vector, test_at());
	SGE_TEST_CASE(Test_Vector, test_append());
	SGE_TEST_CASE(Test_Vector, test_push_back());
	SGE_TEST_CASE(Test_Vector, test_emplace_back());
#endif
}