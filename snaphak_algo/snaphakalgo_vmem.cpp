#include "snaphakalgo_predef.hpp"
#include "snaphakalgo.hpp"
#include <Windows.h>

/*
struct snaphak_virtmemroutines_t {
	void* (*m_allocate_rw)(size_t size);
	void (*m_release_rw)(void* mem);
	void (*m_discard_contents)(void* mem, size_t size);
	void (*m_prefetch_for_seq_access)(void* mem, size_t size);
};
*/
SNAPHAK_SHARED_SEG
static 
void* cs_vmem_allocate_rw(size_t size) {
	return VirtualAlloc(nullptr, size, MEM_COMMIT, PAGE_READWRITE);
}
SNAPHAK_SHARED_SEG
static 
void* cs_vmem_allocate_rw_absolute(size_t size, void* where) {
	void* res = VirtualAlloc(where, size, MEM_COMMIT, PAGE_READWRITE);
	if(res !=where)
		return nullptr;
	return res;
}
SNAPHAK_SHARED_SEG
static 
void* cs_vmem_allocate_rwx_absolute(size_t size, void* where) {
	void* res = VirtualAlloc(where, size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if(res !=where)
		return nullptr;
	return res;
}


SNAPHAK_SHARED_SEG
static 
void cs_vmem_release(void* mem) {
	VirtualFree(mem, 0, MEM_RELEASE);
}
SNAPHAK_SHARED_SEG
static 
void cs_vmem_discard(void* mem, size_t size) {
	DiscardVirtualMemory(mem, size);
}

SNAPHAK_SHARED_SEG
static 
void cs_vmem_prefetch(void* mem, size_t size){
	WIN32_MEMORY_RANGE_ENTRY entry{};
	entry.NumberOfBytes = size;
	entry.VirtualAddress = mem;
	
	PrefetchVirtualMemory((HANDLE)-1i64, 1, &entry, 0);
}


void vmemalgos_init(snaphak_virtmemroutines_t* routines) {
	routines->m_allocate_rw = cs_vmem_allocate_rw;
	routines->m_release_rw = cs_vmem_release;
	routines->m_discard_contents = cs_vmem_discard;
	routines->m_prefetch_for_seq_access = cs_vmem_prefetch;
	routines->m_allocate_rwx_absolute = cs_vmem_allocate_rwx_absolute;
	routines->m_allocate_rw_absolute = cs_vmem_allocate_rw_absolute;
}