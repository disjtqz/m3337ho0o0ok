#pragma once
struct coro_t;

/*
	when a coro is first called it receives 

	userdata pointer - arg 1

	pointer to self coro - arg 2
	pointer to the coro we were called from - arg 3
*/


//RBX, RBP, RDI, RSI, RSP, R12, R13, R14, R15, and XMM6-15

struct alignas(64) coro_base_t {
	__m128i xmms[10];
	uintptr_t m_rbx;
	uintptr_t m_rbp;
	uintptr_t m_rdi;
	uintptr_t m_rsi;
	uintptr_t m_rsp;
	uintptr_t m_r12;
	uintptr_t m_r13;
	uintptr_t m_r14;
	uintptr_t m_r15;
	uintptr_t m_back_rip;
	uintptr_t m_rax; //return value
	uintptr_t m_rcx; //passes params

	template<typename TStack, typename TFunc, typename TArg>
	void coro_set_up_calling_context(TStack* stackptr, TFunc* fn, TArg arg) {
		m_rsp = (uintptr_t)stackptr;
		m_back_rip = (uintptr_t)(void*)fn;
		m_rcx = (uintptr_t)arg;
	}

	/*uintptr_t yield_to(coro_t* other) {
		return coroswitch(this, other);
	}
	template<typename TYield>
	uintptr_t yield_to(coro_t* other, TYield valto) {
		static_assert(sizeof(TYield) == 8);
		other->m_rax = (uintptr_t)valto;
		return coroswitch(this, other);
	}*/
};

constexpr unsigned corosize = sizeof(coro_base_t);


template<typename TRet, typename TArg>
using corofn_t = TRet (*)(TArg userdata, coro_base_t* self, coro_base_t* caller );


struct snaphak_cororoutines_t {
/*
	when a coro is first called it receives 

	userdata pointer - arg 1

	pointer to self coro - arg 2
	pointer to the coro we were called from - arg 3
*/
	uintptr_t (*m_coroswitch)(coro_base_t* from, coro_base_t* to);
};