#ifndef UTILS_H
#define UTILS_H
#include <string>
#include <windows.h>
#include <sstream>
#include <fstream>

inline std::string GetModulePath()
{
	char temp[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, temp, sizeof(temp));

	std::string str = (temp);
	auto pos = str.find_last_of('\\');
	return str.substr(0, pos + 1);
}

inline std::string GetModuleChildDir(const std::string& ChildDir)
{
	auto ret = GetModulePath() + ChildDir;
	
	int size = ret.size();
	if (size != 0 && ret[size - 1] != '\\')
		ret += "\\";

	return ret;
}

inline bool isKeyDown(int key)
{
	return ((GetAsyncKeyState(key) & 0x8000) == 0x8000);
}

inline std::string gbk2utf8(const std::string & str_gbk)
{
	int len = MultiByteToWideChar(CP_ACP, 0, str_gbk.c_str(), -1, NULL, 0);
	wchar_t * wszUtf8 = new wchar_t[len + 1];
	memset(wszUtf8, 0, sizeof(wchar_t) * (len + 1));
	MultiByteToWideChar(CP_ACP, 0, str_gbk.c_str(), -1, wszUtf8, len);

	len = WideCharToMultiByte(CP_UTF8, 0, wszUtf8, -1, NULL, 0, NULL, NULL);
	char *szUtf8 = new char[len + 1];
	memset(szUtf8, 0, len + 1);
	auto l = WideCharToMultiByte(CP_UTF8, 0, wszUtf8, -1, szUtf8, len, NULL, NULL);

	std::string utf8 = szUtf8;
	delete[] szUtf8;
	delete[] wszUtf8;

	return utf8;
}

inline std::string utf82gbk(const std::string & str_utf8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, str_utf8.c_str(), -1, NULL, 0);
	wchar_t * wszGBK = new wchar_t[len + 1];
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, str_utf8.c_str(), -1, wszGBK, len);

	len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
	char *szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);

	std::string gbk = szGBK;
	delete[] szGBK;
	delete[] wszGBK;

	return gbk;
}


inline std::string toString(const TCHAR * str)
{
#ifdef  UNICODE   
	int len = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	char *szm = new char[len + 1];
	memset(szm, 0, len + 1);

	WideCharToMultiByte(CP_ACP, 0, str, -1, szm, len, NULL, NULL);

	std::string ret = szm;
	delete[] szm;
	return ret;
#else
	std::string ret = str;
	return ret;
#endif
}

inline bool IsPathExist(const string & path)
{
	DWORD dwAttrib = GetFileAttributesA(path.c_str());
	return INVALID_FILE_ATTRIBUTES != dwAttrib;
}

inline std::string replace_all(const std::string& str, const std::string& from, const std::string& to)
{
	std::string ret = str;
	size_t pos = 0;

	while (true)
	{
		pos = ret.find(from, pos);
		if (pos == std::string::npos)
			break;

		ret.replace(pos, from.length(), to);
		pos += to.length();
	}
	return ret;
}

inline void addlevel(std::string& dir)
{
	auto size = dir.size();
	if (size == 0)
	{
		dir += "\\";
		return;
	}

	if (dir[size - 1] != '\\')
		dir += "\\";
}

inline void FindFile(const std::string & dir, std::vector<std::string>& files,const std::string& filter)
{
	std::string root = dir;
	addlevel(root);
	root += "*.*";

	WIN32_FIND_DATAA FindFileData;

	HANDLE hFind = ::FindFirstFileA(root.c_str(), &FindFileData);
	if (INVALID_HANDLE_VALUE == hFind)return;

	while (true)
	{
		if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			std::string tmp = FindFileData.cFileName;
			if(tmp.find(filter) == std::string::npos)
				files.push_back(FindFileData.cFileName);
		}
		if (!::FindNextFileA(hFind, &FindFileData))break;
	}
	::FindClose(hFind);
}


inline bool ReadFile(const std::string & path, std::string & content)
{
	bool ret = false;
	std::ifstream file(path);
	std::stringstream ss;

	if (file)
	{
		// 		content = std::string(std::istreambuf_iterator<char>(file),
		// 			std::istreambuf_iterator<char>(file));

		ss << file.rdbuf();

		content = ss.str();
		ret = true;
	}
	file.close();
	return ret;
}

inline std::string getNameFromPath(const std::string& path)
{
	int pos = path.rfind('\\');
	if (pos != std::string::npos)
	{
		return path.substr(pos + 1);
	}
	return path;
}

inline CString GetFileExtention(const CString& fileName)
{
	TCHAR szDrive[_MAX_DRIVE], szDir[_MAX_DIR], szFileName[_MAX_FNAME], szExt[_MAX_EXT];

	szDrive[0] = _T('\0');
	szDir[0] = _T('\0');
	szFileName[0] = _T('\0');
	szExt[0] = _T('\0');

	SPLITPATH_S(fileName, szDrive, szDir, szFileName, szExt);

	return szExt;
}
#endif // !UTILS_H
