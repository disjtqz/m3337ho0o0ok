#include "mh_defs.hpp"
#include "mhutil/mh_filesystem.hpp"

#include <Windows.h>
#include "win32/ntdll.h"
#include "snaphakalgo.hpp"
#include <filesystem>
static_assert(sizeof(_UNICODE_STRING) == WIN32_INTERNAL_SIZEOF_UNICODE_STRING);
CS_COLD_CODE
void* vmem_map_file(cs_fd_t handle, void** extra, size_t size, size_t offset) {

	*extra = nullptr;
	auto mapping = CreateFileMappingA(handle, nullptr, PAGE_READWRITE, size >> 32, size & 0xFFFFFFFFull, nullptr);
	if (!mapping) {
		return nullptr;
	}

	void* mapped_mem = MapViewOfFileEx(mapping, FILE_MAP_ALL_ACCESS, offset >> 32, offset & 0xFFFFFFFFuLL, size, nullptr);
	if (!mapped_mem) {
		NtClose(mapping);
		return nullptr;
	}
	*extra = (void*)mapping;
	return mapped_mem;
}
CS_COLD_CODE
void vmem_close_mapping(void* mapped_mem, void* extra) {
	UnmapViewOfFile(mapped_mem);
	NtClose((HANDLE)extra);
}
cs_fd_t filesys::open_file(const char* name, filemode_e mode) {
	cs_fd_t fp;
	DWORD acc = 0;
	DWORD creat = 0;

	if (mode == filemode_e::WRITE)
	{
		acc = GENERIC_READ | GENERIC_WRITE;
		creat = CREATE_ALWAYS;
	}
	else if (mode == filemode_e::READ)
	{
		acc = GENERIC_READ;
		creat = OPEN_EXISTING;
	}
	else if (mode == filemode_e::APPEND)
	{
		acc = GENERIC_READ | GENERIC_WRITE;
		creat = OPEN_ALWAYS;
	}

	fp = CreateFileA(name, acc, FILE_SHARE_READ, NULL, creat, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fp == INVALID_HANDLE_VALUE)
	{
		return NULL;
	}
	return fp;
}

void filesys::close_file(cs_fd_t fd) {
	NtClose((HANDLE)fd);
}

struct file_length_args_t {
	_IO_STATUS_BLOCK m_status_block;
	uint64_t m_length[3];

};

int64_t filesys::file_length(cs_fd_t fd) {
	file_length_args_t tmpargsbase;
	file_length_args_t* tmpargs = &tmpargsbase;
	NtQueryInformationFile((HANDLE)fd, &tmpargs->m_status_block, &tmpargs->m_length, 24, FileStandardInformation);

	return tmpargsbase.m_length[1];
}

int64_t filesys::read_file(cs_fd_t fd, void* buffer, size_t ntoread) {
	DWORD nread = 0;
	
	ReadFile((HANDLE)fd, buffer, ntoread, &nread, nullptr);

	return nread;
}

int64_t filesys::write_file(cs_fd_t fd, const void* buffer, size_t size) {
	DWORD nwrite = 0;
	WriteFile((HANDLE)fd, buffer, size, &nwrite, nullptr);
	return nwrite;
}

void filesys::seek_file(cs_fd_t fd, std::int64_t position) {
	IO_STATUS_BLOCK iblock;

	NtSetInformationFile(fd, &iblock, &position, sizeof(std::int64_t), FilePositionInformation);

}

//

/*
	this no worky worky

	jk, fixed it (unicode string length/maxlength stores the length in BYTES, so i just had to do * 2)
*/
static
bool filesys::file_exists(const char* filename) {

	//if (sh::syscall_interface_available()) {
		wchar_t tmppath[MH_FILESYS_PATHBUFFER_LENGTH];

		tmppath[0] = '\\';
		tmppath[1] = '?';
		tmppath[2] = '?';
		tmppath[3] = '\\';

		// \??\C:\

		unsigned namelen = sh::string::to_unicode(&tmppath[4], filename);
		namelen += 4;
		UNICODE_STRING tmpstr;
		tmpstr.Buffer = tmppath;
		tmpstr.Length = namelen * 2;
		tmpstr.MaximumLength = namelen * 2;



		OBJECT_ATTRIBUTES objattr;

		sh::memops::smol_memzero(&objattr, sizeof(OBJECT_ATTRIBUTES));

		FILE_BASIC_INFORMATION info;
		objattr.ObjectName = &tmpstr;

		objattr.Length = sizeof(OBJECT_ATTRIBUTES);
		objattr.Attributes = 64;

	//	MH_UNLIKELY_IF(!sh::syscall_interface_available()) {
			return NtQueryAttributesFile(&objattr, &info) >= 0 && info.FileAttributes != -1;
	//	}
	//	else {
	//		return sh::perform_syscall<_ZwQueryAttributesFile, NTSTATUS>(&objattr, &info) >= 0 && info.FileAttributes != -1;
	//	}
	
/* }
	else {
		cs_fd_t result = open_file(filename, filemode_e::READ);

		if (result)
			NtClose(result);
		return result != nullptr;
	}*/
}

static BOOLEAN(*g_fnptr_RtlDosPathNameToRelativeNtPathName_U)(
	_In_       PCWSTR DosFileName,
	_Out_      PUNICODE_STRING NtFileName,
	_Out_opt_  PWSTR* FilePath,
	_Out_opt_  void* RelativeName
	) = nullptr;


MH_REGFREE_CALL
MH_NOINLINE
static void init_RtlDosPathNameToRelativeNtPathName_U() {
	g_fnptr_RtlDosPathNameToRelativeNtPathName_U = (decltype(g_fnptr_RtlDosPathNameToRelativeNtPathName_U))GetProcAddress(GetModuleHandleA("ntdll.dll"), "RtlDosPathNameToRelativeNtPathName_U");
}

bool filesys::get_ntpath_for(const char* path, win32_path_conversion_context_extern_t* conversion_temp, wchar_t* output) {
	MH_UNLIKELY_IF(!g_fnptr_RtlDosPathNameToRelativeNtPathName_U) {
		init_RtlDosPathNameToRelativeNtPathName_U();
	}

	sh::memops::smol_memzero(conversion_temp, sizeof(*conversion_temp));
	sh::memops::smol_memzero(output, MH_FILESYS_PATHBUFFER_LENGTH);
	unsigned unilength = sh::string::to_unicode(conversion_temp->m_unicode_path_result, path);
	_UNICODE_STRING* puni = reinterpret_cast<_UNICODE_STRING*>(&conversion_temp->m_unicode_str_storage[0]);




	bool res = g_fnptr_RtlDosPathNameToRelativeNtPathName_U(conversion_temp->m_unicode_path_result, puni, nullptr, nullptr);
	if (!res)
		return false;
	if (puni->Buffer) {
		sh::memops::smol_memcpy(output, puni->Buffer, puni->Length);

		output[puni->Length / 2] = 0;

		HeapFree(GetProcessHeap(), 0, puni->Buffer);
	}
	return res;
}

bool filesys::move_file(const char* from, const char* to) {
	return MoveFileExA(from, to, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING);
}