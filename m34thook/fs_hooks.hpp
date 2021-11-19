#pragma once
#include <cstdio>
#include "idStr.hpp"

void hook_idfilesystem();

#define	OVERRIDE_PATHBUF_SIZE		512

FILE* get_override_for_resource(const char* name, size_t* size_out);

//returns length of final string
unsigned get_override_path(const char* override_relative, char (&buf)[OVERRIDE_PATHBUF_SIZE]);
struct idFile;
enum fsLock_t {
	FS_LOCK_SHARED = 0,
	FS_LOCK_EXCLUSIVE = 1,
};
enum fsOrigin_t {
	FS_SEEK_CUR = 0,
	FS_SEEK_END = 1,
	FS_SEEK_SET = 2,
};
#if 1
/*
	checked this against the generated code and vtbl in the game, it matches up perfectly!
*/
class cs_idFile_t {
public:
	virtual   ~cs_idFile_t() {}
	virtual bool  IsIDFile_Memory() = 0;
	virtual bool  IsIDFile_Permanent() = 0;
	virtual bool  isIDFile_Verified() = 0;
	virtual const char* GetFullPath() = 0;
	virtual const char* GetName() = 0;
	virtual uint64_t Read(void*, unsigned int len) = 0;
	virtual uint64_t Write(const void*, unsigned int) = 0;
	virtual uint64_t ReadOfs(long long offset, void*, unsigned int) = 0;
	virtual uint64_t WriteOfs(long long offset, const void*, unsigned int) = 0;
	virtual void* chunky_op(void*, void*, void*) = 0;
	virtual bool  Lock(unsigned int, fsLock_t) = 0;
	virtual bool  Unlock(unsigned int) = 0;
	virtual size_t Length() = 0;
	virtual void  SetLength(unsigned int newlength) = 0;
	virtual size_t Tell() = 0;
	virtual int   SeekEx(long long, fsOrigin_t origin) = 0;
	virtual size_t Printf(const char*, ...) = 0;
	virtual size_t VPrintf(const char* fmt, va_list va) = 0;
	virtual size_t WriteFloatString(const char*, ...) = 0;
	virtual size_t WriteFloatStringVA(const char* fmt, va_list va) = 0;
	virtual size_t Timestamp() = 0;
	virtual bool  IsWritable() = 0;
	virtual void  Flush() = 0;
	virtual void  ForceFlush() = 0;
	virtual size_t GetSectorSize() = 0;
	virtual int  GetDevice() = 0;
	virtual bool  IsOSNative() = 0;
	virtual int  GetFileErrorCode() = 0;
	virtual void  SetFileError() = 0;
	virtual size_t ReadString(idStr*) = 0;
	virtual size_t ReadDebugTag(const char*, const char*, int) = 0;
	virtual uint64_t WriteDebugTag(const char*, const char*) = 0;

};
struct idFileVftbl
{
	void (*dctor)(idFile* thiz, unsigned int);
	bool (*IsIDFile_Memory)(const idFile*);
	bool (*IsIDFile_Permanent)(const idFile*);
	//name is an assumption, only returns true for idfile_verified
	bool (*isIDFile_Verified)(const idFile*);
	const char* (*GetFullPath)(idFile* thiz);
	const char* (*GetName)(idFile* thiz);
	uint64_t(*Read)(idFile* thiz, void*, unsigned int len);
	uint64_t(*Write)(idFile* thiz, const void*, unsigned int);
	uint64_t(*ReadOfs)(idFile* thiz, long long offset, void*, unsigned int);
	uint64_t(*WriteOfs)(idFile* thiz, long long offset, const void*, unsigned int);
	void* (*chunky_op)(idFile*, void*, void*, void*);
	bool (*Lock)(idFile*, unsigned int, fsLock_t);
	bool (*Unlock)(idFile*, unsigned int);
	size_t(*Length)(const idFile* thiz);
	void (*SetLength)(idFile* thiz, unsigned int newlength);
	size_t(*Tell)(const idFile* thiz);
	int  (*SeekEx)(idFile* thiz, long long, fsOrigin_t origin);
	size_t(*Printf)(idFile* thiz, const char*, ...);
	size_t(*VPrintf)(idFile* thiz, const char* fmt, va_list va);
	size_t(*WriteFloatString)(idFile* thiz, const char*, ...);
	size_t(*WriteFloatStringVA)(idFile* thiz, const char* fmt, va_list va);
	size_t(*Timestamp)(const idFile* thiz);
	bool (*IsWritable)(const idFile* thiz);
	void (*Flush)(idFile*);
	void (*ForceFlush)(idFile*);
	size_t(*GetSectorSize)(const idFile*);
	int (*GetDevice)(const idFile*);
	bool (*IsOSNative)(const idFile*);
	int (*GetFileErrorCode)(const idFile*);
	void (*SetFileError)(idFile*);
	size_t(*ReadString)(idFile* thiz, idStr*);
	size_t(*ReadDebugTag)(idFile* thiz, const char*, const char*, int);
	uint64_t(*WriteDebugTag)(idFile* thiz, const char*, const char*);
};

struct idFile {
	idFileVftbl* vftbl;
};
#endif