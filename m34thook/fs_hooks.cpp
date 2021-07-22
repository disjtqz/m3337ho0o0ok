
#include "mh_defs.hpp"
#include "xbyak/xbyak.h"

#include <Windows.h>
#include "game_exe_interface.hpp"
#include "memscan.hpp"
#include "doomoffs.hpp"
#include "meathook.h"
#include "scanner_core.hpp"
#include "fs_hooks.hpp"
#include "memorySystem.hpp"
#include <map>
#include <string>
#include "idStr.hpp"
//idFileSystemLocal::ReadFile(char const*,void **,idFileProps *,fsPathSearch_t)	.text	0000007101D87CB0	00000178	00000040	FFFFFFFFFFFFFFF8	R	.	.	.	B	T	.
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
#define	USE_FILE_LOCKING

#ifdef USE_FILE_LOCKING
#define		STDIOFN_LOCK(name, ...)		_##name##_nolock (__VA_ARGS__)
#define		EXTIOFN_LOCK(name, ...)		name##_nolock (__VA_ARGS__)
#else
#define		STDIOFN_LOCK(name, ...)		name (__VA_ARGS__)
#define		EXTIOFN_LOCK(name, ...)		name (__VA_ARGS__)
#endif
struct cs_idfile_override_t : public idFile {
	FILE* m_cfile;
	const char* m_fname;
	const char* m_fullpath;
	bool m_readable;
	bool m_writable;
	void destroy() {
		if (m_cfile) {

			STDIOFN_LOCK(fclose, m_cfile);
			m_cfile = nullptr;
			m_fullpath = nullptr;
			m_fname = nullptr;
			m_readable = false;
			m_writable = false;
		}
	}

	static bool return_false(const idFile* f) {
		return false;
	}

	static void deconstructor(idFile* thiz, unsigned int) {
		reinterpret_cast<cs_idfile_override_t*>(thiz)->destroy();
		//freeing us causes crash
		//idMemorySystem_free(thiz);
	}

	static const char* get_full_path(idFile* thiz) {
		return reinterpret_cast<cs_idfile_override_t*>(thiz)->m_fullpath;
	}

	static const char* get_name(idFile* thiz) {
		return reinterpret_cast<cs_idfile_override_t*>(thiz)->m_fname;
	}

	static size_t do_read(idFile* thiz, void* buffer, unsigned int len) {

		return STDIOFN_LOCK(fread, buffer, 1, len, reinterpret_cast<cs_idfile_override_t*>(thiz)->m_cfile);
	}
	static size_t do_write(idFile* thiz, const void* buffer, unsigned int len) {
		return STDIOFN_LOCK(fwrite, buffer, 1, len, reinterpret_cast<cs_idfile_override_t*>(thiz)->m_cfile);
	}
	static size_t do_read_offs(idFile* thiz, long long offset, void* buffer, unsigned int len) {
#if 1
		auto t = reinterpret_cast<cs_idfile_override_t*>(thiz);

		size_t old_pos = _ftelli64(t->m_cfile);
		_fseeki64(t->m_cfile, offset, SEEK_SET);


		size_t elements = fread(buffer, 1, len, reinterpret_cast<cs_idfile_override_t*>(thiz)->m_cfile);
		_fseeki64(t->m_cfile, old_pos, SEEK_SET);
		return elements;
#else
		return doomcall<size_t>(doomoffs::_ZN13idFile_Memory7ReadOfsExPvm, thiz, offset, buffer, len);
#endif
	}
	static size_t do_write_offs(idFile* thiz, long long offset, const void* buffer, unsigned int len) {
#if 1
		auto t = reinterpret_cast<cs_idfile_override_t*>(thiz);

		size_t old_pos = _ftelli64(t->m_cfile);
		_fseeki64(t->m_cfile, offset, SEEK_SET);


		size_t elements = fwrite(buffer, 1, len, reinterpret_cast<cs_idfile_override_t*>(thiz)->m_cfile);
		_fseeki64(t->m_cfile, old_pos, SEEK_SET);
		return elements;

#else
		return doomcall<size_t>(doomoffs::_ZN13idFile_Memory8WriteOfsExPKvm, thiz, offset, buffer, len);
#endif
	}

	static size_t do_length(const idFile* thiz) {
		auto t = reinterpret_cast<const cs_idfile_override_t*>(thiz);

		auto oldpos = EXTIOFN_LOCK(_ftelli64, t->m_cfile);
		EXTIOFN_LOCK(_fseeki64, t->m_cfile, 0, SEEK_END);
		auto sz = EXTIOFN_LOCK(_ftelli64, t->m_cfile);
		EXTIOFN_LOCK(_fseeki64, t->m_cfile, oldpos, SEEK_SET);
		return sz;
	}

	static void do_set_length(idFile* thiz, unsigned newl) {

	}

	static size_t do_tell(const idFile* thiz) {
		auto t = reinterpret_cast<const cs_idfile_override_t*>(thiz);
		return EXTIOFN_LOCK(_ftelli64, t->m_cfile);
	}
	static int do_seekex(idFile* thiz, long long offs, fsOrigin_t origin) {
		unsigned outseekd = 0;
		auto t = reinterpret_cast<cs_idfile_override_t*>(thiz);

		switch (origin) {
		case FS_SEEK_CUR:
			outseekd = SEEK_CUR;
			break;
		case FS_SEEK_END:
			outseekd = SEEK_END;
			break;
		case FS_SEEK_SET:
			outseekd = SEEK_SET;
			break;
		}
		return EXTIOFN_LOCK(_fseeki64, t->m_cfile, offs, outseekd);
	}
	static size_t do_Printf(idFile* thiz, const char* fmt, ...) {
		va_list ap;
		va_start(ap, fmt);
		auto t = reinterpret_cast<cs_idfile_override_t*>(thiz);

		size_t res = vfprintf(t->m_cfile, fmt, ap);

		va_end(ap);
		return res;
	}
	static size_t do_VPrintf(idFile* thiz, const char* fmt, va_list ap) {
		auto t = reinterpret_cast<cs_idfile_override_t*>(thiz);
		return vfprintf(t->m_cfile, fmt, ap);
	}
	static size_t do_WriteFloatString(idFile* thiz, const char*, ...) {
		return 0;
	}
	static size_t do_WriteFloatStringVA(idFile* thiz, const char* fmt, va_list va) {
		return 0;
	}
	static size_t do_Timestamp(const idFile* thiz) {
		return 0;
	}
	static bool is_writeable(const idFile* thiz) {
		return reinterpret_cast<const cs_idfile_override_t*>(thiz)->m_writable;
	}

	static void do_flush(idFile* thiz) {
		STDIOFN_LOCK(fflush, reinterpret_cast<cs_idfile_override_t*>(thiz)->m_cfile);
	}
	static size_t do_GetSectorSize(const idFile*) {
		return 1;
	}
	static int do_GetDevice(const idFile*) {
		return 4;
	}

	static int do_getFileErrorCode(const idFile* thiz) {
		return ferror(reinterpret_cast<const cs_idfile_override_t*>(thiz)->m_cfile);
	}

	static void do_SetFileError(idFile* thiz) {

	}

	static size_t do_ReadString(idFile* thiz, idStr* outstr) {
#if 1
		char scratchbuffer[4096];
		unsigned buffpos = 0;
		auto t = reinterpret_cast<cs_idfile_override_t*>(thiz);

		auto flush_buffer = [&scratchbuffer, &buffpos, outstr]() {

			if (buffpos) {
				scratchbuffer[buffpos] = 0;

				std::string tmpstr{};
				tmpstr = outstr->data;
				tmpstr += scratchbuffer;


				//*outstr += scratchbuffer;
				*outstr = tmpstr.c_str();
				buffpos = 0;

			}
		};


		auto add_char_to_buffer = [&scratchbuffer, &buffpos, &flush_buffer](unsigned c) {
			if (!c) {
				flush_buffer();
				return false;
			}
			else {
				scratchbuffer[buffpos++] = c;
				if (buffpos == 4095) {
					flush_buffer();
				}
				return true;
			}

		};
		size_t nread = 0;
		while (true) {
			if (feof(t->m_cfile)) {
				flush_buffer();
				break;
			}
			nread++;
			unsigned c = STDIOFN_LOCK(fgetc, t->m_cfile);

			if (!add_char_to_buffer(c))
				break;

		}

		return nread;
#else
		return doomcall<size_t>(doomoffs::_ZN6idFile10ReadStringER5idStr, thiz, outstr);
#endif
	}

	static  size_t do_ReadDebugTag(idFile* thiz, const char* a1, const char* a2, int a3) {
		//return doomcall<size_t>(doomoffs::_ZN6idFile12ReadDebugTagEPKcS1_i, thiz, a1, a2, a3);
		return 0;
	}
	static  uint64_t do_WriteDebugTag(idFile* thiz, const char* a1, const char* a2) {
		//return doomcall<size_t>(doomoffs::_ZN6idFile13WriteDebugTagEPKcS1_, thiz, a1, a2);
		return 0;
	}

	static bool do_lock(idFile* thiz, unsigned vv, fsLock_t fl) {
#ifdef USE_FILE_LOCKING

		_lock_file(reinterpret_cast<cs_idfile_override_t*>(thiz)->m_cfile);
#endif
		return true;
	}

	static bool do_unlock(idFile* thiz, unsigned vv) {
#ifdef USE_FILE_LOCKING

		_unlock_file(reinterpret_cast<cs_idfile_override_t*>(thiz)->m_cfile);
#endif
		return true;
	}
	static inline idFileVftbl g_override_file_vftbl{
	.dctor = deconstructor,
	.IsIDFile_Memory = return_false,
	.IsIDFile_Permanent = return_false,
	.GetFullPath = get_full_path,
	.GetName = get_name,
	.Read = do_read,
	.Write = do_write,
	.ReadOfs = do_read_offs,
	.WriteOfs = do_write_offs,
	.Lock = do_lock,
	.Unlock = do_unlock,
	.Length = do_length,
	.SetLength = do_set_length,
	.Tell = do_tell,
	.SeekEx = do_seekex,
	.Printf = do_Printf,
	.VPrintf = do_VPrintf,
	.WriteFloatString = do_WriteFloatString,
	.WriteFloatStringVA = do_WriteFloatStringVA,
	.Timestamp = do_Timestamp,
	.IsWritable = is_writeable,
	.Flush = do_flush,
	.ForceFlush = do_flush,
	.GetSectorSize = do_GetSectorSize,
	.GetDevice = do_GetDevice,
	.IsOSNative = return_false,
	.GetFileErrorCode = do_getFileErrorCode,
	.SetFileError = do_SetFileError,
	.ReadString = do_ReadString,
	.ReadDebugTag = do_ReadDebugTag,
	.WriteDebugTag = do_WriteDebugTag


	};
	static idFile* create(FILE* fp, bool readable, bool writeable, const char* fpath, const char* name) {
		cs_idfile_override_t* overr = (cs_idfile_override_t*)idMemorySystem_malloc(sizeof(cs_idfile_override_t), 52, 0);
		overr->vftbl = &g_override_file_vftbl;
		overr->m_cfile = fp;
		overr->m_readable = readable;
		overr->m_writable = writeable;
		overr->m_fullpath = fpath;
		overr->m_fname = name;
		return overr;
	}
};




static FILE* g_readfile_log = nullptr;
extern const char* gOverrideFileName;
std::string gLastLoadedEntities;
extern std::string gOverrideName;

static void dispose_log() {
	fclose(g_readfile_log);
}

static void readfile_log_path(const char* path) {
#if 1
	if (!g_readfile_log) {
		fopen_s(&g_readfile_log, "resources_seen.txt", "w");
		atexit(dispose_log);
	}
	fprintf(g_readfile_log, "%s\n", path);
	fflush(g_readfile_log);


#endif
}
FILE* get_override_for_resource(const char* name, size_t* size_out) {
	FILE* resfile = nullptr;

	char pathbuf[4096];
	memset(pathbuf, 0, sizeof(pathbuf));



	if ((gOverrideFileName != nullptr) && (strstr(name, ".entities") != 0) && (gOverrideName.size() && !strcmp(gOverrideName.c_str(), name))) {
		strcpy_s(pathbuf, gOverrideFileName);
		gOverrideFileName = nullptr;

	}
	else {
		if (strstr(name, "base\\") == name) {
			name += sizeof("base\\") - 1;
		}


		sprintf_s(pathbuf, ".\\overrides\\%s", name);
	}

	if (strstr(name, ".entities") != 0) {
		gLastLoadedEntities = name;
	}
	readfile_log_path(name);
#if 0
	if (!g_readfile_log) {
		fopen_s(&g_readfile_log, "C:\\Users\\Chris\\readfile_eternal.txt", "w");
		atexit(dispose_log);
	}
	fprintf(g_readfile_log, "%s\n", name);
#endif
	for (unsigned i = 0; pathbuf[i]; ++i) {
		if (pathbuf[i] == '/')
			pathbuf[i] = '\\';

		if(pathbuf[i] == '$') {
			pathbuf[i] = 0;
			break;
		}
	}



	fopen_s(&resfile, pathbuf, "rb");
	if (!resfile)
		return nullptr;

	fseek(resfile, 0, SEEK_END);

	*size_out = ftell(resfile);

	fseek(resfile, 0, SEEK_SET);
	return resfile;
}
static constexpr unsigned INDEX_OF_READFILE = 29;

static constexpr unsigned INDEX_OF_OPENFILEREAD = 37;
static size_t readfile_repl(void* fs, const char* path, void** out_data, void* fileprops, unsigned psrch);
static void* g_old_readfile = (void*)readfile_repl;


static size_t readfile_repl(void* fs, const char* path, void** out_data, void* fileprops, unsigned psrch) {


	size_t res = call_as<size_t>(g_old_readfile, fs, path, out_data, fileprops, psrch);

	size_t sz = 0;
	FILE* overrid = get_override_for_resource(path, &sz);

	if (!overrid)
		return res;

	//	idMemorySystem_free(*out_data);


	void* newdata = idMemorySystem_malloc(sz, 52, 0);

	memset(newdata, 0, sz);
	fread(newdata, 1, sz, overrid);
	fclose(overrid);
	*out_data = newdata;
	if ((int)res < 0) {//id failed to open and read the file, but we succeeded
		return sz;
	}
	return res;

}


static idFile* openfileread_replacement(void* a1, const char* a2, bool a3, int a4, int a5) {
	/*	if(a5 !=FSPATH_SEARCH_BASE && a5 != FSPATH_SEARCH_DEFAULT ) {
			return g_backup_filesystem_vftbl._ZN17idFileSystemLocal12OpenFileReadEPKcbb14fsPathSearch_t(a1, a2, a3, a4, a5);
		}
		else */
	{
		size_t sz = 0;
		FILE* overr = get_override_for_resource(a2, &sz);
		if (overr) {
			return cs_idfile_override_t::create(overr, true, false, a2, a2);
		}
		else {
			return call_as<idFile*>(g_old_readfile, a1, a2, a3, a4, a5);
		}
	}
}
static void* g_old_openfileread2 = nullptr;

static idFile* openfileread2_replacement(void* a1, const char* a2) {
	/*	if(a5 !=FSPATH_SEARCH_BASE && a5 != FSPATH_SEARCH_DEFAULT ) {
			return g_backup_filesystem_vftbl._ZN17idFileSystemLocal12OpenFileReadEPKcbb14fsPathSearch_t(a1, a2, a3, a4, a5);
		}
		else */
	{
		size_t sz = 0;
		readfile_log_path(a2);
		FILE* overr = get_override_for_resource(a2, &sz);
		if (overr) {
			return cs_idfile_override_t::create(overr, true, false, a2, a2);
		}
		else {
			return call_as<idFile*>(g_old_openfileread2, a1, a2);
		}
	}
}

#define OFFSET_RESOURCEMANAGER_GETRESFILE		0x88

static void** get_resourcemanager_vftbl() {
	void* resourceManager2 = *(void**)descan::g_resourceManager2;
	return *(void***)resourceManager2;
}
struct idResourceStorageDiskStreamer {};
struct idResourceStorageInterface {};
struct idResourceHeader
{
  char bytes[119];
  char end;
};
struct	idStrDynStatic_256_ {
	idStr base;
	char buffer[256];
};struct idResourceSpecialHash
{
  char hashType[4];
  char reserved[4];
  char hash[8];
};
struct idResourceMetaData;
struct idResourceEntryOptions
{
  unsigned __int64 uncompressedSize;
  unsigned __int64 dataCheckSum;
  unsigned __int64 generationTimeStamp;
  unsigned __int64 defaultHash;
  unsigned int version;
  unsigned int flags;
  char compMode;
  char reserved0;
  unsigned __int16 variation;
  unsigned int reserved2;
  unsigned __int64 reservedForVariations;
};

struct idResourceEntry
{
  const char *resourceTypeString;
  const char *nameString;
  const char *descString;
  unsigned int *depIndices;
  char **strings;
  idResourceSpecialHash *specialHashes;
  idResourceMetaData **metaEntries;
  unsigned __int64 dataOffset;
  unsigned __int64 dataSize;
  idResourceEntryOptions options;
  unsigned __int16 numStrings;
  unsigned __int16 numSources;
  unsigned __int16 numDependencies;
  unsigned __int16 numSpecialHashes;
  unsigned __int16 numMetaEntries;
  char field_8A;
  char field_8B;
  int field_8C;
};

struct __declspec(align(8)) resourceStorage_t
{
	idStrDynStatic_256_ containerFilename;
	idFile* containerFile;
	__int64 timestamp;
	__int64 field_140;
	idResourceHeader field_148;
	idResourceEntry* resourceEntries;
	/*idHashIndexWithExtra resourceEntryMap;
	idResourceDependency* containerDependencies;
	__int64 field_200;
	__int64 field_208;
	_QWORD** pqword210;
	__int64 magic;
	char field_220;*/
};

static void* g_original_ds_getfile = nullptr;
static volatile bool test1 = true;

static const char* get_name_of_res_at_index(resourceStorage_t* container, unsigned int index) {
	return container->resourceEntries[index].nameString;


}
static unsigned __int64 checksum_override_data(unsigned __int8 *a1, int a2, int a3 = 0xDEADBEEF)
{
    unsigned int v4; // er11
    unsigned int v5; // er8
    unsigned __int64 v6; // r10
    unsigned int v7; // ecx
    int v8; // eax
    int v9; // eax
    int v10; // edx
    int v11; // edx

    v4 = a3 ^ a2;
    v5 = 0;
    if ( a2 >= 8 )
    {
        v6 = (unsigned __int64)(unsigned int)a2 >> 3;
        a2 -= 8 * ((unsigned int)a2 >> 3);
        do
        {
            v7 = 1540483477 * ((1540483477 * *(unsigned int  *)a1) ^ ((unsigned int)(1540483477 * *(unsigned int  *)a1) >> 24));
            v8 = *((unsigned int *)a1 + 1);
            a1 += 8;
            v4 = v7 ^ (1540483477 * v4);
            v5 = (1540483477 * ((1540483477 * v8) ^ ((unsigned int)(1540483477 * v8) >> 24))) ^ (1540483477 * v5);
            --v6;
        }
        while ( v6 );
    }
    if ( a2 >= 4 )
    {
        v9 = *(unsigned int  *)a1;
        a1 += 4;
        v4 = (1540483477 * ((1540483477 * v9) ^ ((unsigned int)(1540483477 * v9) >> 24))) ^ (1540483477 * v4);
        a2 -= 4;
    }
    v10 = a2 - 1;
    if ( !v10 )
        goto LABEL_11;
    v11 = v10 - 1;
    if ( !v11 )
    {
LABEL_10:
        v5 ^= a1[1] << 8;
LABEL_11:
        v5 = 1540483477 * (v5 ^ *a1);
        return ((unsigned __int64)(1540483477
                                 * ((1540483477 * (v4 ^ (v5 >> 18))) ^ ((1540483477
                                                                       * (v5 ^ ((1540483477 * (v4 ^ (v5 >> 18))) >> 22))) >> 17))) << 32) | (1540483477 * ((1540483477 * (v5 ^ ((1540483477 * (v4 ^ (v5 >> 18))) >> 22))) ^ ((1540483477 * ((1540483477 * (v4 ^ (v5 >> 18))) ^ ((1540483477 * (v5 ^ ((1540483477 * (v4 ^ (v5 >> 18))) >> 22))) >> 17))) >> 19)));
    }
    if ( v11 != 1 )
        return ((unsigned __int64)(1540483477
                                 * ((1540483477 * (v4 ^ (v5 >> 18))) ^ ((1540483477
                                                                       * (v5 ^ ((1540483477 * (v4 ^ (v5 >> 18))) >> 22))) >> 17))) << 32) | (1540483477 * ((1540483477 * (v5 ^ ((1540483477 * (v4 ^ (v5 >> 18))) >> 22))) ^ ((1540483477 * ((1540483477 * (v4 ^ (v5 >> 18))) ^ ((1540483477 * (v5 ^ ((1540483477 * (v4 ^ (v5 >> 18))) >> 22))) >> 17))) >> 19)));
    v5 ^= a1[2] << 16;
    goto LABEL_10;
}
static idFile* idResourceStorageDiskStreamer_GetFile_replacement(idResourceStorageDiskStreamer* a1, resourceStorage_t* container, unsigned int resindex) {


	FILE* override = nullptr;
	size_t ressize = 0;
	const char* fgname=get_name_of_res_at_index(container, resindex);
	if(strstr(fgname, ".entities") || strstr(fgname, ".decl")) {
			
	}
	else {
		override = get_override_for_resource(fgname, &ressize);
	}
	if(!override)
		return call_as<idFile*>(g_original_ds_getfile, a1, container, resindex);
	else{
	//	static idFile* create(FILE* fp, bool readable, bool writeable, const char* fpath, const char* name) {

		container->resourceEntries[resindex].options.compMode = 0;
		container->resourceEntries[resindex].options.uncompressedSize = ressize;
		container->resourceEntries[resindex].dataSize = ressize;
		//todo: instead, eliminate the check
		unsigned char* tempmem = new unsigned char[ressize];
		fread(tempmem, 1, ressize, override);
		container->resourceEntries[resindex].options.dataCheckSum = checksum_override_data(tempmem, ressize);


		delete[] tempmem;
		return cs_idfile_override_t::create(override, true, false, fgname,fgname);
	}
}

void hook_idfilesystem() {

#if 1
	Xbyak::CodeGenerator redirector{};
	Xbyak::CodeGenerator reporiginal_thunk{};
	redirector.mov(redirector.rax, (uintptr_t)idResourceStorageDiskStreamer_GetFile_replacement);
	redirector.jmp(redirector.rax);


	mh_disassembler_t disas{};
	void* original_getfile_continuation = nullptr;
	size_t savesize = 0;
	disas.decode_enough_to_make_space(descan::g_resourceStorageDiskStreamer_GetFile, redirector.getSize(), &original_getfile_continuation, &savesize);


	Xbyak::CodeGenerator original_thunk{};

	original_thunk.mov(original_thunk.r9, (uintptr_t)original_getfile_continuation);
	original_thunk.jmp(original_thunk.r9);



	g_original_ds_getfile = alloc_execmem(original_thunk.getSize() + savesize + 32);
	size_t setupsize = savesize;

	memcpy(g_original_ds_getfile, descan::g_resourceStorageDiskStreamer_GetFile, setupsize);

	disas.setup_for_addr(g_original_ds_getfile, 128);

	Xbyak::CodeGenerator* to_use = &original_thunk;

	if (disas.find_next_call(setupsize)) {
		memset((void*)(disas.m_ctx.pc - 5), 0x90, 5);



		reporiginal_thunk.mov(reporiginal_thunk.r9, (uintptr_t)descan::g_alloca_probe);
		reporiginal_thunk.call(reporiginal_thunk.r9);

		reporiginal_thunk.mov(reporiginal_thunk.r9, (uintptr_t)original_getfile_continuation);
		reporiginal_thunk.jmp(reporiginal_thunk.r9);

		to_use = &reporiginal_thunk;



	}

	memcpy(mh_lea<char>(g_original_ds_getfile, savesize), to_use->getCode(), to_use->getSize());


	//g_original_ds_getfile now has the bytes we will clobber on the original + the instructions to jump back to the function, past the part we overwrote

	patch_memory(descan::g_resourceStorageDiskStreamer_GetFile, redirector.getSize(), (char*)redirector.getCode());

#endif



	void** fs_vftbl = **reinterpret_cast<void****>(descan::g_idfilesystemlocal);

	swap_out_ptrs(&fs_vftbl[INDEX_OF_READFILE], &g_old_readfile, 1);



	void** addrof_resman_repl = &get_resourcemanager_vftbl()[OFFSET_RESOURCEMANAGER_GETRESFILE / 8];



	g_old_openfileread2 = openfileread2_replacement;//(void*)openfileread2_replacement;

	swap_out_ptrs(addrof_resman_repl, &g_old_openfileread2, 1);

}