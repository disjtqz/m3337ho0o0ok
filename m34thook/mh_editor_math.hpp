#pragma once


//from trial and error, this seems to be the limit for positions. tested by teleporting here on xyz and firing grenades. grenades went off into the world in most directions, but towards advancing coordinates
//the projectiles freeze in limbo, flying forever
#define			MH_EDGE_OF_REALITY		2047
#define			FLOAT_SMALLEST_NON_DENORM		1.1754944e-38
/*
	all math is double precision and sse1/sse2 compatible

	could probably benefit from also using sse3 (_mm_hadd_pd , _mm_movedup_pd,_mm_addsub_pd,_mm_hsub_pd)
	well, eternal does require SSE4.1, but some people use the intel insn emulator to play on older hardware
	so id rather keep things pre sse4.1 for them

*/
static void two_doublevecs_to_3floats(__m128d xmmlo, __m128d xmmhi, float* outputs) {
	__m128 xx = _mm_cvtpd_ps(xmmlo);
	__m128 xy = _mm_cvtpd_ps(xmmhi);

	outputs[0] = _mm_cvtss_f32(xx);
	outputs[1] = _mm_cvtss_f32(_mm_shuffle_ps(xx, xx, _MM_SHUFFLE(3, 2, 1, 1)));
	outputs[2] = _mm_cvtss_f32(xy);

}

static void float3_to_two_doublevecs(const float* values, __m128d& xmmlo, __m128d& xmmhi) {
	//potentially unsafe load]
	//depends on whether we'll cross a page boundary or not
	__m128 tmp = _mm_loadu_ps(values);

	__m128d cvtlo = _mm_cvtps_pd(tmp);
	__m128d cvthi = _mm_cvtps_pd(_mm_movehl_ps(tmp, tmp));

	xmmlo = cvtlo;
	xmmhi = cvthi;
}
//order is meant to match xmm blend ops

static inline __m128d blend_xmmd(__m128d mask, __m128d taken_if_zero, __m128d taken_if_set) {

	return _mm_or_pd(_mm_and_pd(mask, taken_if_set), _mm_andnot_pd(mask, taken_if_zero));
}
//simple double precision vec3. 

static double scalar_sqrt(double val) {
	__m128d dv = _mm_set_sd(val);
	return _mm_cvtsd_f64(_mm_sqrt_sd(dv, dv));
}

static __m128d vector_sqrt(__m128d vals) {
	return _mm_sqrt_pd(vals);
}
static double newt_step(double y, double x2) {
	return y * (1.5 - (x2 * y * y));   
}
static __m128d newt_step(__m128d y, __m128d x2) {

	__m128d innerexpr = _mm_mul_pd(x2, _mm_mul_pd(y, y));

	return _mm_mul_pd(y, _mm_sub_pd(_mm_set1_pd(1.5), innerexpr));

}
static double scalar_rsqrt(double val) {
	double x2 = val / 2.0;

	double y = scalar_sqrt(val);

	y = 1.0 / y;

	y = newt_step(y, x2);
	return y;
}

static __m128d vector_rsqrt(__m128d vals) {

	__m128d x2 = _mm_div_pd(vals, _mm_set1_pd(2.0));

	__m128d y = vector_sqrt(vals);

	y = _mm_div_pd(_mm_set1_pd(1.0), y);

	y = newt_step(y, x2);
	return y;


}


struct editor_vec3_t {
#define editor_vec3_base_t		editor_vec3_t


	editor_vec3_base_t(const idVec3* v) {
		*this = v;

	}
	editor_vec3_base_t(const idVec3& v) : editor_vec3_base_t(&v) {}
	


	editor_vec3_base_t(double _x, double _y, double _z) :xmmlo(_mm_setr_pd(_x, _y)), xmmhi(_mm_set_sd(_z)) {}


	editor_vec3_base_t& operator = (const idVec3& other) {

		return *this = &other;
	}

	editor_vec3_base_t& operator =(const idVec3* other) {
		float3_to_two_doublevecs(&other->x, xmmlo, xmmhi);
		return *this;
	}

	void set(double x, double y, double z) {
		xmmlo = _mm_setr_pd(x, y);
		xmmhi = _mm_set_sd(z);

	}
#include "xmacro_vec3_base.hpp"

#undef editor_vec3_base_t
	void to_floats(float* out) const {
		two_doublevecs_to_3floats(xmmlo, xmmhi, out);



	}


	double hsum() const {

		__m128d tmp = _mm_add_sd(xmmlo, xmmhi);

		return _mm_cvtsd_f64(_mm_add_sd(tmp, _mm_unpackhi_pd(tmp, tmp)));

		//return x + y + z;
	}

	double distance3d(editor_vec3_t other) const {

		double sum = (*this - other).squared().hsum();
		return do_sqrt(sum);
	}


	double dot(editor_vec3_t other) const {

		return (*this * other).hsum();

	}

	std::string to_string() const {

		return std::to_string(get_x()) + " " + std::to_string(get_y()) + " " + std::to_string(get_z());

	}

	double normalize() {
		double sqrlen = dot(*this);

		/*double sqrtval = do_sqrt(sqrlen);
		return *this / sqrtval;*/

		double rsqr = scalar_rsqrt(sqrlen);

		*this = *this * rsqr;
		return rsqr * sqrlen;

	}
	editor_vec3_t normalized() const {

		editor_vec3_t b = *this;
		b.normalize();
		return b;

	}

	operator idVec3() const {

		idVec3 res;
		to_floats(&res.x);
		return res;
	}

	idVec3 to_id() const {
		idVec3 res;
		to_floats(&res.x);
		return res;
	}

	editor_vec3_t reorder_YZX() const {
		editor_vec3_t result;

		return editor_vec3_t{ _mm_move_sd(xmmlo, xmmhi), xmmlo };

	}

	editor_vec3_t reorder_ZXY() const {
		__m128d lowpart = _mm_unpacklo_pd(xmmhi, xmmlo);
		__m128d highpart = _mm_unpackhi_pd(xmmlo, xmmlo);
		return editor_vec3_t{ lowpart, highpart };
	}

	/*
		broadcast one element to the 3 values and set value 4 to zero
	*/
	editor_vec3_t splat3_x() const {

		__m128d r1 = _mm_unpacklo_pd(xmmlo, xmmlo);

		__m128d zhi = _mm_setzero_pd();

		__m128d r2 = _mm_move_sd(zhi, xmmlo);

		return editor_vec3_t{ r1, r2 };
	}

	editor_vec3_t splat3_y() const {
		__m128d r1 = _mm_unpackhi_pd(xmmlo, xmmlo);

		__m128d zhi = _mm_setzero_pd();

		__m128d r2 = _mm_unpackhi_pd(xmmlo, zhi);

		return editor_vec3_t{ r1, r2 };

	}
	editor_vec3_t splat3_z() const {
		__m128d r1 = _mm_unpacklo_pd(xmmhi, xmmhi);

		__m128d zhi = _mm_setzero_pd();

		__m128d r2 = _mm_move_sd(zhi, xmmhi);

		return editor_vec3_t{ r1, r2 };
	}
	//like splat, but does not zero element 3. makes no guarantees of e3's status
	editor_vec3_t bcast_x() const {

		__m128d r1 = _mm_unpacklo_pd(xmmlo, xmmlo);

		return editor_vec3_t{ r1, r1 };
	}
	editor_vec3_t bcast_y() const {

		__m128d r1 = _mm_unpackhi_pd(xmmlo, xmmlo);

		return editor_vec3_t{ r1, r1 };
	}

	editor_vec3_t bcast_z() const {

		__m128d r1 = _mm_unpacklo_pd(xmmhi, xmmhi);

		return editor_vec3_t{ r1, xmmhi };
	}



	editor_vec3_t cross(editor_vec3_t b) const {

		editor_vec3_t p1 = this->reorder_YZX() * b.reorder_ZXY();

		editor_vec3_t p2 = this->reorder_ZXY() * b.reorder_YZX();

		return p1 - p2;
	}



	static editor_vec3_t from_u64s(uint64_t x, uint64_t y, uint64_t z) {
		return editor_vec3_t{ _mm_castsi128_pd(_mm_setr_epi64(_mm_cvtsi64_m64(x), _mm_cvtsi64_m64(y))), _mm_castsi128_pd(_mm_setr_epi64(_mm_cvtsi64_m64(z), _mm_cvtsi64_m64(0ULL))) };
	}

	editor_vec3_t precise_normalized()const {

		double m_valtemp[3];
		m_valtemp[0] = get_x();
		m_valtemp[1] = get_y();
		m_valtemp[2] = get_z();

		sh::math::precise_normalize3d(m_valtemp);
		return editor_vec3_t{ m_valtemp[0], m_valtemp[1], m_valtemp[2] };
	}



};


struct editor_normal_t {
#define editor_vec3_base_t	editor_normal_t

#include "xmacro_vec3_base.hpp"

#undef editor_vec3_base_t

	editor_normal_t(editor_vec3_t v) {

		double vnorm = v.normalize();
		xmmlo = v.xmmlo;
		xmmhi = _mm_unpacklo_pd(v.xmmhi, _mm_set_sd(vnorm));
	}
	

	/*
		returns a value ranging from 0 to 1 that represents how equal the two vectors
		probably pretty slo

		
	*/
	double fuzz(editor_normal_t other) {


		editor_normal_t addend{ _mm_set1_pd(1.0), _mm_set_sd(1.0) };

		//change range from -1.0 to 1.0 into 0.0 to 2.0
		editor_normal_t tposrange = *this + addend;
		editor_normal_t oposrange = other + addend;

		editor_normal_t ubound = tposrange.maximum(oposrange);
		editor_normal_t lbound = tposrange.minimum(oposrange);
		editor_normal_t presum = lbound / ubound;

		
		__m128d presumd = _mm_add_pd(presum.xmmlo, presum.xmmhi);

		__m128d totalsum = _mm_add_sd(_mm_unpackhi_pd(presumd, presumd), presumd);
		//average of 4 components fuzzed
		return _mm_cvtsd_f64(totalsum) / 4.0;

	}
};

//a single soa iter 
class editor_vec3_soa_t {
	__m128d m_xvalues;
	__m128d m_yvalues;
	__m128d m_zvalues;

	template<typename TCallable>
	editor_vec3_soa_t _binary_op(TCallable&& cb, editor_vec3_soa_t vsoa) const {

		editor_vec3_soa_t result;
		result.m_xvalues = cb(m_xvalues, vsoa.m_xvalues);
		result.m_yvalues = cb(m_yvalues, vsoa.m_yvalues);
		result.m_zvalues = cb(m_zvalues, vsoa.m_zvalues);
		return result;
	}

public:
	editor_vec3_soa_t() : m_xvalues(_mm_setzero_pd()), m_yvalues(_mm_setzero_pd()), m_zvalues(_mm_setzero_pd()) {}
	//broadcast components from vec3 to both doubles of each component vector
	editor_vec3_soa_t(editor_vec3_t v) : editor_vec3_soa_t() {

		m_xvalues = v.bcast_x().xmmlo;

		m_yvalues = v.bcast_y().xmmlo;
		m_zvalues = v.bcast_z().xmmlo;

	}

	

#define	BINOPER_FROM_XMM(op, xmfunc)		\
	editor_vec3_soa_t operator op (editor_vec3_soa_t other) const {\
		return _binary_op(xmfunc, other);\
	}
	void load(unsigned index, const double* x, const double* y, const double* z) {
		m_xvalues = _mm_load_pd(x + index);
		m_yvalues = _mm_load_pd(y + index);
		m_zvalues = _mm_load_pd(z + index);
	}
	void store(unsigned index, double* x, double* y, double* z) {
		_mm_store_pd(x + index, m_xvalues);
		_mm_store_pd(y + index, m_yvalues);
		_mm_store_pd(z + index, m_zvalues);

	}

	
	BINOPER_FROM_XMM(+, _mm_add_pd);


	BINOPER_FROM_XMM(-, _mm_sub_pd);

	BINOPER_FROM_XMM(*, _mm_mul_pd);

	BINOPER_FROM_XMM(/, _mm_div_pd);

	BINOPER_FROM_XMM(< , _mm_cmplt_pd);
	BINOPER_FROM_XMM(<= , _mm_cmple_pd);
	BINOPER_FROM_XMM(== , _mm_cmpeq_pd);
	BINOPER_FROM_XMM(!=, _mm_cmpneq_pd);
	BINOPER_FROM_XMM(>, _mm_cmpgt_pd);
	BINOPER_FROM_XMM(>= , _mm_cmpge_pd);

	BINOPER_FROM_XMM(&, _mm_and_pd);
	BINOPER_FROM_XMM(| , _mm_or_pd);
	BINOPER_FROM_XMM(^, _mm_xor_pd);

#define	BINFN_FROM_XMM(op, xmfunc)		\
	editor_vec3_soa_t op(editor_vec3_soa_t other) const {\
		return _binary_op(xmfunc, other);\
	}

	BINFN_FROM_XMM(maximum, _mm_max_pd);
	BINFN_FROM_XMM(minimum, _mm_min_pd);

	__m128d hsum() const {
		return _mm_add_pd(m_xvalues, _mm_add_pd(m_yvalues, m_zvalues));
	}

	__m128d dot(editor_vec3_soa_t other) const {
		return (*this + other).hsum();
	}



	editor_vec3_soa_t normalized() const {
		__m128d sqrlen = dot(*this);

		
		__m128d sqr2 = vector_rsqrt(sqrlen);

		
		editor_vec3_soa_t result;
		result.m_xvalues = _mm_mul_pd(m_xvalues, sqr2);
		result.m_yvalues = _mm_mul_pd(m_yvalues, sqr2);
		result.m_zvalues = _mm_mul_pd(m_zvalues, sqr2);
		return result;
	}	

	__m128d distance3d(editor_vec3_soa_t other) const {
		editor_vec3_soa_t tmpdiff = *this - other;
		tmpdiff = tmpdiff * tmpdiff;

		return vector_sqrt(tmpdiff.hsum());

	}


};

struct editor_mat3_t;
enum class angle_component_e : unsigned {
	PITCH,
	YAW,
	ROLL
};
struct editor_angles_t {
	union {
		struct {
			double pitch;
			double yaw;
			double roll;
			double padding;
		};
		struct {
			__m128d xmmlo;
			__m128d xmmhi;
		};
	};
	void set_from_idangles(idAngles& a) {

		float3_to_two_doublevecs(&a.pitch, xmmlo, xmmhi);

	}
	editor_angles_t() : xmmlo(_mm_setzero_pd()), xmmhi(_mm_setzero_pd()) {

	}

	editor_mat3_t to_mat3() const;

	editor_angles_t(idAngles& ang) {
		float3_to_two_doublevecs(&ang.pitch, xmmlo, xmmhi);
	}
	editor_angles_t(double d) : xmmlo(_mm_set1_pd(d)), xmmhi(_mm_set1_pd(d)) {

	}
	editor_angles_t(double p, double y, double r) : xmmlo(_mm_setr_pd(p, y)), xmmhi(_mm_set_sd(r)) {

	}

	double* get_component_ptr(angle_component_e cmp) {
		switch (cmp) {
		case angle_component_e::PITCH:
			return &pitch;
		case angle_component_e::YAW:
			return &yaw;
		case angle_component_e::ROLL:
			return &roll;
		}

		cs_assume_m(false);
	}

	idAngles to_id() const {
		idAngles result;
		two_doublevecs_to_3floats(xmmlo, xmmhi, &result.pitch);
		return result;
	}
};



/*
idMat3 idAngles::ToMat3() const
{
	idMat3 mat;
	float sr, sp, sy, cr, cp, cy;


	idMath::SinCos( DEG2RAD( yaw ), sy, cy );
	idMath::SinCos( DEG2RAD( pitch ), sp, cp );
	idMath::SinCos( DEG2RAD( roll ), sr, cr );

	mat.mat[ 0 ].Set( cp * cy, cp * sy, -sp );
	mat.mat[ 1 ].Set( sr * sp * cy + cr * -sy, sr * sp * sy + cr * cy, sr * cp );
	mat.mat[ 2 ].Set( cr * sp * cy + -sr * -sy, cr * sp * sy + -sr * cy, cr * cp );

	return mat;
}

*/


struct editor_mat3_t {
	editor_vec3_t mat[3];

	idMat3 to_id()const;
};
inline idMat3 editor_mat3_t::to_id()const {
	idMat3 result;
	for (unsigned i = 0; i < 3; ++i) {
		mat[i].to_floats(&result.mat[i].x);
	}
	return result;
}
inline editor_mat3_t editor_angles_t::to_mat3() const {
	editor_mat3_t mat;
	double sr, sp, sy, cr, cp, cy;
	using namespace sh::math;


	sincos(DEG2RAD(yaw), sy, cy);
	sincos(DEG2RAD(pitch), sp, cp);
	sincos(DEG2RAD(roll), sr, cr);

	mat.mat[0].set(cp * cy, cp * sy, -sp);
	mat.mat[1].set(sr * sp * cy + cr * -sy, sr * sp * sy + cr * cy, sr * cp);
	mat.mat[2].set(cr * sp * cy + -sr * -sy, cr * sp * sy + -sr * cy, cr * cp);

	return mat;


}
//bounding box
struct editor_bounds_t {
	editor_vec3_t b[2];

	
	

	editor_vec3_t center() const {

		return (b[0] + b[1]) * 0.5;
	}


};
