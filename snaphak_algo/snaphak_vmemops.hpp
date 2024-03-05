
#define		SNAPHAK_SIZE_FLAG_HUGEPAGES		(1ULL << 62)


struct snaphak_virtmemroutines_t {
	void* (*m_allocate_rw)(size_t size);
	void* (*m_allocate_rwx)(size_t size);
	void* (*m_allocate_rwx_absolute)(size_t size,void* where);
	void* (*m_allocate_rw_absolute)(size_t size,void* where);
	void (*m_release_rw)(void* mem);
	void (*m_discard_contents)(void* mem, size_t size);
	void (*m_prefetch_for_seq_access)(void* mem, size_t size);
	//for engine instrumentation purposes only, we dont need hugepages for normal user releases that dont collect info on the engine
	int (*m_try_enable_hugepages)();
};