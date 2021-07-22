#pragma once

using sh_thrdproc_t = int (*)(void*);
typedef struct  {}*snaphak_signal_t;

static constexpr int64_t* snaphak_ms_to_timeout(int timeout, int64_t* scratch) {
	if(!scratch) {
		return nullptr;
	}
	*scratch = static_cast<int64_t>(timeout) * -10000i64;
	return scratch;
}

#define		SNAPHAK_FOREVER	(nullptr)

struct snaphak_smtroutines_t {
	bool (*m_cmpxchg16b)(volatile uint64_t* destination, uint64_t xhigh, uint64_t xlow, uint64_t* cmpres);
	int (*m_xadd32)(volatile int* value, int addend);
	int64_t (*m_xadd64)(volatile int64_t* value, int64_t addend);

	bool (*m_bts64)(volatile uint64_t* m_ptr, unsigned m_bitidx);
	bool (*m_btr64)(volatile uint64_t* m_ptr, unsigned m_bitidx);
	bool (*m_btc64)(volatile uint64_t* m_ptr, unsigned m_bitidx);
	uintptr_t (*m_create_thread)(sh_thrdproc_t function, void* parms, unsigned procnum, size_t stackSize, bool suspended, int* out_threadid);

	void (*m_yield_thread)();
	void (*m_sleep_thread)(size_t ms);


	snaphak_signal_t (*m_create_signal)();
	void (*m_destroy_signal)(snaphak_signal_t);
	void (*m_signal_raise)(snaphak_signal_t sig);
	bool (*m_signal_wait)(snaphak_signal_t sig, int64_t* timeout);
	bool (*m_signal_raise_and_wait)(snaphak_signal_t raise, snaphak_signal_t wait, int64_t* timeout);
};