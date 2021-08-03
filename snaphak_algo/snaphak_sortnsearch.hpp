#pragma once

struct snaphak_snsroutines_t {
	void (*m_sort4ptr)(void** ptrs);
	//void (*m_sort16ptr_with_scratch)(void** ptrs, void** scratchbuf);
	void (*m_sort16ptr_unrolled)(void** ptrs);
	void (*m_sort16ptr)(void* ptrs[8]);
};