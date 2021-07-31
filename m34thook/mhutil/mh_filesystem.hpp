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
}