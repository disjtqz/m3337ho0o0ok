union {
	struct {
		double x, y, z;
	};
	struct {
		__m128d xmmlo;
		__m128d xmmhi;
	};
};


editor_vec3_base_t() {
	xmmlo = xmmhi = _mm_setzero_pd();
}
editor_vec3_base_t(__m128d lowpart, __m128d highpart) : xmmlo(lowpart), xmmhi(highpart) {}
editor_vec3_base_t(double dval) {
	xmmlo = _mm_set1_pd(dval);
	xmmhi = xmmlo;
}
//names are chosen so as to not conflict with stupid min/max macros everyone still fucking define in the global scope
editor_vec3_base_t minimum(editor_vec3_base_t other) const {
	__m128d vlo = _mm_min_pd(xmmlo, other.xmmlo);
	__m128d vhi = _mm_min_pd(xmmhi, other.xmmhi);

	return editor_vec3_base_t{ vlo, vhi };
}


editor_vec3_base_t maximum(editor_vec3_base_t other) const {
	__m128d vlo = _mm_max_pd(xmmlo, other.xmmlo);
	__m128d vhi = _mm_max_pd(xmmhi, other.xmmhi);

	return editor_vec3_base_t{ vlo, vhi };
}

/*
	prefer these to directly accessing x, y and z, since these get
	them in the context of vector elements
*/
double get_x() const {
	return _mm_cvtsd_f64(xmmlo);
}
double get_y() const {
	return _mm_cvtsd_f64(_mm_unpackhi_pd(xmmlo, xmmlo));
}

double get_z() const {
	return _mm_cvtsd_f64(xmmhi);
}
editor_vec3_base_t operator <(editor_vec3_base_t other) const {
	return editor_vec3_base_t{ _mm_cmplt_pd(xmmlo, other.xmmlo), _mm_cmplt_pd(xmmhi, other.xmmhi) };
}
editor_vec3_base_t operator >(editor_vec3_base_t other) const {
	return editor_vec3_base_t{ _mm_cmpgt_pd(xmmlo, other.xmmlo), _mm_cmpgt_pd(xmmhi, other.xmmhi) };
}
editor_vec3_base_t operator <=(editor_vec3_base_t other) const {

	return editor_vec3_base_t{ _mm_cmple_pd(xmmlo, other.xmmlo), _mm_cmple_pd(xmmhi, other.xmmhi) };
}
editor_vec3_base_t operator >=(editor_vec3_base_t other) const {
	return editor_vec3_base_t{ _mm_cmpge_pd(xmmlo, other.xmmlo), _mm_cmpge_pd(xmmhi, other.xmmhi) };
}
editor_vec3_base_t operator ==(editor_vec3_base_t other) const {
	return editor_vec3_base_t{ _mm_cmpeq_pd(xmmlo, other.xmmlo), _mm_cmpeq_pd(xmmhi, other.xmmhi) };
}

editor_vec3_base_t operator !=(editor_vec3_base_t other) const {
	return editor_vec3_base_t{ _mm_cmpneq_pd(xmmlo, other.xmmlo), _mm_cmpneq_pd(xmmhi, other.xmmhi) };
}

editor_vec3_base_t operator |(editor_vec3_base_t other) const {
	return editor_vec3_base_t{ _mm_or_pd(xmmlo, other.xmmlo), _mm_or_pd(xmmhi, other.xmmhi) };
}
editor_vec3_base_t operator ^(editor_vec3_base_t other) const {
	return editor_vec3_base_t{ _mm_xor_pd(xmmlo, other.xmmlo), _mm_xor_pd(xmmhi, other.xmmhi) };
}
editor_vec3_base_t operator &(editor_vec3_base_t other) const {
	return editor_vec3_base_t{ _mm_and_pd(xmmlo, other.xmmlo), _mm_and_pd(xmmhi, other.xmmhi) };
}
editor_vec3_base_t operator +(editor_vec3_base_t other)const {
	return editor_vec3_base_t(_mm_add_pd(xmmlo, other.xmmlo), _mm_add_pd(xmmhi, other.xmmhi));
}
editor_vec3_base_t operator -(editor_vec3_base_t other) const {
	return editor_vec3_base_t(_mm_sub_pd(xmmlo, other.xmmlo), _mm_sub_pd(xmmhi, other.xmmhi));
}
editor_vec3_base_t operator *(editor_vec3_base_t other) const {
	return editor_vec3_base_t(_mm_mul_pd(xmmlo, other.xmmlo), _mm_mul_pd(xmmhi, other.xmmhi));
}
editor_vec3_base_t operator /(editor_vec3_base_t other)const {
	return editor_vec3_base_t(_mm_div_pd(xmmlo, other.xmmlo), _mm_div_pd(xmmhi, other.xmmhi));
}

/*
	create from a uint64 broadcasted to every element
	Z will also be set to value, unlike from_u64s
*/
static editor_vec3_base_t from_bcast_u64(uint64_t value) {
	__m128d result = _mm_castsi128_pd(_mm_set1_epi64x(value));
	return editor_vec3_base_t{ result, result };
}

editor_vec3_base_t absolute_value() const {
	return *this & from_bcast_u64(~(1ULL << 63));
}


//using this as a mask, take elements from taken_if_set where we have the mask set, and from taken_if_zero where not set
editor_vec3_base_t select(editor_vec3_base_t taken_if_zero, editor_vec3_base_t taken_if_set) const {
	return editor_vec3_base_t{ blend_xmmd(xmmlo, taken_if_zero.xmmlo, taken_if_set.xmmhi), blend_xmmd(xmmhi, taken_if_zero.xmmhi, taken_if_set.xmmhi) };
}


//helper func for dp sqrt
static double do_sqrt(double val) {
	return scalar_sqrt(val);

}

editor_vec3_base_t squared()const {
	return *this * *this;
}


vec3_simd_t to_vec3_simd() const {

	Vec4d tmp4d{ xmmlo, xmmhi };
	Vec4f asfs = to_float(tmp4d);

	return (__m128)asfs;


}