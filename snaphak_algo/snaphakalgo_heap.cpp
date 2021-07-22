#include "snaphakalgo_predef.hpp"
#include "snaphakalgo.hpp"
#include <Windows.h>

static void* g_rtl_createheap = nullptr;


static unsigned heapflags_to_winheap_flags(unsigned flags) {
	unsigned res_flags = 0;
	if(flags & sh::heap::_heapflag_no_serialize) {
		res_flags |= HEAP_NO_SERIALIZE;
	}
	return res_flags;
}
struct RTL_HEAP_PARAMETERS {
	ULONG Length;
	SIZE_T SegmentReserve;
	SIZE_T SegmentCommit;
	SIZE_T DeCommitFreeBlockThreshold;
	SIZE_T DeCommitTotalFreeThreshold;
	SIZE_T MaximumAllocationSize;
	SIZE_T VirtualMemoryThreshold;
	SIZE_T InitialCommit;
	SIZE_T InitialReserve;
	void* CommitRoutine;
	SIZE_T Reserved[2];
};
CS_COLD_CODE
static
sh_heap_t create_heap_from_mem(void* mem, size_t memsize, unsigned flags ) {
	
	RTL_HEAP_PARAMETERS params;
	memset(&params, 0, sizeof(params));
	params.Length = sizeof(params);
	auto heapfn = reinterpret_cast<PVOID(*)(
		ULONG                Flags,
		PVOID                HeapBase,
		SIZE_T               ReserveSize,
		SIZE_T               CommitSize,
		PVOID                Lock,
		RTL_HEAP_PARAMETERS * Parameters
		)>(g_rtl_createheap);
	

	return  reinterpret_cast<sh_heap_t>(heapfn(heapflags_to_winheap_flags(flags), mem, memsize, memsize, nullptr, &params));
}

/*
	void (*m_destroy_heap)(sh_heap_t heap);

	void* (*m_alloc_from_heap)(sh_heap_t heap, size_t nbytes, unsigned heapflags);
	void (*m_free_from_heap)(sh_heap_t heap, void* mem, unsigned heapflags);

	void* (*m_realloc_from_heap)(sh_heap_t heap, void* oldmem, size_t nbytes_new, unsigned heapflags);
*/

CS_COLD_CODE
static void destroy_heap(sh_heap_t heap) {
	HeapDestroy((HANDLE)heap);
}
SNAPHAK_SHARED_SEG
static void* alloc_from_heap(sh_heap_t heap, size_t nbytes, unsigned heapflags) {
	return HeapAlloc((HANDLE)heap, heapflags_to_winheap_flags(heapflags), nbytes);
}
SNAPHAK_SHARED_SEG
static void free_from_heap(sh_heap_t heap, void* mem, unsigned heapflags) {
	HeapFree((HANDLE)heap, heapflags_to_winheap_flags(heapflags), mem);
}
SNAPHAK_SHARED_SEG
static void* realloc_from_heap(sh_heap_t heap, void* oldmem, size_t nbytes_new, unsigned heapflags) {
	return HeapReAlloc((HANDLE)heap, heapflags_to_winheap_flags(heapflags), oldmem, nbytes_new);
}
SNAPHAK_SHARED_SEG
static
bool _lock_heap(sh_heap_t heap){
	return HeapLock((HANDLE)heap);
}
SNAPHAK_SHARED_SEG
static
bool unlock_heap(sh_heap_t heap){
	return HeapUnlock((HANDLE)heap);
}

void heapalgos_init(snaphak_heaproutines_t* routines) {
	g_rtl_createheap = GetProcAddress(GetModuleHandleA("ntdll.dll"), "RtlCreateHeap");

	routines->m_create_heap_from_mem = create_heap_from_mem;
	routines->m_destroy_heap = destroy_heap;
	routines->m_alloc_from_heap = alloc_from_heap;
	routines->m_free_from_heap = free_from_heap;
	routines->m_realloc_from_heap = realloc_from_heap;
	routines->m_lock_heap = _lock_heap;
	routines->m_unlock_heap = unlock_heap;


}