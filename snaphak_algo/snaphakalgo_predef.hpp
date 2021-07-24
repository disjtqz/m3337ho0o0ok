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