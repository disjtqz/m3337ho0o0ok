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
