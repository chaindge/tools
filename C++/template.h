#ifndef TEMPLATE_H
#define TEMPLATE_H
#include <vector>

template<class T>
inline bool vector_has_elem(const std::vector<T>& vec, const T& elem)
{
	return (std::find(vec.begin(), vec.end(), elem) != vec.end());
}

template<class T>
inline void merge_vector(std::vector<T>& to, const std::vector<T>& from)
{
	for (auto & item : from) 
		if (!vector_has_elem(to, item)) 
			to.push_back(item);
}

template<class T>
inline void erase(std::vector<T>& vec, const T& elem)
{
	auto iter = std::find(vec.begin(), vec.end(), elem);
	if (iter != vec.end())
		vec.erase(iter);
}

template<class T>
inline void SubtractVector(std::vector<T>& out_,const std::vector<T>& set1_,const std::vector<T>& set2_)
{
	for (auto& item : set1_)
		if(!vector_has_elem(set2_,item))
			out_.push_back(item);
}

template <class T>
inline std::vector<T> operator-(const std::vector<T>& left, const std::vector<T>& right)
{
	std::vector<T> ret;
	SubtractVector(ret, left, right);
	return ret;
}

template <class T>
inline T join_vector(const std::vector<T>& vec, const T& splice)
{
	T ret; int size = vec.size();
	for (int i = 0; i < size; ++i)
	{
		ret += vec[i];
		if (i != size - 1)
			ret += splice;
	}
	/*for (auto iter = vec.begin(),int c = 0; iter != vec.end(); iter++,c++)
	{
		ret += *iter;
		if (c != end)
			ret += splice;
	}*/
	return ret;
}
#endif // !TEMPLATE_H
