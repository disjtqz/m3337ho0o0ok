#pragma once
/*
	routines for using 80-bit floating point
*/
struct sh_real_t {
	char m_bytes[10];
};

struct snaphak_realroutines_t {
	sh_real_t (*m_add_real)(sh_real_t x, sh_real_t y);
	sh_real_t (*m_sub_real)(sh_real_t x, sh_real_t y);
	sh_real_t (*m_mul_real)(sh_real_t x, sh_real_t y);
	sh_real_t (*m_div_real)(sh_real_t x, sh_real_t y);
	sh_real_t (*m_sqrt_real)(sh_real_t x);
	sh_real_t (*m_neg_real)(sh_real_t x);
	sh_real_t (*m_real_from_double)(double input);	
	double (*m_double_from_real)(sh_real_t x);
	float (*m_float_from_real)(sh_real_t x);

	sh_real_t (*m_floor)(sh_real_t x);

	sh_real_t (*m_real_from_i16)(int16_t ival);
	sh_real_t (*m_real_from_i32)(int32_t ival);
	sh_real_t (*m_real_from_i64)(int64_t ival);

	void (*m_sincos)(sh_real_t value, sh_real_t* out_sin, sh_real_t* out_cos);
	sh_real_t (*m_atan2)(sh_real_t iny, sh_real_t inx);
};