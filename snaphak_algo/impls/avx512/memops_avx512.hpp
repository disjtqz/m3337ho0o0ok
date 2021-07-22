IMPL_CODE_SEG
static 
void cs_zeromem_nt(void* _to, size_t size) {
	
	char* to = (char*)_to;

	while((reinterpret_cast<uintptr_t>(to)&63) && size != 0) {
		*to = 0;
		--size;
		++to;
	}

	while(size >= 64) {
		_mm512_stream_si512((__m512i*)to, _mm512_setzero_si512());
		size-=64;
		to+=64;
	}


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
	while((reinterpret_cast<uintptr_t>(to)&63) && size != 0) {
		*to = *from;
		--size;
		++to;
		++from;
	}

	while(size >= 64) {
		_mm512_stream_si512((__m512i*)to, _mm512_loadu_si512((const __m512i*)from));
		size-=64;
		from += 64;
		to+=64;
	}


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
	while((reinterpret_cast<uintptr_t>(to)&63) && size != 0) {
		*to = *from;
		--size;
		++to;
		++from;
	}


	

	while(size >= 64) {
		_mm512_stream_si512((__m512i*)to, _mm512_loadu_si512((const __m512i*)from));

		__asm__("clflushopt (%0)"
		: 
		: "r"(from - 64)
			:
			);
		size-=64;
		from += 64;
		to+=64;
	}


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
	while((reinterpret_cast<uintptr_t>(to)&63) && size != 0) {
		*to = *from;
		--size;
		++to;
		++from;
	}


	
	while(size >= 64) {
				_mm512_stream_si512((__m512i*)to, _mm512_loadu_si512((const __m512i*)from));

		__asm__("clwb (%0)"
		: 
		: "r"(from - 64)
			:
			);
		size-=64;
		from += 64;
		to+=64;
	}

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
	while((reinterpret_cast<uintptr_t>(to)&63) && size != 0) {
		*to = *from;
		--size;
		++to;
		++from;
	}


	

	while(size >= 64) {
		_mm512_stream_si512((__m512i*)to, _mm512_loadu_si512((const __m512i*)from));

		_mm_clflush(from - 64);
		size-=64;
		from += 64;
		to+=64;
	}


	while(size) {
		*to = *from;
		++to;
		++from;
		--size;
	}
}