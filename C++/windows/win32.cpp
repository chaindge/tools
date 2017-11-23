#include "stdafx.h"
#include "win32.h"

#include "boost/algorithm/string.hpp"

void addlevel(std::string& dir)
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

void FindDictory(const std::string & dir, std::vector<std::string>& subdirs)
{
	std::string root = dir;
	addlevel(root);
	root += "*.*";

	WIN32_FIND_DATAA FindFileData;
	
	HANDLE hFind = ::FindFirstFileA(root.c_str(), &FindFileData);
	if (INVALID_HANDLE_VALUE == hFind)return;

	while (true)
	{
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (FindFileData.cFileName[0] != '.')
			{
				subdirs.push_back(FindFileData.cFileName);
			}
		}
		if (!::FindNextFileA(hFind, &FindFileData))break;
	}
	::FindClose(hFind);
}

bool RenameDir(const std::string & from, const std::string & to)
{
	auto ret = ::MoveFileA(from.c_str(), to.c_str());
	auto err = GetLastError();

	return (ret == TRUE);
}

bool DeleteDir(const std::string & dir)
{
// 	std::string tdir = dir + "\0";
// 	SHFILEOPSTRUCTA FileOp;
// 	FileOp.fFlags = FOF_NOCONFIRMATION;
// 	FileOp.hNameMappings = NULL;
// 	FileOp.hwnd = NULL;
// 	FileOp.lpszProgressTitle = NULL;
// 	FileOp.pFrom = tdir.c_str();
// 	FileOp.pTo = NULL;
// 	FileOp.wFunc = FO_DELETE;
// 
// 	auto ret = SHFileOperationA(&FileOp);
// 	return  ret == 0;

	std::string root = dir;
	addlevel(root);
	root += "*.*";

	CFileFind tempFind;
	BOOL IsFinded = tempFind.FindFile(root.c_str());
	while (IsFinded)
	{
		IsFinded = tempFind.FindNextFile();

		if (!tempFind.IsDots())
		{
			std::string sub_path = dir + "\\" + tempFind.GetFileName().GetString();
			if (tempFind.IsDirectory())
			{
				DeleteDir(sub_path);
			}
			else
			{
				::DeleteFileA(sub_path.c_str());
			}
		}
	}
	tempFind.Close();
	auto ret = ::RemoveDirectoryA(dir.c_str());
	return ret == TRUE;
}

bool CreateDir(const std::string & dir)
{
	auto ret = ::CreateDirectoryA(dir.c_str(),nullptr);
	return (ret == TRUE);
}

std::string GetModulePath()
{
	char temp[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, temp, sizeof(temp));

	std::string str = (temp);
	auto pos = str.find_last_of('\\');
	return str.substr(0, pos + 1);
}

std::string JoinPath(const std::string & root, const std::string & dir)
{
	std::string ret = root;

	addlevel(ret);

	if (dir.size() != 0 && dir[0] == '\\')
		ret += dir.substr(1);
	else 
		ret += dir;

	return ret;
}

void FileInitialize(HWND hwnd,
	OPENFILENAMEA& ofn,
	const std::string& defaultFileTypeDescription,
	const std::string& defaultFileExtension)
{
	std::string filter = defaultFileTypeDescription + '\0' + defaultFileExtension + '\0' +
		"All Files (*.*)" + '\0' + "*.*" + '\0' + '\0';

	static char szFilter[255];

	for (unsigned int i = 0; i < filter.size(); ++i)
	{
		szFilter[i] = filter.at(i);
	}

	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hwnd;
	ofn.hInstance = NULL;
	ofn.lpstrFilter = szFilter;
	ofn.lpstrCustomFilter = NULL;
	ofn.nMaxCustFilter = 0;
	ofn.nFilterIndex = 0;
	ofn.lpstrFile = NULL;          // Set in Open and Close functions
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFileTitle = NULL;          // Set in Open and Close functions
	ofn.nMaxFileTitle = MAX_PATH;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = NULL;
	ofn.Flags = 0;             // Set in Open and Close functions
	ofn.nFileOffset = 0;
	ofn.nFileExtension = 0;
	ofn.lpstrDefExt = defaultFileExtension.c_str();
	ofn.lCustData = 0L;
	ofn.lpfnHook = NULL;
	ofn.lpTemplateName = NULL;

}



BOOL FileOpenDlg(HWND  hwnd,
	char*              pstrFileName,
	char*              pstrTitleName,
	const std::string& title,
	const std::string& initDir,
	const std::string& defaultFileTypeDescription,
	const std::string& defaultFileExtension)
{
	OPENFILENAMEA ofn;

	FileInitialize(hwnd, ofn, defaultFileTypeDescription, defaultFileExtension);

	ofn.lpstrFile = pstrFileName;
	ofn.lpstrFileTitle = pstrTitleName;
	ofn.Flags = OFN_HIDEREADONLY | OFN_CREATEPROMPT | OFN_NOCHANGEDIR;
	ofn.lpstrTitle = title.c_str();
	ofn.lpstrInitialDir = initDir.c_str();

	return GetOpenFileNameA(&ofn);
}


std::string get_folder_of_path(const std::string& path)
{
	auto pos = path.rfind('\\');
	if (pos == std::string::npos)
		return "";

	return path.substr(0, pos + 1);
}

std::string get_full_path(const std::string& file)
{
	return "";
}

std::string PathHandler::to_path(const std::vector<std::string>& vec_path)
{
	std::string ret;
	for (int i = 0; i < vec_path.size(); ++i)
	{
		ret += vec_path[i];
		if (i != vec_path.size() - 1)
			ret += "\\";
	}

	return ret;
}

void PathHandler::to_vector(const std::string & path, std::vector<std::string>& vec_path)
{
	//boost::trim(path);

	boost::split(vec_path, path, boost::is_any_of("\\"));

	if (vec_path.rbegin()->empty())
		vec_path.pop_back();

}

std::string PathHandler::to_relative_path(const std::string & base, const std::string & path)
{
	std::vector<std::string> vec_base;
	std::vector<std::string> vec_path;
	std::vector<std::string> ret_path;

	to_vector(base, vec_base);
	to_vector(path, vec_path);

	int i_base = vec_base.size();
	int i_path = vec_path.size();
	int i_min = min(i_base, i_path);

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

std::string PathHandler::to_path(const std::string & path)
{
	std::vector<std::string> vec_path;
	std::vector<std::string> ret_path;
	to_vector(path, vec_path);

	for (int i = 0; i < vec_path.size(); ++i)
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

bool PathHandler::is_absolute_path(const std::string & path)
{
	if (path.size() >= 2 && path[1] == ':')
		return true;

	return false;
}
