#ifndef __PATHUTIL__
#define __PATHUTIL__

#include <string>
#include <vector>

class PathUtil
{
public:
	/**
	 *	vector to path
	 */
	static std::string to_path(const std::vector<std::string>& vec_path);

	/**
	 *	split path to vector
	 */
	static void to_vector(const std::string& path, std::vector<std::string>& vec_path);

	/**
	 *	return the relative path to base 
	 */
	static std::string to_relative_path(const std::string& base, const std::string& path);

	/**
	 *	parse .. or . 
	 */
	static std::string to_path(const std::string& path);

	/**
	 *	if path is absolutly path ,return true
	 */
	static bool is_absolute_path(const std::string& path);

	/**
	 *	get folder of the file name
	 */
	static std::string get_folder(const std::string& filename);

	static void trim_left(std::string& str);

	static void trim(std::string& str);

	static void trim_right(std::string& str);

	static void split(std::vector<std::string>& result, const std::string& str, const std::string& divide);
};



#endif //__PATHUTIL__
