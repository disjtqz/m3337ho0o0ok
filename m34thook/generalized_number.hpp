#include "mh_defs.hpp"
#include <cstdint>
#include <cmath>
/*
	created this for peeking/poking values so that we can have a unified value representation

	uses __m128 value, low part is integral part (which is unusual, yeah, but it's because we expect integers to be more common so we only have to do cvtsi32_si128 for them)
	and high part is fractional
*/

namespace mh_gennum::detail {
	static constexpr uint64_t SIGNBIT_DOUBLE = (1ULL << 63);

	MH_PURE
		static inline __m128i setr_epi64(std::uint64_t low, std::uint64_t high) {
		union {
			__m128i i64parts;
			std::uint64_t arrpart[2];
		}resval;

		resval.arrpart[0] = low;
		resval.arrpart[1] = high;
		return resval.i64parts;
	}
	MH_PURE
		static inline __m128d make_twod_int(std::uint64_t low, std::uint64_t high) {

		return 	_mm_castsi128_pd(setr_epi64(low, high));

	}

	MH_PURE
		static inline __m128d get_absmask_double() {
		return make_twod_int(SIGNBIT_DOUBLE, SIGNBIT_DOUBLE);
	}
	//static constexpr double CVTU2D_VAL = 4294967296.0;




	MH_PURE
		static inline __m128d floor_sse2(__m128d vals) {

		//actually, de requires sse4.1-sse4.2 so we can count on us having the floor insn

		return _mm_floor_pd(vals);

		//__m128d highpartsd = _mm_cvtepu32_pd()

	}
	MH_PURE
		static inline double floor_scalar(double vals) {
		return _mm_cvtsd_f64(_mm_floor_pd(_mm_set_sd(vals)));
	}
	template<typename T>
	static constexpr unsigned signbit_for = (sizeof(T) * CHAR_BIT) - 1;

	static constexpr unsigned DOUBLE_MAX_INT_PRECISION_BITS = 52;
	static constexpr unsigned DOUBLE_FRACT_FIXUP_SHIFT = signbit_for<uint64_t> -DOUBLE_MAX_INT_PRECISION_BITS;
}

namespace mh_gennum {


	struct alignas(32) num_t {
		std::uint64_t m_integral;
		double m_fractional;
		/*
			sign is 1 if position, -1 if negative

		*/
		std::ptrdiff_t m_sign;

		//this doubleword could probably store some info about how we should represent the value when stringifying it
		//if the field is a bitmask, it makes more sense to print it as one for the user
		std::uint64_t m_reserved;
		template<typename T, bool unsigned_typ = false>
		static num_t from_integral(T v) {
			num_t result;
			ptrdiff_t sign;
			ptrdiff_t vext = v;
			if constexpr (unsigned_typ) {
				sign = 1;
				vext &= ((1ULL << (sizeof(T) * CHAR_BIT)) - 1ULL);

			}
			else {
				sign = static_cast<ptrdiff_t>(vext >> detail::signbit_for<T>) | 1LL;

				vext = sign ? -vext : vext;

			}
			result.m_integral = static_cast<uint64_t>(vext);
			result.m_fractional = 0;
			result.m_sign = sign;
			result.m_reserved = 0;
			return result;
		}

		MH_HEADER_NOINLINE static num_t from_int64(std::int64_t v);
		MH_HEADER_NOINLINE static num_t from_int32(std::int32_t v);
		MH_HEADER_NOINLINE static num_t from_int16(std::int16_t v);
		MH_HEADER_NOINLINE static num_t from_int8(std::int8_t v);
		MH_HEADER_NOINLINE static num_t from_uint64(std::uint64_t v);
		MH_HEADER_NOINLINE static num_t from_uint32(std::uint32_t v);
		MH_HEADER_NOINLINE static num_t from_uint16(std::uint16_t v);
		MH_HEADER_NOINLINE static num_t from_uint8(std::uint8_t v);

		MH_HEADER_NOINLINE
			static num_t from_double(double d);

		static num_t from_float(float d) {
			return from_double(static_cast<double>(d));
		}
		double as_double_mag() const {
			double ipart = static_cast<double>(m_integral);

			double fpart = m_fractional;

			return (fpart + ipart);
		}

		double as_double() const {
			return as_double_mag() * m_sign;
		}

		int64_t as_int64() {
			return m_integral * m_sign;
		}
		double extract_fract_part() const {
			return m_fractional;
		}
		MH_HEADER_NOINLINE
			static num_t parse(const char* s, char** out_endptr = nullptr);

		void as_string(char* out_buffer, size_t maxchars) {

			if (m_sign < 0) {
				out_buffer[0] = '-';
				out_buffer++;
				maxchars--;
			}

			if (m_fractional == 0) {
				out_buffer += sprintf_s(out_buffer, maxchars, "%llu", m_integral);
			}
			else {

				sprintf_s(out_buffer, maxchars, "%f", as_double_mag());
			}
		}
	};
	MH_HEADER_NOINLINE
		num_t num_t::from_double(double d) {
		ptrdiff_t sign;

		sign = d < 0 ? -1LL : 1LL;

		d = sign < 0 ? -d : d;

		double dfloor = detail::floor_scalar(d);

		double dfract = d - dfloor;

		uint64_t dint = static_cast<int64_t>(dfloor);

		num_t result;
		result.m_integral = dint;
		result.m_fractional = dfract;
		result.m_sign = sign;
		result.m_reserved = 0;
		return result;

	}
	MH_HEADER_NOINLINE
		num_t num_t::parse(const char* s, char** out_endptr) {

		char* s_danger = const_cast<char*>(s);
		ptrdiff_t signres;
		std::uint64_t ipart;
		double fpart;
		char* s_int_end;




		if (s_danger[0] == '0' && (s_danger[1] == 'X' || s_danger[1] == 'x')) {
			s_danger += 2;

			ipart = std::strtoull(s_danger, &s_int_end, 16);
			fpart = 0;
			signres = 1ULL;
		}
		else {
			signres = (-static_cast<ptrdiff_t>(s_danger[0] == '-'));

			s_danger -= signres;
			signres |= 1;

			s_int_end = nullptr;
			ipart = std::strtoull(s_danger, &s_int_end, 10);
			fpart = 0;
			if (s_int_end[0] == '.') {
				//this works because strtod sees the . and interprets it as a fraction

				fpart = std::strtod(s_int_end, &s_int_end);

			}
			if (out_endptr) {

				*out_endptr = s_int_end;
			}


		}
		num_t result;
		result.m_integral = ipart;
		result.m_fractional = fpart;
		result.m_sign = signres;
		result.m_reserved = 0ULL;
		return result;
	}
	MH_HEADER_NOINLINE num_t num_t::from_int64(std::int64_t v) {
		return from_integral(v);
	}

	MH_HEADER_NOINLINE num_t num_t::from_int32(std::int32_t v) {
		return from_integral(v);
	}

	MH_HEADER_NOINLINE num_t num_t::from_int16(std::int16_t v) {
		return from_integral(v);
	}

	MH_HEADER_NOINLINE num_t num_t::from_int8(std::int8_t v) {
		return from_integral(v);
	}

	MH_HEADER_NOINLINE num_t num_t::from_uint64(std::uint64_t v) {
		return from_integral(v);
	}

	MH_HEADER_NOINLINE num_t num_t::from_uint32(std::uint32_t v) {
		return from_integral(v);
	}

	MH_HEADER_NOINLINE num_t num_t::from_uint16(std::uint16_t v) {
		return from_integral(v);
	}

	MH_HEADER_NOINLINE num_t num_t::from_uint8(std::uint8_t v) {
		return from_integral(v);
	}
}
