#pragma once

struct snaphak_allocator_procs_t {
	void* (*m_allocate)(void* ud, size_t size);
	void (*m_free)(void* ud, void* mem, size_t memsize);
	void* (*m_reallocate)(void* ud, void* mem,  size_t newsize,size_t oldsize);
	void (*m_bulk_free)(void* ud, void** mems, size_t* sizes, size_t nentries);

	bool (*m_lock_access)(void* ud);
	bool (*m_unlock_access)(void* ud);
};


struct snaphak_allocator_t {
	snaphak_allocator_procs_t* m_alprocs;
	void* m_ud;

	void* allocate(size_t size) {
		return m_alprocs->m_allocate(m_ud, size);
	
	}

	void free(void* mem, size_t memsize = 0) {
		m_alprocs->m_free(m_ud, mem, memsize);
	}
	void *reallocate(void* mem, size_t newsize,size_t oldsize = 0) {
		return m_alprocs->m_reallocate(m_ud, mem, newsize,oldsize);
	}
	void bulk_free(void** mems, size_t nentries, size_t* sizes = nullptr) {
		if(m_alprocs->m_bulk_free) {
			m_alprocs->m_bulk_free(m_ud, mems, sizes, nentries);
		
		}
		else {
			if(sizes) {
				for(unsigned i = 0; i < nentries; ++i) {
					m_alprocs->m_free(m_ud, mems[i], sizes[i]);
				}
			}
			else {
				for(unsigned i = 0; i < nentries; ++i) {
					m_alprocs->m_free(m_ud, mems[i], 0);
				}				
			}
		}
	}
	bool lock() {
		if(m_alprocs->m_lock_access)
			return m_alprocs->m_lock_access(m_ud);
		else
			return true;
	}
	bool unlock() {
		if(m_alprocs->m_unlock_access)
			return m_alprocs->m_unlock_access(m_ud);
		else
			return true;
	}
};
