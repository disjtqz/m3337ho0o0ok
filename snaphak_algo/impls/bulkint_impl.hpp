#pragma once
#ifndef SHALGO_DISABLE_INTBULK
constexpr unsigned g_niter_per_element = IMPL_VECTOR_WIDTH / 4;
using ivec_local_t = unsigned __attribute__((ext_vector_type(g_niter_per_element)));
using bytevec_local_t = unsigned char __attribute__((ext_vector_type(IMPL_VECTOR_WIDTH)));

template<typename T>
static uint64_t cs_movemask(T val) {
#if IMPL_VECTOR_WIDTH==64
	return _mm512_movepi8_mask(val);
#elif IMPL_VECTOR_WIDTH==32
	return _mm256_movemask_epi8(val);
#else
	return _mm_movemask_epi8(val);
#endif
}

template<typename T>
static uint64_t cs_movemask_i32_findindex(T val) {
	#if IMPL_VECTOR_WIDTH==64
	return __builtin_ctz(_mm512_movepi32_mask(val));
#elif IMPL_VECTOR_WIDTH==32
	return __builtin_ctz(_mm256_movemask_epi8(val)) / 4;
#else
	return __builtin_ctz(_mm_movemask_epi8(val)) / 4;
#endif
}

IMPL_CODE_SEG
static unsigned find_first_equal32(unsigned* values, unsigned nvalues, unsigned tofind) {

	


	unsigned i = 0;


	for (; (i + g_niter_per_element) < nvalues; i += g_niter_per_element) {
		ivec_local_t tocmp = *reinterpret_cast<ivec_local_t*>(&values[i]);
		auto mask = tocmp == tofind;

		unsigned realmask = cs_movemask(mask);

		if(realmask) {
			return i + (__builtin_ctz(realmask) / 4);
		}

	}
#pragma clang loop unroll(disable) vectorize(disable)

	for (; i < nvalues; ++i) {
		if (values[i] == tofind) {
			return i;
		}
	}
	return ~0u;
}
IMPL_CODE_SEG
static unsigned find_first_notequal32(unsigned* values, unsigned nvalues, unsigned tofind) {

	


	unsigned i = 0;

	for (; (i + g_niter_per_element) < nvalues; i += g_niter_per_element) {
		ivec_local_t tocmp = *reinterpret_cast<ivec_local_t*>(&values[i]);
		auto mask = tocmp != tofind;

		unsigned realmask = cs_movemask(mask);

		if(realmask) {
			return i + (__builtin_ctz(realmask) / 4);
		}

	}
#pragma clang loop unroll(disable) vectorize(disable)

	for (; i < nvalues; ++i) {
		if (values[i] == tofind) {
			return i;
		}
	}
	return ~0u;
}
IMPL_CODE_SEG
static 
void addscalar_32(unsigned * values, unsigned nvalues, unsigned addend) {

	unsigned i = 0;

	for (; (i + g_niter_per_element) < nvalues; i += g_niter_per_element) {
		*reinterpret_cast<ivec_local_t*>(&values[i]) += addend;
	}
#pragma clang loop unroll(disable) vectorize(disable)

	for (; i < nvalues; ++i) {
		/*if (values[i] == tofind) {
			return i;
		}*/
		values[i] += addend;
	}
}

IMPL_CODE_SEG
static 
void subscalar_32(unsigned * values, unsigned nvalues, unsigned addend) {

	unsigned i = 0;

	for (; (i + g_niter_per_element) < nvalues; i += g_niter_per_element) {
		*reinterpret_cast<ivec_local_t*>(&values[i]) -= addend;
	}
#pragma clang loop unroll(disable) vectorize(disable)
	for (; i < nvalues; ++i) {
		/*if (values[i] == tofind) {
			return i;
		}*/
		values[i] += addend;
	}
}

IMPL_CODE_SEG
static 
void mulscalar_32(unsigned * values, unsigned nvalues, unsigned addend) {

	unsigned i = 0;

	for (; (i + g_niter_per_element) < nvalues; i += g_niter_per_element) {
		*reinterpret_cast<ivec_local_t*>(&values[i]) *= addend;
	}
#pragma clang loop unroll(disable) vectorize(disable)
	for (; i < nvalues; ++i) {
		/*if (values[i] == tofind) {
			return i;
		}*/
		values[i] += addend;
	}
}
static void ibulkalgos_init(snaphak_ibulkroutines_t* ibulk) {
	ibulk->m_find_first_equal32 = find_first_equal32;
	ibulk->m_find_first_notequal32 = find_first_notequal32;
	ibulk->m_addscalar_32 = addscalar_32;
	ibulk->m_subscalar_32 = subscalar_32;
	ibulk->m_mulscalar_32 = mulscalar_32;
}
#endif