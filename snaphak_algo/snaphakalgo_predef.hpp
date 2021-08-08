#pragma once

#define			CS_NOINLINE		__declspec(noinline)

#define			CS_FORCEINLINE	__forceinline

#define			CS_RESTRICT	
#define		CS_DATA_SEG(name)			__declspec(allocate(name))
#define		CS_CODE_SEG(name)			__declspec(code_seg(name)) 
#define		CS_COLD_CODE				CS_CODE_SEG(".cold")
#if defined(__clang__)

#define		cs_assume_m(...)		if(!(__VA_ARGS__)) __builtin_unreachable()

#else
#define		cs_assume_m(...)		if(!(__VA_ARGS__)) __assume(false)
#endif

#define			cs_offsetof_m(...)		__builtin_offsetof(__VA_ARGS__)

static unsigned long long pair2u32(unsigned low, unsigned high) {
	return static_cast<unsigned long long>(low) | (static_cast<unsigned long long>(high)<<32);
}

static void unpair2u32(unsigned long long paired, unsigned& lo, unsigned& hi) {
	lo = paired;
	hi = paired>>32;
}

#if defined(__clang__)

#define		MH_TRIVIAL_ABI		__attribute__((trivial_abi))		

#define		MH_LEAF		__attribute__((leaf))

#define		MH_DISABLE_STATIC_DCTOR		__attribute__((no_destroy))

#define		MH_NOESCAPE		__attribute__((noescape))

#else
#define		MH_TRIVIAL_ABI			

#define		MH_LEAF		

#define		MH_DISABLE_STATIC_DCTOR		

#define		MH_NOESCAPE		
#endif
#define		MH_NOALIAS		__declspec(noalias)

#define		MH_SMALLLOOP	_Pragma("clang loop unroll(disable) vectorize(disable)")

#define		SHALGO_DISABLE_INTBULK
#define		SHALGO_DISABLE_BITMAP
#define		SHALGO_ONE_SEGMENT

//matches asm version
struct low_gpregs_t {
	unsigned long long m_rax;
	unsigned long long m_rcx;
	unsigned long long  m_rdx;
	unsigned long long  m_rbx;
	unsigned long long  m_rsi;
	unsigned long long  m_rdi;
	unsigned long long  m_rbp;
};

