#pragma once


struct snaphak_sroutines_t {
	bool (*m_streq)(const char* s1, const char* s2);
	bool (*m_strieq)(const char* s1, const char* s2);
	unsigned (*m_strlen)(const char* s1);
	unsigned (*m_hashfnv32)(const char* s1, unsigned length);


	int (*m_str_to_long)(const char* str, const char** out_endptr, unsigned base);

	int (*m_atoi_fast)(const char* str);
	unsigned (*m_int2str_base10)(unsigned int val, char* dstbuf, unsigned size);
	unsigned(*m_int2str_base16)(unsigned int val, char* dstbuf, unsigned size);
	unsigned (*m_uint2str_base10)(unsigned int val, char* dstbuf, unsigned size);

	//precision default=4
	unsigned (*m_float2str_fast)(float fvalue, char* dstbuf, unsigned precision);
	//out_endpos default = nullptr
	double (*m_fast_atof)(const char* p, const char** out_endpos);

	int (*m_strcmp)(const char* s1, const char* s2);

	const char* (*m_strstr)(const char* s, const char* fnd);
	const char* (*m_strchr)(const char* s, char fnd);

	unsigned (*m_strcspn)(const char* s, const char* spanner);
	size_t (*m_strspn)(const char* s, const char* s2);

	char* (*m_strtok_r)(char* str, const char* delim, char** saveptr);
};