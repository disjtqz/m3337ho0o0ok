#pragma once


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



struct editor_vec3_t {
	union {
		struct {
			double x, y, z;
		};
		struct {
			__m128d xmmlo;
			__m128d xmmhi;
		};
	};


	editor_vec3_t() {
		xmmlo = xmmhi = _mm_setzero_pd();
	}


	editor_vec3_t(const idVec3* v) {
		*this = v;

	}
	editor_vec3_t(const idVec3& v) : editor_vec3_t(&v) {}
	editor_vec3_t(__m128d lowpart, __m128d highpart) : xmmlo(lowpart), xmmhi(highpart) {}


	editor_vec3_t(double _x, double _y, double _z) :xmmlo(_mm_setr_pd(_x, _y)), xmmhi(_mm_set_sd(_z)) {}
	editor_vec3_t(double dval) {
		xmmlo = _mm_set1_pd(dval);
		xmmhi = xmmlo;
	}

	editor_vec3_t& operator = (const idVec3& other) {

		return *this = &other;
	}

	editor_vec3_t& operator =(const idVec3* other) {
		float3_to_two_doublevecs(&other->x, xmmlo, xmmhi);
		return *this;
	}

	void set(double x, double y, double z) {
		xmmlo = _mm_setr_pd(x, y);
		xmmhi = _mm_set_sd(z);

	}

	void to_floats(float* out) const {
		two_doublevecs_to_3floats(xmmlo, xmmhi, out);



	}
	editor_vec3_t operator +(editor_vec3_t other)const {
		return editor_vec3_t(_mm_add_pd(xmmlo, other.xmmlo), _mm_add_pd(xmmhi, other.xmmhi));
	}
	editor_vec3_t operator -(editor_vec3_t other) const {
		return editor_vec3_t(_mm_sub_pd(xmmlo, other.xmmlo), _mm_sub_pd(xmmhi, other.xmmhi));
	}
	editor_vec3_t operator *(editor_vec3_t other) const {
		return editor_vec3_t(_mm_mul_pd(xmmlo, other.xmmlo), _mm_mul_pd(xmmhi, other.xmmhi));
	}
	editor_vec3_t operator /(editor_vec3_t other)const {
		return editor_vec3_t(_mm_div_pd(xmmlo, other.xmmlo), _mm_div_pd(xmmhi, other.xmmhi));
	}
	//helper func for dp sqrt
	static double do_sqrt(double val) {
		__m128d dv = _mm_set_sd(val);
		return _mm_cvtsd_f64(_mm_sqrt_sd(dv, dv));

	}

	editor_vec3_t squared()const {
		return *this * *this;
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


	editor_vec3_t normalized() const {


		double sqrlen = dot(*this);

		double sqrtval = do_sqrt(sqrlen);
		return *this / sqrtval;

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
	//names are chosen so as to not conflict with stupid min/max macros everyone still fucking define in the global scope
	editor_vec3_t minimum(editor_vec3_t other) const {
		__m128d vlo = _mm_min_pd(xmmlo, other.xmmlo);
		__m128d vhi = _mm_min_pd(xmmhi, other.xmmhi);

		return editor_vec3_t{ vlo, vhi };
	}


	editor_vec3_t maximum(editor_vec3_t other) const {
		__m128d vlo = _mm_max_pd(xmmlo, other.xmmlo);
		__m128d vhi = _mm_max_pd(xmmhi, other.xmmhi);

		return editor_vec3_t{ vlo, vhi };
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

	editor_vec3_t operator <(editor_vec3_t other) const {
		return editor_vec3_t{ _mm_cmplt_pd(xmmlo, other.xmmlo), _mm_cmplt_pd(xmmhi, other.xmmhi) };
	}
	editor_vec3_t operator >(editor_vec3_t other) const {
		return editor_vec3_t{ _mm_cmpgt_pd(xmmlo, other.xmmlo), _mm_cmpgt_pd(xmmhi, other.xmmhi) };
	}
	editor_vec3_t operator <=(editor_vec3_t other) const {
		
		return editor_vec3_t{ _mm_cmple_pd(xmmlo, other.xmmlo), _mm_cmple_pd(xmmhi, other.xmmhi) };
	}
	editor_vec3_t operator >=(editor_vec3_t other) const {
		return editor_vec3_t{ _mm_cmpge_pd(xmmlo, other.xmmlo), _mm_cmpge_pd(xmmhi, other.xmmhi) };
	}
	editor_vec3_t operator ==(editor_vec3_t other) const {
		return editor_vec3_t{ _mm_cmpeq_pd(xmmlo, other.xmmlo), _mm_cmpeq_pd(xmmhi, other.xmmhi) };
	}

	editor_vec3_t operator !=(editor_vec3_t other) const {
		return editor_vec3_t{ _mm_cmpneq_pd(xmmlo, other.xmmlo), _mm_cmpneq_pd(xmmhi, other.xmmhi) };
	}

	editor_vec3_t operator |(editor_vec3_t other) const {
		return editor_vec3_t{ _mm_or_pd(xmmlo, other.xmmlo), _mm_or_pd(xmmhi, other.xmmhi) };
	}
	editor_vec3_t operator ^(editor_vec3_t other) const {
		return editor_vec3_t{ _mm_xor_pd(xmmlo, other.xmmlo), _mm_xor_pd(xmmhi, other.xmmhi) };
	}
	editor_vec3_t operator &(editor_vec3_t other) const {
		return editor_vec3_t{ _mm_and_pd(xmmlo, other.xmmlo), _mm_and_pd(xmmhi, other.xmmhi) };
	}

	static editor_vec3_t from_u64s(uint64_t x, uint64_t y, uint64_t z) {
		return editor_vec3_t{ _mm_castsi128_pd(_mm_setr_epi64(_mm_cvtsi64_m64(x), _mm_cvtsi64_m64(y))), _mm_castsi128_pd(_mm_setr_epi64(_mm_cvtsi64_m64(z), _mm_cvtsi64_m64(0ULL))) };
	}
	/*
		create from a uint64 broadcasted to every element
		Z will also be set to value, unlike from_u64s
	*/
	static editor_vec3_t from_bcast_u64(uint64_t value) {
		__m128d result = _mm_castsi128_pd(_mm_set1_epi64x(value));
		return editor_vec3_t{ result, result };
	}

	editor_vec3_t absolute_value() const {
		return *this & from_bcast_u64(~(1ULL << 63));
	}
	//using this as a mask, take elements from taken_if_set where we have the mask set, and from taken_if_zero where not set
	editor_vec3_t select(editor_vec3_t taken_if_zero, editor_vec3_t taken_if_set) const {
		return editor_vec3_t{ blend_xmmd(xmmlo, taken_if_zero.xmmlo, taken_if_set.xmmhi), blend_xmmd(xmmhi, taken_if_zero.xmmhi, taken_if_set.xmmhi) };
	}
};
struct editor_mat3_t;

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
idMat3 editor_mat3_t::to_id()const {
	idMat3 result;
	for (unsigned i = 0; i < 3; ++i) {
		mat[i].to_floats(&result.mat[i].x);
	}
	return result;
}
editor_mat3_t editor_angles_t::to_mat3() const {
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
