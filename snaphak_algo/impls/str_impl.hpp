#if defined(__AVX2__)
#define		STR_TOLOWER_CONST(...)				(((__VA_ARGS__) <= 'Z' && (__VA_ARGS__) >= 'A') ? ((__VA_ARGS__) + ('a' - 'A')) : (__VA_ARGS__))
#define hasbetween(x,m,n) \
((~0UL/255*(127+(n))-((x)&~0UL/255*127)&~(x)&((x)&~0UL/255*127)+~0UL/255*(127-(m)))&~0UL/255*128)
CS_FORCEINLINE
static inline __m128i tolower_m128i(__m128i first_16_tested) {
	__m128i upperv1 = _mm_set1_epi8('Z' + 1);
	__m128i upperv2 = _mm_set1_epi8('A' - 1);

	__m128i addend = _mm_set1_epi8(('a' - 'A'));

	__m128i xlated = _mm256_castsi256_si128(_mm256_blendv_epi8(
		_mm256_castsi128_si256(first_16_tested),
		_mm256_castsi128_si256(_mm_add_epi8(first_16_tested, addend)),
		_mm256_castsi128_si256(_mm_and_si128(_mm_cmpgt_epi8(upperv1, first_16_tested),

			_mm_cmpgt_epi8(first_16_tested, upperv2)))));
	return xlated;
}
#endif
template<bool insensitive>
CS_FORCEINLINE static bool cs_strcmpeq_impl(const char* s1, const char* s2) {
#if defined(__AVX2__)

	const __m128i* s1x = (const __m128i*)s1;
	const __m128i* s2x = (const __m128i*)s2;

	while (true) {
		__m128i curr1 = _mm_loadu_si128(s1x);
		__m128i curr2 = _mm_loadu_si128(s2x);

		__m128i z1 = _mm_cmpeq_epi8(curr1, _mm_setzero_si128());

		__m128i z2 = _mm_cmpeq_epi8(curr1, _mm_setzero_si128());


		unsigned z1m = _mm_movemask_epi8(z1);
		unsigned z2m = _mm_movemask_epi8(z2);

		if (_tzcnt_u32(z1m) != _tzcnt_u32(z2m)) {
			return false;
		}
		if constexpr (insensitive) {
			curr1 = tolower_m128i(curr1);
			curr2 = tolower_m128i(curr2);
		}

		__m128i currcmp = _mm_cmpeq_epi8(curr1, curr2);

		unsigned mmcmp = (~_mm_movemask_epi8(currcmp)) & 0xffff;

		if (z1m) {
			unsigned z1mmask = _tzcnt_u32(z1m);
			if (mmcmp & ((1 << (z1mmask + 1)) - 1)) {
				return false;
			}
			return true;
		}
		else {
			if (mmcmp != 0) {
				return false;
			}
		}
		s1x++;
		s2x++;
	}
	cs_assume_m(false);
#else
	if constexpr (insensitive) {
		return !_strcmpi(s1, s2);
	}
	else {
		return !strcmp(s1, s2);
	}

#endif
}
#if defined(__AVX2__)
__attribute__((naked))
static bool cs_strcmpeq_avx2_asm(const char* s1, const char* s2) {
	__asm {
		vmovdqu xmm1, xmmword ptr[rcx]
		vpxor   xmm0, xmm0, xmm0
		vpcmpeqb xmm2, xmm1, xmm0
		vpmovmskb r9d, xmm2
		vpcmpeqb xmm1, xmm1, xmmword ptr[rdx]
		vpmovmskb r8d, xmm1
		test    r9w, r9w
		jnz     short loc_1800A8B77
		mov     eax, 10h

		ALIGN 16
		loc_1800A8B50:
		cmp     r8w, 0FFFFh
			jnz     short loc_1800A8B99
			vmovdqu xmm1, xmmword ptr[rcx + rax]
			vpcmpeqb xmm2, xmm1, xmm0
			vpcmpeqb xmm1, xmm1, xmmword ptr[rdx + rax]
			vpmovmskb r9d, xmm2
			vpmovmskb r8d, xmm1
			add     rax, 10h
			test    r9w, r9w
			jz      short loc_1800A8B50

			loc_1800A8B77 :
		movzx   eax, r9w
			tzcnt   eax, eax
			mov     ecx, 0FFFFFFFEh
			shlx    eax, ecx, eax
			not eax
			andn    eax, r8d, eax
			test    eax, 0FFFFh
			setz    al
			test al, al
			retn

			loc_1800A8B99 :
		xor eax, eax
			retn
	}
}

#endif
IMPL_CODE_SEG
static
bool cs_stricmpeq(const char* s1, const char* s2) {
	return cs_strcmpeq_impl<true>(s1, s2);
}
IMPL_CODE_SEG
static
bool cs_strcmpeq(const char* s1, const char* s2) {
	return cs_strcmpeq_impl<false>(s1, s2);
}
#if defined(__AVX2__)
static
unsigned fast_avx2_strlen(const char* s) {

	bool done = false;
	__m256i msk;
	const char* sorig = s;
	//	__debugbreak();
	while (true) {
		__m256i data = _mm256_loadu_si256((__m256i*)s);

		msk = _mm256_cmpeq_epi8(data, _mm256_setzero_si256());

		if (_mm256_testz_si256(msk, msk)) {
			s += 32;
		}
		else {
			break;
		}
	}
	uint32_t movemask = _mm256_movemask_epi8(msk);

	unsigned l = s - sorig;

	l += _tzcnt_u32(movemask);

	return l;
}

#endif
static
unsigned fast_sse_strlen(const char* s) {

	bool done = false;
	__m128i msk;
	const char* sorig = s;
	//	__debugbreak();
	while (true) {
		__m128i data = _mm_loadu_si128((__m128i*)s);

		msk = _mm_cmpeq_epi8(data, _mm_setzero_si128());

		if (_mm_testz_si128(msk, msk)) {
			s += 16;
		}
		else {
			break;
		}
	}
	uint32_t movemask = _mm_movemask_epi8(msk);

	unsigned l = s - sorig;

	l += _tzcnt_u32(movemask);

	return l;
}
IMPL_CODE_SEG
static unsigned hashfnv32(const char* s1, unsigned length) {
	unsigned const char* recs1 = (unsigned const char*)s1;

	unsigned hcode = 0x811C9DC5;


	for (unsigned i = 0; i < length; ++i)
	{
		unsigned currchar = recs1[i];

		hcode = 0x1000193 * (hcode ^ currchar);
	}
	return hcode;

}
IMPL_CODE_SEG

static unsigned fast_strlen_func_impl(const char* s) {
	unsigned u_p = (uintptr_t)const_cast<char*>(s);
#if defined(__AVX2__)
	if ((u_p & 31)) {
#endif
		if (!(u_p & 15)) {
			return fast_sse_strlen(s);
		}
		return strlen(s);
#if defined(__AVX2__)
	}
	else {
		return fast_avx2_strlen(s);
	}
#endif
}
static
int strtol_func(unsigned __int8* str, unsigned __int8** endptr, int base);

IMPL_CODE_SEG

static int strconvt_str_to_long(const char* str, const char** out_endptr, unsigned base) {
	return strtol_func((unsigned char*)const_cast<char*>(str), (unsigned char**)const_cast<char**>(out_endptr), base);
}



static constexpr unsigned char IsTable[] =
{
  0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
  0x03, 0x03, 0x03, 0x03, 0x03, 0x01, 0x01, 0x01, 0x01, 0x01,
  0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
  0x01, 0x01, 0x01, 0x0A, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C,
  0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x38,
  0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x38, 0x0C,
  0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x58, 0x58, 0x58, 0x58,
  0x58, 0x58, 0x48, 0x48, 0x48, 0x48, 0x48, 0x48, 0x48, 0x48,
  0x48, 0x48, 0x48, 0x48, 0x48, 0x48, 0x48, 0x48, 0x48, 0x48,
  0x48, 0x48, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x98, 0x98,
  0x98, 0x98, 0x98, 0x98, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
  0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88, 0x88,
  0x88, 0x88, 0x88, 0x88, 0x0C, 0x0C, 0x0C, 0x0C, 0x01, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
template<unsigned base, bool store_endptr = false>
static inline int strtol_func_tmpl(unsigned __int8* str, unsigned __int8** endptr = nullptr)
{
	unsigned __int8* v4; // rsi
	unsigned __int8 v5; // r10
	unsigned __int8* i; // r9
	char v7; // bl
	int v8; // er11
	unsigned __int8* v9; // rbp
	unsigned int v10; // edx
	int v11; // ecx
	int v12; // ecx
	unsigned int v13; // eax
	bool v14; // zf
	__int64 result; // rax

	v4 = str;
	if constexpr (store_endptr) {
		if (endptr)
			*endptr = str;
	}
	v5 = *str;
	for (i = str; (IsTable[(unsigned __int8)(v5 + 1)] & 2) != 0; ++i)
		v5 = i[1];
	if (((v5 - 43) & 0xFD) == 0)
		++i;
	if (base)
	{
		if ((unsigned int)(base - 2) > 0x22)
			return 0i64;
		if (base == 16 && *i == 48) {
			if (((i[1] - 88) & 0xDF) == 0)
				i += 2;
			goto LABEL_18;
		}
	}

LABEL_18:
	v7 = 0;
	v8 = 0;
	v9 = i;
	v10 = 0;
	while (1)
	{
		v11 = *i;
		if ((unsigned __int8)(*i - 48) > 9u)
		{
			if ((unsigned int)(v11 - 65) <= 0x19)
				*(unsigned char*)&v11 = v11 + 32;
			if ((unsigned __int8)(v11 - 97) > 8u)
				v12 = ((unsigned __int8)v11 < 0x6Au || (unsigned __int8)v11 > 0x72u) && (unsigned __int8)(v11 - 115) > 7u ? 37 : (unsigned __int8)v11 - 87;
			else
				v12 = (unsigned __int8)v11 - 87;
		}
		else
		{
			v12 = v11 - 48;
		}
		if (v12 >= base)
			break;
		v13 = v10;
		v10 = v12 + base * v10;
		if (v10 < v13)
			v7 = 1;
		++i;
	}
	if (i == v9)
		i = v4;
	if constexpr (store_endptr) {
		if (endptr)
			*endptr = i;
	}
	if (v10 >= 0x80000000)
	{
		if (v10 != 0x80000000)
		{
		LABEL_42:
			v14 = v5 == 45;
			goto LABEL_43;
		}
		v14 = v5 == 45;
		if (v5 != 45)
		{
		LABEL_43:
			*(unsigned char*)&v8 = v14;
			return (unsigned int)(v8 + 0x7FFFFFFF);
		}
	}
	if (v7)
		goto LABEL_42;
	result = -v10;
	if (v5 != 45)
		result = v10;
	return result;
}
IMPL_CODE_SEG
static
int strconvt_atoi_fast(const char* str) {
	return strtol_func_tmpl<10, false>((unsigned char*)const_cast<char*>(str), nullptr);
}
IMPL_CODE_SEG

static
int  strtol_func(unsigned __int8* str, unsigned __int8** endptr, int base)
{
	if (base == 10) {
		return strtol_func_tmpl<10, true>(str, endptr);
	}
	else if (base == 16) {
		return strtol_func_tmpl<16, true>(str, endptr);
	}
	else if (base == 2) {
		return strtol_func_tmpl<2, true>(str, endptr);
	}
	return -1;
}
template<unsigned int radix, int sign>
static inline unsigned xtoa_s(unsigned int val, char* DstBuf, unsigned size)
{
	char* v7; // r10
	int* v9; // rax
	int v10; // ebx
	unsigned v11; // rsi
	char* v12; // rcx
	char* v13; // r8
	char* v14; // r14
	char* v15; // r9
	unsigned int v16; // edx
	char v17; // al
	char v18; // cl

	v7 = DstBuf;
	*DstBuf = 0;
	v10 = 34;
	v11 = 0i64;
	v12 = DstBuf;
	if (sign)
	{
		*DstBuf = 45;
		v12 = DstBuf + 1;
		v11 = 1i64;
		val = -val;
	}
	v13 = v12;
	do
	{
		v14 = v12 + 1;
		v15 = v12;
		v16 = val % radix;
		val /= radix;
		v17 = 87;
		if (v16 <= 9)
			v17 = 48;
		++v11;
		*v12 = v16 + v17;
		if (!val)
			break;
		++v12;
	} while (v11 < size);
	*v14 = 0;
	do
	{
		v18 = *v15;
		*v15-- = *v13;
		*v13++ = v18;
	} while (v13 < v15);
	return static_cast<unsigned>(v14 - DstBuf);
}
IMPL_CODE_SEG

static
unsigned strconvt_int2str_base10(unsigned int val, char* dstbuf, unsigned size) {
	if (static_cast<int>(val) < 0) {
		return xtoa_s<10, 1>(val, dstbuf, size);
	}
	else {
		return xtoa_s<10, 0>(val, dstbuf, size);
	}
}
IMPL_CODE_SEG

static
unsigned strconvt_uint2str_base10(unsigned int val, char* dstbuf, unsigned size) {

	return xtoa_s<10, 0>(val, dstbuf, size);

}
IMPL_CODE_SEG

static
unsigned strconvt_int2str_base16(unsigned int val, char* dstbuf, unsigned size) {
	return xtoa_s<16, 0>(val, dstbuf, size);
}

#define white_space(c) ((c) == ' ' || (c) == '\t')
#define valid_digit(c) ((c) >= '0' && (c) <= '9')
IMPL_CODE_SEG

static
double strconvt_fast_atof(const char* p, const char** out_endpos)
{
	int frac;
	double sign, value, scale;

	// Skip leading white space, if any.

	while (white_space(*p)) {
		p += 1;
	}

	// Get sign, if any.

	sign = 1.0;
	if (*p == '-') {
		sign = -1.0;
		p += 1;

	}
	else if (*p == '+') {
		p += 1;
	}

	// Get digits before decimal point or exponent, if any.

	for (value = 0.0; valid_digit(*p); p += 1) {
		value = value * 10.0 + (*p - '0');
	}

	// Get digits after decimal point, if any.

	if (*p == '.') {
		double pow10 = 10.0;
		p += 1;
		while (valid_digit(*p)) {
			value += (*p - '0') / pow10;
			pow10 *= 10.0;
			p += 1;
		}
	}

	// Handle exponent, if any.

	frac = 0;
	scale = 1.0;
	//expect to have nice normal fp numbers most of the time
	if ((*p == 'e') || (*p == 'E')) [[unlikely]] {
		unsigned int expon;

	// Get sign of exponent, if any.

	p += 1;
	if (*p == '-') {
		frac = 1;
		p += 1;

	}
else if (*p == '+') {
 p += 1;
}

	// Get digits of exponent, if any.

	for (expon = 0; valid_digit(*p); p += 1) {
		expon = expon * 10 + (*p - '0');
	}
	if (expon > 308) expon = 308;

	// Calculate scaling factor.
#pragma clang loop unroll(disable)
	while (expon >= 50) { scale *= 1E50; expon -= 50; }
#pragma clang loop unroll(disable)
	while (expon >= 8) { scale *= 1E8;  expon -= 8; }
#pragma clang loop unroll(disable)
	while (expon > 0) { scale *= 10.0; expon -= 1; }
	}
		if (out_endpos)
			*out_endpos = p;
	// Return signed and scaled floating point result.

	return sign * (frac ? (value / scale) : (value * scale));
}
static double g_tentothe[32] = { 0 };
IMPL_CODE_SEG

static
unsigned strconvt_float2str_fast(float fvalue, char* dstbuf, unsigned precision) {

	bool sign = fvalue < 0;
	if (sign)
		fvalue = -fvalue;




	float fractpart = fvalue - __builtin_floorf(fvalue);//idMath::Frac(fvalue);
	float ipart = fvalue - fractpart;

	unsigned fracti = static_cast<unsigned>(static_cast<double>(fractpart) * g_tentothe[precision]);
	unsigned i = static_cast<unsigned>(ipart);


	char* buffer_pos = dstbuf;

	if (sign) {
		*buffer_pos = '-';
		buffer_pos += 1;
	}
	buffer_pos += xtoa_s<10, 0>(i, buffer_pos, 32);

	if (fracti) {
		*buffer_pos = '.';
		buffer_pos += 1;
		buffer_pos += xtoa_s<10, 0>(fracti, buffer_pos, 32);

	}
	*buffer_pos = 0;

	return buffer_pos - dstbuf;



}
IMPL_CODE_SEG
static
int __cdecl cs_strcmp(const char* Str1, const char* Str2)
{
	signed __int64 v2; // r9
	char v3; // al
	unsigned __int8 v4; // dl
	__int64 v5; // rax
	unsigned __int64 v6; // rdx
	int result; // eax
	unsigned __int64 v8; // rdx
	unsigned __int64 v9; // rdx
	unsigned int v10; // edx

	v2 = Str2 - Str1;
	if (((unsigned __int8)Str1 & 7) != 0)
	{
#pragma clang loop unroll(disable)
		while (1)
		{
		LABEL_2:
			v3 = *Str1;
			v4 = Str1[v2];
			if (*Str1 != v4)
				return -((unsigned int)*Str1 < v4) - (((unsigned int)*Str1 < v4) - 1);
			++Str1;
			if (!v3)
				break;
			if (((unsigned __int8)Str1 & 7) == 0)
				goto LABEL_5;
		}

		result = 0;
	}
	else
	{
		while (1)
		{
		LABEL_5:
			if ((((unsigned short)v2 + (unsigned short)Str1) & 0xFFFu) > 0xFF8)
				goto LABEL_2;
			v5 = *(long long*)Str1;
			v6 = *(long long*)&Str1[v2];
			if (*(long long*)Str1 != v6)
				goto LABEL_2;
			Str1 += 8;
			if ((((v6 + 0x7EFEFEFEFEFEFEFFi64) ^ ~v5) & 0x8101010101010100ui64) != 0)
			{
				if (!(unsigned char)v6)
					break;
#define			BYTE1(x)		((x >> 8)&0xff)
				if (!BYTE1(v6))
					break;
				v8 = v6 >> 16;
				if (!(unsigned char)v8)
					break;
				if (!BYTE1(v8))
					break;
				v9 = v8 >> 16;
				if (!(unsigned char)v9)
					break;
				if (!BYTE1(v9))
					break;
				v10 = (unsigned short)(v9 >> 16);
				if (!(unsigned char)v10 || !BYTE1(v10))
					break;
			}
		}
		result = 0;
	}
	return result;
}
#undef BYTE1
IMPL_CODE_SEG
CS_NOINLINE
static
const char* __cdecl cs_strstr(const char* Str, const char* SubStr)
{
	char* v3; // r8
	char v5; // al
	signed __int64 i; // r9
	const char* v7; // rdx

	v3 = (char*)Str;
	if (!*SubStr)
		return (char*)Str;
	v5 = *Str;
	if (!*Str)
		return 0i64;
	for (i = Str - SubStr; ; ++i)
	{
		v7 = SubStr;
		if (v5)
			break;
	LABEL_9:
		if (!*v7)
			return v3;
		v5 = *++v3;
		if (!*v3)
			return 0i64;
	}
	while (*v7)
	{
		if (v7[i] == *v7)
		{
			++v7;
			if (v7[i])
				continue;
		}
		goto LABEL_9;
	}
	return v3;
}
IMPL_CODE_SEG
CS_NOINLINE
static
const char* __fastcall _strchr_sse2(const char* a1, char a2)
{
	__m128i v2; // xmm1
	__m128i v3; // xmm1
	__m128i v4; // xmm1
	__m128i v5; // xmm4
	__int64 v6; // rax
	__int8* result; // rax
	__m128i v8; // xmm4
	unsigned int v9; // ecx
	__m128i v10; // xmm4
	__m128i v11; // xmm0
	unsigned __int64 v12; // rax
	__m128i v13; // xmm2
	__m128i v14; // xmm3
	__m128i v15; // xmm4
	__m128i v16; // xmm0
	__m128i v17; // xmm4
	unsigned int v18; // er8
	__m128i v19; // xmm4
	unsigned int v20; // eax
	__m128i v21; // xmm4
	__m128i v22; // xmm3

	v2 = _mm_cvtsi32_si128(a2);
	v3 = _mm_unpacklo_epi8(v2, v2);
	v4 = _mm_shuffle_epi32(_mm_unpacklo_epi16(v3, v3), 0);
	if (((unsigned __int16)a1 & 0xFFFu) > 0xFC0)
	{
		v17 = _mm_load_si128((const __m128i*)((unsigned __int64)a1 & 0xFFFFFFFFFFFFFFC0LL));
		v18 = _mm_movemask_epi8(_mm_or_si128(_mm_cmpeq_epi8(v17, v4), _mm_cmpeq_epi8(v17, _mm_setzero_si128())));
		v19 = _mm_load_si128((const __m128i*)(((unsigned __int64)a1 & 0xFFFFFFFFFFFFFFC0LL) + 16));
		v20 = _mm_movemask_epi8(_mm_or_si128(_mm_cmpeq_epi8(v19, v4), _mm_cmpeq_epi8(v19, _mm_setzero_si128())));
		v21 = _mm_load_si128((const __m128i*)(((unsigned __int64)a1 & 0xFFFFFFFFFFFFFFC0LL) + 32));
		v22 = _mm_load_si128((const __m128i*)(((unsigned __int64)a1 & 0xFFFFFFFFFFFFFFC0LL) + 48));
		v12 = (((unsigned __int64)(unsigned int)_mm_movemask_epi8(_mm_or_si128(_mm_cmpeq_epi8(v4, v22), _mm_cmpeq_epi8(_mm_setzero_si128(), v22))) << 48) | v18 | ((unsigned __int64)(unsigned int)_mm_movemask_epi8(_mm_or_si128(_mm_cmpeq_epi8(v21, v4), _mm_cmpeq_epi8(v21, _mm_setzero_si128()))) << 32) | ((unsigned __int64)v20 << 16)) >> ((unsigned __int8)a1 - ((unsigned __int8)a1 & 0xC0u));
		if (v12)
			goto LABEL_10;
		goto LABEL_7;
	}
	v5 = _mm_loadu_si128((const __m128i*)a1);
	*reinterpret_cast<int*>(&v6) = _mm_movemask_epi8(_mm_or_si128(_mm_cmpeq_epi8(v5, v4), _mm_cmpeq_epi8(v5, _mm_setzero_si128())));
	if ((unsigned)v6)
	{
		_BitScanForward((unsigned long*)&v6, v6);
		result = (__int8*)&a1[v6];
		if (*result != a2)
			result = 0LL;
		return result;
	}
	v8 = _mm_loadu_si128((const __m128i*)a1 + 1);
	v9 = _mm_movemask_epi8(_mm_or_si128(_mm_cmpeq_epi8(v8, v4), _mm_cmpeq_epi8(v8, _mm_setzero_si128())));
	v10 = _mm_loadu_si128((const __m128i*)a1 + 2);
	v11 = _mm_loadu_si128((const __m128i*)a1 + 3);
	v12 = ((unsigned __int64)(unsigned int)_mm_movemask_epi8(_mm_or_si128(_mm_cmpeq_epi8(v11, v4), _mm_cmpeq_epi8(_mm_setzero_si128(), v11))) << 48) | ((unsigned __int64)v9 << 16) | ((unsigned __int64)(unsigned int)_mm_movemask_epi8(_mm_or_si128(_mm_cmpeq_epi8(v10, v4), _mm_cmpeq_epi8(v10, _mm_setzero_si128()))) << 32);
	if (!v12)
	{
	LABEL_7:
		a1 = (const char*)((unsigned __int64)a1 & 0xFFFFFFFFFFFFFFC0LL);
		do
		{
			a1 += 64;
			v13 = _mm_min_epu8(_mm_xor_si128(_mm_load_si128((const __m128i*)a1 + 1), v4), *((__m128i*)a1 + 1));
			v14 = _mm_min_epu8(_mm_xor_si128(_mm_load_si128((const __m128i*)a1 + 2), v4), *((__m128i*)a1 + 2));
			v15 = _mm_min_epu8(_mm_xor_si128(_mm_load_si128((const __m128i*)a1 + 3), v4), *((__m128i*)a1 + 3));
		} while (!_mm_movemask_epi8(
			_mm_cmpeq_epi8(
				_mm_min_epu8(
					_mm_min_epu8(
						_mm_min_epu8(
							_mm_min_epu8(_mm_xor_si128(_mm_load_si128((const __m128i*)a1), v4), *(__m128i*)a1),
							v13),
						v14),
					v15),
				_mm_setzero_si128())));
		v16 = _mm_load_si128((const __m128i*)a1);
		v12 = ((unsigned __int64)(unsigned int)_mm_movemask_epi8(_mm_cmpeq_epi8(v15, _mm_setzero_si128())) << 48) | (unsigned int)_mm_movemask_epi8(_mm_or_si128(_mm_cmpeq_epi8(v16, v4), _mm_cmpeq_epi8(v16, _mm_setzero_si128()))) | ((unsigned __int64)(unsigned int)_mm_movemask_epi8(_mm_cmpeq_epi8(v14, _mm_setzero_si128())) << 32) | ((unsigned __int64)(unsigned int)_mm_movemask_epi8(_mm_cmpeq_epi8(v13, _mm_setzero_si128())) << 16);
	}
LABEL_10:
	_BitScanForward64((unsigned  long*)&v12, v12);
	result = (__int8*)&a1[v12];
	if (*result != a2)
		result = 0LL;
	return result;
}

IMPL_CODE_SEG
static
const char* __fastcall _strstr_sse2_unaligned(const char* a1, const char* a2)
{
	unsigned int v2; // eax
	unsigned int v3; // edx
	__m128i v4; // xmm1
	__m128i v5; // xmm2
	__m128i v6; // xmm1
	__m128i v7; // xmm2
	__m128i v8; // xmm1
	__m128i v9; // xmm2
	__m128i v10; // xmm6
	__m128i v11; // xmm0
	unsigned __int64 v12; // r8
	unsigned __int64 v13; // rax
	const char* result; // rax
	char v15; // dl
	__int64 v16; // rdx
	char v17; // cl
	__m128i v18; // xmm6
	__m128i v19; // xmm0
	unsigned __int64 v20; // r8
	unsigned __int64 v21; // rax
	char v22; // dl
	__int64 v23; // rdx
	char v24; // cl
	__int64 v25; // r11
	unsigned __int64 v27; // rdi
	__m128i v28; // xmm0
	__m128i v29; // xmm10
	__m128i v30; // xmm6
	__m128i v31; // xmm9
	__m128i v32; // xmm3
	__m128i v33; // xmm8
	__m128i v34; // xmm4
	__m128i v35; // xmm5
	__m128i v36; // xmm8
	unsigned __int64 v37; // r8
	unsigned __int64 v38; // rcx
	char* v39; // rcx
	__int64 v40; // rax
	char v41; // dl
	char v42; // dl
	__m128i v43; // xmm3
	__m128i v44; // xmm5
	__m128i v45; // xmm8
	__m128i v46; // xmm7
	__m128i v47; // xmm3
	__m128i v48; // xmm5
	unsigned __int64 i; // r8
	unsigned __int64 v50; // rax
	const char* v51; // rax
	char v52; // dl
	__int64 v53; // rdx
	char v54; // cl

	v2 = (unsigned __int8)*a2;
	if (!(char)v2)
		return a1;
	v3 = (unsigned __int8)a2[1];
	if (!(char)v3)
		return (const char*)_strchr_sse2(a1, (unsigned __int8)v2);
	v4 = _mm_cvtsi32_si128(v2);
	v5 = _mm_cvtsi32_si128(v3);
	v6 = _mm_unpacklo_epi8(v4, v4);
	v7 = _mm_unpacklo_epi8(v5, v5);
	v8 = _mm_shuffle_epi32(_mm_unpacklo_epi16(v6, v6), 0);
	v9 = _mm_shuffle_epi32(_mm_unpacklo_epi16(v7, v7), 0);
	if (((unsigned __int16)a1 & 0xFFFu) > 0xFBFuLL)
	{
		v43 = _mm_load_si128((const __m128i*)((unsigned __int64)a1 & 0xFFFFFFFFFFFFFFC0LL));
		v44 = _mm_load_si128((const __m128i*)(((unsigned __int64)a1 & 0xFFFFFFFFFFFFFFC0LL) + 16));
		v45 = _mm_or_si128(
			_mm_cmpeq_epi8(v43, _mm_setzero_si128()),
			_mm_min_epu8(
				_mm_cmpeq_epi8(v43, v9),
				_mm_cmpeq_epi8(_mm_loadu_si128((const __m128i*)(((unsigned __int64)a1 & 0xFFFFFFFFFFFFFFC0LL) - 1)), v8)));
		v46 = _mm_or_si128(
			_mm_cmpeq_epi8(v44, _mm_setzero_si128()),
			_mm_min_epu8(
				_mm_cmpeq_epi8(v44, v9),
				_mm_cmpeq_epi8(_mm_loadu_si128((const __m128i*)(((unsigned __int64)a1 & 0xFFFFFFFFFFFFFFC0LL) + 15)), v8)));
		v47 = _mm_load_si128((const __m128i*)(((unsigned __int64)a1 & 0xFFFFFFFFFFFFFFC0LL) + 32));
		v48 = _mm_load_si128((const __m128i*)(((unsigned __int64)a1 & 0xFFFFFFFFFFFFFFC0LL) + 48));
		for (i = (((unsigned __int64)(unsigned int)_mm_movemask_epi8(
			_mm_or_si128(
				_mm_cmpeq_epi8(_mm_setzero_si128(), v48),
				_mm_min_epu8(
					_mm_cmpeq_epi8(v48, v9),
					_mm_cmpeq_epi8(
						_mm_loadu_si128((const __m128i*)(((unsigned __int64)a1 & 0xFFFFFFFFFFFFFFC0LL)
							+ 47)),
						v8)))) << 48) | (unsigned int)_mm_movemask_epi8(v45) | ((unsigned __int64)(unsigned int)_mm_movemask_epi8(_mm_or_si128(_mm_cmpeq_epi8(v47, _mm_setzero_si128()), _mm_min_epu8(_mm_cmpeq_epi8(v47, v9), _mm_cmpeq_epi8(_mm_loadu_si128((const __m128i*)(((unsigned __int64)a1 & 0xFFFFFFFFFFFFFFC0LL) + 31)), v8)))) << 32) | ((unsigned __int64)(unsigned int)_mm_movemask_epi8(v46) << 16)) >> ((unsigned __int8)a1 - ((unsigned __int8)a1 & 0xC0u));
			i;
			i &= i - 1)
		{
			_BitScanForward64((unsigned long*)&v50, i);
			v51 = &a1[v50];
			if (!*v51)
				return 0LL;
			if (a1 != v51)
			{
				v52 = a2[2];
				if (!v52)
					return v51 - 1;
				if (v52 == v51[1])
				{
					v53 = 0LL;
					do
					{
						v54 = a2[v53 + 3];
						if (!v54)
							return v51 - 1;
						++v53;
					} while (v54 == v51[v53 + 1]);
				}
			}
		}
	LABEL_26:
		v25 = -512LL;
		v27 = (unsigned __int64)a1 & 0xFFFFFFFFFFFFFFC0LL;
		do
		{
			do
			{
			LABEL_27:
				v28 = _mm_load_si128((const __m128i*)(v27 + 64));
				v29 = _mm_load_si128((const __m128i*)(v27 + 80));
				v30 = _mm_or_si128(_mm_xor_si128(_mm_loadu_si128((const __m128i*)(v27 + 63)), v8), _mm_xor_si128(v28, v9));
				v31 = _mm_load_si128((const __m128i*)(v27 + 96));
				v32 = _mm_or_si128(_mm_xor_si128(_mm_loadu_si128((const __m128i*)(v27 + 79)), v8), _mm_xor_si128(v29, v9));
				v33 = _mm_load_si128((const __m128i*)(v27 + 112));
				v27 += 64LL;
				v34 = _mm_or_si128(_mm_xor_si128(_mm_loadu_si128((const __m128i*)(v27 + 31)), v8), _mm_xor_si128(v31, v9));
				v35 = _mm_or_si128(_mm_xor_si128(_mm_loadu_si128((const __m128i*)(v27 + 47)), v8), _mm_xor_si128(v33, v9));
			} while (!_mm_movemask_epi8(
				_mm_cmpeq_epi8(
					_mm_min_epu8(
						_mm_min_epu8(_mm_min_epu8(_mm_min_epu8(v28, v29), v31), v33),
						_mm_min_epu8(_mm_min_epu8(_mm_min_epu8(v32, v30), v34), v35)),
					_mm_setzero_si128())));
			v36 = _mm_load_si128((const __m128i*)(v27 + 16));
			v37 = ((unsigned __int64)(unsigned int)_mm_movemask_epi8(
				_mm_or_si128(
					_mm_cmpeq_epi8(v36, _mm_setzero_si128()),
					_mm_min_epu8(
						_mm_cmpeq_epi8(v36, v9),
						_mm_cmpeq_epi8(_mm_loadu_si128((const __m128i*)(v27 + 15)), v8)))) << 16) | ((unsigned __int64)(unsigned int)_mm_movemask_epi8(_mm_cmpeq_epi8(_mm_min_epu8(v34, *(__m128i*)(v27 + 32)), _mm_setzero_si128())) << 32) | (unsigned int)_mm_movemask_epi8(_mm_cmpeq_epi8(_mm_min_epu8(v30, *(__m128i*)v27), _mm_setzero_si128())) | ((unsigned __int64)(unsigned int)_mm_movemask_epi8(_mm_cmpeq_epi8(_mm_min_epu8(v35, *(__m128i*)(v27 + 48)), _mm_setzero_si128())) << 48);
		} while (!v37);
		while (1)
		{
			_BitScanForward64((unsigned long*)&v38, v37);
			v39 = (char*)(v27 + v38);
			if (!*v39)
				return 0LL;
			v40 = 0LL;
			v41 = a2[2];
			if (!v41)
				return v39 - 1;
			if (v41 == v39[1])
			{
				do
				{
					v42 = a2[v40 + 3];
					if (!v42)
						return v39 - 1;
					++v40;
				} while (v42 == v39[v40 + 1]);
			}
			v25 += v40;
			if ((__int64)(v27 - (long long)a1) < v25)
				return (const char*)cs_strstr((char*)v27, a2);
			v37 &= v37 - 1;
			if (!v37)
				goto LABEL_27;
		}
	}
	v10 = _mm_loadu_si128((const __m128i*)a1);
	v11 = _mm_loadu_si128((const __m128i*)a1 + 1);
	v12 = ((unsigned __int64)(unsigned int)_mm_movemask_epi8(
		_mm_or_si128(
			_mm_min_epu8(
				_mm_cmpeq_epi8(v11, v8),
				_mm_cmpeq_epi8(_mm_loadu_si128((const __m128i*)(a1 + 17)), v9)),
			_mm_cmpeq_epi8(_mm_setzero_si128(), v11))) << 16) | (unsigned int)_mm_movemask_epi8(_mm_or_si128(_mm_min_epu8(_mm_cmpeq_epi8(v10, v8), _mm_cmpeq_epi8(_mm_loadu_si128((const __m128i*)(a1 + 1)), v9)), _mm_cmpeq_epi8(v10, _mm_setzero_si128())));
	if (!v12)
	{
	LABEL_15:
		v18 = _mm_loadu_si128((const __m128i*)a1 + 2);
		v19 = _mm_loadu_si128((const __m128i*)a1 + 3);
		v20 = ((unsigned __int64)(unsigned int)_mm_movemask_epi8(
			_mm_or_si128(
				_mm_min_epu8(
					_mm_cmpeq_epi8(v18, v8),
					_mm_cmpeq_epi8(_mm_loadu_si128((const __m128i*)(a1 + 33)), v9)),
				_mm_cmpeq_epi8(v18, _mm_setzero_si128()))) << 32) | ((unsigned __int64)(unsigned int)_mm_movemask_epi8(_mm_or_si128(_mm_min_epu8(_mm_cmpeq_epi8(v19, v8), _mm_cmpeq_epi8(_mm_loadu_si128((const __m128i*)(a1 + 49)), v9)), _mm_cmpeq_epi8(_mm_setzero_si128(), v19))) << 48);
		if (v20)
		{
			while (1)
			{
				_BitScanForward64((unsigned long*)&v21, v20);
				result = &a1[v21];
				if (!*result)
					return 0LL;
				v22 = a2[2];
				if (!v22)
					return result;
				if (v22 == result[2])
					break;
			LABEL_25:
				v20 &= v20 - 1;
				if (!v20)
					goto LABEL_26;
			}
			v23 = 0LL;
			while (1)
			{
				v24 = a2[v23 + 3];
				if (!v24)
					return result;
				if (v24 != result[++v23 + 2])
					goto LABEL_25;
			}
		}
		goto LABEL_26;
	}
	while (1)
	{
		_BitScanForward64((unsigned long*)&v13, v12);
		result = &a1[v13];
		if (!*result)
			return 0LL;
		v15 = a2[2];
		if (!v15)
			return result;
		if (v15 == result[2])
			break;
	LABEL_14:
		v12 &= v12 - 1;
		if (!v12)
			goto LABEL_15;
	}
	v16 = 0LL;
	while (1)
	{
		v17 = a2[v16 + 3];
		if (!v17)
			return result;
		if (v17 != result[++v16 + 2])
			goto LABEL_14;
	}
}
static  __m128i __m128i_shift_right = _mm_setr_epi8(0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
#define LOBYTE(x)	*reinterpret_cast<char*>(&x)
CS_NOINLINE
IMPL_CODE_SEG
static
const char* __fastcall _strcspn_sse2(const char* a1, const char* a2)
{
	__int64 v2; // rcx
	const char* v4; // rax
	char v6[256]; // [rsp+0h] [rbp-100h] BYREF

	memset(v6, 0, sizeof(v6));
	v2 = 0LL;
	do
	{
		if (!*a2)
			break;
		LOBYTE(v2) = *a2;
		v6[v2] = *a2;
		LOBYTE(v2) = a2[1];
		if (!(char)v2)
			break;
		v6[v2] = v2;
		LOBYTE(v2) = a2[2];
		if (!(char)v2)
			break;
		v6[v2] = v2;
		LOBYTE(v2) = a2[3];
		a2 += 4;
		v6[v2] = v2;
	} while ((char)v2);
	v4 = a1 - 4;
	while (1)
	{
		v4 += 4;
		LOBYTE(v2) = *v4;
		if (v6[v2] == *v4)
			return (const char*)(v4 - a1);
		LOBYTE(v2) = v4[1];
		if (v6[v2] == (char)v2)
			goto LABEL_13;
		LOBYTE(v2) = v4[2];
		if (v6[v2] == (char)v2)
			goto LABEL_12;
		LOBYTE(v2) = v4[3];
		if (v6[v2] == (char)v2)
		{
			++v4;
		LABEL_12:
			++v4;
		LABEL_13:
			++v4;
			return (const char*)(v4 - a1);
		}
	}
}
CS_NOINLINE
IMPL_CODE_SEG
static
const char* __fastcall _strspn_sse2(const char* a1, const char* a2)
{
	__int64 v2; // rcx
	const char* v4; // rax
	char v6[256]; // [rsp+0h] [rbp-100h] BYREF

	memset(v6, 0, sizeof(v6));
	v2 = 0LL;
	do
	{
		if (!*a2)
			break;
		LOBYTE(v2) = *a2;
		v6[v2] = *a2;
		LOBYTE(v2) = a2[1];
		if (!(char)v2)
			break;
		v6[v2] = v2;
		LOBYTE(v2) = a2[2];
		if (!(char)v2)
			break;
		v6[v2] = v2;
		LOBYTE(v2) = a2[3];
		a2 += 4;
		v6[v2] = v2;
	} while ((char)v2);
	v4 = a1 - 4;
	while (1)
	{
		v4 += 4;
		LOBYTE(v2) = *v4;
		if ((*v4 & (unsigned __int8)v6[v2]) == 0)
			return (const char*)(v4 - a1);
		LOBYTE(v2) = v4[1];
		if (((unsigned __int8)v2 & (unsigned __int8)v6[v2]) == 0)
			goto LABEL_13;
		LOBYTE(v2) = v4[2];
		if (((unsigned __int8)v2 & (unsigned __int8)v6[v2]) == 0)
			goto LABEL_12;
		LOBYTE(v2) = v4[3];
		if (((unsigned __int8)v2 & (unsigned __int8)v6[v2]) == 0)
		{
			++v4;
		LABEL_12:
			++v4;
		LABEL_13:
			++v4;
			return (const char*)(v4 - a1);
		}
	}
}
IMPL_CODE_SEG
CS_NOINLINE
static
size_t __fastcall _strspn_sse42(const char* a1, const char* a2)
{
	int v2; // edx
	__m128i v3; // xmm3
	const char* v4; // r8
	int v5; // esi
	const __m128i* v6; // rdi
	__m128i v7; // xmm1
	int v8; // ecx
	size_t result; // rax
	int i; // ecx
	__m128i v11; // xmm1
	int v12; // ecx

	if (!*a2)
		return 0LL;
	v2 = (unsigned __int8)a2 & 0xF;
	if (((unsigned __int8)a2 & 0xF) != 0)
	{
		v3 = _mm_shuffle_epi8(
			_mm_load_si128((const __m128i*)((unsigned __int64)a2 & 0xFFFFFFFFFFFFFFF0LL)),
			_mm_loadu_si128((const __m128i*)((char*)&__m128i_shift_right + ((unsigned __int8)a2 & 0xF))));
		if (16 - v2 != _mm_cmpistri(v3, v3, 58))
			goto LABEL_4;
		v11 = _mm_load_si128((const __m128i*)(((unsigned __int64)a2 & 0xFFFFFFFFFFFFFFF0LL) + 16));
		v12 = _mm_cmpistri(v11, v11, 58);
		if (v12 + 16 - v2 <= 16)
		{
			if (v12)
				v3 = _mm_loadu_si128((const __m128i*)a2);
			goto LABEL_4;
		}
		return (size_t)_strspn_sse2(a1, a2);
	}
	v3 = _mm_load_si128((const __m128i*)a2);
	if (_mm_cmpistri(v3, v3, 58) == 16 && a2[16])
		return (size_t)_strspn_sse2(a1, a2);
LABEL_4:
	v4 = a1;
	v5 = (unsigned __int8)a1 & 0xF;
	if (((unsigned __int8)a1 & 0xF) == 0
		|| (v6 = (const __m128i*)((unsigned __int64)a1 & 0xFFFFFFFFFFFFFFF0LL),
			v7 = _mm_shuffle_epi8(_mm_load_si128(v6), _mm_loadu_si128((const __m128i*)((char*)&__m128i_shift_right + v5))),
			v8 = _mm_cmpistri(v3, v7, 18),
			result = v8,
			16 - v5 <= v8)
		&& (a1 = (const char*)(&v6[1]), 16 - v5 <= _mm_cmpistri(v7, v7, 58)))
	{
		for (i = _mm_cmpistri(v3, *(__m128i*)a1, 18);
			!_mm_cmpistrc(v3, *(__m128i*)a1, 18);
			i = _mm_cmpistri(v3, *(__m128i*)a1, 18))
		{
			a1 += 16;
		}
		result = &a1[i] - v4;
	}
	return result;
}
IMPL_CODE_SEG
CS_NOINLINE
static
unsigned __fastcall _strcspn_sse42(const char* a1, const char* a2)
{
	int v2; // edx
	__m128i v3; // xmm3
	const char* v4; // rsi
	int v5; // er8
	const __m128i* v6; // rdi
	__m128i v7; // xmm1
	int v8; // ecx
	__m128i v10; // xmm1
	int v11; // ecx
	int v12; // edx
	__m128i v13; // xmm1
	int v14; // ecx

//	if (!*a2)
	//	return (unsigned __int64)j_strlen_ifunc();
	v2 = (unsigned __int8)a2 & 0xF;
	if (((unsigned __int8)a2 & 0xF) == 0)
	{
		v3 = _mm_load_si128((const __m128i*)a2);
		if (_mm_cmpistri(v3, v3, 58) != 16 || !a2[16])
			goto LABEL_4;
		return (unsigned __int64)_strcspn_sse2(a1, a2);
	}
	v3 = _mm_shuffle_epi8(
		_mm_load_si128((const __m128i*)((unsigned __int64)a2 & 0xFFFFFFFFFFFFFFF0LL)),
		_mm_loadu_si128((const __m128i*)((const char*)&__m128i_shift_right + ((unsigned __int8)a2 & 0xF))));
	if (16 - v2 != _mm_cmpistri(v3, v3, 58))
		goto LABEL_4;
	v13 = _mm_load_si128((const __m128i*)(((unsigned __int64)a2 & 0xFFFFFFFFFFFFFFF0LL) + 16));
	v14 = _mm_cmpistri(v13, v13, 58);
	if (v14 + 16 - v2 > 16)
		return (unsigned __int64)_strcspn_sse2(a1, a2);
	if (v14)
		v3 = _mm_loadu_si128((const __m128i*)a2);
LABEL_4:
	v4 = a1;
	v5 = (unsigned __int8)a1 & 0xF;
	if (((unsigned __int8)a1 & 0xF) != 0)
	{
		v6 = (const __m128i*)((unsigned __int64)a1 & 0xFFFFFFFFFFFFFFF0LL);
		v7 = _mm_shuffle_epi8(_mm_load_si128(v6), _mm_loadu_si128((__m128i*)((char*)&__m128i_shift_right + v5)));
		v8 = _mm_cmpistri(v3, v7, 2);
		if (_mm_cmpistrc(v3, v7, 2))
			return v8;
		v8 = _mm_cmpistri(v7, v7, 58);
		a1 = //_mm_extract_epi8(v6[1], 1);
			(const char*)(&v6[1]);
		//v6[1].m128i_i8;
		if (16 - v5 > v8)
			return v8;
	}
	while (1)
	{
		v10 = _mm_load_si128((const __m128i*)a1);
		v11 = _mm_cmpistri(v3, v10, 2);
		v12 = _mm_cmpistrz(v3, v10, 2);
		if (_mm_cmpistrc(v3, v10, 2))
			return &a1[v11] - v4;
		if (v12)
			break;
		a1 += 16;
	}
	return &a1[_mm_cmpistri(v10, v10, 58)] - v4;
}
IMPL_CODE_SEG
static
char* __fastcall cs_strtok_r(char* str, const char* delim, char** saveptr)
{
	char* v4; // rbx
	char* v5; // rax
	char* result; // rax

	v4 = str;
	if (!str)
		v4 = *saveptr;
	if (*v4 && (v4 += _strspn_sse42(v4, delim), *v4))
	{
		v5 = &v4[_strcspn_sse42(v4, delim)];
		if (*v5)
			*v5++ = 0;
		*saveptr = v5;
		result = v4;
	}
	else
	{
		*saveptr = v4;
		result = 0LL;
	}
	return result;
}

IMPL_CODE_SEG
static
char* cs_strcpy(char* destbuf, const char* srcbuf) {
	/*
		todo: vectorized version
	*/

	while (*srcbuf) {

		*destbuf = *srcbuf;
		++destbuf;
		++srcbuf;
	}
	*destbuf = 0;
	return destbuf;
}

IMPL_CODE_SEG
static unsigned cs_to_unicode(wchar_t* uniout, const char* asciiin) {

	unsigned i = 0;
	while (true) {
		uniout[i] = asciiin[i];

		if (!uniout[i])
			return i;

		++i;

	}
	cs_assume_m(false);
	return 0;
}

IMPL_CODE_SEG
static
unsigned cs_from_unicode(char* dstbuf, const wchar_t* inbuf) {
	unsigned i = 0;

	while (true) {
		dstbuf[i] = inbuf[i];
		if (!dstbuf[i])
			return i;
		++i;

	}
	cs_assume_m(false);
	return 0;
}
/*

	int (*m_str_to_long)(const char* str, const char** out_endptr, unsigned base);

	int (*m_atoi_fast)(const char* str);
	unsigned (*m_int2str_base10)(unsigned int val, char* dstbuf, unsigned size);
	unsigned(*m_int2str_base16)(unsigned int val, char* dstbuf, unsigned size);
	unsigned (*m_uint2str_base10)(unsigned int val, char* dstbuf, unsigned size);

	//precision default=4
	unsigned (*m_float2str_fast)(float fvalue, char* dstbuf, unsigned precision);
	//out_endpos default = nullptr
	double (*m_fast_atof)(const char* p, const char** out_endpos);
*/


extern "C" unsigned fast_strlen_func_impl_avx2_asm(const char* s);
CS_COLD_CODE
CS_NOINLINE
static void str_algos_init(snaphak_sroutines_t* out_str) {

	g_tentothe[0] = 1.0;
	double currpow10 = 1.0;
#pragma clang loop unroll(disable) vectorize(disable)
	for (unsigned i = 1; i < 32; ++i) {
		currpow10 *= 10.0;
		g_tentothe[i] = currpow10;
	}

	out_str->m_streq = cs_strcmpeq;
	out_str->m_strieq = cs_stricmpeq;
	out_str->m_strlen = fast_strlen_func_impl;
	out_str->m_asm_strlen = fast_strlen_func_impl_avx2_asm;
	out_str->m_hashfnv32 = hashfnv32;
	out_str->m_str_to_long = strconvt_str_to_long;
	out_str->m_atoi_fast = strconvt_atoi_fast;
	out_str->m_int2str_base10 = strconvt_int2str_base10;
	out_str->m_int2str_base16 = strconvt_int2str_base16;
	out_str->m_uint2str_base10 = strconvt_uint2str_base10;
	out_str->m_float2str_fast = strconvt_float2str_fast;
	out_str->m_fast_atof = strconvt_fast_atof;
	out_str->m_strcmp = cs_strcmp;
	out_str->m_strstr = _strstr_sse2_unaligned;
	out_str->m_strchr = _strchr_sse2;
	out_str->m_strcspn = _strcspn_sse42;
	out_str->m_strspn = _strspn_sse42;
	out_str->m_strtok_r = cs_strtok_r;
	out_str->m_strcpy = cs_strcpy;
	out_str->m_to_unicode = cs_to_unicode;
	out_str->m_from_unicode = cs_from_unicode;
}