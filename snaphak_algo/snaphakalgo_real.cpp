#include "snaphakalgo_predef.hpp"
#include "snaphakalgo.hpp"

//putting these in a seperate seg is wasteful
//#define		REALSEG		CS_CODE_SEG(".realnum")
#define		REALSEG

REALSEG
static sh_real_t cs_realadd(sh_real_t x, sh_real_t y) {

	sh_real_t result;

	__asm {
	    fld     x
        fld     y
        faddp   st(1), st
        fstp result
	}
	return result;
}

REALSEG
static sh_real_t cs_realsub(sh_real_t x, sh_real_t y) {

	sh_real_t result;

	__asm {
	    fld     x
        fld     y
        fsubp   st(1), st
        fstp result
	}
	return result;
}
REALSEG
static sh_real_t cs_realmul(sh_real_t x, sh_real_t y) {

	sh_real_t result;

	__asm {
	    fld     x
        fld     y
        fmulp   st(1), st
        fstp result
	}
	return result;
}

REALSEG
static sh_real_t cs_realdiv(sh_real_t x, sh_real_t y) {

	sh_real_t result;

	__asm {
	    fld     x
        fld     y
        fdivp   st(1), st
        fstp result
	}
	return result;
}
REALSEG
static sh_real_t cs_realsqrt(sh_real_t x) {

	sh_real_t result;

	__asm {
	    fld     x
        fsqrt
        fstp result
	}
	return result;
}

REALSEG
static sh_real_t cs_realneg(sh_real_t x) {

	sh_real_t result;

	__asm {
	    fld     x
        fchs
        fstp result
	}
	return result;
}
REALSEG
static 
sh_real_t cs_real_from_double(double input) {
	double stackval = input;
	sh_real_t result;
	__asm {
		fld qword ptr stackval
		fstp tbyte ptr result
	}
	return result;
}
REALSEG
static 
sh_real_t cs_real_from_i16(int16_t input) {
	int16_t stackval = input;
	sh_real_t result;
	__asm {
		fild word ptr stackval
		fstp tbyte ptr result
	}
	return result;
}
REALSEG
static 
sh_real_t cs_real_from_i32(int32_t input) {
	int32_t stackval = input;
	sh_real_t result;
	__asm {
		fild dword ptr stackval
		fstp tbyte ptr result
	}
	return result;
}
REALSEG
static 
sh_real_t cs_real_from_i64(int64_t input) {
	int64_t stackval = input;
	sh_real_t result;
	__asm {
		fild qword ptr stackval
		fstp tbyte ptr result
	}
	return result;
}

/*
	sh_real_t (*m_real_from_i16)(int64_t ival);
	sh_real_t (*m_real_from_i32)(int32_t ival);
	sh_real_t (*m_real_from_i64)(int64_t ival);*/
REALSEG
static double cs_double_from_real(sh_real_t realnum) {
	double res;
	__asm {
		fld tbyte ptr realnum
		fstp qword ptr res
	}
	return res;
}

REALSEG
static float cs_flt_from_real(sh_real_t realnum) {
	float res;
	__asm {
		fld tbyte ptr realnum
		fstp dword ptr res
	}
	return res;
}

REALSEG
static void cs_realsincos(sh_real_t val,  sh_real_t* out_sin, sh_real_t* out_cos) {
	__asm {
		fld tbyte ptr val
		fsincos
		mov rax, qword ptr[out_cos]

		fstp tbyte ptr [rax]
		mov rax, qword ptr[out_sin]
		fstp tbyte ptr [rax]


	}
}


REALSEG
static  
sh_real_t cs_atan2 (sh_real_t iny, sh_real_t inx) {
	sh_real_t res;

	__asm {
		fld tbyte ptr [iny]
		fld tbyte ptr [inx]
		fpatan
		fstp tbyte ptr [res]

	}
	return res;
}

REALSEG
static  
sh_real_t cs_floorl(sh_real_t v){
	sh_real_t res;
	unsigned short tmpword;
	unsigned short tmpword2;
	__asm {
        fnstcw  WORD PTR tmpword
        fld     TBYTE PTR v
        movzx   eax, WORD PTR tmpword
        and     ah, -13
        or      ah, 4
        mov     WORD PTR tmpword2, ax
        fldcw   WORD PTR tmpword2
        frndint
        fldcw   WORD PTR tmpword
        fstp    TBYTE PTR [res]
	}
	return res;
}
/*
https://godbolt.org/z/o16x6xMK8

*/


static constexpr const unsigned LC2 = 1069547520;

static constexpr const unsigned LCX = 2;

REALSEG
__attribute__((naked))
static double cs_precise_normalize(double* values) {
	__asm {
		fld     QWORD PTR[rcx]
		fld     QWORD PTR[rcx + 8]
		fld     QWORD PTR[rcx + 16]
		fld     st(2)
		fmul    st, st(3)
		fld     st(2)
		fmul    st, st(3)
		faddp   st(1), st
		fld     st(1)
		fmul    st, st(2)
		faddp   st(1), st
		fld1
		fld     st(1)
		fsqrt
		//fdivr   QWORD PTR LC0
		
		fdiv
		fld     st(0)
		fmul    st, st(1)
		fld     st(2)
		//fmul    DWORD PTR LC1
		fidiv dword ptr LCX
		fmulp   st(1), st
		fsubr   DWORD PTR LC2
		fmulp   st(1), st
		fmul    st(1), st
		fxch    st(1)
#if 0
		fstp    QWORD PTR[rcx]
		fmul    st(3), st
		fxch    st(3)
		movlps  xmm0, QWORD PTR[rcx]
#else
		fstp    QWORD PTR[rsp-8]
		fmul    st(3), st
		fxch    st(3)
		movlps  xmm0, QWORD PTR[rsp-8]
#endif
		fstp    QWORD PTR[rcx]
		fxch    st(1)
		fmul    st, st(2)
		fstp    QWORD PTR[rcx + 8]
		fmulp   st(1), st
		fstp    QWORD PTR[rcx + 16]
		ret

	}
}

//https://godbolt.org/z/eT3Y9EadP
REALSEG
__attribute__((naked))
double cs_precise_distance3d(double* vx, double* vy) {
	__asm {
		//modified to remove stack frame
		//sub     rsp, 24
		fld     QWORD PTR[rdx]
		fsubr   QWORD PTR[rcx]
		fld     QWORD PTR[rdx + 8]
		fsubr   QWORD PTR[rcx + 8]
		fld     QWORD PTR[rdx + 16]
		fsubr   QWORD PTR[rcx + 16]
		fxch    st(2)
		fmul    st, st(0)
		fxch    st(1)
		fmul    st, st(0)
		faddp   st(1), st
		fxch    st(1)
		fmul    st, st(0)
		faddp   st(1), st
		fsqrt
		fstp    QWORD PTR[rsp - 8]
		movlps  xmm0, QWORD PTR[rsp - 8]
		//add     rsp, 24
		ret
	}
}

void realnum_init(snaphak_realroutines_t* algo) {
	algo->m_add_real = cs_realadd;
	algo->m_sub_real = cs_realsub;
	algo->m_mul_real = cs_realmul;
	algo->m_div_real = cs_realdiv;
	algo->m_sqrt_real = cs_realsqrt;
	algo->m_neg_real = cs_realneg;
	algo->m_real_from_double = cs_real_from_double;
	algo->m_double_from_real = cs_double_from_real;
	algo->m_float_from_real = cs_flt_from_real;
	algo->m_real_from_i16 = cs_real_from_i16;
	algo->m_real_from_i32 = cs_real_from_i32;
	algo->m_real_from_i64 = cs_real_from_i64;
	algo->m_sincos=cs_realsincos;
	algo->m_atan2 = cs_atan2;
	algo->m_floor = cs_floorl;
	algo->m_precise_normalize = cs_precise_normalize;
	algo->m_precise_distance3d = cs_precise_distance3d;
}