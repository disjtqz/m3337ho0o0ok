/*
	adding loop unroll disable pragmas/vectorize disable pragmas here because while clang loop unrolling is really good and i'd like to keep it enabled,
	it is too aggressive and often unrolls code that doesnt execute very frequently, like our misaligned pointer paths here. these loop pragmas shave off about 6kb
*/

IMPL_CODE_SEG
static 
void cs_zeromem_nt(void* _to, size_t size) {
	
	char* to = (char*)_to;
	//unfortunately clang will always replace the misaligned paths with calls to memset
	
#pragma clang loop vectorize(disable)
#pragma clang loop unroll(disable)
	while((reinterpret_cast<uintptr_t>(to)&15) && size != 0) {
		*to = 0;
		--size;
		++to;
	}

	while(size >= 16) {
		_mm_stream_si128((__m128i*)to, _mm_setzero_si128());
		size-=16;
		to+=16;
	}
#pragma clang loop vectorize(disable)
#pragma clang loop unroll(disable)
	while(size) {
		*to = 0;
		++to;
		--size;
	}
}
IMPL_CODE_SEG
static 
void cs_movemem_nt(void* _to, const void* _from, size_t size) {
	
	char* to = (char*)_to;
	const char* from = (const char*)_from;
#pragma clang loop vectorize(disable)
#pragma clang loop unroll(disable)
	while((reinterpret_cast<uintptr_t>(to)&15) && size != 0) {
		*to = *from;
		--size;
		++to;
		++from;
	}

	while(size >= 16) {
		_mm_stream_si128((__m128i*)to, _mm_loadu_si128((const __m128i*)from));
		size-=16;
		from += 16;
		to+=16;
	}
#pragma clang loop vectorize(disable)
#pragma clang loop unroll(disable)
	while(size) {
		*to = *from;
		++to;
		++from;
		--size;
	}
}
IMPL_CODE_SEG
static 
void cs_movemem_flush_opt_nt(void* _to, const void* _from, size_t size) {
	
	char* to = (char*)_to;
	const char* from = (const char*)_from;
#pragma clang loop vectorize(disable)
#pragma clang loop unroll(disable)
	while((reinterpret_cast<uintptr_t>(to)&15) && size != 0) {
		*to = *from;
		--size;
		++to;
		++from;
	}


	

	while(size >= 64) {
		_mm_stream_si128((__m128i*)to, _mm_loadu_si128((const __m128i*)from));
		_mm_stream_si128(((__m128i*)to) + 1, _mm_loadu_si128(((const __m128i*)from) + 1));
		_mm_stream_si128(((__m128i*)to) + 2, _mm_loadu_si128(((const __m128i*)from) + 2));
		_mm_stream_si128(((__m128i*)to) + 3, _mm_loadu_si128(((const __m128i*)from) + 3));
		__asm__("clflushopt (%0)"
		: 
		: "r"(from - 64)
			:
			);
		size-=64;
		from += 64;
		to+=64;
	}
#pragma clang loop vectorize(disable)
#pragma clang loop unroll(disable)
	while(size) {
		*to = *from;
		++to;
		++from;
		--size;
	}
}
IMPL_CODE_SEG
static 
void cs_movemem_clean_nt(void* _to, const void* _from, size_t size) {
	
	char* to = (char*)_to;
	const char* from = (const char*)_from;
#pragma clang loop vectorize(disable)
#pragma clang loop unroll(disable)
	while((reinterpret_cast<uintptr_t>(to)&15) && size != 0) {
		*to = *from;
		--size;
		++to;
		++from;
	}


	

	while(size >= 64) {
		_mm_stream_si128((__m128i*)to, _mm_loadu_si128((const __m128i*)from));
		_mm_stream_si128(((__m128i*)to) + 1, _mm_loadu_si128(((const __m128i*)from) + 1));
		_mm_stream_si128(((__m128i*)to) + 2, _mm_loadu_si128(((const __m128i*)from) + 2));
		_mm_stream_si128(((__m128i*)to) + 3, _mm_loadu_si128(((const __m128i*)from) + 3));
		__asm__("clwb (%0)"
		: 
		: "r"(from - 64)
			:
			);
		size-=64;
		from += 64;
		to+=64;
	}

#pragma clang loop vectorize(disable)
#pragma clang loop unroll(disable)
	while(size) {
		*to = *from;
		++to;
		++from;
		--size;
	}
}
IMPL_CODE_SEG
static 
void cs_movemem_flush_noopt_nt(void* _to, const void* _from, size_t size) {
	
	char* to = (char*)_to;
	const char* from = (const char*)_from;
#pragma clang loop vectorize(disable)
#pragma clang loop unroll(disable)
	while((reinterpret_cast<uintptr_t>(to)&15) && size != 0) {
		*to = *from;
		--size;
		++to;
		++from;
	}


	

	while(size >= 64) {
		_mm_stream_si128((__m128i*)to, _mm_loadu_si128((const __m128i*)from));
		_mm_stream_si128(((__m128i*)to) + 1, _mm_loadu_si128(((const __m128i*)from) + 1));
		_mm_stream_si128(((__m128i*)to) + 2, _mm_loadu_si128(((const __m128i*)from) + 2));
		_mm_stream_si128(((__m128i*)to) + 3, _mm_loadu_si128(((const __m128i*)from) + 3));
		_mm_clflush(from - 64);
		size-=64;
		from += 64;
		to+=64;
	}
#pragma clang loop vectorize(disable)
#pragma clang loop unroll(disable)
	while(size) {
		*to = *from;
		++to;
		++from;
		--size;
	}
}