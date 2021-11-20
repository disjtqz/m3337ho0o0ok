#pragma once


/*
	this is taken from a fork of rbdoom3bfg that ive been working on, on and off, for about a year and a half.
	the majority of id's original math stuff is replaced with vectorized sse/avx code. i copied and pasted in some of the non avx code here, 
	as doom eternal only requires sse4.1 to run so some users may not have avx


*/

//agners vectorlib is used in some places here for emulating
//avx2/avx intrinsics that arent available in sse4.1
#include "agner-vector/vectorclass.h"
#define		ID_FORCE_INLINE		CS_FORCEINLINE
#define		CS_SUPER_PURE		MH_PURE
using vec3_simd_t = __m128;

using plane_simd_t = __m128;
//plane.h defs
#define	ON_EPSILON					0.1f
#define DEGENERATE_DIST_EPSILON		1e-4f

#define	SIDE_FRONT					0
#define	SIDE_BACK					1
#define	SIDE_ON						2
#define	SIDE_CROSS					3

// plane sides
#define PLANESIDE_FRONT				0
#define PLANESIDE_BACK				1
#define PLANESIDE_ON				2
#define PLANESIDE_CROSS				3

// plane types
#define PLANETYPE_X					0
#define PLANETYPE_Y					1
#define PLANETYPE_Z					2
#define PLANETYPE_NEGX				3
#define PLANETYPE_NEGY				4
#define PLANETYPE_NEGZ				5
#define PLANETYPE_TRUEAXIAL			6	// all types < 6 are true axial planes
#define PLANETYPE_ZEROX				6
#define PLANETYPE_ZEROY				7
#define PLANETYPE_ZEROZ				8
#define PLANETYPE_NONAXIAL			9

#define		CS_RESTRICT     __restrict

#define		cs_is_constant_p(...)		__builtin_constant_p(__VA_ARGS__)

#define		cs_likely_constant_str_p(...)	(__builtin_constant_p(__builtin_strlen(__VA_ARGS__)) || __builtin_constant_p((__VA_ARGS__)[0]))
//#define		cs_likely_constant_str_p(...) false
#define     cs_is_constant_condition_true_p(...)    (cs_is_constant_p(__VA_ARGS__) &&( __VA_ARGS__))
#if defined(__clang__) || defined(__GNUC__)
#define     cs_index_m128(m,idx)        (m)[(idx)]

#define     cs_index_m256(m, idx)       (m)[(idx)]

#define     cs_index_m256d(m, idx)       (m)[(idx)]
#else
#define     cs_index_m128(m, idx)       (m).m128_f32[idx]
#define     cs_index_m256(m, idx)       (m).m256_f32[idx]
#define     cs_index_m256d(m, idx)      (m).m256_f64[idx]
#endif
#if defined(__clang__)
#define     ASSUME_FLOAT_EQ(x, y) ({float __tmpxass = x; float __tmpyass = y; cs_assume_m(*reinterpret_cast<int*>(&__tmpxass) == *reinterpret_cast<int*>(&__tmpyass));})


#define     IS_CONSTANT_FLOAT(x) ({float __tmpxass = x; cs_is_constant_p(*reinterpret_cast<int*>(&__tmpxass) );})
#define     NUM_CONSTANT_ELES_IN_VEC4(v)   (((IS_CONSTANT_FLOAT(cs_index_m128(v, 0)) ? 1 : 0)) + ((IS_CONSTANT_FLOAT(cs_index_m128(v, 1)) ? 1 : 0))+ ((IS_CONSTANT_FLOAT(cs_index_m128(v, 2)) ? 1 : 0)) + ((IS_CONSTANT_FLOAT(cs_index_m128(v, 3)) ? 1 : 0)))

#define     cs_dp_ps(x, y, selector) ({\
        const __m128 __x = x;\
        const __m128 __y = y;\
        __m128 __result = _mm_dp_ps(__x, __y, (selector));\
        float __resx = .0f;\
        float __resy = .0f;\
        float __resz = .0f;\
        float __resw = .0f;\
        if((selector) & 0b00010000) __resx = cs_index_m128(__x, 0) * cs_index_m128(__y, 0);\
        if((selector) & 0b00100000) __resy = cs_index_m128(__x, 1) * cs_index_m128(__y, 1);\
        if((selector) & 0b01000000) __resz = cs_index_m128(__x, 2) * cs_index_m128(__y, 2);\
        if((selector) & 0b10000000) __resw = cs_index_m128(__x, 3) * cs_index_m128(__y, 3);\
        float ressum = __resx+__resy+__resz+__resw;\
        float __outx = ressum;\
        float __outy = ressum;\
        float __outz = ressum;\
        float __outw = ressum;\
        if((selector) & 0b0001){ASSUME_FLOAT_EQ(cs_index_m128(__result, 0),ressum);}\
        else {__outx = .0f;ASSUME_FLOAT_EQ(cs_index_m128(__result, 0),.0f);}\
        if((selector) & 0b0010){ASSUME_FLOAT_EQ(cs_index_m128(__result, 1),ressum);}\
        else {__outy = .0f;ASSUME_FLOAT_EQ(cs_index_m128(__result, 1),.0f);}\
        if((selector) & 0b0100){ASSUME_FLOAT_EQ(cs_index_m128(__result, 2), ressum);}\
        else {__outz = .0f;ASSUME_FLOAT_EQ(cs_index_m128(__result, 2), .0f);}\
        if((selector) & 0b1000){ASSUME_FLOAT_EQ(cs_index_m128(__result, 3), ressum);}\
        else {__outw = .0f;ASSUME_FLOAT_EQ(cs_index_m128(__result, 3),.0f);}\
        if((NUM_CONSTANT_ELES_IN_VEC4(x) + NUM_CONSTANT_ELES_IN_VEC4(y)) >= 6) \
            __result = _mm_setr_ps(__outx, __outy, __outz, __outw);\
        __result;\
    })
#else
#define     cs_dp_ps(x, y, selector) _mm_dp_ps(x, y, selector)
#endif
ID_FORCE_INLINE
static __m128 cs_m128_setr_ivals(unsigned v0, unsigned v1, unsigned v2, unsigned v3) {
	return _mm_castsi128_ps(_mm_setr_epi32(v0, v1, v2, v3));
}

static __m128 cs_m128_setr_ivals(unsigned v0) {
	return cs_m128_setr_ivals(v0, v0, v0, v0);
}
static void cs_check_vec3_simd(vec3_simd_t v) {

}

static void cs_check_vec4_simd(__m128 v) {}
static void cs_break_on_nan(float f) {}
using cs_float_pair_t = __m128;
struct cs_m128_pair_t {
	__m128 first;
	__m128 second;
};

template<unsigned selector>
ID_FORCE_INLINE
static __m128 cs_permute_ps(__m128 x) {
#if defined(SIMD_PREFER_SHUFFLE_OVER_PERMUTE)

	return _mm_shuffle_ps(x, x, selector);
#else

	return _mm_permute_ps(x, selector);
#endif
}


template<unsigned which>
CS_FORCEINLINE
static __m128 cs_extract_ps(__m128 v) {
	static_assert(which < 4u);
#if defined(CS_EXTRACTPS_JUST_USE_INTRIN) || 1
	int fk = _mm_extract_ps(v, which);
	return _mm_set_ss(*reinterpret_cast<float*>(&fk));
#else

	if constexpr (which == 0) {
		return _mm_set_ss(_mm_cvtss_f32(v));
	}
	else if constexpr (which == 1) {
		return _mm_set_ss(_mm_cvtss_f32(_mm_movehdup_ps(v)));
	}
	else if constexpr (which == 2) {
		return _mm_set_ss(_mm_cvtss_f32(_mm_movehl_ps(v, v)));
	}
	else {
		return _mm_set_ss(_mm_cvtss_f32(_mm_shuffle_ps(v, v, _MM_SHUFFLE(3, 3, 3, 3))));
	}
#endif

}

/*
	hi goes to the third float, lo to the first
*/
CS_SUPER_PURE
ID_FORCE_INLINE
static cs_float_pair_t cs_pair2float(float lo, float hi) {
	return _mm_movelh_ps(_mm_set_ss(lo), _mm_set_ss(hi));
}
CS_SUPER_PURE
ID_FORCE_INLINE
static cs_float_pair_t cs_pair2ss(__m128 lo, __m128 hi) {
	return _mm_movelh_ps(lo, hi);
}
template<bool RZeroed = false>
ID_FORCE_INLINE CS_SUPER_PURE
static __m128 cs_rsqrtss_newt(__m128 r) {
	/*

		x2 = number * 0.5F;
		y  = number;
		i  = * ( long * ) &y;                       // evil floating point bit level hacking
		i  = 0x5f3759df - ( i >> 1 );               // what the fuck?
		y  = * ( float * ) &i;
		y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
	*/

#if 0
	__m128 rhalf = _mm_mul_ss(_mm_set_ss(0.5f), r);

	__m128 guesstimate;
	if constexpr (!RZeroed) {
		guesstimate = _mm_rsqrt_ss(r);
	}
	else {
		guesstimate = cs_rsqrtss_novex(r);
	}

	__m128 v0 = _mm_mul_ss(rhalf, guesstimate);

	__m128 v1 = _mm_mul_ss(v0, guesstimate);

	//_mm_xor_ps(v1, _mm_castsi128_ps(_mm_set_epi32(0,)))

	__m128 res = _mm_mul_ss(_mm_sub_ss(_mm_set_ps(0, 0, 0, 1.5F), v1), guesstimate);
	//msk_den_check = -1 if r <= FLT_SMALLEST_NON_DENORMAL 

	return res;
#else
	return _mm_div_ss(_mm_set1_ps(1.0f), _mm_sqrt_ss(r));
#endif
}

template<unsigned DestIdx, unsigned SrcIdx, bool ZeroElement0 = false, bool ZeroElement1 = false, bool ZeroElement2 = false, bool ZeroElement3 = false>
CS_SUPER_PURE
ID_FORCE_INLINE
static __m128 cs_insert_from_ps128(__m128 to, __m128 from) {
	//#define		ENCODE_INSERT(frombidx, toidx, zeroedmask)	((toidx << 4) | (frombidx << 6) | (zeroedmask))

	constexpr unsigned zeromask = ((int)ZeroElement0) | (((int)ZeroElement1) << 1) | (((int)ZeroElement2) << 2) | (((int)ZeroElement3) << 3);
	return _mm_insert_ps(to, from, (DestIdx << 4) | (SrcIdx << 6) | zeromask);
}

ID_FORCE_INLINE CS_SUPER_PURE
static __m128 cs_rsqrtps_newt(__m128 r) {
#if 1
	/*

		x2 = number * 0.5F;
		y  = number;
		i  = * ( long * ) &y;                       // evil floating point bit level hacking
		i  = 0x5f3759df - ( i >> 1 );               // what the fuck?
		y  = * ( float * ) &i;
		y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
	*/
	__m128 rhalf = _mm_mul_ps(_mm_set1_ps(0.5f), r);
	__m128 guesstimate = _mm_rsqrt_ps(r);

	__m128 v0 = _mm_mul_ps(rhalf, guesstimate);

	__m128 v1 = _mm_mul_ps(v0, guesstimate);

	//_mm_xor_ps(v1, _mm_castsi128_ps(_mm_set_epi32(0,)))

	return _mm_mul_ps(_mm_sub_ps(_mm_set1_ps(1.5F), v1), guesstimate);

#else
	return _mm_div_ps(_mm_set1_ps(1.0f), _mm_sqrt_ps(r));

#endif
}

ID_FORCE_INLINE
static __m128 cs_permutev_ps(__m128 x, __m128i indices) {
	//horrendous, sse4.1 doesnt have permutevar_ps

	return _mm_castsi128_ps(lookup4(Vec4i{ _mm_castps_si128(x) }, Vec4i{ indices }));

}
namespace vec3_simd {
	ID_FORCE_INLINE
		CS_SUPER_PURE
		static vec3_simd_t neg_xyz(vec3_simd_t v) {
		cs_check_vec3_simd(v);
		return _mm_xor_ps(v, _mm_castsi128_ps(_mm_set_epi32(0, 1U << 31, 1U << 31, 1U << 31)));
	}

	ID_FORCE_INLINE
		CS_SUPER_PURE
		static vec3_simd_t clear_ele3(vec3_simd_t v) {
		return _mm_and_ps(v, _mm_castsi128_ps(_mm_set_epi32(0, ~0u, ~0u, ~0u)));
	}
	//insertps is 0.5 through, this sequence technically is as well,
	//but this sequence can execute in p0123 so is less likely to get tied up
	ID_FORCE_INLINE
		CS_SUPER_PURE
		static vec3_simd_t make_ele3_1_0(vec3_simd_t v) {


		return _mm_or_ps(clear_ele3(v), _mm_setr_ps(.0f, .0f, .0f, 1.0f));
	}

	//if you already checked for < 0 and > 1 then use this

	CS_SUPER_PURE
		static inline vec3_simd_t lerp_unchecked(vec3_simd_t v1, vec3_simd_t v2, vec3_simd_t l) {
		cs_check_vec3_simd(v1);
		cs_check_vec3_simd(v2);
		cs_check_vec3_simd(l);
		return _mm_add_ps(_mm_mul_ps(_mm_sub_ps(v2, v1), l), v1);
		//v1 + l * (v2 - v1);
	}
	CS_SUPER_PURE
		static inline vec3_simd_t lerp(vec3_simd_t v1, vec3_simd_t v2, vec3_simd_t l) {
		cs_check_vec3_simd(v1);
		cs_check_vec3_simd(v2);
		cs_check_vec3_simd(l);
#if !defined(DISABLE_VECTORIZED_VEC3_CODE)
		__m128 masklt = _mm_cmplt_ps(l, _mm_setzero_ps());
		__m128 maskgt = _mm_cmpge_ps(l, _mm_set1_ps(1.0f));
		if (_mm_movemask_ps(_mm_or_ps(masklt, maskgt))) {

			return _mm_or_ps(_mm_and_ps(masklt, v1), _mm_and_ps(maskgt, v2));
		}
		else {
			return lerp_unchecked(v1, v2, l);
		}
#else
		idVec3 fromv1, fromv2, froml;
		fromv1.from_vec4f(v1);
		fromv2.from_vec4f(v2);
		froml.from_vec4f(l);
		idVec3 result;
		result.Lerp(fromv1, fromv2, froml.x);
		return result.to_vec4f();
#endif
	}
	static inline void store3_mem(idVec3& to, vec3_simd_t v) {
		cs_check_vec3_simd(v);
		//_mm_storel_pi (__m64* mem_addr, __m128 a)

		//Store the lower 2 single-precision (32-bit) floating-point elements from a into memory.
		_mm_storel_pi(reinterpret_cast<__m64*>(&to.x), v);
		_mm_store_ss(&to.z, _mm_movehl_ps(v, v));
	}
	struct split_vec3_t {
		__m128 laneval1;
		__m128 laneval2;
	};

	ID_FORCE_INLINE
		CS_SUPER_PURE
		static split_vec3_t split_vec3_upper_lanes(vec3_simd_t val) {
		__m128 ele2 = _mm_movehl_ps(val, val);
		__m128 ele1 = _mm_movehdup_ps(val);
		return split_vec3_t{ .laneval1 = ele1, .laneval2 = ele2 };

	}
	ID_FORCE_INLINE CS_SUPER_PURE
		static __m128 horiz_sum3_ss(vec3_simd_t intermed) {
#if defined(DISABLE_VECTORIZED_VEC3_CODE)
		idVec3 shitter;
		shitter.from_vec4f(intermed);
		return _mm_set_ss(shitter.x + shitter.y + shitter.z);
#else
		auto [ele1, ele2] = split_vec3_upper_lanes(intermed);

		return _mm_add_ss(_mm_add_ss(ele1, ele2), intermed);
#endif
		//return _mm_add_ss(_mm_add_ss(permute4<1, 0, 0, 0>(intermed), intermed), permute4<2, 0, 0, 0>(intermed));
	}
	CS_SUPER_PURE
		static inline float horiz_sum3(vec3_simd_t intermed) {
		__m128 summedsqr = horiz_sum3_ss(intermed);
		float fres = _mm_cvtss_f32(summedsqr);

		/*if ((intermed[0] + intermed[1] + intermed[2]) != fres) {
			cs_assume_m(false);
		}*/
		return fres;
	}	
	/*
		or of all in low 32
	*/
	CS_SUPER_PURE
		static inline __m128 horiz_or3(vec3_simd_t intermed) {

		auto [ele1, ele2] = split_vec3_upper_lanes(intermed);

		return _mm_or_ps(_mm_or_ps(ele1, ele2), intermed);;
	}
	CS_SUPER_PURE
		static inline __m128 horiz_and3(vec3_simd_t intermed) {

		auto [ele1, ele2] = split_vec3_upper_lanes(intermed);

		return _mm_and_ps(_mm_and_ps(ele1, ele2), intermed);;
	}
	CS_SUPER_PURE
		static inline __m128 multiplyvxv_to_scalar_ss(vec3_simd_t x, vec3_simd_t y) {
		cs_check_vec3_simd(x);
		cs_check_vec3_simd(y);
#if !defined(DISABLE_VECTORIZED_VEC3_CODE)

#if !defined(VEC3_DOT_FORCE_DPPS_VARIANT)
		Vec4f intermed = _mm_mul_ps(x, y);

		__m128 summedsqr = horiz_sum3_ss(intermed);//_mm_add_ss(_mm_add_ss(permute4<1, 0, 0, 0>(intermed), intermed), permute4<2, 0, 0, 0>(intermed));
		return summedsqr;
#else
		return cs_dp_ps(x, y, 0b01110001);
#endif
#else
		idVec3 xx, yy;
		xx.from_vec4f(x);
		yy.from_vec4f(y);
		return _mm_set_ss(xx * yy);
#endif
	}
	CS_SUPER_PURE
		static inline __m128 dot_ss(vec3_simd_t x, vec3_simd_t y) {
		cs_check_vec3_simd(x);
		cs_check_vec3_simd(y);
		return multiplyvxv_to_scalar_ss(x, y);
	}
	CS_SUPER_PURE
		static inline float multiplyvxv_to_scalar(vec3_simd_t x, vec3_simd_t y) {
		cs_check_vec3_simd(x);
		cs_check_vec3_simd(y);
		return _mm_cvtss_f32(dot_ss(x, y));

	}
	//the fast full dot product, not using dpps
	CS_FORCEINLINE  CS_SUPER_PURE
		static __m128 dot_fast_ss(vec3_simd_t x, vec3_simd_t y) {
		cs_check_vec3_simd(x);
		cs_check_vec3_simd(y);
		return horiz_sum3_ss(_mm_mul_ps(x, y));
	}
	CS_FORCEINLINE CS_SUPER_PURE
		static float dot_fast(vec3_simd_t x, vec3_simd_t y) {
		cs_check_vec3_simd(x);
		cs_check_vec3_simd(y);
		return _mm_cvtss_f32(dot_fast_ss(x, y));
	}
	CS_SUPER_PURE
		static inline __m128 tinydot_ss(vec3_simd_t x, vec3_simd_t y) {

		cs_check_vec3_simd(x);
		cs_check_vec3_simd(y);
		return cs_dp_ps(x, y, 0b01110001);
	}
	CS_SUPER_PURE
		static inline __m128 tinydot_bcast4_ss(vec3_simd_t x, vec3_simd_t y) {
		cs_check_vec3_simd(x);
		cs_check_vec3_simd(y);
		return cs_dp_ps(x, y, 0b01111111);
	}
	CS_SUPER_PURE
		static inline __m128 tinydot_bcast3_ss(vec3_simd_t x, vec3_simd_t y) {
		cs_check_vec3_simd(x);
		cs_check_vec3_simd(y);
		return cs_dp_ps(x, y, 0b01110111);
	}
	CS_SUPER_PURE
		static inline float tinydot(vec3_simd_t x, vec3_simd_t y) {
		cs_check_vec3_simd(x);
		cs_check_vec3_simd(y);
		return _mm_cvtss_f32(tinydot_ss(x, y));
	}
	template<bool opt_size = true>
	CS_SUPER_PURE
		static inline float dot(vec3_simd_t x, vec3_simd_t y) {
		cs_check_vec3_simd(x);
		cs_check_vec3_simd(y);
		if constexpr (opt_size) {
			return tinydot(x, y);
		}
		else {
			return dot_fast(x, y);
		}
	}
	CS_SUPER_PURE
		static inline vec3_simd_t combine_3ss_to_vec3(vec3_simd_t ele0, vec3_simd_t ele1, vec3_simd_t ele2) {
		return _mm_movelh_ps(_mm_unpacklo_ps(ele0, ele1), ele2);
	}
	ID_FORCE_INLINE CS_SUPER_PURE
		static vec3_simd_t cross_unshuffled(vec3_simd_t x, vec3_simd_t y) {
		cs_check_vec3_simd(x);
		cs_check_vec3_simd(y);
#if !defined(VEC3_CROSS_PACK_XY_TO_M256)
		__m128 tmp0 = _mm_shuffle_ps(y, y, _MM_SHUFFLE(3, 0, 2, 1));
		__m128 tmp1 = _mm_shuffle_ps(x, x, _MM_SHUFFLE(3, 0, 2, 1));
		tmp0 = _mm_mul_ps(tmp0, x);
		tmp1 = _mm_mul_ps(tmp1, y);
		__m128 tmp2 = _mm_sub_ps(tmp0, tmp1);


		return tmp2;
#else
		__m256 packedboth = _mm256_castps128_ps256(x);
		packedboth = _mm256_insertf128_ps(packedboth, y, 1);
		__m256 tmp = _mm256_shuffle_ps(packedboth, packedboth, _MM_SHUFFLE(3, 0, 2, 1));
		tmp = _mm256_mul_ps(packedboth, tmp);

		return _mm_sub_ps(clear_ele3(_mm256_castps256_ps128(tmp)), clear_ele3(_mm256_extractf128_ps(tmp, 1)));
#endif
	}
	ID_FORCE_INLINE CS_SUPER_PURE
		static vec3_simd_t cross_result_shuffle(vec3_simd_t tmp2) {
		return _mm_shuffle_ps(tmp2, tmp2, _MM_SHUFFLE(3, 0, 2, 1));

	}


	ID_FORCE_INLINE CS_SUPER_PURE
		static vec3_simd_t cross(vec3_simd_t x, vec3_simd_t y) {
		cs_check_vec3_simd(x);
		cs_check_vec3_simd(y);
		return cross_result_shuffle(cross_unshuffled(x, y));

	}

	ID_FORCE_INLINE CS_SUPER_PURE
		static __m128 length_squared_ss(vec3_simd_t x) {
		cs_check_vec3_simd(x);
#if defined(VEC3_LENGTH_USE_DPPS)
		__m128 res = cs_dp_ps(x, x, 0b01110001);
		cs_assume_m128_top3_zeroed(res);
		cs_assume_isdotof3(cs_index_m128(res, 0), x, x);
		return res;
#else
		return horiz_sum3_ss(_mm_mul_ps(x, x));
#endif
	}

	ID_FORCE_INLINE CS_SUPER_PURE
		static __m128 length_squared_fast_ss(vec3_simd_t x) {
		cs_check_vec3_simd(x);
		return horiz_sum3_ss(_mm_mul_ps(x, x));
	}
	ID_FORCE_INLINE CS_SUPER_PURE
		static float length_squared(vec3_simd_t x) {
		cs_check_vec3_simd(x);
		return _mm_cvtss_f32(length_squared_ss(x));
		//return horiz_sum3(_mm_mul_ps(x, x));
	}
	ID_FORCE_INLINE CS_SUPER_PURE
		static float length_squared_fast(vec3_simd_t x) {
		cs_check_vec3_simd(x);
		return _mm_cvtss_f32(length_squared_fast_ss(x));
		//return horiz_sum3(_mm_mul_ps(x, x));
	}
	CS_SUPER_PURE
		static inline float length(vec3_simd_t x) {
		cs_check_vec3_simd(x);
		//return _mm_cvtss_f32(_mm_sqrt_ss(horiz_sum3_ss(_mm_mul_ps(x, x))));
		return _mm_cvtss_f32(
			_mm_sqrt_ss(length_squared_ss(x))
		);
	}
	CS_SUPER_PURE
		static inline float length_fast(vec3_simd_t x) {
		cs_check_vec3_simd(x);
		//return _mm_cvtss_f32(_mm_sqrt_ss(horiz_sum3_ss(_mm_mul_ps(x, x))));
		return _mm_cvtss_f32(
			_mm_sqrt_ss(length_squared_fast_ss(x))
		);
	}

	ID_FORCE_INLINE
		CS_SUPER_PURE
		static vec3_simd_t calc_abs(vec3_simd_t v) {
		cs_check_vec3_simd(v);
		return _mm_and_ps(v, _mm_castsi128_ps(_mm_set1_epi32(0x7fFFFFFF)));
	}
	ID_FORCE_INLINE
		CS_SUPER_PURE
		static vec3_simd_t calc_abs_zero_ele4(vec3_simd_t v) {
		if (cs_is_constant_condition_true_p(cs_index_m128(v, 3) == .0f)) {
			if (cs_is_constant_condition_true_p(cs_index_m128(v, 0) >= .0f && cs_index_m128(v, 1) >= .0f && cs_index_m128(v, 2) >= .0f)) {

			}
			else {
				v = _mm_and_ps(v, _mm_castsi128_ps(_mm_set1_epi32(0x7fFFFFFF)));

			}
		}
		else {
			if (cs_is_constant_condition_true_p(cs_index_m128(v, 0) >= .0f && cs_index_m128(v, 1) >= .0f && cs_index_m128(v, 2) >= .0f)) {

			}
			else {
				v = _mm_and_ps(v, _mm_castsi128_ps(_mm_set_epi32(0, 0x7fFFFFFF, 0x7fFFFFFF, 0x7fFFFFFF)));
			}
		}
		cs_assume_m(cs_index_m128(v, 0) >= .0f && cs_index_m128(v, 1) >= .0f && cs_index_m128(v, 2) >= .0f && cs_index_m128(v, 3) == .0f);
		return v;
	}
#define		GT_TERM(term)		(1 << ((term << 1) + 1))
#define		LT_TERM(term)		(1<<(term<<1) )
		CS_SUPER_PURE
		//generates direction mask but doesnt or all of the values together
		static inline __m128 direction_mask_unpacked(vec3_simd_t v) {


		__m128 ltzeromask = _mm_cmplt_ps(v, _mm_setzero_ps());
		__m128 gtzeromask = _mm_cmpgt_ps(v, _mm_setzero_ps());
		//mask += ltzeromask[term] ? 1<<(term<<1) : gtzeromask[term] ? 1<<((term<<1)+1) : 0;

		__m128 blmask_gt = cs_m128_setr_ivals(GT_TERM(0), GT_TERM(1), GT_TERM(2), 0);
		__m128 blmask_lt = cs_m128_setr_ivals(LT_TERM(0), LT_TERM(1), LT_TERM(2), 0);

		__m128 gtvals = _mm_and_ps(gtzeromask, blmask_gt);
		__m128 ltvals = _mm_and_ps(ltzeromask, blmask_lt);

		return _mm_or_ps(gtvals, ltvals);

	}




	CS_SUPER_PURE
		static inline unsigned direction_mask(vec3_simd_t v) {
		return _mm_cvtsi128_si32(_mm_castps_si128(vec3_simd::horiz_or3(direction_mask_unpacked(v))));
	}

	/*
		converts a direction mask to a vec3 direction
	*/
	CS_SUPER_PURE
		static inline __m128 from_direction_mask(unsigned value) {


		__m128i inp = _mm_shuffle_epi32(_mm_cvtsi32_si128(value), _MM_SHUFFLE(3, 0, 0, 0));
		__m128i negmask_c = _mm_setr_epi32(LT_TERM(0), LT_TERM(1), LT_TERM(2), 0);
		__m128i posmask_c = _mm_setr_epi32(GT_TERM(0), GT_TERM(1), GT_TERM(2), 0);

		__m128i posmask = _mm_and_si128(inp, posmask_c);
		__m128i negmask = _mm_and_si128(inp, negmask_c);
		posmask = _mm_cmpeq_epi32(posmask, posmask_c);
		negmask = _mm_cmpeq_epi32(negmask, negmask_c);
		posmask = _mm_sub_epi32(_mm_setzero_si128(), posmask);//_mm_sign_epi32(posmask)

		return _mm_cvtepi32_ps(_mm_or_si128(posmask, negmask));
	}




#undef LT_TERM
#undef GT_TERM
	template<bool approx = false, bool check_rsqrt = true>
	CS_SUPER_PURE
		static inline __m128 rsqrt_checked(__m128 input) {
		__m128 invlength;
		auto do_rsqrt = [input, &invlength]() {
			if constexpr (approx)
				invlength = _mm_rsqrt_ss(input);
			else
				invlength = cs_rsqrtss_newt<true>(input);
		};

		if constexpr (check_rsqrt) {
			if (_mm_cvtss_f32(input) > idMath::FLT_SMALLEST_NON_DENORMAL) {
				do_rsqrt();
			}
			else {
				invlength = _mm_set_ss(idMath::INFINITY);
			}
		}
		else {
			do_rsqrt();
		}
		return invlength;
	}
	template<bool approx = false>
	CS_SUPER_PURE
		static inline __m128 rsqrtps_unchecked(__m128 input) {
		__m128 invlength;
		auto do_rsqrt = [input, &invlength]() {
			if constexpr (approx)
				invlength = _mm_rsqrt_ps(input);
			else
				invlength = cs_rsqrtps_newt<true>(input);
		};

		do_rsqrt();
		return invlength;
	}
	CS_FORCEINLINE CS_SUPER_PURE
		static vec3_simd_t normalized_icache_precise(vec3_simd_t res) {
		cs_check_vec3_simd(res);
		__m128 summersqrx3 = tinydot_bcast3_ss(res, res);
		__m128 sqrted = _mm_sqrt_ps(summersqrx3);
		__m128 rcped = _mm_div_ps(_mm_setr_ps(1.0f, 1.0f, 1.0f, .0f), sqrted);
		return _mm_mul_ps(res, rcped);
	}
	CS_FORCEINLINE CS_SUPER_PURE
		static vec3_simd_t normalized_icache_approx(vec3_simd_t res) {
		cs_check_vec3_simd(res);
		__m128 summersqrx3 = tinydot_bcast3_ss(res, res);
		__m128 rsqrted = _mm_rsqrt_ps(summersqrx3);
		return _mm_mul_ps(res, rsqrted);
	}
	template<bool approximation = false, bool check_rsqrt = true>
	CS_SUPER_PURE
		static inline vec3_simd_t normalized(vec3_simd_t res) {
#if defined(VEC3_SIMD_NORMALIZE_FORCE_COMPACT_PRECISE)
		return normalized_icache_precise(res);
#else
		cs_check_vec3_simd(res);
		Vec4f intermed = _mm_mul_ps(res, res);

		__m128 summedsqr = horiz_sum3_ss(intermed);//_mm_add_ss(_mm_add_ss(permute4<1, 0, 0, 0>(intermed), intermed), permute4<2, 0, 0, 0>(intermed));

		//(x > FLT_SMALLEST_NON_DENORMAL) ? idMath::RSqrt(x) : INFINITY;


		__m128 invlength = rsqrt_checked<approximation, check_rsqrt>(summedsqr);
		/*
		 assume high element of invlength is zero, using 3, 0, 0, 0 will ensure the result has 0 in the third element
		*/
		Vec4f normvec = _mm_mul_ps(/*_mm_broadcastss_ps(invlength)*/ cs_permute_ps<_MM_SHUFFLE(3, 0, 0, 0)>(invlength), res);
		cs_assume_m(normvec[0] <= 1.0f && normvec[0] >= -1.0f && normvec[1] <= 1.0f && normvec[1] >= -1.0f && normvec[2] <= 1.0f && normvec[2] >= -1.0f);
		cs_check_vec3_simd(normvec);
		return normvec;
#endif

	}
	template<bool approximation = false, bool check_rsqrt = true>
	static inline vec3_simd_t normalized_with_valout(vec3_simd_t res, vec3_simd_t& valout) {
#if defined(VEC3_SIMD_NORMALIZE_FORCE_COMPACT_PRECISE)
		return normalized_icache_precise(res);
#else
		cs_check_vec3_simd(res);
		Vec4f intermed = _mm_mul_ps(res, res);

		__m128 summedsqr = horiz_sum3_ss(intermed);//_mm_add_ss(_mm_add_ss(permute4<1, 0, 0, 0>(intermed), intermed), permute4<2, 0, 0, 0>(intermed));

		//(x > FLT_SMALLEST_NON_DENORMAL) ? idMath::RSqrt(x) : INFINITY;


		__m128 invlength = rsqrt_checked<approximation, check_rsqrt>(summedsqr);
		/*
		 assume high element of invlength is zero, using 3, 0, 0, 0 will ensure the result has 0 in the third element
		*/
		invlength = cs_permute_ps<_MM_SHUFFLE(3, 0, 0, 0)>(invlength);
		valout = _mm_mul_ss(invlength, summedsqr);
		Vec4f normvec = _mm_mul_ps(/*_mm_broadcastss_ps(invlength)*/ invlength, res);
		cs_assume_m(normvec[0] <= 1.0f && normvec[0] >= -1.0f && normvec[1] <= 1.0f && normvec[1] >= -1.0f && normvec[2] <= 1.0f && normvec[2] >= -1.0f);
		cs_check_vec3_simd(normvec);
		cs_break_on_nan(_mm_cvtss_f32(valout));
		return normvec;
#endif

	}
}


namespace plane_simd {
	template<bool opt_size = true>
	CS_SUPER_PURE
		static inline __m128 distance_ss(plane_simd_t plane, vec3_simd_t vec) {
		if constexpr (!opt_size) {
			vec3_simd_t muled = _mm_mul_ps(plane, vec);
			__m128 sumfirst3 = vec3_simd::horiz_sum3_ss(muled);
			return _mm_add_ss(cs_permute_ps<_MM_SHUFFLE(3, 3, 3, 3)>(plane), sumfirst3);
		}
		else {
			__m128 lowpart = cs_extract_ps<3>(plane);
			__m128 dotted = vec3_simd::tinydot_ss(plane, vec);
			return _mm_add_ss(dotted, lowpart);
		}
	}
	template<bool opt_size = true>
	CS_SUPER_PURE
		static inline float distance(plane_simd_t plane, vec3_simd_t vec) {
		return _mm_cvtss_f32(distance_ss<opt_size>(plane, vec));
	}
	CS_SUPER_PURE
		static inline float get_dist(plane_simd_t plane) {
		return -_mm_cvtss_f32(cs_extract_ps<3>(plane));
	}
	//dont feel like including fix_degenerate_normals
#if 0
	
	template<bool opt_size = true>
	CS_SUPER_PURE
		static inline plane_simd_t from_points(const vec3_simd_t p1, const vec3_simd_t p2, const vec3_simd_t p3, bool fixDegenerate) {
#ifdef DISABLE_VECTORIZED_PLANE_CODE

		idVec3 pp1, pp2, pp3;
		pp1.from_vec4f(p1);
		pp2.from_vec4f(p2);
		pp3.from_vec4f(p3);
		idPlane plres;
		plres.FromPoints(pp1, pp2, pp3, fixDegenerate);
		return plres.to_vec4f();
#else

		vec3_simd_t normal = vec3_simd::cross(_mm_sub_ps(p1, p2), _mm_sub_ps(p3, p2));

		vec3_simd_t normalized = vec3_simd::normalized(normal);
		if (fixDegenerate) {
			normalized = vec3_simd::fix_degenerate_normals(normalized).result;
		}

		float intermed_d = -vec3_simd::dot<opt_size>(normalized, p2);//vec3_simd::horiz_sum3(_mm_mul_ps(normalized, p2));
		//result of vec3_simd::normalize should have zero in upper element

		return cs_insert_from_ps128<3, 0>(normalized, _mm_set_ss(intermed_d));
#endif
		//_mm_permute2_ps()

		/*Vec4f tmp = normalized;
		tmp[3] = intermed_d;
		return tmp;*/

	}
#endif
	//no fix_degenerate_normals... 
	template<bool opt_size = true>
	CS_SUPER_PURE
		static inline plane_simd_t from_points_fast(const vec3_simd_t p1, const vec3_simd_t p2, const vec3_simd_t p3) {
#ifdef DISABLE_VECTORIZED_PLANE_CODE

		idVec3 pp1, pp2, pp3;
		pp1.from_vec4f(p1);
		pp2.from_vec4f(p2);
		pp3.from_vec4f(p3);
		idPlane plres;
		plres.FromPoints(pp1, pp2, pp3, false);
		return plres.to_vec4f();
#else

		__m128 shuffle_x = _mm_sub_ps(p1, p2);
		__m128 shuffle_y = _mm_sub_ps(p3, p2);
		vec3_simd_t normal;
		{
			__m128 tmp0 = _mm_shuffle_ps(shuffle_y, shuffle_y, _MM_SHUFFLE(3, 0, 2, 1));
			__m128 tmp1 = _mm_shuffle_ps(shuffle_x, shuffle_x, _MM_SHUFFLE(3, 0, 2, 1));
			tmp0 = _mm_mul_ps(tmp0, shuffle_x);
			tmp1 = _mm_mul_ps(tmp1, shuffle_y);
			__m128 tmp2 = _mm_sub_ps(tmp0, tmp1);

			if (cs_is_constant_condition_true_p(cs_index_m128(shuffle_x, 3) == .0f || cs_index_m128(shuffle_y, 3) == .0f)) {
				cs_assume_m(cs_index_m128(tmp2, 3) == .0f);
			}
			normal = tmp2;

		}

		normal = _mm_shuffle_ps(normal, normal, _MM_SHUFFLE(3, 0, 2, 1));//vec3_simd::cross_result_shuffle(normal);

		vec3_simd_t normaldot = cs_dp_ps(normal, normal, 0b01110111);

		vec3_simd_t normalp1 = _mm_rsqrt_ps(normaldot);
		//_mm_div_ss(_mm_set_ss(1.0f), _mm_sqrt_ss(normaldot));



		vec3_simd_t normalized = _mm_mul_ps(normalp1, normal);//vec3_simd::normalized(normal);


		float intermed_d = -_mm_cvtss_f32(cs_dp_ps(normalized, p2, 0b01110001));//-vec3_simd::dot<opt_size>(normalized, p2);//vec3_simd::horiz_sum3(_mm_mul_ps(normalized, p2));
		//result of vec3_simd::normalize should have zero in upper element

		return cs_insert_from_ps128<3, 0>(normalized, _mm_set_ss(intermed_d));
#endif
		//_mm_permute2_ps()

		/*Vec4f tmp = normalized;
		tmp[3] = intermed_d;
		return tmp;*/

	}
	CS_SUPER_PURE
		static inline plane_simd_t fit_through_point(plane_simd_t pl, vec3_simd_t p) {
#ifdef DISABLE_VECTORIZED_PLANE_CODE
		idPlane ppl;
		idVec3 pp;
		ppl.from_vec4f(pl);
		pp.from_vec4f(p);
		ppl.FitThroughPoint(pp);
		return ppl.to_vec4f();

#else
		__m128 negated_d = _mm_xor_ps(vec3_simd::dot_ss(pl, p), cs_m128_setr_ivals(1U << 31, 0, 0, 0));
		return cs_insert_from_ps128<3, 0>(pl, negated_d);
#endif
		//d = -( Normal() * p );
	}
	CS_SUPER_PURE
		static inline plane_simd_t set_normal(plane_simd_t pl, vec3_simd_t norm) {
		return cs_insert_from_ps128<3, 3>(norm, pl);
		//return _mm_blend_ps(pl, norm, 0b111);
	}
	CS_FORCEINLINE

		static bool ray_intersection(plane_simd_t plane, vec3_simd_t start, vec3_simd_t dir, float& scale) {
#ifdef DISABLE_VECTORIZED_PLANE_CODE
		idPlane pplane;
		idVec3 st, di;

		pplane.from_vec4f(plane);
		st.from_vec4f(start);
		di.from_vec4f(dir);
		return pplane.RayIntersection(st, di, scale);

#else
		float d1 = vec3_simd::tinydot(plane, start) + _mm_cvtss_f32(cs_extract_ps<3>(plane));
		float d2 = vec3_simd::tinydot(plane, dir);

		if (!d2) {
			return false;
		}
		scale = -(d1 / d2);
		return true;
#endif
	}
}

struct bounds_simd_t {
	__m128 b0;
	__m128 b1;
};

namespace bounds_simd {
	template<bool completely_branchless = false>
	ID_FORCE_INLINE
		CS_SUPER_PURE
		static unsigned plane_side(__m128 b0, __m128 b1, __m128 plvec, const float epsilon = ON_EPSILON) {
		cs_check_vec3_simd(b0);
		cs_check_vec3_simd(b1);
		cs_check_vec4_simd(plvec);
#if !defined(DISABLE_VECTORIZED_BOUNDS_CODE)
		Vec4f center = _mm_mul_ps(_mm_add_ps(b0, b1), _mm_set1_ps(0.5f));

		//Vec4f tmpdist = _mm_mul_ps(center, plvec);
		//get the "d" element of the plane
		//float d1 = _mm_cvtss_f32(_mm_permute_ps(plvec, _MM_SHUFFLE(3, 3, 3, 3))) + vec3_simd::horiz_sum3(tmpdist);

		__m128 d1 = _mm_add_ss(vec3_simd::dot_fast_ss(center, plvec), cs_extract_ps<3>(plvec));

		//	__m128 d1 = (_mm_add_ss(_mm_permute_ps(plvec, _MM_SHUFFLE(3, 3, 3, 3)), vec3_simd::horiz_sum3_ss(tmpdist)));

#if 0
		Vec4f b1centerdiff = _mm_mul_ps(plvec, _mm_sub_ps(b1, center));

		Vec4f absded = _mm_and_ps(_mm_castsi128_ps(_mm_set1_epi32(0x7fffffff)), b1centerdiff);


		__m128 d2 = vec3_simd::horiz_sum3_ss(absded);//horizontal_add(absded);


#else
		__m128 d2 = vec3_simd::dot_fast_ss(vec3_simd::calc_abs_zero_ele4(plvec), vec3_simd::calc_abs_zero_ele4(_mm_sub_ps(b1, center)));
#endif
		//now fabs and horizontal add to d2



		/*if (d1 - d2 > epsilon)
		{
			return PLANESIDE_FRONT;
		}
		if (d1 + d2 < -epsilon)
		{
			return PLANESIDE_BACK;
		}
		return PLANESIDE_CROSS;


		__m128 mskgt = _mm_cmpgt_ss(sub1, _mm_set1_ps(epsilon));
		__m128 msklt = _mm_cmplt_ss(sub2, _mm_set1_ps(-epsilon));*/
		//generated by icc
		__m128 sub1 = _mm_sub_ss(d1, d2);
		__m128 sub2 = _mm_add_ss(d1, d2);
		if constexpr (!completely_branchless) {
			if (_mm_comigt_ss(sub1, _mm_set_ss(epsilon))) {
				return PLANESIDE_FRONT;
			}

			return (static_cast<unsigned>(_mm_cvtsi128_si32(_mm_castps_si128(_mm_cmplt_ss(sub2, _mm_set_ss(-epsilon))))) & static_cast<unsigned>(-2)) + 3;
		}
		else {
			__m128 gtmask = _mm_cmpgt_ss(sub1, _mm_set_ss(epsilon));

			__m128 ltmask = _mm_cmplt_ss(sub2, _mm_set_ss(-epsilon));
			__m128 intermed = _mm_unpacklo_ps(ltmask, gtmask);
			uint64_t packed_result = _mm_cvtsi128_si64(_mm_castps_si128(intermed));
			unsigned msk_front = packed_result >> 32;
			unsigned msk_rest = packed_result;
			return ((msk_rest & static_cast<unsigned>(-2)) + 3) & ~msk_front;
			//return (static_cast<unsigned>(_mm_cvtsi128_si32(_mm_castps_si128()) & static_cast<unsigned>(-2)) + 3;

		}
#else
		idBounds tmpbounds;
		idPlane tmpplane;
		bounds_simd::store(tmpbounds, bounds_simd_t{ .b0 = b0, .b1 = b1 });
		tmpplane.from_vec4f(plvec);
		return tmpbounds.PlaneSide(tmpplane, epsilon);
#endif
	}	
	template<bool opt_size = true>
		ID_FORCE_INLINE
		CS_SUPER_PURE
		static float plane_distance(__m128 b0, __m128 b1, __m128 planev) {
		cs_check_vec3_simd(b0);
		cs_check_vec3_simd(b1);
		cs_check_vec4_simd(planev);
#if !defined(DISABLE_VECTORIZED_BOUNDS_CODE)

		vec3_simd_t center = _mm_mul_ps(_mm_add_ps(b0, b1), _mm_setr_ps(0.5f, 0.5f, 0.5f, .0f));

		__m128 d1 = plane_simd::distance_ss<opt_size>(planev, center);


		vec3_simd_t b1minuscenter = _mm_sub_ps(b1, center);

		__m128 d2;
		if constexpr (!opt_size) {
			vec3_simd_t b4fabssum = _mm_mul_ps(b1minuscenter, planev);

			d2 = vec3_simd::horiz_sum3_ss(vec3_simd::calc_abs_zero_ele4(b4fabssum));
		}
		else {
			d2 = vec3_simd::tinydot_ss(vec3_simd::calc_abs(b1minuscenter), vec3_simd::calc_abs(planev));
		}
		d1 = _mm_moveldup_ps(d1);
		d2 = _mm_moveldup_ps(d2);

		__m128 tmpboth = _mm_addsub_ps(d1, d2);
		/*
			tmpboth[0] = d1 - d2
			tmpboth[1] = d1 + d2
		*/
		__m128 ltzcomp = _mm_movehdup_ps(tmpboth);
		__m128 gtz = _mm_cmpgt_ps(tmpboth, _mm_setzero_ps());
		__m128 ltz = _mm_cmplt_ps(ltzcomp, _mm_setzero_ps());

		float result = _mm_cvtss_f32(_mm_or_ps(_mm_and_ps(gtz, tmpboth), _mm_and_ps(ltz, ltzcomp)));
		cs_break_on_nan(result);
		return result;
#else
		idBounds tmpbounds;
		idPlane tmpplane;
		bounds_simd::store(tmpbounds, bounds_simd_t{ .b0 = b0, .b1 = b1 });
		tmpplane.from_vec4f(planev);
		return tmpbounds.PlaneDistance(tmpplane);
#endif
	}
		ID_FORCE_INLINE
			CS_SUPER_PURE
			static bool intersects_bounds(__m128 b0, __m128 b1, __m128 ab0, __m128 ab1) {
#if !defined(DISABLE_VECTORIZED_BOUNDS_CODE)
			cs_check_vec3_simd(b0);
			cs_check_vec3_simd(b1);
			cs_check_vec3_simd(ab0);
			cs_check_vec3_simd(ab1);
			__m128 msk1 = _mm_cmplt_ps(ab1, b0);
			__m128 msk2 = _mm_cmpgt_ps(ab0, b1);
			__m128 fullmsk = vec3_simd::clear_ele3(_mm_or_ps(msk1, msk2));
			return _mm_testz_ps(fullmsk, fullmsk);
#else
			idBounds tb, ab;
			bounds_simd::store(tb, b0, b1);
			bounds_simd::store(ab, ab0, ab1);
			return tb.IntersectsBounds(ab);
#endif
		}
		ID_FORCE_INLINE
			CS_SUPER_PURE
			static bounds_simd_t cleared_bounds() {
			//		b[0][0] = b[0][1] = b[0][2] = idMath::INFINITY;
			//b[1][0] = b[1][1] = b[1][2] = -idMath::INFINITY;
			return bounds_simd_t{ .b0 = _mm_setr_ps(INFINITY,INFINITY,INFINITY,0), .b1 = _mm_setr_ps(-INFINITY,-INFINITY,-INFINITY,0) };
		}

		ID_FORCE_INLINE
			CS_SUPER_PURE
			static bounds_simd_t add_bounds(bounds_simd_t thiz, bounds_simd_t other) {

			cs_check_vec3_simd(thiz.b0);
			cs_check_vec3_simd(thiz.b1);
			cs_check_vec3_simd(other.b0);
			cs_check_vec3_simd(other.b1);
#ifdef DISABLE_VECTORIZED_BOUNDS_CODE

			idBounds bt;
			idBounds bo;
			bounds_simd::store(bt, thiz);
			bounds_simd::store(bo, other);

			bt.AddBounds(bo);
			return bounds_simd::load(bt);
#else
			return bounds_simd_t{ .b0 = _mm_min_ps(thiz.b0, other.b0), .b1 = _mm_max_ps(other.b1, thiz.b1) };
#endif
		}
		ID_FORCE_INLINE
			CS_SUPER_PURE
			static bounds_simd_t add_bounds(__m128 tb0, __m128 tb1, __m128 ob0, __m128 ob1) {
			cs_check_vec3_simd(tb0);
			cs_check_vec3_simd(tb1);
			cs_check_vec3_simd(ob0);
			cs_check_vec3_simd(ob1);
#ifdef DISABLE_VECTORIZED_BOUNDS_CODE

			idBounds bt;
			idBounds bo;
			bounds_simd::store(bt, bounds_simd_t{ tb0, tb1 });
			bounds_simd::store(bo, bounds_simd_t{ ob0, ob1 });

			bt.AddBounds(bo);
			return bounds_simd::load(bt);
#else
			return bounds_simd_t{ .b0 = _mm_min_ps(tb0, ob0), .b1 = _mm_max_ps(ob1, tb1) };
#endif
		}
		ID_FORCE_INLINE
			CS_SUPER_PURE
			static bool contains_point(vec3_simd_t b0, vec3_simd_t b1, vec3_simd_t pv) {
			cs_check_vec3_simd(b0);
			cs_check_vec3_simd(b1);
			cs_check_vec3_simd(pv);
#ifdef DISABLE_VECTORIZED_BOUNDS_CODE
			idBounds tb;
			idVec3 p;

			bounds_simd::store(tb, b0, b1);
			p.from_vec4f(pv);
			return tb.ContainsPoint(p);

#else
			vec3_simd_t cmpmask = vec3_simd::clear_ele3(_mm_or_ps(_mm_cmplt_ps(pv, b0), _mm_cmpgt_ps(pv, b1)));
			return _mm_testz_ps(cmpmask, cmpmask);
#endif
		}
		ID_FORCE_INLINE
			CS_SUPER_PURE
			static bounds_simd_t add_point(vec3_simd_t b0, vec3_simd_t b1, vec3_simd_t pt) {
			cs_check_vec3_simd(b0);
			cs_check_vec3_simd(b1);
			cs_check_vec3_simd(pt);
			return bounds_simd_t{ .b0 = _mm_min_ps(b0, pt), .b1 = _mm_max_ps(b1,pt) };
		}

		ID_FORCE_INLINE
			CS_SUPER_PURE
			static bool is_cleared(vec3_simd_t b0, vec3_simd_t b1) {
			cs_check_vec3_simd(b0);
			cs_check_vec3_simd(b1);
			//b[0][0] > b[1][0];
			return _mm_comigt_ss(b0, b1);
		}
		template<bool ele_3_is_zero = false>
		ID_FORCE_INLINE
			CS_SUPER_PURE
			static bool test_overlap(__m128 xb0, __m128 xb1, __m128 yb0, __m128 yb1) {
			cs_check_vec3_simd(xb0);
			cs_check_vec3_simd(xb1);
			cs_check_vec3_simd(yb0);
			cs_check_vec3_simd(yb1);
			//todo: create non vectorized version of this one
			__m128 mask1 = _mm_cmpgt_ps(yb0, xb1);
			__m128 mask2 = _mm_cmplt_ps(yb1, xb0);
			__m128 ored = _mm_or_ps(mask1, mask2);
			if (!ele_3_is_zero) {
				ored = cs_insert_from_ps128<3, 3, false, false, false, true>(ored, _mm_setzero_ps());
			}
			return _mm_testz_ps(ored, ored);

		}

		CS_FORCEINLINE
			CS_SUPER_PURE
			static bounds_simd_t expand(vec3_simd_t b0, vec3_simd_t b1, float b) {
			/*return idBounds(idVec3(b[0][0] - d, b[0][1] - d, b[0][2] - d),
				idVec3(b[1][0] + d, b[1][1] + d, b[1][2] + d));*/
			cs_check_vec3_simd(b0);
			cs_check_vec3_simd(b1);
			cs_break_on_nan(b);
#if defined(DISABLE_VECTORIZED_BOUNDS_CODE)
			idBounds res;
			bounds_simd::store(res, b0, b1);
			res.ExpandSelf(b);
			return bounds_simd::load(res);
#else
			return bounds_simd_t{ .b0 = _mm_sub_ps(b0, _mm_setr_ps(b, b, b, .0f)), .b1 = _mm_add_ps(b1, _mm_setr_ps(b, b, b, .0f)) };
#endif
		}
		static 
		bool  ray_intersection(__m128 b0, __m128 b1, vec3_simd_t start, vec3_simd_t dir, float& scale) {
			cs_check_vec3_simd(b0);
			cs_check_vec3_simd(b1);
			cs_check_vec3_simd(start);
			cs_check_vec3_simd(dir);
#if !defined(DISABLE_VECTORIZED_BOUNDS_CODE)
			__m128 ltb0mask = _mm_cmplt_ps(start, b0);
			__m128 gtb1mask = _mm_cmpgt_ps(start, b1);


			__m128 either_not_inside = _mm_or_ps(ltb0mask, gtb1mask);

			__m128 blended_sides = _mm_and_ps(_mm_blendv_ps(b1, b0, ltb0mask), either_not_inside);


			__m128 fs = _mm_sub_ps(start, blended_sides);

			__m128 should_continue = _mm_or_ps(_mm_xor_ps(either_not_inside, cs_m128_setr_ivals(~0u)), _mm_cmpeq_ps(dir, _mm_set1_ps(.0f)));
			unsigned should_continue_mask = _mm_movemask_ps(should_continue);
			int ax0 = -1;

			__m128 inversedirmul = _mm_xor_ps(_mm_mul_ps(fs, _mm_rcp_ps(dir)), cs_m128_setr_ivals(1U << 31));

			__m128 absfs = vec3_simd::calc_abs_zero_ele4(fs);


			for (unsigned i = 0; i < 3; ++i) {
				/*if(cs_index_m128(dir, i) == .0f)
					continue;*/
				if (should_continue_mask & (1 << i)) {
					continue;
				}
				float f = cs_index_m128(fs, i);
				if (ax0 < 0 || cs_index_m128(absfs, i) > fabsf(scale * cs_index_m128(dir, i)))
				{
					scale = cs_index_m128(inversedirmul, i);
					ax0 = i;
				}
			}
			if (ax0 < 0)
			{
				scale = 0.0f;

				return !(_mm_movemask_ps(either_not_inside) & 0b111);
				// return true if the start point is inside the bounds
				// 
				//return (inside == 3);
			}
			/*int ax1 = (ax0 + 1) % 3;
			int ax2 = (ax0 + 2) % 3;
			idVec3 hit;
			hit[ax1] = start[ax1] + scale * dir[ax1];
			hit[ax2] = start[ax2] + scale * dir[ax2];

			return (hit[ax1] >= b[0][ax1] && hit[ax1] <= b[1][ax1] &&
				hit[ax2] >= b[0][ax2] && hit[ax2] <= b[1][ax2]);*/

			__m128i bcasti = _mm_set1_epi32(ax0);
			__m128i addax = _mm_add_epi32(bcasti, _mm_setr_epi32(1, 2, 0, 0));
			__m128i mod3lut = _mm_setr_epi8(0, 1, 2, 0, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
			__m128i modax = _mm_shuffle_epi8(mod3lut, addax);

			__m128 shufstart = cs_permutev_ps(start, modax);
			__m128 shufdir = cs_permutev_ps(dir, modax);
			__m128 twoscale = _mm_moveldup_ps(_mm_set_ss(scale));

			__m128 tmphit = _mm_add_ps(shufstart, _mm_mul_ps(shufdir, twoscale));

			b0 = cs_permutev_ps(b0, modax);
			b1 = cs_permutev_ps(b1, modax);


			__m128 tmpcond = _mm_and_ps(_mm_cmpge_ps(tmphit, b0), _mm_cmple_ps(tmphit, b1));

			return (_mm_movemask_ps(tmpcond) & 0b11) == 0b11;
#else

			idBounds b;
			bounds_simd::store(b, b0, b1);
			idVec3 startv, dirv;
			startv.from_vec4f(start);
			dirv.from_vec4f(dir);
			return b.RayIntersection(startv, dirv, scale);
#endif


		}

		/*
		
			it is far less expensive to calculate whether a line with a definite endpoint intersects
			a bounds than a ray, 
		*/

		ID_FORCE_INLINE
			CS_SUPER_PURE
			static bool line_intersection(__m128 b0, __m128 b1, vec3_simd_t start, vec3_simd_t end) {
#if defined(DISABLE_VECTORIZED_BOUNDS_CODE)
			idBounds t;
			idVec3 s;
			idVec3 e;
			bounds_simd::store(t, b0, b1);
			s.from_vec4f(start);
			e.from_vec4f(end);
			return t.LineIntersection(s, e);
#else
			cs_check_vec3_simd(b0);
			cs_check_vec3_simd(b1);
			cs_check_vec3_simd(start);
			cs_check_vec3_simd(end);
			vec3_simd_t center = _mm_mul_ps(_mm_set_ps(.0f, .5f, .5f, 0.5f), _mm_add_ps(b0, b1));
			vec3_simd_t extents = _mm_sub_ps(b1, center);
			vec3_simd_t linedir = _mm_mul_ps(_mm_set_ps(.0f, .5f, .5f, 0.5f), _mm_sub_ps(end, start));
			vec3_simd_t linecenter = _mm_add_ps(start, linedir);
			vec3_simd_t dir = _mm_sub_ps(linecenter, center);

			vec3_simd_t abslinedir = vec3_simd::calc_abs_zero_ele4(linedir);
			vec3_simd_t absdir = vec3_simd::calc_abs_zero_ele4(dir);

			vec3_simd_t any_retfalse = _mm_cmpgt_ps(absdir, _mm_add_ps(extents, abslinedir));

			if (_mm_movemask_ps(any_retfalse) & 0b111) {
				return false;
			}

			vec3_simd_t crossed = vec3_simd::cross(linedir, dir);

			crossed = vec3_simd::calc_abs_zero_ele4(crossed);
#if 0
			__m256 ld_as_256 = _mm256_castps128_ps256(abslinedir);
			__m256 extents_as_256 = _mm256_castps128_ps256(extents);
			/*
				because ele 3 of ld is zero, the mulres should have 0 in the third element of each half
				and the sum will too
				crossed already has 0 in the top element, so it wont cmpgt to true,
			*/
			__m256 partial = _mm256_mul_ps(_mm256_permutevar8x32_ps(extents_as_256, _mm256_setr_epi32(1, 0, 0, 0, 2, 2, 1, 0)), _mm256_permutevar8x32_ps(ld_as_256, _mm256_setr_epi32(2, 2, 1, 3, 1, 0, 0, 3)));
			__m128 cmpmask = _mm_cmpgt_ps(crossed, _mm_add_ps(_mm256_castps256_ps128(partial), _mm256_extractf128_ps(partial, 1)));
#else
			//emulate the avx2/avx code
			Vec8f ld_as_256{ abslinedir,_mm_setzero_ps() };
			Vec8f extents_as_256{ extents, _mm_setzero_ps() };

			Vec8f partial = permute8< 1, 0, 0, 0, 2, 2, 1, 0>(extents_as_256)* permute8< 2, 2, 1, 3, 1, 0, 0, 3>(ld_as_256);

			__m128 cmpmask = _mm_cmpgt_ps(crossed, _mm_add_ps(partial.get_low(), partial.get_high()));

#endif
			if (!_mm_testz_ps(cmpmask, cmpmask)) {
				return false;
			}
			return true;
#endif
	}
		CS_FORCEINLINE
			static void axis_projection(__m128 b0, __m128 b1, vec3_simd_t dir, float& minout, float& maxout) {
#if defined(DISABLE_VECTORIZED_BOUNDS_CODE)

			idBounds b;
			bounds_simd::store(b, b0, b1);
			idVec3 dirt;
			dirt.from_vec4f(dir);
			b.AxisProjection(dirt, minout, maxout);

#else
			cs_check_vec3_simd(b0);
			cs_check_vec3_simd(b1);
			cs_check_vec3_simd(dir);

			vec3_simd_t center = _mm_mul_ps(_mm_add_ps(b0, b1), _mm_setr_ps(0.5f, 0.5f, 0.5f, .0f));

			vec3_simd_t extents = _mm_sub_ps(b1, center);
			float d1 = vec3_simd::tinydot(dir, center);
			dir = vec3_simd::calc_abs_zero_ele4(dir);
			extents = vec3_simd::calc_abs_zero_ele4(extents);
			float d2 = vec3_simd::tinydot(dir, extents);
			minout = d1 - d2;
			maxout = d1 + d2;
			cs_break_on_nan(minout);
			cs_break_on_nan(maxout);
#endif
		}
		CS_SUPER_PURE
			static inline
			__m128 get_radius_ss(__m128 b0, __m128 b1) {
#if !defined(DISABLE_VECTORIZED_BOUNDS_CODE)
			cs_check_vec3_simd(b0);
			cs_check_vec3_simd(b1);
			b0 = vec3_simd::calc_abs_zero_ele4(b0);
			b1 = vec3_simd::calc_abs_zero_ele4(b1);
			__m128 selecteds = _mm_max_ps(b0, b1);
			__m128 total = vec3_simd::tinydot_ss(selecteds, selecteds);
			return _mm_sqrt_ss(total);

#else
			idBounds rbounds{};
			rbounds[0].from_vec4f(b0);
			rbounds[1].from_vec4f(b1);
			return _mm_set_ss(rbounds.GetRadius());
#endif
		}
		CS_SUPER_PURE
			static inline
			float get_radius(__m128 b0, __m128 b1) {
			cs_check_vec3_simd(b0);
			cs_check_vec3_simd(b1);
			return _mm_cvtss_f32(get_radius_ss(b0, b1));
		}
		//if vec start pointing in normal is in bounds bx, return the opposite point within bounds (bottom left facing to lower left) = (top right)
		CS_SUPER_PURE
			static vec3_simd_t opposing_point_in_bounds(bounds_simd_t bx, vec3_simd_t start, vec3_simd_t normal) {

			float scale = 0.0f;
			__m128 radius = bounds_simd::get_radius_ss(bx.b0, bx.b1);


			if (!bounds_simd::ray_intersection(bx.b0, bx.b1, start, normal, scale)) {
				return start;
			}
			__m128 splatrad = cs_permute_ps<_MM_SHUFFLE(3, 0, 0, 0)>(radius);

			if (!bounds_simd::ray_intersection(bx.b0, bx.b1, _mm_add_ps(_mm_mul_ps(splatrad, normal), start), vec3_simd::neg_xyz(normal), scale)) {
				return start;
			}
			__m128 splatscale = cs_permute_ps<_MM_SHUFFLE(3, 0, 0, 0)>(_mm_set_ss(scale));
			return _mm_add_ps(start, _mm_mul_ps(normal, _mm_sub_ps(splatrad, splatscale)));
		}
}