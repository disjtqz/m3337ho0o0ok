
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
#include "idLib.hpp"
#include "idStr.hpp"
#include "id_resources_lowlevel.hpp"
#include "cmdsystem.hpp"
#include "snaphakalgo.hpp"
#include "mhutil/mh_filesystem.hpp"
/*
	the directory containing the doom exe
	we need this for gamepass versions
*/
static std::string g_basepath = "";
static std::string g_overrides_dir = "";
//idFileSystemLocal::ReadFile(char const*,void **,idFileProps *,fsPathSearch_t)	.text	0000007101D87CB0	00000178	00000040	FFFFFFFFFFFFFFF8	R	.	.	.	B	T	.





//#define	USE_FILE_LOCKING

#ifdef USE_FILE_LOCKING
#define		STDIOFN_LOCK(name, ...)		_##name##_nolock (__VA_ARGS__)
#define		EXTIOFN_LOCK(name, ...)		name##_nolock (__VA_ARGS__)
#else
#define		STDIOFN_LOCK(name, ...)		name (__VA_ARGS__)
#define		EXTIOFN_LOCK(name, ...)		name (__VA_ARGS__)
#endif

class cs_idfile_override_virt_t : public cs_idFile_t {
	FILE* m_cfile;
	const char* m_fname;
	const char* m_fullpath;
	bool m_readable;
	bool m_writable;
public:
	virtual ~cs_idfile_override_virt_t() {
		if (m_cfile) {

			STDIOFN_LOCK(fclose, m_cfile);
			m_cfile = nullptr;
			m_fullpath = nullptr;
			m_fname = nullptr;
			m_readable = false;
			m_writable = false;
		}
	}



	virtual const char* GetFullPath() {
		return this->m_fullpath;
	}

	virtual const char* GetName() {
		return this->m_fname;
	}

	virtual size_t Read( void* buffer, unsigned int len) {

		return STDIOFN_LOCK(fread, buffer, 1, len, this->m_cfile);
	}
	virtual size_t Write(const void* buffer, unsigned int len) {
		return STDIOFN_LOCK(fwrite, buffer, 1, len, this->m_cfile);
	}
	virtual size_t ReadOfs(long long offset, void* buffer, unsigned int len) {
		auto t = this;

		size_t old_pos = _ftelli64(t->m_cfile);
		_fseeki64(t->m_cfile, offset, SEEK_SET);


		size_t elements = fread(buffer, 1, len, this->m_cfile);
		_fseeki64(t->m_cfile, old_pos, SEEK_SET);
		return elements;

	}
	virtual size_t WriteOfs( long long offset, const void* buffer, unsigned int len) {
		auto t = this;

		size_t old_pos = _ftelli64(t->m_cfile);
		_fseeki64(t->m_cfile, offset, SEEK_SET);


		size_t elements = fwrite(buffer, 1, len, this->m_cfile);
		_fseeki64(t->m_cfile, old_pos, SEEK_SET);
		return elements;

	}

	virtual size_t Length() {
		auto t = this;

		auto oldpos = EXTIOFN_LOCK(_ftelli64, t->m_cfile);
		EXTIOFN_LOCK(_fseeki64, t->m_cfile, 0, SEEK_END);
		auto sz = EXTIOFN_LOCK(_ftelli64, t->m_cfile);
		EXTIOFN_LOCK(_fseeki64, t->m_cfile, oldpos, SEEK_SET);
		return sz;
	}

	virtual void SetLength(unsigned newl) {

	}

	virtual size_t Tell() {
		return EXTIOFN_LOCK(_ftelli64, m_cfile);
	}
	virtual int SeekEx(long long offs, fsOrigin_t origin) {
		unsigned outseekd = 0;
		auto t = this;

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
	virtual size_t Printf(const char* fmt, ...) {
		va_list ap;
		va_start(ap, fmt);
		auto t = this;

		size_t res = vfprintf(t->m_cfile, fmt, ap);

		va_end(ap);
		return res;
	}
	virtual size_t VPrintf(const char* fmt, va_list ap) {
		auto t = this;
		return vfprintf(t->m_cfile, fmt, ap);
	}
	virtual size_t WriteFloatString( const char*, ...) {
		return 0;
	}
	virtual size_t WriteFloatStringVA( const char* fmt, va_list va) {
		return 0;
	}
	virtual size_t Timestamp() {
		return 0;
	}
	virtual bool IsWritable() {
		return m_writable;
	}

	virtual void Flush() {
		STDIOFN_LOCK(fflush, this->m_cfile);
	}
	virtual void  ForceFlush() {
		Flush();
	}
	virtual size_t GetSectorSize() {
		return 1;
	}
	virtual int GetDevice() {
		return 4;
	}

	virtual int GetFileErrorCode() {
		return ferror(m_cfile);
	}

	virtual void SetFileError() {

	}

	virtual size_t ReadString(idStr* outstr) {
#if 1
		char scratchbuffer[4096];
		unsigned buffpos = 0;
		auto t = this;

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

	virtual size_t ReadDebugTag( const char* a1, const char* a2, int a3) {
		return 0;
	}
	virtual uint64_t WriteDebugTag(const char* a1, const char* a2) {
		return 0;
	}
	virtual void* chunky_op(void*, void*, void*) {
		return nullptr;
	}
	virtual bool Lock( unsigned vv, fsLock_t fl) {
#ifdef USE_FILE_LOCKING

		_lock_file(this->m_cfile);
#endif
		return true;
	}

	virtual bool Unlock( unsigned vv) {
#ifdef USE_FILE_LOCKING

		_unlock_file(this->m_cfile);
#endif
		return true;
	}
	/*
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
	*/

	virtual bool IsIDFile_Memory() {
		return false;
	}
	virtual bool IsIDFile_Permanent() {
		return false;
	}

	virtual bool IsOSNative() {
		return false;
	}

	virtual bool isIDFile_Verified() {
		return false;
	}

	cs_idfile_override_virt_t(FILE* fp, bool readable, bool writeable, const char* fpath, const char* name) {


		this->m_cfile = fp;
		this->m_readable = readable;
		this->m_writable = writeable;
		this->m_fullpath = fpath;
		this->m_fname = name;
	}


	inline void* operator new (size_t sz) {

		return idMemorySystem_malloc(sz, 52, 0);

	}

	inline void operator delete(void* thz) {
		//todo: figure out the correct allocator/deallocator for this! overriding delete causes a crash
		//idMemorySystem_free(thz);
	}

	static idFile* create(FILE* fp, bool readable, bool writeable, const char* fpath, const char* name) {
		cs_idfile_override_virt_t* overr = new cs_idfile_override_virt_t(fp, readable, writeable, fpath, name);
		return (idFile*)overr;
	}
};


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
#if 0
		cs_idfile_override_t* overr = (cs_idfile_override_t*)idMemorySystem_malloc(sizeof(cs_idfile_override_t), 52, 0);
		overr->vftbl = &g_override_file_vftbl;
		overr->m_cfile = fp;
		overr->m_readable = readable;
		overr->m_writable = writeable;
		overr->m_fullpath = fpath;
		overr->m_fname = name;
		return overr;
#else
		return cs_idfile_override_virt_t::create(fp, readable, writeable, fpath, name);
#endif
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
#if 0
	if (!g_readfile_log) {
		fopen_s(&g_readfile_log, "resources_seen.txt", "w");
		atexit(dispose_log);
	}
	fprintf(g_readfile_log, "%s\n", path);
	fflush(g_readfile_log);


#endif
}

class pathlogger_t {
	FILE* m_outfile;
public:
	pathlogger_t(const char* name) {
		fopen_s(&m_outfile, name, "w");

	}

	~pathlogger_t() {
		fclose(m_outfile);
	}

	inline pathlogger_t& operator <<(const char* s) {
		fprintf_s(m_outfile, "%s\n", s);
		return *this;
	}
};
static pathlogger_t g_print_resulting_path{ "override_paths.log" };



unsigned get_override_path(const char* name, char(&pathbuf)[OVERRIDE_PATHBUF_SIZE]) {


	unsigned pathbuf_fixup_search_start = 0;

	if ((gOverrideFileName != nullptr) && (sh::string::strstr(name, ".entities") != 0) && (gOverrideName.size() && sh::string::streq(gOverrideName.c_str(), name))) {
		strcpy_s(pathbuf, gOverrideFileName);
		gOverrideFileName = nullptr;

	}
	else {
		sh::memops::smol_memcpy(pathbuf, g_overrides_dir.c_str(), g_overrides_dir.length());
		pathbuf_fixup_search_start = g_overrides_dir.length();
		sh::string::strcpy(&pathbuf[g_overrides_dir.length()], name);
		//sprintf_s(pathbuf, "%s\\overrides\\%s",g_basepath.c_str(), name);
	}

	if (sh::string::strstr(name, ".entities") != 0) {
		gLastLoadedEntities = name;
	}
	unsigned i;
	for (i = pathbuf_fixup_search_start; pathbuf[i]; ++i) {
		if (pathbuf[i] == '/')
			pathbuf[i] = '\\';

		if (pathbuf[i] == '$') {
			pathbuf[i] = 0;
			break;
		}
	}
	return i;

}
FILE* get_override_for_resource(const char* name, size_t* size_out) {
	FILE* resfile = nullptr;

	char pathbuf[MH_FILESYS_PATHBUFFER_LENGTH];

	//memsetting the buf is not necessary 
	//memset(pathbuf, 0, sizeof(pathbuf));

	get_override_path(name, pathbuf);

	readfile_log_path(name);



	//fast pre-test
	if (!filesys::file_exists(pathbuf)) {
		return nullptr;
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


static void* g_original_ds_getfile = nullptr;

static const char* get_name_of_res_at_index(resourceStorage_t* container, unsigned int index) {
	return container->resourceEntries[index].nameString;


}
static idCVar* g_checkdatachecksum = nullptr;

static idFile* idResourceStorageDiskStreamer_GetFile_replacement(idResourceStorageDiskStreamer* a1, resourceStorage_t* container, unsigned int resindex) {


	FILE* override = nullptr;
	size_t ressize = 0;
	const char* fgname = get_name_of_res_at_index(container, resindex);

	if (sh::string::strstr(fgname, ".entities") || sh::string::strstr(fgname, ".decl")) {

	}
	else {
		override = get_override_for_resource(fgname, &ressize);
	}
	if (!override)
		return call_as<idFile*>(g_original_ds_getfile, a1, container, resindex);
	else {
		//	static idFile* create(FILE* fp, bool readable, bool writeable, const char* fpath, const char* name) {

		container->resourceEntries[resindex].options.compMode = 0;
		//this happens too early to use cvar_data
		if (!g_checkdatachecksum) {
			g_checkdatachecksum = idCVar::Find("resourceStorage_checkDataCheckSum");

		}

		container->resourceEntries[resindex].options.uncompressedSize = ressize;
		container->resourceEntries[resindex].dataSize = ressize;

		//todo: instead, eliminate the check
#if 0
		unsigned char* tempmem = new unsigned char[ressize];
		fread(tempmem, 1, ressize, override);
		container->resourceEntries[resindex].options.dataCheckSum = checksum_override_data(tempmem, ressize);
		delete[] tempmem;
#else
		g_checkdatachecksum->data->valueInteger = 0;
#endif


		return cs_idfile_override_t::create(override, true, false, fgname, fgname);
	}
}


static bool g_installed_fs_hooks = false;

void hook_idfilesystem() {
	if (g_installed_fs_hooks)
		return;

	g_installed_fs_hooks = true;
	//wayyyy overallocate. i think the max path for win is below this but also WHO KNOWS WHAT THE FUTURE HOLDS


	char* tmpbuffer_filename = new char[65536];
	//hmodule is really the base of the image in memory ;)
	unsigned bufflen = GetModuleFileNameA((HMODULE)(g_blamdll.image_base), tmpbuffer_filename, 65536);
	//dont check for error, if this failed we have bigger problems and should have crashed before now
	unsigned backpos;
	//zero out the exe filename
	for (backpos = bufflen - 1; tmpbuffer_filename[backpos] != '\\'; --backpos) {
		tmpbuffer_filename[backpos] = 0;
	}

	tmpbuffer_filename[backpos] = 0;
	g_basepath = tmpbuffer_filename;
	delete[] tmpbuffer_filename;
	g_overrides_dir = g_basepath + "\\overrides\\";

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

#else
	g_original_ds_getfile = detour_with_thunk_for_original(descan::g_resourceStorageDiskStreamer_GetFile, (void*)idResourceStorageDiskStreamer_GetFile_replacement, true);
#endif



	void** fs_vftbl = **reinterpret_cast<void****>(descan::g_idfilesystemlocal);

	swap_out_ptrs(&fs_vftbl[INDEX_OF_READFILE], &g_old_readfile, 1);



	void** addrof_resman_repl = &get_resourcemanager_vftbl()[OFFSET_RESOURCEMANAGER_GETRESFILE / 8];



	g_old_openfileread2 = openfileread2_replacement;//(void*)openfileread2_replacement;

	swap_out_ptrs(addrof_resman_repl, &g_old_openfileread2, 1);

}