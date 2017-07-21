#include "stdafx.h"
#include "ZipAchieve.h"

ZipAchieve::ZipAchieve()
	: _zip_file(nullptr)
	, _unz_file(nullptr)
	, _errclose(0)
	, _err(0)
	, _str_err("")
{
	fill_win32_filefunc64A(&_ffunc);
}

ZipAchieve::ZipAchieve(const std::string& filename, OpenTag tag)
{
	_zip_file = nullptr;
	_unz_file = nullptr;
	_errclose = 0;
	_err = 0;
	_str_err = "";

	fill_win32_filefunc64A(&_ffunc);
	
	if (tag == ZIP)
		zip_open(filename);
	else if (tag == UNZIP)
		unz_open(filename);
}

ZipAchieve::~ZipAchieve()
{
	close();
}

bool ZipAchieve::zip_open(const std::string& filename)
{
	if (_zip_file == nullptr)
	{
		_zip_file = zipOpen2_64(filename.c_str(), APPEND_STATUS_CREATE, NULL, &_ffunc);
		return _zip_file != nullptr;
	}
	return false;
}

bool ZipAchieve::unz_open(const std::string & filename)
{
	if (_unz_file == nullptr)
	{
		_unz_file = unzOpen2_64(filename.c_str(), &_ffunc);
		unz_get_file_list();

		return _unz_file != nullptr;
	}
	return false;
}

bool ZipAchieve::zip_close()
{
	if (_zip_file)
	{
		_errclose = zipClose(_zip_file, NULL);
		_zip_file = nullptr;
		
		return ZIP_ERRNO == _errclose;
	}

	return true;
}

bool ZipAchieve::unz_close()
{
	if (_unz_file)
	{
		_errclose = unzClose(_unz_file);
		_unz_file = nullptr;
		_unz_file_list.clear();

		return _errclose == UNZ_OK;
	}

	return true;
}

bool ZipAchieve::close()
{
	bool zip = zip_close();
	bool unz = unz_close();

	return zip && unz;
}

bool ZipAchieve::add_file(const std::string& inzip_name, const std::string& filePath)
{
	if (_zip_file)
	{
		// open file
		bool is_ok = true;

		FILE* fin = nullptr;
		fin = fopen(filePath.c_str(), "rb");
		if (fin == nullptr)
		{
			_str_err = "error in opening " + filePath + " for reading";
			is_ok = false;
		}

		// allocate buf
		void* buf = nullptr;
		int size_buf = WRITEBUFFERSIZE;

		if (is_ok) {
			buf = (void*)malloc(size_buf);
			if (buf == nullptr)
			{
				_str_err = "Error allocating memory";
				is_ok = false;
			}
		}

		// open file in zip
		if (is_ok) {
			if (!open_newfile_inzip(inzip_name))
			{
				is_ok = false;
			}
		}

		if (!is_ok)
		{
			if (fin)
			{
				fclose(fin);
				fin = nullptr;
			}

			if (buf)
			{
				free(buf);
				buf = nullptr;
			}

			return false;
		}

		// read file to zip
		int err = ZIP_OK;
		int size_read = 0;

		do
		{
			err = ZIP_OK;
			size_read = (int)fread(buf, 1, size_buf, fin);
			if (size_read < size_buf)
				if (feof(fin) == 0)
				{
					_str_err = "error in reading " + filePath;
					err = ZIP_ERRNO;
				}

			if (size_read > 0)
			{
				err = write_currentfile(buf, size_read);
				if (err < 0)
				{
					_str_err = "error in writing " + inzip_name + " in the zipfile";
				}

			}
		} while ((err == ZIP_OK) && (size_read > 0));


		fclose(fin);
		free(buf);

		close_currentfile_inzip();
		return true;
	}
	return false;
}

bool ZipAchieve::add_stream(const std::string& inzip_name, void* pStream, size_t size)
{
	if (_zip_file)
	{
		if (open_newfile_inzip(inzip_name))
		{
			write_currentfile(pStream, size);
			close_currentfile_inzip();
			return true;
		}
	}

	return false;
}


int ZipAchieve::get_error() { return _err; }
int ZipAchieve::get_close_error() { return _errclose; }

int ZipAchieve::write_currentfile(void* pStream, unsigned int count)
{
	_err = zipWriteInFileInZip(_zip_file, pStream, count);
	return _err;
}

bool ZipAchieve::open_newfile_inzip(const std::string& name)
{
	zip_fileinfo zi;
	unsigned long crcFile = 0;
	int zip64 = 0;

	zi.tmz_date.tm_sec = zi.tmz_date.tm_min = zi.tmz_date.tm_hour =
		zi.tmz_date.tm_mday = zi.tmz_date.tm_mon = zi.tmz_date.tm_year = 0;
	zi.dosDate = 0;
	zi.internal_fa = 0;
	zi.external_fa = 0;

	_err = zipOpenNewFileInZip3_64(_zip_file, name.c_str(), &zi,
		NULL, 0, NULL, 0, NULL,
		Z_DEFLATED,
		Z_BEST_COMPRESSION, 0,
		-MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
		nullptr, crcFile, zip64);

	return _err == ZIP_OK;
}

bool ZipAchieve::close_currentfile_inzip()
{
	_err = zipCloseFileInZip(_zip_file);
	return _err == ZIP_ERRNO;
}

int ZipAchieve::unz_get_file_list()
{
	if (_unz_file == nullptr) return 0;

	unz_global_info64 gi;

	char array[256] = { 0 };
	_err = unzGetGlobalInfo64(_unz_file, &gi);
	if (_err != UNZ_OK)
	{
		sprintf(array, "error %d with zipfile in unzGetGlobalInfo", _err);
		_str_err = array;

		return 0;
	}

	_err = unzGoToFirstFile(_unz_file);
	while (_err == UNZ_OK)
	{
		char filename_inzip[256] = { 0 };
		unz_file_info64 file_info;
		_err = unzGetCurrentFileInfo64(_unz_file, &file_info, filename_inzip, sizeof(filename_inzip), NULL, 0, NULL, 0);
		_unz_file_list[std::string(filename_inzip)] = file_info;

		if (_err == UNZ_OK)
		{
			_err = unzGoToNextFile(_unz_file);
		}
	}

	return _err;
}


bool ZipAchieve::make_file_current(const std::string & inzip_name)
{
	if (_unz_file)
	{
		_err = unzLocateFile(_unz_file, inzip_name.c_str(), 1);
		return _err == UNZ_OK;
	}
	return false;
}

bool ZipAchieve::unz_open_current_file()
{
	_err = unzOpenCurrentFile(_unz_file);
	return UNZ_OK == _err;
}

bool ZipAchieve::unz_close_current_file()
{
	_err = unzCloseCurrentFile(_unz_file);
	return UNZ_OK == _err;
}


uint64_t ZipAchieve::get_file_size(const std::string & inzip_name)
{
	for (auto& info : _unz_file_list)
		if (info.first == inzip_name)
			return info.second.uncompressed_size;
	return 0;
}

bool ZipAchieve::is_file_exist(const std::string& inzip_name)
{
	for (auto& info : _unz_file_list)
		if (info.first == inzip_name)
			return true;
	return false;
}

bool ZipAchieve::read_file(const std::string& inzip_name, const std::string& filePath)
{
	if (make_file_current(inzip_name))
	{
		if (unz_open_current_file())
		{
			// open file
			bool is_ok = true;

			FILE* fout = nullptr;
			fout = fopen(filePath.c_str(), "wb");
			if (fout == nullptr)
			{
				_str_err = "error in opening " + filePath + " for writing";
				is_ok = false;
			}

			// allocate buf
			void* buf = nullptr;
			int size_buf = WRITEBUFFERSIZE;

			if (is_ok) {
				buf = (void*)malloc(size_buf);
				if (buf == nullptr)
				{
					_str_err = "Error allocating memory";
					is_ok = false;
				}
			}

			if (!is_ok)
			{
				if (fout)
				{
					fclose(fout);
					fout = nullptr;
				}

				if (buf)
				{
					free(buf);
					buf = nullptr;
				}

				return false;
			}

			// read 
			do
			{
				_err = unzReadCurrentFile(_unz_file, buf, size_buf);
				if (_err < 0)
				{
					char aray[256] = { 0 };
					sprintf(aray,"error %d with zipfile in unzReadCurrentFile", _err);
					_str_err = aray;
					break;
				}
				if (_err > 0)
					if (fwrite(buf, _err, 1, fout) != 1)
					{
						_str_err = "error in writing extracted file";
						_err = 0;
						break;
					}
			} while (_err > 0);

			fclose(fout);
			free(buf);

			unz_close_current_file();
			return true;
		}
	}

	return false;
}

int ZipAchieve::unz_read_currentfile(void * pStream, unsigned int count)
{
	_err = unzReadCurrentFile(_unz_file, pStream, count);
	return _err;
}

bool ZipAchieve::read_stream(const std::string& inzip_name, void* pStream, size_t size)
{
	if (make_file_current(inzip_name))
	{
		if (unz_open_current_file())
		{
			unz_read_currentfile(pStream, size);
			unz_close_current_file();
			return true;
		}
	}

	return false;
}