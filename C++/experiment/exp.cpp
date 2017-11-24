#include "type_checker.h"
#include "test_case.h"

#include <iostream>

void json_test()
{
	jsutDocument doc;
	doc.SetObject();

	json_coder_test(doc);
	json_stream_test(doc);
}

template< class T > struct is_pointer_helper : std::false_type {};
template< class T > struct is_pointer_helper<T*> : std::true_type {};
template< class T > struct is_pointer : is_pointer_helper<typename std::remove_cv<T>::type> {};


template<class T>
struct traits {
	static void some(T vlue) {
		std::cout << "T\n";
	}
};

template<class T>
struct traits<T*> {
	static void some(T* value) {
		std::cout << "T*\n";
	}
};

template<class T>
struct traits<T * const > {
	static void some(T* value) {
		std::cout << "T* const \n";
	}
};


template<class T>
struct traits<const T*> {
	static void some(const T* value) {
		std::cout << "const T*\n";
	}
};

template<class T>
struct traits<T&> {
	static void some(T& value) {
		std::cout << "T&\n";
	}
};


#include <type_traits>

// func is enabled if all Ts... have the same type as T
template<typename T, typename... Ts>
std::enable_if_t<std::conjunction<std::is_same<T, Ts>...>::value>
func(T, Ts...) {
	std::cout << "all types in pack are T\n";
}


// otherwise
template<typename T, typename... Ts>
std::enable_if_t<!std::conjunction<std::is_same<T, Ts>...>::value>
func(T, Ts...) {
std::cout << "not all types in pack are T\n";
}



template <class Ty1, class Ty2>
struct is_decay_same
	: std::is_same<std::remove_cv_t<std::remove_reference_t<Ty1>>, Ty2>
{
};

template <class Ty1, class Ty2>
using is_decay_same_t = typename is_decay_same<Ty1, Ty2>::type;

template <class Ty>
struct out_value {
	static void output() {
		std::cout << Ty::value << '\n';
	}
};


void traits_test()
{
	std::cout << std::boolalpha;

	func(1, 2, 3);
	func(1, 2, "hello!");

	out_value<is_decay_same_t<int&, int>>::output();
	out_value<is_decay_same_t<const int&, const int>>::output();
	out_value<is_decay_same_t<int&&, int>>::output();
	out_value<is_decay_same_t<const int&&, const int>>::output();
}


int main(int argc, const char* argv[])
{
	type_checker();
	json_test();

	traits_test();

	system("pause");
	return 0;
}
