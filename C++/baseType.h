#ifndef _BASETYPE_H
#define _BASETYPE_H

#define TEXT(text) #text

#define COPY_AND_ASSIGN_CTOR(theClass)     \
	theClass(const theClass &);            \
	theClass(theClass &&);                 \
	theClass &operator=(const theClass &); \
	theClass &operator=(theClass &&)

#define DISALLOW_COPY_AND_ASSIGN(theClass)          \
	theClass(const theClass &) = delete;            \
	theClass(theClass &&) = delete;                 \
	theClass &operator=(const theClass &) = delete; \
	theClass &operator=(theClass &&) = delete

#define MEMBER_RW_REF(_type, _name, _func)                 \
  public:                                                  \
	void set##_func(const _type &value) { _name = value; } \
	const _type &get##_func() { return _name; }            \
                                                           \
  protected:                                               \
	_type _name

#define MEMBER_RW(_type, _name, _func)              \
  public:                                           \
	void set##_func(_type value) { _name = value; } \
	_type get##_func() { return _name; }            \
                                                    \
  protected:                                        \
	_type _name

#define SAFE_DELETE(ptr) \
	if (ptr)             \
	{                    \
		delete ptr;      \
		ptr = nullptr;   \
	}

// singleton base
template<typename ClassType> class Singleton
{
public:
	static ClassType * getInstance() {
		static ClassType instance;
		return &instance;
	}
protected:
	Singleton() {}
	virtual ~Singleton() {}
};

// each class inherient from Singleton should include this macro
#define DECLARE_SINGLETON(theClass) friend Singleton<theClass>;	COPY_AND_ASSIGN_CTOR(theClass)

#endif // !_BASETYPE_H
