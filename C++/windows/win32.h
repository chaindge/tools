#pragma once
#include <string>
#include <vector>

void addlevel(std::string& dir);
void FindDictory(const std::string& dir, std::vector<std::string>& subdirs);
bool RenameDir(const std::string& from, const std::string& to);
bool DeleteDir(const std::string& dir);
bool CreateDir(const std::string& dir);

std::string GetModulePath();

std::string JoinPath(const std::string& root,const std::string& dir);

// path 要求绝对路径
std::string get_folder_of_path(const std::string& path);
std::string get_full_path(const std::string& file);

BOOL FileOpenDlg(HWND  hwnd,
	char*              pstrFileName,
	char*              pstrTitleName,
	const std::string& title,
	const std::string& initDir = "",
	const std::string& defaultFileTypeDescription = "",
	const std::string& defaultFileExtension = "");


class CSARTFolderPickerDialog : public CFolderPickerDialog
{
public:
	CSARTFolderPickerDialog(CWnd* pWnd, const std::string& title, const std::string& initDir = "")
		:CFolderPickerDialog(0, 0, pWnd) 
	{
		GetOFN().lpstrTitle = title.c_str();
	}

	inline bool GetFolder(std::string& out_folder)
	{
		if (IDOK == DoModal())
		{
			out_folder = m_szFileName;
			return true;
		}
		return false;
	}

	inline std::string GetSelectFolder() { return std::string(m_szFileName); }
};

inline bool SelectFolder(CWnd* pWnd, std::string& selected_folder, const std::string& title, const std::string& initDir = "")
{
	CSARTFolderPickerDialog fd(pWnd, title, initDir);
	return fd.GetFolder(selected_folder);
}

class PathHandler
{
public:
	static std::string to_path(const std::vector<std::string>& vec_path);
	static void to_vector(const std::string& path, std::vector<std::string>& vec_path);
	static std::string to_relative_path(const std::string& base, const std::string& path);
	static std::string to_path(const std::string& path);
	static bool is_absolute_path(const std::string& path);
};