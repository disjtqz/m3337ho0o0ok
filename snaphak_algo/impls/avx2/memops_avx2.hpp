IMPL_CODE_SEG
static 
void cs_zeromem_nt(void* _to, size_t size) {
	
	char* to = (char*)_to;
#pragma clang loop vectorize(disable)
#pragma clang loop unroll(disable)
	while((reinterpret_cast<uintptr_t>(to)&31) && size != 0) {
		*to = 0;
		--size;
		++to;
	}

	while(size >= 32) {
		_mm256_stream_si256((__m256i*)to, _mm256_setzero_si256());
		size-=32;
		to+=32;
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
	while((reinterpret_cast<uintptr_t>(to)&31) && size != 0) {
		*to = *from;
		--size;
		++to;
		++from;
	}

	while(size >= 32) {
		_mm256_stream_si256((__m256i*)to, _mm256_loadu_si256((const __m256i*)from));
		size-=32;
		from += 32;
		to+=32;
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
	while((reinterpret_cast<uintptr_t>(to)&31) && size != 0) {
		*to = *from;
		--size;
		++to;
		++from;
	}


	

	while(size >= 64) {
		_mm256_stream_si256((__m256i*)to, _mm256_loadu_si256((const __m256i*)from));
		_mm256_stream_si256(((__m256i*)to) + 1, _mm256_loadu_si256(((const __m256i*)from) +1));
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
	while((reinterpret_cast<uintptr_t>(to)&31) && size != 0) {
		*to = *from;
		--size;
		++to;
		++from;
	}


	
	while(size >= 64) {
		_mm256_stream_si256((__m256i*)to, _mm256_loadu_si256((const __m256i*)from));
		_mm256_stream_si256(((__m256i*)to) + 1, _mm256_loadu_si256(((const __m256i*)from) +1));
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
	while((reinterpret_cast<uintptr_t>(to)&31) && size != 0) {
		*to = *from;
		--size;
		++to;
		++from;
	}


	

	while(size >= 64) {
		_mm256_stream_si256((__m256i*)to, _mm256_loadu_si256((const __m256i*)from));
		_mm256_stream_si256(((__m256i*)to) + 1, _mm256_loadu_si256(((const __m256i*)from) +1));
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