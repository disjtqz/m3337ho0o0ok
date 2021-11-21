#pragma once
/*typedef struct {

}* sh_heap_t;
*/

using sh_heap_t = void*;

#if defined(SNAPHAKALGO_BUILDING)
namespace sh::heap {
#include "sh_heap_shared.hpp"
}
#endif

struct snaphak_heaproutines_t {
	sh_heap_t (*m_create_heap_from_mem)(void* mem, size_t memsize, unsigned heapflags);
	void (*m_destroy_heap)(sh_heap_t heap);

	void* (*m_alloc_from_heap)(sh_heap_t heap, size_t nbytes, unsigned heapflags);
	void (*m_free_from_heap)(sh_heap_t heap, void* mem, unsigned heapflags);

	void* (*m_realloc_from_heap)(sh_heap_t heap, void* oldmem, size_t nbytes_new, unsigned heapflags);
	bool (*m_lock_heap)(sh_heap_t heap);
	bool (*m_unlock_heap)(sh_heap_t heap);

};
