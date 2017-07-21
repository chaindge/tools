#ifndef __ZIPACHIEVE_H__
#define __ZIPACHIEVE_H__
#include <string>

#include "zlib/zip.h"
#include "zlib/unzip.h"
#include "zlib/iowin32.h"

/*#pragma comment(lib,"zip_static")*/
/*#pragma comment(lib,"libzlib")*/

#define WRITEBUFFERSIZE (16384)

#include <unordered_map>

class ZipAchieve
{
public:
	enum OpenTag { ZIP, UNZIP };

	ZipAchieve();

	ZipAchieve(const std::string& filename, OpenTag tag = ZIP);

	~ZipAchieve();

	bool zip_open(const std::string& filename);
	bool unz_open(const std::string& filename);

	bool zip_close();
	bool unz_close();

	bool close();

	// zip
	bool add_file(const std::string& inzip_name, const std::string& filePath);
	bool add_stream(const std::string& inzip_name, void* pStream, size_t size);

	// unzip
	bool read_file(const std::string& inzip_name, const std::string& filePath);
	bool read_stream(const std::string& inzip_name, void* pStream, size_t size);
	uint64_t get_file_size(const std::string& inzip_name);
	bool is_file_exist(const std::string& inzip_name);

	// error
	int get_error();
	int get_close_error();
	std::string get_str_error() { return _str_err; }
protected:
	int write_currentfile(void* pStream, unsigned int count);

	bool open_newfile_inzip(const std::string& name);

	bool close_currentfile_inzip();

	int unz_get_file_list();
	bool make_file_current(const std::string& inzip_name);
	bool unz_open_current_file();
	bool unz_close_current_file();
	int unz_read_currentfile(void* pStream, unsigned int count);
private:
	zipFile _zip_file;
	unzFile _unz_file;
	int _errclose;
	int _err;
	std::string _str_err;

	zlib_filefunc64_def _ffunc;
	std::unordered_map<std::string, unz_file_info64> _unz_file_list;
};


#endif //__ZIPACHIEVE_H__
