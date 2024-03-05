#pragma once
#ifndef SHALGO_DISABLE_INTBULK
constexpr unsigned g_niter_per_element = IMPL_VECTOR_WIDTH / 4;
constexpr unsigned g_niter_per_element16 = IMPL_VECTOR_WIDTH / 2;

using ivec_local_t = unsigned __attribute__((ext_vector_type(g_niter_per_element)));
using bytevec_local_t = unsigned char __attribute__((ext_vector_type(IMPL_VECTOR_WIDTH)));
using ivec16_local_t = unsigned short __attribute__((ext_vector_type(IMPL_VECTOR_WIDTH / 2)));


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

static unsigned ctz_op(unsigned value) {
#if IMPL_VECTOR_WIDTH==16
	assert(false);
	return 0;
	//return  std::countr_zero(value);
#else
	return __builtin_ctz(value);
#endif
}

template<typename T>
static uint64_t cs_movemask_i32_findindex(T val) {
	#if IMPL_VECTOR_WIDTH==64
	return __builtin_ctz(_mm512_movepi32_mask(val));
#elif IMPL_VECTOR_WIDTH==32
	return __builtin_ctz(_mm256_movemask_epi8(val)) / 4;
#else
	return std::countr_zero(_mm_movemask_epi8(val)) / 4;
#endif
}
template<typename T>
static uint64_t cs_movemask_i16_findindex(T val) {
#if IMPL_VECTOR_WIDTH==64
	return __builtin_ctz(_mm512_movepi32_mask(val));
#elif IMPL_VECTOR_WIDTH==32
	
	return __builtin_ctz(_mm256_movemask_epi8(val)) / 2;
#else
	return std::countr_zero(_mm_movemask_epi8(val)) / 2;
#endif
}
IMPL_CODE_SEG
static unsigned find_first_equal16(unsigned short* values, unsigned nvalues, unsigned tofind) {

	assert(false);
	return 69;
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
#if 1
#if IMPL_VECTOR_WIDTH==32
typedef unsigned long long u64x2_t __attribute__((__vector_size__(32)));

IMPL_CODE_SEG
static
unsigned find_indexof_u64(unsigned long long* hashes, unsigned long long findhash) {
	unsigned long long* oldhashes = hashes;

	u64x2_t fhash = u64x2_t{ findhash, findhash,findhash,findhash };

	//#pragma clang loop unroll_count(8)
	u64x2_t msk1;
	u64x2_t msk2;

	while (true) {

		//unsigned long long hval1 = hashes[0];
		//unsigned long long hval2 = hashes[1];
		u64x2_t* hptr = (u64x2_t*)hashes;

		u64x2_t current1 = hptr[0];
		u64x2_t current2 = hptr[1];


		msk1 = current1 == fhash;
		msk2 = current2 == fhash;

		u64x2_t any = (msk1 | msk2);

		if (any[0] | any[1] | any[2] | any[3]) {
			break;
		}
		hashes += 8;
	}
	unsigned delta = hashes - oldhashes;

	u64x2_t sumpart1 = msk1 & u64x2_t{ 0, 1,2,3 };
	u64x2_t sumpart2 = msk2 & u64x2_t{ 4, 5, 6, 7};

	u64x2_t sumfinal = (sumpart1 | sumpart2);

	sumfinal = sumfinal | __builtin_shufflevector(sumfinal, sumfinal, 2, 3, -1, -1);

	sumfinal = sumfinal | __builtin_shufflevector(sumfinal, sumfinal, 1, -1, -1, -1);


	return delta + sumfinal[0];
}
#elif IMPL_VECTOR_WIDTH==16
typedef unsigned long long u64x2_t __attribute__((__vector_size__(16)));

IMPL_CODE_SEG
static
unsigned find_indexof_u64(unsigned long long* hashes, unsigned long long findhash) {
	unsigned long long* oldhashes = hashes;

	u64x2_t fhash = u64x2_t{ findhash, findhash };

	//#pragma clang loop unroll_count(8)
	u64x2_t msk1;
	u64x2_t msk2;

	while (true) {

		//unsigned long long hval1 = hashes[0];
		//unsigned long long hval2 = hashes[1];
		u64x2_t* hptr = (u64x2_t*)hashes;

		u64x2_t current1 = hptr[0];
		u64x2_t current2 = hptr[1];


		msk1 = current1 == fhash;
		msk2 = current2 == fhash;

		u64x2_t any = (msk1 | msk2);

		if (any[0] | any[1]) {
			break;
		}
		hashes += 4;
	}
	unsigned delta = hashes - oldhashes;

	u64x2_t sumpart1 = msk1 & u64x2_t{ 0, 1 };
	u64x2_t sumpart2 = msk2 & u64x2_t{ 2, 3 };

	u64x2_t sumfinal = (sumpart1 | sumpart2);

	sumfinal = sumfinal | __builtin_shufflevector(sumfinal, sumfinal, 1, -1);

	return delta + sumfinal[0];
}

#elif IMPL_VECTOR_WIDTH==64
typedef unsigned long long u64x2_t __attribute__((__vector_size__(64)));

IMPL_CODE_SEG
static
unsigned find_indexof_u64(unsigned long long* hashes, unsigned long long findhash) {
	unsigned long long* oldhashes = hashes;

	u64x2_t fhash = u64x2_t{ findhash, findhash,findhash,findhash,findhash, findhash,findhash,findhash };

	//#pragma clang loop unroll_count(8)
	u64x2_t msk1;
	u64x2_t msk2;

	while (true) {

		//unsigned long long hval1 = hashes[0];
		//unsigned long long hval2 = hashes[1];
		u64x2_t* hptr = (u64x2_t*)hashes;

		u64x2_t current1 = hptr[0];
		u64x2_t current2 = hptr[1];


		msk1 = current1 == fhash;
		msk2 = current2 == fhash;

		u64x2_t any = (msk1 | msk2);

		if (any[0] | any[1] | any[2] | any[3] | any[4] | any[5] | any[6] | any[7]) {
			break;
		}
		hashes += 8;
	}
	unsigned delta = hashes - oldhashes;

	u64x2_t sumpart1 = msk1 & u64x2_t{ 0, 1,2,3,4, 5, 6, 7 };
	u64x2_t sumpart2 = msk2 & u64x2_t{ 8, 9, 10, 11, 12, 13, 14, 15 };

	u64x2_t sumfinal = (sumpart1 | sumpart2);

	sumfinal = sumfinal | __builtin_shufflevector(sumfinal, sumfinal, 4, 5, 6, 7, -1, -1, -1, -1);
	sumfinal = sumfinal | __builtin_shufflevector(sumfinal, sumfinal, 2, 3, -1, -1, -1, -1, -1, -1);

	sumfinal = sumfinal | __builtin_shufflevector(sumfinal, sumfinal, 1, -1, -1, -1, -1, -1, -1, -1);


	return delta + sumfinal[0];
}
#endif
#else
typedef unsigned long long u64x2_t __attribute__((__vector_size__(32)));

IMPL_CODE_SEG
static
unsigned find_indexof_u64(unsigned long long* hashes, unsigned long long findhash) {
	
	__m128i vals;

	__m128i* valptr = (__m128i*)hashes;
	__m128i searcher = _mm_setr_epi64(findhash, findhash);

	while (true) {

		__m128i_mm_loadu_epi64(valptr)

	}
}
#endif
static void ibulkalgos_init(snaphak_ibulkroutines_t* ibulk) {
	ibulk->m_find_first_equal32 = find_first_equal32;
	ibulk->m_find_first_equal16 = find_first_equal16;

	ibulk->m_find_first_notequal32 = find_first_notequal32;
	ibulk->m_addscalar_32 = addscalar_32;
	ibulk->m_subscalar_32 = subscalar_32;
	ibulk->m_mulscalar_32 = mulscalar_32;
	ibulk->m_indexof_u64 = find_indexof_u64;
}
#endif