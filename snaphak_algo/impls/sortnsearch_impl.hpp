#pragma once
/*
	generated these from gcc using std::sort and --param early-inlining-insns=2000
*/
IMPL_CODE_SEG
static void sort16ptr(void* ptrs[8])
{
	void** v2; // rbx
	void** v3; // rbp
	void* v4; // rsi
	unsigned __int64 v5; // rdx
	void** v6; // rax
	void** v7; // rcx

	v2 = ptrs + 1;
	v3 = ptrs + 16;
	do
	{
		while (1)
		{
			v4 = *v2;
			if (*v2 < *ptrs)
				break;
			v5 = (unsigned __int64)*(v2 - 1);
			v6 = v2 - 1;
			if ((unsigned __int64)v4 >= v5)
			{
				*v2 = v4;
			}
			else
			{
				do
				{
					v6[1] = (void*)v5;
					v7 = v6;
					v5 = (unsigned __int64)*--v6;
				} while ((unsigned __int64)v4 < v5);
				*v7 = v4;
			}
			if (v3 == ++v2)
				return;
		}
		if (ptrs != v2)
			memmove(ptrs + 1, ptrs, (char*)v2 - (char*)ptrs);
		++v2;
		*ptrs = v4;
	} while (v3 != v2);
}
IMPL_CODE_SEG
static void sort16ptr_unrolled(void** ptrs)
{
	void** result; // rax
	void** v3; // r9
	void** v4; // rbp
	void** i; // rsi
	void* v6; // r10
	void** v7; // rbx
	void** v8; // r8
	void* v9; // rdx
	void** v10; // rcx
	void* v11; // r11
	void* v12; // rdx
	void* v13; // r11
	void* v14; // rdx
	void* v15; // r11
	void* v16; // rdx
	void** v17; // r8

	result = (void**)*ptrs;
	v3 = ptrs + 1;
	v4 = ptrs + 16;
	for (i = ptrs + 2; ; ++i)
	{
		v7 = (void**)*(i - 1);
		v8 = i - 1;
		if (v7 >= result)
			break;
		if (v8 != ptrs)
		{
			result = (void**)memmove(v3, ptrs, (char*)v8 - (char*)ptrs);
			v3 = result;
		}
		*ptrs = v7;
		if (v4 == i)
			return; //result;
	LABEL_5:
		result = v7;
	}
	v6 = *(i - 2);
	if (v7 < v6)
	{
		result = i - 2;
		do
		{
			v9 = *(result - 1);
			result[1] = v6;
			v10 = result;
			if (v7 >= v9)
				break;
			*result = v9;
			v11 = *(result - 2);
			v10 = result - 1;
			if (v7 >= v11)
				break;
			v12 = *(result - 3);
			*(result - 1) = v11;
			v10 = result - 2;
			if (v7 >= v12)
				break;
			v13 = *(result - 4);
			*(result - 2) = v12;
			v10 = result - 3;
			if (v7 >= v13)
				break;
			v14 = *(result - 5);
			*(result - 3) = v13;
			v10 = result - 4;
			if (v7 >= v14)
				break;
			v15 = *(result - 6);
			*(result - 4) = v14;
			v10 = result - 5;
			if (v7 >= v15)
				break;
			v16 = *(result - 7);
			*(result - 5) = v15;
			v10 = result - 6;
			v17 = result - 7;
			if (v7 >= v16)
				break;
			*(result - 6) = v16;
			v6 = *(result - 8);
			result -= 8;
			v10 = v17;
		} while (v7 < v6);
		*v10 = v7;
	}
	if (v4 != i)
	{
		v7 = (void**)*ptrs;
		goto LABEL_5;
	}
	//return result;
}


#if 0
using _QWORD = uint64_t;
using _DWORD = unsigned int;
IMPL_CODE_SEG
static
void __fastcall sort16ptr_monster(void** a1, void** a2)
{
	void* v2; // rbx
	void** v3; // rsi
	void** v4; // r10
	void** i; // r9
	void* v6; // rdi
	void* result; // rax
	long long v8; // r11
	void* v9; // r8
	long long v10; // rdi
	long long v11; // rbx
	void* v12; // r8
	void* v13; // r11
	void* v14; // rbx
	void* v15; // r11
	void* v16; // r11
	void* v17; // r11
	void* v18; // r11
	void* v19; // r11
	void* v20; // r11
	void** v21; // rdi
	long long v22; // rbx
	long long v23; // r8
	int v24; // er11
	long long v25; // r11

	v2 = *a1;
	v3 = a1 + 16;
	v4 = a1 + 2;
	for (i = a1 + 1; ; ++i)
	{
		result = *i;
		if (*i >= v2)
			break;
		if (i != a1)
		{
			*a2 = v2;
			a2[1] = a1[1];
			a2[2] = a1[2];
			a2[3] = a1[3];
			a2[4] = a1[4];
			a2[5] = a1[5];
			a2[6] = a1[6];
			a2[7] = a1[7];
			a2[8] = a1[8];
			a2[9] = a1[9];
			a2[10] = a1[10];
			a2[11] = a1[11];
			a2[12] = a1[12];
			a2[13] = a1[13];
			a2[14] = a1[14];
			v8 = i - a1;
			a2[15] = a1[15];
			if ((_DWORD)v8)
			{
				if ((unsigned int)(v8 - 1) <= 3 || a1 == a2)
				{
					v22 = (unsigned int)v8;
					v23 = 0ll;
					v24 = v8 & 7;
					if (!v24)
						goto LABEL_58;
					if (v24 != 1ll)
					{
						if (v24 != 2ll)
						{
							if (v24 != 3ll)
							{
								if (v24 != 4ll)
								{
									if (v24 != 5ll)
									{
										if (v24 != 6ll)
										{
											a1[1] = *a2;
											v23 = 1ll;
										}
										a1[v23 + 1] = a2[v23];
										++v23;
									}
									a1[v23 + 1] = a2[v23];
									++v23;
								}
								a1[v23 + 1] = a2[v23];
								++v23;
							}
							a1[v23 + 1] = a2[v23];
							++v23;
						}
						a1[v23 + 1] = a2[v23];
						++v23;
					}
					a1[v23 + 1] = a2[v23];
					if (++v23 != v22)
					{
					LABEL_58:
						do
						{
							a1[v23 + 1] = a2[v23];
							a1[v23 + 2] = a2[v23 + 1];
							a1[v23 + 3] = a2[v23 + 2];
							a1[v23 + 4] = a2[v23 + 3];
							a1[v23 + 5] = a2[v23 + 4];
							a1[v23 + 6] = a2[v23 + 5];
							a1[v23 + 7] = a2[v23 + 6];
							v25 = v23 + 7;
							v23 += 8ll;
							a1[v25 + 1] = a2[v25];
						} while (v23 != v22);
					}
				}
				else
				{
					v9 = 0ll;
					v10 = 16ll * ((unsigned int)v8 >> 1);
					v11 = ((unsigned char)((unsigned long long)(v10 - 16) >> 4) + 1) & 7;
					if ((((unsigned char)((unsigned long long)(v10 - 16) >> 4) + 1) & 7) == 0)
						goto LABEL_59;
					if (v11 != 1)
					{
						if (v11 != 2)
						{
							if (v11 != 3)
							{
								if (v11 != 4)
								{
									if (v11 != 5)
									{
										if (v11 != 6)
										{
											v9 = (void*)0x10;
											*(__m128i*)(a1 + 1) = _mm_load_si128((const __m128i*)a2);
										}
										*(__m128i*)((char*)v9 + (_QWORD)a1 + 8) = _mm_load_si128((const __m128i*)((char*)v9 + (_QWORD)a2));
										v9 = (char*)v9 + 16;
									}
									*(__m128i*)((char*)v9 + (_QWORD)a1 + 8) = _mm_load_si128((const __m128i*)((char*)v9 + (_QWORD)a2));
									v9 = (char*)v9 + 16;
								}
								*(__m128i*)((char*)v9 + (_QWORD)a1 + 8) = _mm_load_si128((const __m128i*)((char*)v9 + (_QWORD)a2));
								v9 = (char*)v9 + 16;
							}
							*(__m128i*)((char*)v9 + (_QWORD)a1 + 8) = _mm_load_si128((const __m128i*)((char*)v9 + (_QWORD)a2));
							v9 = (char*)v9 + 16;
						}
						*(__m128i*)((char*)v9 + (_QWORD)a1 + 8) = _mm_load_si128((const __m128i*)((char*)v9 + (_QWORD)a2));
						v9 = (char*)v9 + 16;
					}
					*(__m128i*)((char*)v9 + (_QWORD)a1 + 8) = _mm_load_si128((const __m128i*)((char*)v9 + (_QWORD)a2));
					v9 = (char*)v9 + 16;
					if (v9 != (void*)v10)
					{
					LABEL_59:
						do
						{
							*(__m128i*)((char*)v9 + (_QWORD)a1 + 8) = _mm_load_si128((const __m128i*)((char*)v9 + (_QWORD)a2));
							*(__m128i*)((char*)v9 + (_QWORD)a1 + 24) = _mm_load_si128((const __m128i*)((char*)v9 + (_QWORD)a2 + 16));
							*(__m128i*)((char*)v9 + (_QWORD)a1 + 40) = _mm_load_si128((const __m128i*)((char*)v9 + (_QWORD)a2 + 32));
							*(__m128i*)((char*)v9 + (_QWORD)a1 + 56) = _mm_load_si128((const __m128i*)((char*)v9 + (_QWORD)a2 + 48));
							*(__m128i*)((char*)v9 + (_QWORD)a1 + 72) = _mm_load_si128((const __m128i*)((char*)v9 + (_QWORD)a2 + 64));
							*(__m128i*)((char*)v9 + (_QWORD)a1 + 88) = _mm_load_si128((const __m128i*)((char*)v9 + (_QWORD)a2 + 80));
							*(__m128i*)((char*)v9 + (_QWORD)a1 + 104) = _mm_load_si128((const __m128i*)((char*)v9 + (_QWORD)a2 + 96));
							*(__m128i*)((char*)v9 + (_QWORD)a1 + 120) = _mm_load_si128((const __m128i*)((char*)v9
								+ (_QWORD)a2
								+ 112));
							v9 = (char*)v9 + 128;
						} while (v9 != (void*)v10);
					}
					if ((v8 & 1) != 0)
						a1[((unsigned int)(i - a1) & 0xFFFFFFFE) + 1] = a2[(i - a1) & 0xFFFFFFFEll];
				}
			}
		}
		*a1 = result;
		if (v3 == v4)
			return ;//result;
	LABEL_5:
		++v4;
		v2 = result;
	}
	v6 = *(v4 - 2);
	if (result < v6)
	{
		v12 = v4 - 2;
		do
		{
			v13 = (void*)*((_QWORD*)v12 - 1);
			*((_QWORD*)v12 + 1) = (_QWORD)v6;
			v14 = v12;
			if (result >= v13)
				break;
			*(_QWORD*)v12 = (_QWORD)v13;
			v15 = (void*)*((_QWORD*)v12 - 2);
			v14 = (char*)v12 - 8;
			if (result >= v15)
				break;
			*((_QWORD*)v12 - 1) = (_QWORD)v15;
			v16 = (void*)*((_QWORD*)v12 - 3);
			v14 = (char*)v12 - 16;
			if (result >= v16)
				break;
			*((_QWORD*)v12 - 2) = (_QWORD)v16;
			v17 = (void*)*((_QWORD*)v12 - 4);
			v14 = (char*)v12 - 24;
			if (result >= v17)
				break;
			*((_QWORD*)v12 - 3) = (_QWORD)v17;
			v18 = (void*)*((_QWORD*)v12 - 5);
			v14 = (char*)v12 - 32;
			if (result >= v18)
				break;
			*((_QWORD*)v12 - 4) = (_QWORD)v18;
			v19 = (void*)*((_QWORD*)v12 - 6);
			v14 = (char*)v12 - 40;
			if (result >= v19)
				break;
			*((_QWORD*)v12 - 5) = (_QWORD)v19;
			v20 = (void*)*((_QWORD*)v12 - 7);
			v14 = (char*)v12 - 48;
			v21 = (void**)((char*)v12 - 56);
			if (result >= v20)
				break;
			*((_QWORD*)v12 - 6) = (_QWORD)v20;
			v12 = (char*)v12 - 64;
			v14 = v21;
			v6 = *(void**)v12;
		} while ((unsigned long long)result < *(_QWORD*)v12);
		*(_QWORD*)v14 = (_QWORD)result;
	}
	if (v3 != v4)
	{
		result = *a1;
		goto LABEL_5;
	}
	return ;//result;
}
#endif

IMPL_CODE_SEG
static
void  cs_sort4ptr(void** ptrs)
{
	void* current_value; // rax
	void** end; // rbx
	void** midp; // r9
	void** i; // r8
	void* v6; // rcx
	void* v7; // rdx
	size_t v8; // rcx
	__m128i v9; // xmm0
	void** v10; // rax
	void** v11; // r11

	current_value = *ptrs;
	end = ptrs + 4;
	midp = ptrs + 2;
	for (i = ptrs + 1; ; ++i)
	{
		v7 = *i;
		if (*i >= current_value)
			break;
		if (i != ptrs)
		{
			v8 = i - ptrs;
			if ((unsigned int)v8 <= 1)
			{
				if ((unsigned)v8)
					ptrs[1] = current_value;
			}
			else
			{
				
				v9 = _mm_insert_epi64(_mm_cvtsi64_si128((long long)current_value), (signed long long)ptrs[1], 1);
				if ((unsigned)v8 == 3)
					ptrs[3] = ptrs[2];
				_mm_storeu_si128(reinterpret_cast<__m128i*>(ptrs + 1), v9);
			}
		}
		*ptrs = v7;
		if (end == midp)
			return;
	LABEL_5:
		++midp;
		current_value = v7;
	}
	v6 = *(midp - 2);
	if (v7 < v6)
	{
		v10 = midp - 2;
		do
		{
			v10[1] = v6;
			v11 = v10;
			v6 = *--v10;
		}     while (v7 < v6);
		*v11 = v7;
	}
	if (end != midp)
	{
		v7 = *ptrs;
		goto LABEL_5;
	}
}

static void snsalgos_init(snaphak_snsroutines_t* sns) {
	sns->m_sort16ptr = sort16ptr;
	sns->m_sort16ptr_unrolled = sort16ptr_unrolled;
	//sns->m_sort16ptr_with_scratch = sort16ptr_monster;
	sns->m_sort4ptr = cs_sort4ptr;
}