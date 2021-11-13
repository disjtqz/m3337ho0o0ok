#pragma once


static void two_doublevecs_to_3floats(__m128d xmmlo, __m128d xmmhi, float* outputs) {
	__m128 xx = _mm_cvtpd_ps(xmmlo);
	__m128 xy = _mm_cvtpd_ps(xmmhi);

	outputs[0] = _mm_cvtss_f32(xx);
	outputs[1] = _mm_cvtss_f32(_mm_shuffle_ps(xx, xx, _MM_SHUFFLE(3, 2, 1, 1)));
	outputs[2] = _mm_cvtss_f32(xy);

}

static void float3_to_two_doublevecs(const float* values, __m128d& xmmlo, __m128d& xmmhi) {
	__m128 tmp = _mm_loadu_ps(values);

	__m128d cvtlo = _mm_cvtps_pd(tmp);
	__m128d cvthi = _mm_cvtps_pd(_mm_movehl_ps(tmp, tmp));

	xmmlo = cvtlo;
	xmmhi = cvthi;
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
		return x + y + z;
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

};
