#pragma once
enum class mh_configvar_type_t : unsigned char {
	INTEGER64,
	FLOAT64,
	STRING_VAL,
	

};
union mh_configpayload_t {
	long long* const m_ll;
	double* const m_dbl;
	char** const m_str;

	constexpr mh_configpayload_t(long long* ll) : m_ll(ll) {}
	constexpr mh_configpayload_t(double* dbl) : m_dbl(dbl) {}
	constexpr mh_configpayload_t(char** str) : m_str(str) {}
};



#pragma pack(push, 1)
struct mh_configvar_header_t {
	const mh_configvar_type_t m_type;
	const unsigned char m_flag_bit;
	const char* const m_key;
	const unsigned m_keylength;
	mh_configpayload_t m_payload;

	template<size_t nbytes_for_key>
	constexpr mh_configvar_header_t(long long* ll, const char(&key)[nbytes_for_key], unsigned char flag_bit=0xff) : m_type(mh_configvar_type_t::INTEGER64), m_key(&key[0]), m_keylength(nbytes_for_key - 1), m_payload(ll), m_flag_bit(flag_bit) {}

	template<size_t nbytes_for_key>
	constexpr mh_configvar_header_t(double* dbl, const char(&key)[nbytes_for_key]) : m_type(mh_configvar_type_t::FLOAT64), m_key(&key[0]), m_keylength(nbytes_for_key - 1), m_payload(dbl), m_flag_bit(0xff) {}

	template<size_t nbytes_for_key>
	constexpr mh_configvar_header_t(char** str, const char(&key)[nbytes_for_key]) : m_type(mh_configvar_type_t::STRING_VAL), m_key(&key[0]), m_keylength(nbytes_for_key - 1), m_payload(str), m_flag_bit(0xff) {}


	void set_payload(long long value) const {
		if (m_flag_bit != 0xFF) {

			uint64_t mask = 1ULL << m_flag_bit;

			if(value != 0LL) {
				*(m_payload.m_ll) |= mask;
			}
			else {
				*(m_payload.m_ll) &= ~mask;
			}

		}
		else
			*(m_payload.m_ll) = value;
	}
	void set_payload(double value) const {
		*(m_payload.m_dbl) = value;
	}
	void set_payload(char* value) const {
		*(m_payload.m_str) = value;
	}


};
#pragma pack(pop)

#define		MAX_CONFIGVAR_STRING_MEM		2048



void mh_configfile_load(
	const mh_configvar_header_t* cfgarr,
	unsigned nconfigvars);