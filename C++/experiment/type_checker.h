#include <iostream>
#include <iomanip>

#define print_sizeof(type)	\
	std::cout << "sizeof(" << std::setw(12) <<  \
	std::setiosflags(std::ios::left) << #type << \
	") = " << std::setw(2) << sizeof(type) << std::endl

#define type_checker()		\
	print_sizeof(char);		\
	print_sizeof(wchar_t);	\
	print_sizeof(int);		\
	print_sizeof(short);	\
	print_sizeof(long);		\
	print_sizeof(long long);\
	print_sizeof(void*);	\
	print_sizeof(bool);		\
	print_sizeof(float);	\
	print_sizeof(double);	\
	print_sizeof(long double);	\
	print_sizeof(int64_t)
