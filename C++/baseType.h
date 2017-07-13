#ifndef _BASETYPE_H
#define _BASETYPE_H

#define COPY_AND_ASSIGN_CTOR(theClass) \
	theClass(const theClass&); \
	theClass(theClass&&); \
	theClass& operator=(const theClass&); \
	theClass& operator=(theClass&&)

#define TEXT(text) #text

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
