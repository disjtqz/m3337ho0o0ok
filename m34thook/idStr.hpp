#pragma once
#include "game_exe_interface.hpp"
#include "scanner_core.hpp"
struct idStr {
	void* vftbl;
	char* data;
	int len;
	//top bit is used to store a flag that indicates if the string data is static or not
	int allocedAndFlag;
	char baseBuffer[20];


	idStr() {
		call_as<void>(descan::g_idstr__idstr, this);
		//doomcall<void>(doomoffs::_ZN5idStrC2Ev, this);
	}


	~idStr() {
		call_as<void>(descan::g_idstr_dctor, this);
	}

	idStr& operator = (const char* s) {
		call_as<void>(descan::g_idstr_assign_charptr, this, s);
		return *this;
	}
	static int  idStr::Find(const char* str, const char c, int start, int end)
	{
		__int64 v4; // r10
		int v5; // er8
		const char* v6; // rax
		__int64 v7; // rax

		v4 = start;
		if (end == -1)
		{
			v5 = 0;
			if (*str)
			{
				v6 = str;
				do
				{
					++v6;
					++v5;
				}       while (*v6);
			}
			end = v5 - 1;
		}
		v7 = v4;
		if (v4 > end)
			return 0xFFFFFFFFi64;
		while (str[v7] != c)
		{
			++v7;
			v4 = v4 + 1;
			if (v7 > end)
				return 0xFFFFFFFFi64;
		}
		return (unsigned int)v4;
	}
};
static const char* blankstr = "";
struct idAtomicString
{
	char* str;
	void set(const char* s) {
		call_as<void>(descan::g_atomic_string_set, this, s);
	}
};
template<size_t N, bool canbdynamic = false>
struct idStrStaticBase
{
	idStr base;
	char buffer[N];
};

template<size_t N>
struct idStrStatic
{
	idStrStaticBase<N, false> base;
};
template<size_t N>
struct idStrDynStatic
{
	idStrStaticBase<N, true> base;
};
