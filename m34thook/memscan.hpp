#pragma once
#include "mh_defs.hpp"

#include <cstring>
#include <cstdint>
#include "game_exe_interface.hpp"
#include <cassert>
#include <cstdio>

#include <thread>
#include <atomic>
#include <tuple>
#include <mutex>
#include <array>
/*
	typical l1 size is 32k
	but we'll probably lose a chunk of that while executing
	16k gives the best performance
*/
#define		SCANNER_WORKGROUP_BLOCK_SIZE		(16*1024u)
#define		DONT_FLUSH_MEM
#define		DISABLE_PREFETCHING_SCANNER_VARIANTS		
#define		ASSERT_ALL_LOCATED

#define		SCANGROUP_CALL_UNROLL		6
/*
	diminishing returns past 2 unrolls, at a certain point it becomes less performant
	due to icache running out and the uop cache not having enough space
*/
#define		SCAN_UNROLL_AMNT		2
#define			BEHAVIOR_PURE		MH_PURE
struct scanstate_t {
	blamdll_t* const dll;
	unsigned char* addr;
	uintptr_t backref_position;
	unsigned char* backref_stack[8];

	scanstate_t(unsigned char* _addr, blamdll_t* _dll) :  dll(_dll), addr(_addr),backref_position(0) {}

	inline scanstate_t& operator = (scanstate_t& other) {
		addr = other.addr;
		backref_position = other.backref_position;
#pragma clang unroll(8)
		for(unsigned i = 0; i < 8; ++i)
			backref_stack[i]=other.backref_stack[i];
		return *this;

	}
};
enum class memsection_e {
	any,
	text,
	rdata,
	data
};


template<memsection_e section, typename TTest>
MH_FORCEINLINE
static inline bool test_section_address(blamdll_t* dll, TTest t) {
	return dll->is_in_image(t);

}

MH_FLATTEN
MH_FORCEINLINE

static inline bool scancmp_bytes(const unsigned char* state, const unsigned char* values, size_t nvalues) {
#if 1
	unsigned i = 0;

#ifdef __AVX512BW__
	while((i+64) < nvalues) {
		__m512i addri = _mm512_loadu_si512((state+i));
		__m512i vali = _mm512_loadu_si512((values + i));

		if(_mm512_cmpeq_epi8_mask(addri, vali) != (~0ULL)) {
			return false;
		}
		i += 64;
	}
#endif
#ifdef __AVX2__
	
	while ((i + 32) < nvalues) {
		__m256i addri = _mm256_loadu_si256((const __m256i*)(state + i));
		__m256i vali = _mm256_loadu_si256((const __m256i*)(values + i));

		if (_mm256_movemask_epi8(_mm256_cmpeq_epi8(addri, vali)) != 0xFFFFFFFF)
			return false;
		i += 32;
	}
#endif

	while ((i + 16) < nvalues) {
		__m128i addri = _mm_loadu_si128((const __m128i*)(state + i));
		__m128i vali = _mm_loadu_si128((const __m128i*)(values + i));

		if (_mm_movemask_epi8(_mm_cmpeq_epi8(addri, vali)) != 0xFFFF)
			return false;
		i += 16;
	}

	while ((i + 8) < nvalues) {
		uint64_t addri = *reinterpret_cast<const uint64_t*>(state + i);
		uint64_t vali = *reinterpret_cast<const uint64_t*>(values + i);
		if (addri != vali)
			return false;

		i += 8;
	}

	while ((i + 4) < nvalues) {
		uint32_t addri = *reinterpret_cast<const uint32_t*>(state + i);
		uint32_t vali = *reinterpret_cast<const uint32_t*>(values + i);
		if (addri != vali)
			return false;

		i += 4;
	}

	while ((i + 2) < nvalues) {
		uint32_t addri = *reinterpret_cast<const uint16_t*>(state + i);
		uint32_t vali = *reinterpret_cast<const uint16_t*>(values + i);
		if (addri != vali)
			return false;

		i += 2;
	}
	while (i < nvalues) {
		uint32_t addri = *reinterpret_cast<const uint8_t*>(state + i);
		uint32_t vali = *reinterpret_cast<const uint8_t*>(values + i);
		if (addri != vali)
			return false;

		i += 1;
	}
	return true;
#else
	return !memcmp(state, values, nvalues);
#endif
}


template<unsigned char... bs>
struct scanbytes {
	static constexpr unsigned char values[] = { bs... };
	static constexpr unsigned required_valid_size = sizeof...(bs);
	static constexpr unsigned matched_size = sizeof...(bs);


	MH_FORCEINLINE
		static bool match(scanstate_t& state) {

#if 1
		if constexpr (sizeof(values) == 1) {
			constexpr unsigned char cmpval = values[0];

			bool res = *state.addr == cmpval;
			state.addr += 1;

			return res;
		}
		else if constexpr (sizeof(values) == 2) {
			constexpr unsigned short cmpval = (((unsigned short)values[1]) << 8) | ((unsigned short)values[0]);

			bool res = *(unsigned short*)state.addr == cmpval;
			state.addr += 2;

			return res;
		}
		else if constexpr (sizeof(values) == 3) {
			constexpr unsigned short cmpval1 = (((unsigned short)values[1]) << 8) | ((unsigned short)values[0]);
			constexpr unsigned char cmpval2 = values[2];

			bool res = (*(unsigned short*)state.addr == cmpval1) & (cmpval2 == state.addr[2]);


			state.addr += 3;
			return res;
		}
		else if constexpr (sizeof(values) == 4) {
			constexpr unsigned cmpval = ((unsigned)values[0]) | (((unsigned)values[1]) << 8) |
				(((unsigned)values[2]) << 16) | (((unsigned)values[3]) << 24);

			bool res = *((unsigned*)state.addr) == cmpval;
			state.addr += 4;
			return res;
		}
		else {


			if (scancmp_bytes(state.addr, values, sizeof...(bs))) {
				//if(scancmp_bytes<0, bs...>(state)) {
				state.addr += sizeof...(bs);
				return true;
			}
		}
#else
		if (!memcmp((const void*)state.addr, (const void*)values, sizeof...(bs))) {
			state.addr += sizeof...(bs);
			return true;
		}
#endif
		return false;
	}

};

template<void** to>
struct match_riprel32_to {

	static constexpr unsigned required_valid_size = 4;

	MH_FORCEINLINE
		static bool match(scanstate_t& state) {
		if (!state.dll->is_in_image(state.addr))
			return false;
		ptrdiff_t df = *reinterpret_cast<int*>(state.addr);

		if (*to != (void*)(state.addr + 4 + df)) {
			return false;
		}

		state.addr += 4;
		return true;
	}
};

template<unsigned char... data>
struct riprel32_data_equals {
	static constexpr unsigned char values[] = { data... };

	static constexpr unsigned required_valid_size = 4;
	MH_FORCEINLINE
		static bool match(scanstate_t& state) {
		unsigned char* base = state.addr + 4;

		signed displ = *(signed*)state.addr;
		base += displ;

		if ((char*)base < state.dll->image_base || (((char*)base) + sizeof...(data)) >(state.dll->image_base + state.dll->image_size))
			return false;
		state.addr += 4;

		return scancmp_bytes(base, values, sizeof...(data));//!memcmp((const void*)base, (const void*)(&values[0]), sizeof...(data));
	}

};

template<typename recursive_pat>
struct match_calltarget_riprel32_recursive {
	static constexpr unsigned required_valid_size = 4;
	static bool match(scanstate_t& state) {

		unsigned char* base = state.addr + 4;

		signed displ = *(signed*)state.addr;
		base += displ;


		if ((char*)base < state.dll->text_base || (char*)base >(state.dll->text_base + state.dll->text_size))
			return false;

		scanstate_t recurse_state{ base, state.dll };


		bool result = recursive_pat::match(recurse_state);

		state.addr += 4;
		return result;
	}
};

template<unsigned N>
struct skip {
	static constexpr unsigned required_valid_size = N;
	MH_FORCEINLINE
		static bool match(scanstate_t& state) {
		state.addr += N;
		return true;
	}
};
template<unsigned N>
struct skip_and_push {
	static constexpr unsigned required_valid_size = N;
	MH_FORCEINLINE
		static bool match(scanstate_t& state) {
		state.backref_stack[state.backref_position++] = state.addr;

		state.addr += N;
		return true;
	}
};
template<void** rva_out>
struct skip_and_capture_rva {
	//static inline T value{};
	static constexpr unsigned required_valid_size = 4;
	MH_FORCEINLINE
		static bool match(scanstate_t& state) {


		ptrdiff_t df = *reinterpret_cast<int*>(state.addr);

		
		store8_no_cache(rva_out, (void*)(state.addr + 4 + df));

		//*rva_out = (void*)(state.addr + 4 + df);

		state.addr += 4;
		return true;
	}
};

template<unsigned* number_out>
struct skip_and_capture_4byte_value {
	static constexpr unsigned required_valid_size = 4;
	MH_FORCEINLINE
		static bool match(scanstate_t& state) {

		//*number_out = *reinterpret_cast<unsigned*>(state.addr);


		store4_no_cache(number_out, *reinterpret_cast<unsigned*>(state.addr));
		state.addr += 4;
		return true;
	}
};
template<unsigned* number_out>
struct skip_and_capture_1byte_value {
	static constexpr unsigned required_valid_size = 4;
	MH_FORCEINLINE
		static bool match(scanstate_t& state) {

		*number_out = *state.addr;

		state.addr += 1;
		return true;
	}
};
template<unsigned n>
struct align_next {
	static constexpr unsigned required_valid_size = n;
	MH_FORCEINLINE
		static bool match(scanstate_t& state) {
		state.addr += (n - 1);
		state.addr = (unsigned char*)(((uintptr_t)state.addr) & ~(uintptr_t)(n - 1));

		return true;
	}
};

template<unsigned stackdelta, unsigned nbytes>
struct cmp_backref {
	static constexpr unsigned required_valid_size = nbytes;
	MH_FORCEINLINE
	static bool match(scanstate_t& state) {

		
		if(!scancmp_bytes(state.backref_stack[state.backref_position - stackdelta], state.addr, nbytes ))
			return false;

		state.addr+=nbytes;

		return true;
	}
};

template<typename T, T mask, T expected_value>
struct masked_eq_value {
	static constexpr unsigned required_valid_size = sizeof(T);
	MH_FORCEINLINE
		static bool match(scanstate_t& state) {
		T bval = *(T*)(&state.addr[0]);
		if ((bval & mask) != expected_value)
			return false;
		state.addr += sizeof(T);
		return true;
	}
};
/*
	checks if current byte & mask == expected_value
*/
template<unsigned char mask, unsigned char expected_value>
using masked_eq_byte = masked_eq_value<unsigned char, mask, expected_value>;

template<unsigned  mask, unsigned  expected_value>
using masked_eq_u32 = masked_eq_value<unsigned, mask, expected_value>;



enum memscanner_flags_e {
	_test_mapped_displ_in_image = 1,

};
template<uint64_t scanner_flags>
struct memscanner_factory_t {
	template<typename... Ts>
	struct memscanner_t {

		template<typename TCurr, typename... TRest>
		MH_FORCEINLINE
			static bool test_addr(scanstate_t& state) {

			if (!TCurr::match(state)) {
				return false;
			}

			if constexpr (sizeof...(TRest) != 0) {
				return test_addr<TRest...>(state);
			}
			return true;

		}
		template<typename TCurr, typename... TRest>
		static constexpr unsigned compute_overall_required_mapped_bytes() {
			unsigned v = TCurr::required_valid_size;
			if constexpr (sizeof...(TRest) != 0) {
				return v + compute_overall_required_mapped_bytes<TRest...>();
			}
			return v;
		}

		static constexpr unsigned required_mapped_bytes = compute_overall_required_mapped_bytes<Ts...>();

		MH_FORCEINLINE
			static bool match(scanstate_t& state) {
			if constexpr ((scanner_flags & _test_mapped_displ_in_image) != 0) {
				if (!state.dll->is_in_image(state.addr + required_mapped_bytes))
					return false;
			}
			return test_addr<Ts...>(state);
		}
	};
};
/*
	match another memscanner_t within up to n bytes after the current address
	added for locate_game_engine which had bytes removed before it
*/
template<unsigned within_n_bytes, typename subscanner>
struct match_within_variable_distance {
	static constexpr unsigned required_valid_size = within_n_bytes + subscanner::required_mapped_bytes;
	MH_FORCEINLINE
		static bool match(scanstate_t& state) {


		for (unsigned i = 0; i < within_n_bytes; ++i) {

			scanstate_t substate = state;
			if (subscanner::match(substate)) {
				state = substate;
				return true;
			}
			state.addr++;
		}

		return false;


	}
};

void* scanner_late_get_cvar(const char* s);
unsigned scanner_late_get_struct_size(const char* s);
MH_NOINLINE
unsigned scanner_late_get_field_offset(const char* cls, const char* fld);
void* scanner_late_get_eventdef(const char* name);
/*
	late-stage scanner types
*/
template<typename TYuckyStr>
struct late_riprel_to_cvar_data {

	static constexpr const char* parm = yuckystring_str_from_type_m(TYuckyStr);


	static inline void* g_cvarloc = nullptr;
	static constexpr unsigned required_valid_size = 4;

	MH_NOINLINE
	MH_REGFREE_CALL
	static void init_cvar() {
		g_cvarloc = scanner_late_get_cvar(parm);
	}

	MH_FORCEINLINE
	static bool match(scanstate_t& state) {
		MH_UNLIKELY_IF(!g_cvarloc) {
			init_cvar();
		}
		if (!state.dll->is_in_image(state.addr))
			return false;
		ptrdiff_t df = *reinterpret_cast<int*>(state.addr);

		if (g_cvarloc != (void*)(state.addr + 4 + df)) {
			return false;
		}

		state.addr += 4;
		return true;


	}
};


#define		late_riprel_to_cvar_data_m(...)		late_riprel_to_cvar_data<yuckystring_m(__VA_ARGS__)>

template<typename TYuckyStr>
struct late_riprel_to_eventdef {

	static constexpr const char* parm = yuckystring_str_from_type_m(TYuckyStr);


	static inline void* g_evloc = nullptr;
	static constexpr unsigned required_valid_size = 4;

	//copy and paste, idc. should be a generic late get template with the implementation just calling the init func and the match being shared between them
	MH_NOINLINE
		MH_REGFREE_CALL
		static void init_cvar() {
		g_evloc = scanner_late_get_eventdef(parm);
	}

	MH_FORCEINLINE
		static bool match(scanstate_t& state) {
		MH_UNLIKELY_IF(!g_evloc) {
			init_cvar();
			if (!g_evloc) {
				mh_error_message("Wasnt able to find eventdef %s", parm);
			}
		}
		if (!state.dll->is_in_image(state.addr))
			return false;
		ptrdiff_t df = *reinterpret_cast<int*>(state.addr);

		if (g_evloc != (void*)(state.addr + 4 + df)) {
			return false;
		}

		state.addr += 4;
		return true;


	}
};

#define		late_riprel_to_eventdef_m(...)		late_riprel_to_eventdef<yuckystring_m(__VA_ARGS__)>

/*
	another late stage scanner type, must run after phase2
*/
template<typename T, /*const char (*const strparm)()*/ typename TStr>
struct late_match_sizeof_type {
	
	static constexpr const char* parm = yuckystring_str_from_type_m(TStr);

	static inline unsigned g_structsize = 0;
	static constexpr unsigned required_valid_size = sizeof(T);

	MH_NOINLINE
	MH_REGFREE_CALL
	static void init_cvar() {
		g_structsize = scanner_late_get_struct_size(parm);
	}

	MH_FORCEINLINE
	static bool match(scanstate_t& state) {
		MH_UNLIKELY_IF(!g_structsize) {
			init_cvar();
		}
		
		if((*reinterpret_cast<T*>(state.addr)) != g_structsize) {
			return false;
		}

		state.addr+=sizeof(T);
		return true;


	}
};

#define		match_sizeof_type_m(size_type, ...)			late_match_sizeof_type<size_type, yuckystring_m(__VA_ARGS__)>


#define		late_riprel_to_eventdef_m(...)		late_riprel_to_eventdef<yuckystring_m(__VA_ARGS__)>


//only supports one field, no nested fields
template<typename TValue, typename TClassname, typename Targs>
struct late_match_field_offset {

	static constexpr const char* cls = yuckystring_str_from_type_m(TClassname);
	static constexpr const char* fld = yuckystring_str_from_type_m(Targs);
	//dont use zero as guard because the field might actually be at zero
	static inline unsigned g_fieldsize = ~0u;
	static constexpr unsigned required_valid_size = sizeof(TValue);

	MH_NOINLINE
		MH_REGFREE_CALL
		static void init_cvar() {
		g_fieldsize = scanner_late_get_field_offset(cls, fld);
	}

	MH_FORCEINLINE
		static bool match(scanstate_t& state) {
		MH_UNLIKELY_IF(!~g_fieldsize) {
			init_cvar();
		}

		if ((*reinterpret_cast<TValue*>(state.addr)) != g_fieldsize) {
			return false;
		}

		state.addr += sizeof(TValue);
		return true;


	}

};
#define		match_fieldoffs_m(size_type, cls, fld)			late_match_field_offset<size_type, yuckystring_m(cls), yuckystring_m(fld)>

using workgroup_result_t = void*;


using worker_match_behavior_t = workgroup_result_t(*)(unsigned displacement);

template<typename... Ts>
using memscanner_t = typename memscanner_factory_t<0>::template  memscanner_t<Ts...>;

template<uint64_t flags, typename... Ts>
using memscanner_with_flags_t = typename memscanner_factory_t<flags>::template memscanner_t<Ts...>;


/*
	execute a memscanner on a small window of the program
*/
template<worker_match_behavior_t ScanBehavior>
MH_FORCEINLINE static void* scan_function_boundaries(void* func, unsigned assumed_size) {
	unsigned fdelta = (char*)func - g_blamdll.image_base;


	unsigned fend = fdelta + assumed_size;
	for (; fdelta < fend; ++fdelta) {
		void* res = ScanBehavior(fdelta);
		if (res)
			return res;
	}
	return nullptr;
}
template<worker_match_behavior_t ScanBehavior>
MH_FORCEINLINE static void* scan_guessed_function_boundaries(void* func) {
	unsigned char* pf = (unsigned char*)func;
	/*
		while in range of image and not breakpoint (in between align16 for functions) and not at ret
	*/
	while (pf >= (unsigned char*)g_blamdll.image_base && pf < (unsigned char*)(g_blamdll.image_base + g_blamdll.image_size) && *pf != 0xCC && *pf != 0xC3) {
		++pf;
	}

	unsigned assumed_size = (char*)pf - (char*)func;
	return scan_function_boundaries<ScanBehavior>(func, assumed_size);
}
//made this noinline since it executes after scanners typically
MH_NOINLINE
static void* hunt_assumed_func_start_back(void* func) {
	MH_UNLIKELY_IF(!func) {
		return nullptr;
	}
	unsigned char* pf = (unsigned char*)func;
	/*
		while in range of image and not breakpoint (in between align16 for functions) and not at ret
	*/
	while ((*pf != 0xCC && *pf != 0xC3) || (reinterpret_cast<uintptr_t>(pf + 1) & 0xF)) {
		--pf;
	}



	return pf + 1;
}

MH_NOINLINE
static void* hunt_assumed_func_start_forward(void* func) {
	MH_UNLIKELY_IF(!func) {
		return nullptr;
	}
	unsigned char* pf = (unsigned char*)func;
	/*
		while in range of image and not breakpoint (in between align16 for functions) and not at ret
	*/

	
	while (!((reinterpret_cast<uintptr_t>(pf) & 0xF) == 0 && pf[-1] == 0xCC)) {
		pf++;
	}



	return pf;
}


template<typename T>
MH_FORCEINLINE
BEHAVIOR_PURE
static inline void* run_simple_scanner() {



	for (unsigned i = 0; i < g_blamdll.text_size; ++i) {
		scanstate_t scan{ (unsigned char*)(i + g_blamdll.text_base), &g_blamdll };

		if (T::match(scan)) {

			return (void*)(reinterpret_cast<unsigned*>(scan.addr)[0] + get_reach_base());

		}
	}
	if (g_blamdll.reloc_base) {

		for (unsigned i = 0; i < g_blamdll.reloc_size; ++i) {
			scanstate_t scan{ (unsigned char*)(i + g_blamdll.reloc_base), &g_blamdll };

			if (T::match(scan)) {

				return (void*)(reinterpret_cast<unsigned*>(scan.addr)[0] + get_reach_base());

			}
		}
	}

	return nullptr;

}

template<typename T>
MH_FORCEINLINE
BEHAVIOR_PURE
static inline void* run_simple_data_scanner() {



	for (unsigned i = 0; i < g_blamdll.data_size; ++i) {
		scanstate_t scan{ (unsigned char*)(i + g_blamdll.data_base), &g_blamdll };

		if (T::match(scan)) {

			return (void*)scan.addr;

		}
	}
	return nullptr;

}
template<typename T>
MH_FORCEINLINE
BEHAVIOR_PURE
static inline void* run_simple_image_scanner() {



	for (unsigned i = 0; i < g_blamdll.image_size; ++i) {
		scanstate_t scan{ (unsigned char*)(i + g_blamdll.image_base), &g_blamdll };

		if (T::match(scan)) {

			return (void*)scan.addr;

		}
	}
	return nullptr;

}

template<typename T>
BEHAVIOR_PURE
static inline void* locate_func() {


	for (unsigned i = 0; i < g_blamdll.image_size; ++i) {
		scanstate_t scan{ (unsigned char*)(i + g_blamdll.image_base), &g_blamdll };

		if (T::match(scan)) {

			return (void*)(i + g_blamdll.image_base);

		}
	}
	return nullptr;

}

template<typename T>
MH_FORCEINLINE
BEHAVIOR_PURE
static inline void* locate_rdata_ptr_to() {


	for (unsigned i = 0; i < g_blamdll.rdata_size; i += 8) {

		char* p = *(char**)(i + g_blamdll.rdata_base);
		if (p >= g_blamdll.text_base && p < (g_blamdll.text_base + g_blamdll.text_size)) {

			scanstate_t scan{ (unsigned char*)(p), &g_blamdll };

			if (T::match(scan)) {

				return (void*)(i + g_blamdll.rdata_base);

			}
		}
	}
	return nullptr;
}

template<typename T>
MH_FORCEINLINE
BEHAVIOR_PURE
static inline void* locate_csrelative_address_preceding() {

	for (unsigned i = 0; i < g_blamdll.image_size; ++i) {
		scanstate_t scan{ (unsigned char*)(i + g_blamdll.image_base), &g_blamdll };

		if (T::match(scan)) {



			auto displ = *(signed*)((unsigned char*)(i + g_blamdll.image_base) - 4);

			return (void*)(i + g_blamdll.image_base + displ);

		}
	}
	return nullptr;
}

/*
	for extracting trivial hs function calls
*/
template<typename T>
MH_FORCEINLINE
BEHAVIOR_PURE
static inline void* extract_csrelative_call_address_before_first_retn_in_func(void* func_base) {
	char* fn = (char*)func_base;
	for (unsigned i = 0; fn[i] != 0xC3; ++i) {
		scanstate_t scan{ (unsigned char*)(&fn[i]), &g_blamdll };

		if (T::match(scan)) {


			auto displ = *(signed*)((unsigned char*)(&fn[i]) - 4);


			return (void*)(i + fn + displ);

		}
	}

}
template<typename T>
BEHAVIOR_PURE
static inline void* extract_csrelative_call_address_before_first_retn_in_func_last4bytes(void* func_base) {

	char* fn = (char*)func_base;
	for (unsigned i = 0; fn[i] != 0xC3; ++i) {
		scanstate_t scan{ (unsigned char*)(&fn[i]), &g_blamdll };

		if (T::match(scan)) {

			return (void*)(*reinterpret_cast<signed*>(scan.addr - 4) + scan.addr);
		}
	}
	return nullptr;
}

template<typename T>
MH_FORCEINLINE
BEHAVIOR_PURE
void* locate_csrelative_address_4bytes_before_end() {



	for (unsigned i = 0; i < g_blamdll.text_size; ++i) {
		scanstate_t scan{ (unsigned char*)(i + g_blamdll.text_base), &g_blamdll };

		if (T::match(scan)) {
			return (void*)(*reinterpret_cast<signed*>(scan.addr - 4) + scan.addr);
		}
	}
	return nullptr;
}


template<typename T, memsection_e constrain = memsection_e::any>
MH_FORCEINLINE
BEHAVIOR_PURE
static inline void* locate_csrelative_address_after() {


	for (unsigned i = 0; i < g_blamdll.image_size; ++i) {
		scanstate_t scan{ (unsigned char*)(i + g_blamdll.image_base), &g_blamdll };

		if (T::match(scan)) {



			auto displ = (scan.addr + 4) + *(signed*)scan.addr;

			if (!test_section_address<constrain>(&g_blamdll, displ)) {
				continue;
			}

			return (void*)(displ);

		}
	}
	return nullptr;
}


template<typename T>
MH_FORCEINLINE
BEHAVIOR_PURE
static workgroup_result_t scanbehavior_simple(unsigned i) {
	scanstate_t scan{ (unsigned char*)(i + g_blamdll.image_base), &g_blamdll };

	if (T::match(scan)) {

		return (void*)scan.addr;

	}
	return nullptr;
}
template<typename T>
MH_FORCEINLINE
BEHAVIOR_PURE
static workgroup_result_t scanbehavior_locate_func(unsigned i) {
	scanstate_t scan{ (unsigned char*)(i + g_blamdll.image_base), &g_blamdll };

	if (T::match(scan)) {

		return (void*)(i + g_blamdll.image_base);

	}
	return nullptr;
}
template<typename T>
BEHAVIOR_PURE
static inline void* scanbehavior_locate_func_with_start_search(unsigned i) {
	void* res = scanbehavior_locate_func<T>(i);

	return hunt_assumed_func_start_back(res);
}

template<typename T>
MH_FORCEINLINE
BEHAVIOR_PURE
static workgroup_result_t scanbehavior_locate_csrel_preceding(unsigned i) {
	scanstate_t scan{ (unsigned char*)(i + g_blamdll.image_base), &g_blamdll };

	if (T::match(scan)) {



		auto displ = *(signed*)((unsigned char*)(i + g_blamdll.image_base) - 4);

		return (void*)(i + g_blamdll.image_base + displ);

	}
	return nullptr;
}
template<typename T>
MH_FORCEINLINE
BEHAVIOR_PURE
static workgroup_result_t scanbehavior_locate_csrel_after(unsigned i) {
	scanstate_t scan{ (unsigned char*)(i + g_blamdll.image_base), &g_blamdll };

	if (T::match(scan)) {



		auto displ = (scan.addr + 4) + *(signed*)scan.addr;

		if (!test_section_address<memsection_e::any>(&g_blamdll, displ)) {
			return nullptr;
		}

		return (void*)(displ);

	}
	return nullptr;
}
/*
	for scanners that obtain their value using skip_and_capture_rva and the whatnot
*/
template<void** value, typename T>
MH_FORCEINLINE
BEHAVIOR_PURE
static workgroup_result_t scanbehavior_identity(unsigned i) {
	scanstate_t scan{ (unsigned char*)(i + g_blamdll.image_base), &g_blamdll };

	if (T::match(scan)) {


		return *value;

	}
	return nullptr;
}
template<unsigned NTH, typename T>
MH_FORCEINLINE
BEHAVIOR_PURE
static workgroup_result_t scanbehavior_locate_nth_call_after(unsigned i) {
	scanstate_t scan{ (unsigned char*)(i + g_blamdll.image_base), &g_blamdll };

	if (T::match(scan)) {



		mh_disassembler_t disasm{};
		disasm.setup_for_addr(scan.addr, ~0u);

		for(unsigned i = 0; i < NTH; ++i) {
			disasm.find_next_call(~0u);
		}

		return disasm.get_call_target();



	}
	return nullptr;
}

/*
	locate a pointer to another pointer
*/
template<void** p>
MH_FORCEINLINE
BEHAVIOR_PURE
static workgroup_result_t scanbehavior_pointer_scan(unsigned i) {
#if defined(__AVX512DQ__)
	if((i%64))
		return nullptr;	
	__m512i checker = _mm512_set1_epi64((long long)*p);




	__m512i checkptr = _mm512_load_si512((__m512i*)(i + g_blamdll.image_base));

	
	unsigned checkval = _mm512_cmpeq_epi64_mask(checker, checkptr);


	MH_UNLIKELY_IF(checkval) {
		return i + g_blamdll.image_base + (__lzcnt(checkval) * 8);
	}

	return nullptr;

#elif defined(__AVX2__)
	if((i%32))
		return nullptr;	
	__m256i checker = _mm256_set1_epi64x((long long)*p);




	__m256i checkptr = _mm256_load_si256((__m256i*)(i + g_blamdll.image_base));

	
	__m256i checkres = _mm256_cmpeq_epi8(checker, checkptr);

	unsigned checkval = _mm256_movemask_epi8(checkres);

	MH_UNLIKELY_IF((checkval&0xFF) == 0xFF) {
		return i + g_blamdll.image_base;
	}
	MH_UNLIKELY_IF((checkval&0xFF00) == 0xFF00){
		return i + g_blamdll.image_base+8;
	}
	MH_UNLIKELY_IF((checkval&0xFF0000) == 0xFF0000) {
		return i + g_blamdll.image_base+16;
	}
	MH_UNLIKELY_IF((checkval&0xFF000000) == 0xFF000000){
		return i + g_blamdll.image_base+24;
	}
	return nullptr;
#else
	if((i%16))
		return nullptr;	
	__m128i checker = _mm_set1_epi64x((long long)*p);




	__m128i checkptr = _mm_load_si128((__m128i*)(i + g_blamdll.image_base));

	
	__m128i checkres = _mm_cmpeq_epi8(checker, checkptr);

	unsigned checkval = _mm_movemask_epi8(checkres);

	MH_UNLIKELY_IF((checkval&0xFF) == 0xFF) {
		return i + g_blamdll.image_base;
	}
	MH_UNLIKELY_IF((checkval&0xFF00) == 0xFF00){
		return i + g_blamdll.image_base+8;
	}

	return nullptr;
#endif

}

template<workgroup_result_t (*sub_behavior)(unsigned i)>
MH_FORCEINLINE
BEHAVIOR_PURE
static workgroup_result_t locate_vftbl_member(void** vftbl, size_t nptrs_to_consider, size_t maxbytes_per_member) {
	for(unsigned i = 0 ; i < nptrs_to_consider; ++i) {
		
		char* ptr =(char*) (vftbl[i]);

		if(ptr < g_blamdll.image_base || ptr > (g_blamdll.image_base+g_blamdll.image_size) ) {
			continue;
		}

		for(unsigned bytedispl = 0; bytedispl < maxbytes_per_member; ++bytedispl) {
			
			
			unsigned newdispl = (ptr - g_blamdll.image_base) + bytedispl;


			workgroup_result_t res = sub_behavior(newdispl);
			if(res){
				return res;				
			}

		}

	}
	return nullptr;
}

template<workgroup_result_t (*ScanBehavior)(unsigned i)>
BEHAVIOR_PURE
static workgroup_result_t run_range_scanner(void* _base, void* _end){
	unsigned char* base = (unsigned char*)_base;
	unsigned char* end = (unsigned char*)_end;
	
	for(; base < end; ++base) {
		
		auto result = ScanBehavior(base - (unsigned char*)g_blamdll.image_base);
		if(result)
			return result;

	}
	return nullptr;

}
struct scangroup_listnode_t {
	struct block_scangroup_entry_t* m_next;
	struct block_scangroup_entry_t* m_prev;

	scangroup_listnode_t() : m_next(nullptr), m_prev(nullptr) {}
};



struct block_scangroup_entry_t : public scangroup_listnode_t {



	workgroup_result_t* m_result_receiver;
	const char* m_scannode_name;
	virtual workgroup_result_t execute_on_block(unsigned displ) = 0;
	//same as execute_on_block, but prefetches for the next block
	virtual workgroup_result_t execute_on_block_prefetching(unsigned displ) = 0;
	template<bool prefetching = false>
	block_scangroup_entry_t* run(unsigned i) {
#if !defined(DISABLE_PREFETCHING_SCANNER_VARIANTS)
		workgroup_result_t res = prefetching ?
			execute_on_block_prefetching(i) :
			execute_on_block(i);
#else
		workgroup_result_t res = execute_on_block(i);
#endif
		block_scangroup_entry_t* nextscanner = m_next;

		if (res) {
			/*
				we're done, remove us from the execution list
			*/
			if (m_result_receiver) {
				store8_no_cache(m_result_receiver, res);
				//* = res;
			}
			if (m_prev) {
				m_prev->m_next = m_next;
			}

			if (m_next) {
				m_next->m_prev = m_prev;
			}
		}
		return nextscanner;
	}
	block_scangroup_entry_t() : scangroup_listnode_t() {}

};
//extradata is in case we need to add more stuff here
template<

	workgroup_result_t* out_result_global,
	worker_match_behavior_t behavior,
	const char** bs_name = nullptr,
	typename Extradata = void>
	struct blockscan_entry_definition_t {


	struct implementer_t : public block_scangroup_entry_t {
		implementer_t() : block_scangroup_entry_t() {
			m_result_receiver = out_result_global;
			m_scannode_name = (bs_name != nullptr ? *bs_name : nullptr);
		}
		MH_FLATTEN
		virtual workgroup_result_t execute_on_block(unsigned displ) override {
			mh_assume_m( displ % SCANNER_WORKGROUP_BLOCK_SIZE == 0);
			unsigned end = displ + SCANNER_WORKGROUP_BLOCK_SIZE;
			end = end > g_blamdll.image_size ? g_blamdll.image_size : end;
			mh_assume_m( reinterpret_cast<uintptr_t>(displ +g_blamdll.image_base) % SCANNER_WORKGROUP_BLOCK_SIZE == 0);
#pragma unroll SCAN_UNROLL_AMNT
			for (unsigned i = displ; i < end; ++i) {
				void* res = behavior(i);
				if (res)
					return res;

				//return behavior(displ);
			}
			return nullptr;
		}
		
		virtual workgroup_result_t execute_on_block_prefetching(unsigned displ) override {

			mh_assume_m( displ % SCANNER_WORKGROUP_BLOCK_SIZE == 0);

#if !defined(DISABLE_PREFETCHING_SCANNER_VARIANTS)

			unsigned end = displ + SCANNER_WORKGROUP_BLOCK_SIZE;
			end = end > g_blamdll.image_size ? g_blamdll.image_size : end;


			unsigned prefetchiters = (end - displ) / 64;
			mh_assume_m( reinterpret_cast<uintptr_t>(displ +g_blamdll.image_base) % SCANNER_WORKGROUP_BLOCK_SIZE == 0);

			for (unsigned iiter = 0; iiter < prefetchiters; ++iiter) {
				_mm_prefetch((iiter * 64) + displ + SCANNER_WORKGROUP_BLOCK_SIZE + g_blamdll.image_base, _MM_HINT_T0);

				for (unsigned j = 0; j < 64; ++j) {
					void* res = behavior(((iiter * 64) + j) + displ);
					if (res)
						return res;
				}
				/*
					we're ending our usage of this block so flush it from the cache to make room for the next block
				*/
#ifndef DONT_FLUSH_MEM
				_mm_clflush(((iiter * 64) + displ) + g_blamdll.image_base);
#endif
			}


			//handle modulo cachesize 
			for (unsigned i = (prefetchiters * 64) + displ; i < end; ++i) {
				//if (!(i & 63))
			//		_mm_prefetch(i + SCANNER_WORKGROUP_BLOCK_SIZE + g_blamdll.image_base, _MM_HINT_T1);

				void* res = behavior(i);
				if (res)
					return res;

				//return behavior(displ);
			}
			return nullptr;
#endif
		}
	};

	static inline implementer_t g_blockscan_node{};

};

__declspec(noinline)
void scanner_failure_message(const char* scanner_name);


template<typename... Ts>
struct scangroup_t {
	static inline scangroup_listnode_t m_list;
	template<typename TCurr, typename... TNext>
	static void link_group(scangroup_listnode_t* prevnode) {

		prevnode->m_next = &TCurr::g_blockscan_node;

		TCurr::g_blockscan_node.m_prev = (block_scangroup_entry_t*)prevnode;

		if constexpr (sizeof...(TNext) != 0) {
			link_group<TNext...>(&TCurr::g_blockscan_node);
		}

	}


	scangroup_t() {
		link_group<Ts...>(&m_list);
	}

#ifdef ASSERT_ALL_LOCATED
	template<typename T, typename... TRest>
	static void assert_all_located() {
		MH_UNLIKELY_IF (!*(T::g_blockscan_node.m_result_receiver)) {
			scanner_failure_message(T::g_blockscan_node.m_scannode_name ? T::g_blockscan_node.m_scannode_name : "<unknown>");
		}

		if constexpr (sizeof...(TRest) != 0) {
			assert_all_located<TRest...>();
		}
	}
#endif
	static void execute_on_image() {

		unsigned i = 0;

		unsigned end = g_blamdll.image_size;
		if (end <= SCANNER_WORKGROUP_BLOCK_SIZE) {
			__assume(0);
		}

		
		for (; i + SCANNER_WORKGROUP_BLOCK_SIZE < end; i += SCANNER_WORKGROUP_BLOCK_SIZE) {


			MH_UNLIKELY_IF (!m_list.m_next)
				return;
			block_scangroup_entry_t* entry = m_list.m_next;

			/*
				unroll so that mispredicts happen less frequently when indirectly calling

			*/
#pragma unroll SCANGROUP_CALL_UNROLL
			while (entry->m_next) {
				//		if(!entry->m_next)
					//		entry->run<true>(i);
					//	else
				entry = entry->run(i);

			}
			entry->run<true>(i);

		}

#ifdef ASSERT_ALL_LOCATED
		assert_all_located<Ts...>();
#endif
	}

};



template<typename T>
struct runfunc_forwarder_t {
	static inline T g_tval{};
	static DWORD runfunc(void* unused) {
		g_tval.execute_on_image();
		return 0;
	}
};

template<typename... Ts>
struct parallel_scangroup_group_t {

	static inline std::array< HANDLE, sizeof...(Ts)> m_threads{ nullptr };


	parallel_scangroup_group_t() {
		
	}



	//template<unsigned currentidx = 0>
	template<unsigned currentidx, typename CurrentScannerT, typename... TNext>
	static void queue_up_scanners() {
		using scanner_type = CurrentScannerT;
	
		DWORD unused_id;
		m_threads[currentidx] = CreateThread(nullptr, 65536,&runfunc_forwarder_t<CurrentScannerT>::runfunc, nullptr, 0,&unused_id); //new std::thread{ &runfunc_forwarder_t<CurrentScannerT>::runfunc };

		if constexpr (sizeof...(TNext) != 0) {
			queue_up_scanners<currentidx + 1, TNext...>();
		}
	}

	static void execute_on_image() {
		queue_up_scanners<0, Ts...>();
		//delete also does join
		//todo: should we actually run one of the scangroups on our current thread?
		//for (auto&& thrd : m_threads) {
			//thrd->join();

			//delete thrd;
			//thrd = nullptr;
		//}
		WaitForMultipleObjects(sizeof...(Ts), &m_threads[0], true, INFINITE);
		//FlushProcessWriteBuffers();

	}

};



