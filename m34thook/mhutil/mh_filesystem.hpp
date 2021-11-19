#pragma once
#include <cstdint>
enum class filemode_e
{
	READ = 0,
	WRITE = 1,
	APPEND = 2
} ;
using cs_fd_t = void*;

enum class seekmode_e {
	begin = 0,
	current = 1,
	end = 2
};

#define		MH_FILESYS_PATHBUFFER_LENGTH 512

static constexpr unsigned WIN32_INTERNAL_SIZEOF_UNICODE_STRING = 16;

//RtlDosPathNameToRelativeNtPathName_U
//temporary objects/buffers used by win32 path conversion
struct win32_path_conversion_context_extern_t {
	char m_unicode_str_storage[WIN32_INTERNAL_SIZEOF_UNICODE_STRING];
	wchar_t m_unicode_path_result[MH_FILESYS_PATHBUFFER_LENGTH];
};

namespace filesys {
	cs_fd_t open_file(const char* name, filemode_e mode);
	void close_file(cs_fd_t fd);
	std::int64_t file_length(cs_fd_t fd);

	std::int64_t read_file(cs_fd_t fd, void* buffer, size_t ntoread);
	std::int64_t write_file(cs_fd_t fd, const void* buffer, size_t size);



	/*
		using this is faster than the end-relative or current relative seek operations 
		with SetFilePointer, because those require an extra syscall (queryinformation) to 
		get the length or current pointer
	*/
	void seek_file(cs_fd_t fd, std::int64_t position );

	/*
		very fast file existence check. calls NtQueryAttributesFile, so does not
		require creating a handle to check
	*/
	bool file_exists(const char* filename);

	bool get_ntpath_for(const char* path, win32_path_conversion_context_extern_t* conversion_temp, wchar_t* output);


	bool move_file(const char* from, const char* to);

}