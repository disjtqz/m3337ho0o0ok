#pragma once

enum class memcpy_strategy_e {
	STANDARD_MEMCPY = 0,
	NT_COPY = 1,
	NT_COPY_FLUSH = 2,
	NT_COPY_CLEAN = 3,
	SMOL_CPY = 4
};


struct generalized_ifield_t {
	struct {
		uint32_t m_offset : 24;
		uint32_t m_dtype : 2; //0 = uint8, 1 = uint16, 2 = uint32, 3 = uint64
		uint32_t m_signed_type : 1;
	};
};
struct snaphak_memroutines_t {
	//does not fence
	void (*m_ntmovemem)(void* to, const void* from, size_t size);

	void (*m_movemem_flush)(void* to, const void* from, size_t size);
	void (*m_movemem_clean)(void* to, const void* from, size_t size);
	//does not fence
	void (*m_ntzeromem)(void* to, size_t size);
	//returns true if sfence needed
	bool (*m_flushcache)(const void* mem, size_t size);
	//clean a region from the cache (keep it in cache but no longer dirty, so when evicted does not need to be written back)
	bool (*m_cleancache)(const void* mem, size_t size);

	void (*m_smol_memzero)( void* mem, size_t size);
	void (*m_smol_memcpy)(void* to, const void* from, size_t size);


};


