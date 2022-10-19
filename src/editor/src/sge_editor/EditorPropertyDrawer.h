#pragma once

namespace sge {

struct EditorPropertyDrawRequest {
	const TypeInfo*		objectType	= nullptr;
	const FieldInfo*	field		= nullptr;
	Vector<void*, 16>	objects;

	template<class T> inline
	const T& getFirstObjectValue() { return field->getValue<T>(objects[0]); }

	template<class T> inline
	bool isMixedValue() {
		if (objects.size() <= 1)
			return false;

		const T& v = getFirstObjectValue<T>();
		for (auto& o : objects) {
			if (v != field->getValue<T>(o)) {
				return true;
			}
		}
		return false;
	}

	template<class T> inline
	void setAllValue(const T& v) {
		for (auto& o : objects) {
			field->setValue<T>(o, v);
		}
	}
};

class EditorPropertyDrawer : NonCopyable {
public:
	using DrawRequest = EditorPropertyDrawRequest;
	virtual void draw(DrawRequest& req) {}	
};

class EditorPropertyDrawer_struct : public EditorPropertyDrawer {
public:
	virtual void draw(DrawRequest& req) override;
};

class EditorPropertyDrawer_float : public EditorPropertyDrawer {
public:
	virtual void draw(DrawRequest& req) override;
};

class EditorPropertyDrawer_Vec3f : public EditorPropertyDrawer {
public:
	virtual void draw(DrawRequest& req) override;
};

class EditorPropertyDrawer_Quat4f : public EditorPropertyDrawer {
public:
	virtual void draw(DrawRequest& req) override;
};

}