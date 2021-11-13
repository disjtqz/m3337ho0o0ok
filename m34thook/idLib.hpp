#pragma once
#include <cstdarg>

namespace idLib {
	void VPrintf(int severity, const char* fmt, va_list ap);
	static void Printf(const char* format, ...) {
		va_list ap;
		va_start(ap, format);
		VPrintf(1, format, ap);
		va_end(ap);
	}
	//for compat with decomp code
	template<typename... Ts>
	static void Warning(const char* format, Ts... vs) {

		Printf(format, vs...);
	}
}
enum memTag_t;
struct __declspec(align(8)) idHashIndex
{
	int* hash;
	int* indexChain;
	int hashSize;
	int indexSize;
	int granularity;
	int hashMask;
	int lookupMask;
	memTag_t memTag;
};
struct idHashIndexWithExtra
{
  idHashIndex base;
  int field_28;
  int initial_gran;
};
struct idListVoid
{
	unsigned __int64* list;
	int num;
	int size;
	__int16 granularity;
	unsigned __int8 memTag;
	unsigned __int8 listStatic;
};


class idCallback {
public:
	virtual ~idCallback() {}
	virtual void Call() = 0;
	virtual idCallback* Clone() = 0;
};