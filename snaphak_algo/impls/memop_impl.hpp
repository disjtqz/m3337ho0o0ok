

#if defined(__AVX512F__)
#include "avx512/memops_avx512.hpp"
#elif defined(__AVX2__)
#include "avx2/memops_avx2.hpp"
#else
#include "generic/memops_generic.hpp"
#endif

__attribute__((target("clflushopt")))
static 
bool flushcache(const void* mem, size_t size) {
	
	const char* memmem = (const char*)mem;

	uintptr_t delt = (64 - (reinterpret_cast<uintptr_t>(memmem) & 0x63)) & 0x63;

	if(size < delt)
		return false;


	size -= delt;
	memmem += delt;


	if(size < 64)
		return false;

	
	while(size >= 64) {
		__asm__("clflushopt (%0)"
		: 
		: "r"(memmem)
			:
			);
		size -= 64;
		memmem += 64;
	}
	return true;
	

}
static 
bool flushcache_noopt(const void* mem, size_t size) {
	
	const char* memmem = (const char*)mem;

	/*while(!!(reinterpret_cast<uintptr_t>(memmem) & 0x63) && size) {
		++memmem;
		--size;
	}*/

	uintptr_t delt = (64 - (reinterpret_cast<uintptr_t>(memmem) & 0x63)) & 0x63;

	if(size < delt)
		return false;


	size -= delt;
	memmem += delt;



	if(size < 64)
		return false;

	
	while(size >= 64) {
		__asm__("clflush (%0)"
		: 
		: "r"(memmem)
			:
			);
		size -= 64;
		memmem += 64;
	}
	return false;
	

}
__attribute__((target("clflushopt")))
static 
bool writebackcache(const void* mem, size_t size) {
	
	const char* memmem = (const char*)mem;

	uintptr_t delt = (64 - (reinterpret_cast<uintptr_t>(memmem) & 0x63)) & 0x63;

	if(size < delt)
		return false;


	size -= delt;
	memmem += delt;


	if(size < 64)
		return false;

	
	while(size >= 64) {
		
		__asm__("clwb (%0)"
		: 
		: "r"(memmem)
			:
			);
		size -= 64;
		memmem += 64;
	}
	return true;
	

}

static 
void smolmemzero_fast_small_stosb(void* mem, size_t size){
//	__stosb(mem, 0, size);
	  __asm__("rep stosb" : : "D"(mem), "a"(0), "c"(size));

}
static 
void smolmemzero_no_fast_small_stosb(void* mem, size_t size){
	//__stosb(mem, 0, size);
	memset(mem, 0, size);
}

static void smolmemcpy_fast_small_movsb(void* mem, const void* from, size_t size) {
	__asm__("rep movsb" : : "D"(mem), "S"(from), "c"(size));
	//__movsb(mem, from, size);
}

static void smolmemcpy_no_fast_small_movsb(void* mem, const void* from, size_t size) {
	memcpy(mem, from, size);
}

static 
bool writebackcache_unsupported(const void* mem, size_t size)  {
	return false;
}
CS_COLD_CODE
CS_NOINLINE
static void memop_algos_init(snaphak_algo_t* out_m) {
	
	out_m->m_memroutines.m_ntzeromem = cs_zeromem_nt;
	out_m->m_memroutines.m_ntmovemem = cs_movemem_nt;

	if(out_m->m_has_clflushopt) {
		out_m->m_memroutines.m_flushcache = flushcache;
		out_m->m_memroutines.m_cleancache = writebackcache;
		out_m->m_memroutines.m_movemem_flush = cs_movemem_flush_opt_nt;
		out_m->m_memroutines.m_movemem_clean = cs_movemem_clean_nt;

	}
	else {
		out_m->m_memroutines.m_flushcache = flushcache_noopt;
		out_m->m_memroutines.m_cleancache = writebackcache_unsupported;
		out_m->m_memroutines.m_movemem_flush = cs_movemem_flush_noopt_nt;
		out_m->m_memroutines.m_movemem_clean = cs_movemem_nt;
	}

	if(out_m->m_has_fast_shortrep) {
		out_m->m_memroutines.m_smol_memzero = smolmemzero_fast_small_stosb;
		out_m->m_memroutines.m_smol_memcpy = smolmemcpy_fast_small_movsb;
	}
	else {
		out_m->m_memroutines.m_smol_memzero = smolmemzero_no_fast_small_stosb;
		out_m->m_memroutines.m_smol_memcpy = smolmemcpy_no_fast_small_movsb;		
	}

}