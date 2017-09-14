#ifndef __LOG__
#define __LOG__
#include <string>
#include <functional>
#include <tuple>
#include <sstream>
#include <iostream>


/**
 * stream used to print format output of tuple
 */
class tuplestream
{
private:
	std::stringstream _ss;
public:
	template<typename T>
	void operator()(const T& value) 
	{
		_ss << value;
	}

	template<typename T, typename...Args>
	void operator()(const char* format, const T& head, Args...tail)
	{
		if (format)
		{
			while (*format)
			{
				if (*format == '%' && *++format != '%')
				{
					_ss << head;
					operator()(++format, tail...);
					return;
				}
				else
					_ss << *format++;
			}
		}
	}

	inline std::string str()
	{
		return _ss.str();
	}
};

/**
 *	log level
 */
enum logger_level {
	error,
	debug,
	info,
};


/**
 *	logger
 */
class logger
{
	using logger_proxy = std::function<void(int _level, const std::string& _tag, const std::string& _str) >;
	static void log_sunk(int _level, const std::string& _tag, const std::string& _str) 
	{
		std::cout << "@" << _level << "@" << _tag << "@" << _str << std::endl;
	}

private:
	logger_level _level;
	std::string _tag;
	logger_proxy _proxy;

public:
	logger(logger_level level = debug, const std::string& tag = "tag", logger_proxy proxy = log_sunk)
		: _level(level)
		, _tag(tag)
		, _proxy(proxy) {}
		

	void setLevel(logger_level level) { _level = level; }
	void setTag(const std::string& tag) { _tag = tag; }

	logger_level getLevel() { return _level; }
	std::string getTag() { return _tag; }

	template<class...Tail>
	void printf(const char* format, Tail...tail)
	{
		if (std::_Test_callable(_proxy))
		{
			tuplestream ts;
			ts(format, tail...);
			_proxy(_level, _tag, ts.str());
		}
	}
};

#endif //__LOG__