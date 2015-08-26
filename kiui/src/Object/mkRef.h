//  Copyright (c) 2015 Hugo Amiard hugo.amiard@laposte.net
//  This software is provided 'as-is' under the zlib License, see the LICENSE.txt file.
//  This notice and the license may not be removed or altered from any source distribution.

#ifndef MK_OBJECTREF_H_INCLUDED
#define MK_OBJECTREF_H_INCLUDED

/* mk headers */
#include <Object/mkObjectForward.h>
#include <Object/mkTyped.h>
#include <Object/mkTypeUtils.h>
#include <Object/String/mkString.h>
#include <Object/Util/mkMake.h>

/* Standard */
#include <memory>
#include <vector>
#include <map>
#include <array>

namespace mk
{
	template <class T>
	class Any;

	class MK_OBJECT_EXPORT None
	{
	public:
		template <class T>
		operator T() { return T(); }

		static Type& cls() { static Type ty; return ty; }
	};

	class MK_OBJECT_EXPORT Ref
	{
	public:
		Ref(Type& type) : mType(type), mObject(nullptr), mUpdate(0) {}
		Ref(Object& object, Type& type) : mType(type), mObject(&object), mUpdate(0) {}
		virtual ~Ref() {}

		inline Object* object() const { return mObject; }
		inline Object*& objectref() { return mObject; }
		inline Type& type() const { return mType; }
		inline Imprint* imprint() const { return mType.imprint(); }
		inline bool null() const { return &mType == &None::cls(); }
		inline void setobject(Object* object) { mObject = object; ++mUpdate; }
		inline size_t update() { return mUpdate; }

		virtual unique_ptr<Ref> clone() const { return make_unique<Ref>(*mObject, mType); };

		virtual void setString(const string& value) { UNUSED(value); }
		virtual string getString() { return ""; }

		template <class T>
		inline Any<T>* any() { return static_cast<Any<T>*>(this); }

		template <class T>
		inline const Any<T>* any() const { return static_cast<const Any<T>*>(this); }

		template <class T>
		inline T& ref() { return this->any<T>()->ref(); }

		template <class T>
		inline const T& get() const { return this->any<T>()->get(); }

		template <class T>
		inline void set(typename Pass<T>::ctype val) { this->any<T>()->set(std::forward<typename Pass<T>::ctype>(val)); }

		template <class T>
		inline T* as() const { return mObject->as<T>(); }

		template <class T>
		inline T copy() { return this->any<T>()->copy(); }

		template <class T>
		inline T copy() const { return this->any<T>()->copy(); }

		Ref(const Ref& ref) = delete;
		Ref& operator=(const Ref& ref) = delete;

	protected:
		Object* mObject;
		Type& mType;
		size_t mUpdate;
	};

	template <class T>
	class Allocator
	{};

	template <class T>
	class Any : public Ref, public Allocator<T>
	{
	public:
		Any(typename Pass<T>::forward value) : Ref(typecls<T>()), mContent(Copy<T>::copy(value)) { mObject = RefObject<T>::get(mContent); }
		Any() : Ref(typecls<T>()), mContent() { mObject = RefObject<T>::get(mContent); }

		unique_ptr<Ref> clone() const { return make_unique<Any<T>>(this->copy()); }

		void setString(const string& value) { fromString<T>(value, mContent); ++mUpdate; }
		string getString() { return toString<T>(mContent); }

		T& ref() { return mContent; }
		const T& get() const { return mContent; }
		T copy() { return Copy<T>::copy(mContent); }
		T copy() const { return Copy<T>::copy(mContent); }

		void assign(typename Pass<T>::forward val) { Assign<typename Pass<T>::forward>::set(mContent, std::forward<typename Pass<T>::forward>(val)); ++mUpdate; }
		void set(typename Pass<T>::ctype val) { Assign<typename Pass<T>::ctype>::set(mContent, std::forward<typename Pass<T>::ctype>(val)); ++mUpdate;  }

	public:
		T mContent;
	};

	template <>
	class MK_OBJECT_EXPORT Any<None> : public Ref
	{
	public:
		Any() : Ref(None::cls()) {}

		unique_ptr<Ref> clone() const { return make_unique<Any<None>>(); }
	};

	template <class T>
	struct MakeRef
	{
		static unique_ptr<Ref> make(typename Pass<T>::forward val) { return make_unique<Any<T>>(std::forward<typename Pass<T>::forward>(val)); };
	};

	template <class T>
	struct MakeRef<T*>
	{
		static unique_ptr<Ref> make(T* obj) { return obj ? make_unique<Ref>(obj, T::cls()) : make_unique<Ref>(T::cls()); };
	};

	class MK_OBJECT_EXPORT Lref
	{
	public:
		Lref(unique_ptr<Ref> ref) : mRef(std::move(ref)) {}
		Lref(TypeObject& object) : mRef(make_unique<Ref>(object, object.type())) {}
		Lref(Object& object, Type& type) : mRef(make_unique<Ref>(object, type)) {}
		Lref() : mRef(make_unique<Any<None>>()) {}

		Lref(Lref&& ref) : mRef(std::move(ref.mRef)) {}
		Lref(const Lref& ref) : mRef(ref.mRef ? ref->clone() : nullptr) {}
		Lref& operator=(const Lref& rhs) { Lref(rhs).swap(*this); return *this; }

		Lref& swap(Lref& rhs) { std::swap(mRef, rhs.mRef); return *this; }

		Ref* operator->() { return mRef.get(); }
		const Ref* operator->() const { return mRef.get(); }

	protected:
		unique_ptr<Ref> mRef;
	};

	template <class T>
	inline Lref lref(T&& val) { return Lref(MakeRef<typename UnrefType<T>::type>::make(std::forward<typename Pass<T>::forward>(val))); }

	typedef std::map<string, Lref> LrefDict;
	typedef std::vector<Lref> LrefVector;

	template <class T>
	inline int packLref(LrefVector& vec, T&& val)
	{
		vec.push_back(lref(std::forward<T>(val)));
		return 0;
	}

	template <class... Args>
	inline LrefVector packLrefs(Args&&... args)
	{
		LrefVector ret;
		std::array<int, sizeof...(Args)> pass { packLref<Args>(ret, std::forward<Args>(args))... };
		return ret;
	}
}

#endif // MK_OBJECTREF_H_INCLUDED
