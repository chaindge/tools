#include "PathUtil.h"
#include <ctype.h>
#include <algorithm>

std::string PathUtil::to_path(const std::vector<std::string>& vec_path)
{
	std::string ret;
	int count = vec_path.size();
	for (int i = 0; i < count; ++i)
	{
		ret += vec_path[i];
		if (i != count - 1)
			ret += "\\";
	}

	return ret;
}

void PathUtil::trim_left(std::string& str)
{
	int count = str.size();
	int i = 0;

	for (; i < count; ++i)
		if (!isspace(str[i]))
			break;

	if (i == count)
		str = "";
	else
		str = str.substr(i);
}

void PathUtil::trim(std::string& str)
{
	auto trim_path = str;

	int count = trim_path.size();
	int i = 0, j = trim_path.size() - 1;

	for (; i < count; ++i)
		if (!isspace(trim_path[i]))
			break;

	for (; j > i; --j)
		if (!isspace(trim_path[j]))
			break;

	// whole str is space
	if (j == i) {
		str = "";
	}
	else
		str = trim_path.substr(i, j - i + 1);
}

void PathUtil::trim_right(std::string& str)
{
	int i = str.size() - 1;

	for (; i > -1; --i)
		if (!isspace(str[i]))
			break;

	if (i == -1)
		str = "";
	else
		str = str.substr(0, i + 1);
}

void PathUtil::split(std::vector<std::string>& result,const std::string& str,const std::string& divide)
{
	int index = 0;
	int end = str.size();

	while (true) {
		int cur = str.find(divide, index);
		if (cur == std::string::npos)
		{
			if (index < end)
				result.push_back(str.substr(index));
			break;
		}

		result.push_back(str.substr(index, cur - index));
		index = cur + divide.size();
	}
}

void PathUtil::to_vector(const std::string & path, std::vector<std::string>& vec_path)
{
	auto trim_path = path;
	trim(trim_path);

	split(vec_path, trim_path, "\\");
}

std::string PathUtil::to_relative_path(const std::string & base, const std::string & path)
{
	std::vector<std::string> vec_base;
	std::vector<std::string> vec_path;
	std::vector<std::string> ret_path;

	to_vector(base, vec_base);
	to_vector(path, vec_path);

	int i_base = vec_base.size();
	int i_path = vec_path.size();
	int i_min = std::min(i_base, i_path);

	int index = 0;
	for (index; index < i_min; ++index) {
		if (vec_base[index] != vec_path[index])
			break;
	}

	if (index == 0)
		return path;

	ret_path.resize(i_base - index, "..");
	for (index; index < i_path; ++index)
		ret_path.push_back(vec_path[index]);

	return to_path(ret_path);
}

std::string PathUtil::to_path(const std::string & path)
{
	std::vector<std::string> vec_path;
	std::vector<std::string> ret_path;
	to_vector(path, vec_path);

	int count = vec_path.size();
	for (int i = 0; i < count; ++i)
	{
		std::string item = vec_path[i];

		if (item == ".")
		{
			continue;
		}
		else if (item == "..")
		{
			if (ret_path.empty())
				return path;

			ret_path.pop_back();
		}
		else
		{
			ret_path.push_back(item);
		}
	}

	return to_path(ret_path);
}

bool PathUtil::is_absolute_path(const std::string & path)
{
#ifdef _WIN32
	if (path.size() >= 2 && path[1] == ':') {
		if (path[0] >= 'c' && path[0] <= 'z' ||
			path[0] >= 'A' && path[0] <= 'Z')
			return true;
	}
#endif

	return false;
}

std::string PathUtil::get_folder(const std::string & filename)
{
	auto pos = filename.rfind('\\');
	if (pos == std::string::npos)
		return "";

	return filename.substr(0, pos + 1);
}
