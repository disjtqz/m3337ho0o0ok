#pragma once
// enable various features that are only useful to me
//#define			MH_DEV_BUILD

#include <intrin.h>
#define		MH_FORCEINLINE		__forceinline
#define		MH_CODE_SEG(name)	__declspec(code_seg(name)) 

//for static noinline functions in header files so compiler is aware of what they do unlike with extern at early phases
//but cant bloat our shit
#define		MH_SELECTANY			__declspec(selectany)
#if !defined(__clang__)
#define		MH_NOINLINE			__declspec(noinline)
#define		MH_REGFREE_CALL		
#define		MH_FLATTEN

#define			MH_UNLIKELY_IF(...)			if(__VA_ARGS__)	[[unlikely]]
#define			MH_LIKELY_IF(...)			if(__VA_ARGS__)	[[likely]]

#define			MH_SEMIPURE						
#define			MH_PURE							
#define			MH_NODUPLICATE	
#else
#define		MH_NOINLINE			__attribute__((noinline))
#define		MH_FLATTEN			__attribute__((flatten))

//prevents func call duplication
#define			MH_NODUPLICATE	__attribute__((noduplicate))
/*
	call that preserves all registers.
	use for rarely called functions and the like (initializing an offset from typeinfo system ?)
*/
#define		MH_REGFREE_CALL			__attribute__((no_caller_saved_registers)) __attribute__((disable_tail_calls))  

#define			MH_UNLIKELY_IF(...)			if(__builtin_expect(!!(__VA_ARGS__), false))
#define			MH_LIKELY_IF(...)			if(__builtin_expect(!!(__VA_ARGS__), true))

#define			MH_SEMIPURE						__attribute__((pure)) 
#define			MH_PURE							__attribute__((const))
#endif

#define		mh_assume_m(...)			if(!(__VA_ARGS__)) __assume(false)


#define		MH_HEADER_NOINLINE		  MH_NOINLINE


template<typename T>
concept mh_ptr_sized_c = sizeof(T) == sizeof(void*);

/*
	pointer arithmetic helper
*/
template<typename TTo, mh_ptr_sized_c TFrom>
static inline TTo* mh_lea(TFrom shouldbptr, ptrdiff_t displacement) {
	return (TTo*)(&((char*)shouldbptr)[displacement]);
}

