#pragma once

using snaphak_simdintegral_t = __m128i;

struct snaphak_ibulkroutines_t {
	
	unsigned (*m_find_first_equal32)(unsigned* values, unsigned nvalues, unsigned tofind);
	unsigned (*m_find_first_equal16)(unsigned short* values, unsigned nvalues, unsigned tofind);
	unsigned (*m_find_first_notequal32)(unsigned* values, unsigned nvalues, unsigned tofind);
	void (*m_addscalar_32)(unsigned * values, unsigned nvalues, unsigned addend);
	void (*m_subscalar_32)(unsigned * values, unsigned nvalues, unsigned addend);
	void (*m_mulscalar_32)(unsigned * values, unsigned nvalues, unsigned mult);
	//finds first index of findval in values
	//warning: assumes values are unique. only intended for use in search variable hashes for idtypeinfo
	unsigned (*m_indexof_u64)(unsigned long long* values, unsigned long long findval);

};
