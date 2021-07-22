#include "snaphakalgo_predef.hpp"
#include "snaphakalgo.hpp"

#define		REALX		*(reinterpret_cast<_Float64x *>(&x))
#define		REALY		*(reinterpret_cast<_Float64x *>(&y))

static inline sh_real_t makeres(long double   input) {
	return *reinterpret_cast<sh_real_t*>(&input);
}

#define		REALSEG		CS_CODE_SEG(".realnum")


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
}