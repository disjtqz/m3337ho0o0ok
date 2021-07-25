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
//#define		DISABLE_PREFETCHING_SCANNER_VARIANTS		
#define		ASSERT_ALL_LOCATED
struct scanstate_t {
	unsigned char* addr;
	blamdll_t* dll;
	unsigned char* backref_stack[8];
	uintptr_t backref_position;
	scanstate_t(unsigned char* _addr, blamdll_t* _dll) : addr(_addr), dll(_dll), backref_position(0) {}
};
enum class memsection_e {
	any,
	text,
	rdata,
	data
};

template<char... s>
struct _yuckystring_t {
	static constexpr unsigned yucky_length() {
		char tmpbuf[] = {s...};

		unsigned i=0;
		for(; tmpbuf[i]; ++i) {
			
		}
		return i;
	}

	static constexpr unsigned temp_length = yucky_length();
	using str_storage_type_t = std::array<char, temp_length+1> ;

	static constexpr str_storage_type_t get_actual_string_trimmed() {
		str_storage_type_t result{0};

		char tmpbuf[] = {s...};

		for(unsigned i = 0; i < temp_length; ++i) {
			result[i] = tmpbuf[i];

		}
		return result;
	}

	static constexpr str_storage_type_t actual_string = get_actual_string_trimmed();

};


#define		YUCKYSTRCHAR(s, idx)		(idx < sizeof(s) ? s[idx] : 0)
#define		YUCKYSTRING_MAXLENGTH			64

#define		yuckystring_m(s)		_yuckystring_t<0<sizeof(s)?s[0]:0,1<sizeof(s)?s[1]:0,2<sizeof(s)?s[2]:0,3<sizeof(s)?s[3]:0,4<sizeof(s)?s[4]:0,5<sizeof(s)?s[5]:0,6<sizeof(s)?s[6]:0,7<sizeof(s)?s[7]:0,8<sizeof(s)?s[8]:0,9<sizeof(s)?s[9]:0,10<sizeof(s)?s[10]:0,11<sizeof(s)?s[11]:0,12<sizeof(s)?s[12]:0,13<sizeof(s)?s[13]:0,14<sizeof(s)?s[14]:0,15<sizeof(s)?s[15]:0,16<sizeof(s)?s[16]:0,17<sizeof(s)?s[17]:0,18<sizeof(s)?s[18]:0,19<sizeof(s)?s[19]:0,20<sizeof(s)?s[20]:0,21<sizeof(s)?s[21]:0,22<sizeof(s)?s[22]:0,23<sizeof(s)?s[23]:0,24<sizeof(s)?s[24]:0,25<sizeof(s)?s[25]:0,26<sizeof(s)?s[26]:0,27<sizeof(s)?s[27]:0,28<sizeof(s)?s[28]:0,29<sizeof(s)?s[29]:0,30<sizeof(s)?s[30]:0,31<sizeof(s)?s[31]:0,32<sizeof(s)?s[32]:0,33<sizeof(s)?s[33]:0,34<sizeof(s)?s[34]:0,35<sizeof(s)?s[35]:0,36<sizeof(s)?s[36]:0,37<sizeof(s)?s[37]:0,38<sizeof(s)?s[38]:0,39<sizeof(s)?s[39]:0,40<sizeof(s)?s[40]:0,41<sizeof(s)?s[41]:0,42<sizeof(s)?s[42]:0,43<sizeof(s)?s[43]:0,44<sizeof(s)?s[44]:0,45<sizeof(s)?s[45]:0,46<sizeof(s)?s[46]:0,47<sizeof(s)?s[47]:0,48<sizeof(s)?s[48]:0,49<sizeof(s)?s[49]:0,50<sizeof(s)?s[50]:0,51<sizeof(s)?s[51]:0,52<sizeof(s)?s[52]:0,53<sizeof(s)?s[53]:0,54<sizeof(s)?s[54]:0,55<sizeof(s)?s[55]:0,56<sizeof(s)?s[56]:0,57<sizeof(s)?s[57]:0,58<sizeof(s)?s[58]:0,59<sizeof(s)?s[59]:0,60<sizeof(s)?s[60]:0,61<sizeof(s)?s[61]:0,62<sizeof(s)?s[62]:0,63<sizeof(s)?s[63]:0>
#define		yuckystring_str_from_type_m(...)			(&__VA_ARGS__::actual_string[0])

template<memsection_e section, typename TTest>
MH_FORCEINLINE
static inline bool test_section_address(blamdll_t* dll, TTest t) {
	if constexpr (section == memsection_e::any) {
		return dll->is_in_image(t);
	}
	else if constexpr (section == memsection_e::text) {
		return dll->is_in_text(t);
	}
	else if constexpr (section == memsection_e::rdata) {
		return dll->is_in_rdata(t);
	}
	else {
		return dll->is_in_data(t);
	}
}

MH_FLATTEN
MH_FORCEINLINE

static inline bool scancmp_bytes(const unsigned char* state, const unsigned char* values, size_t nvalues) {
#if 1
	unsigned i = 0;
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

		*rva_out = (void*)(state.addr + 4 + df);

		state.addr += 4;
		return true;
	}
};

template<unsigned* number_out>
struct skip_and_capture_4byte_value {
	static constexpr unsigned required_valid_size = 4;
	MH_FORCEINLINE
		static bool match(scanstate_t& state) {

		*number_out = *reinterpret_cast<unsigned*>(state.addr);

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
/*
	checks if current byte & mask == expected_value
*/
template<unsigned mask, unsigned expected_value>
struct masked_eq_byte {
	static constexpr unsigned required_valid_size = 1;
	MH_FORCEINLINE
	static bool match(scanstate_t& state) {
		unsigned bval = state.addr[0];
		if((bval & mask) != expected_value)
			return false;
		state.addr+=1;
		return true;
	}

};

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
MH_FORCEINLINE static void* hunt_assumed_func_start_back(void* func) {
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

template<typename T>
MH_FORCEINLINE
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
static workgroup_result_t scanbehavior_simple(unsigned i) {
	scanstate_t scan{ (unsigned char*)(i + g_blamdll.image_base), &g_blamdll };

	if (T::match(scan)) {

		return (void*)scan.addr;

	}
	return nullptr;
}
template<typename T>
MH_FORCEINLINE
static workgroup_result_t scanbehavior_locate_func(unsigned i) {
	scanstate_t scan{ (unsigned char*)(i + g_blamdll.image_base), &g_blamdll };

	if (T::match(scan)) {

		return (void*)(i + g_blamdll.image_base);

	}
	return nullptr;
}
template<typename T>
static inline void* scanbehavior_locate_func_with_start_search(unsigned i) {
	void* res = scanbehavior_locate_func<T>(i);

	return hunt_assumed_func_start_back(res);
}

template<typename T>
MH_FORCEINLINE
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
static workgroup_result_t scanbehavior_identity(unsigned i) {
	scanstate_t scan{ (unsigned char*)(i + g_blamdll.image_base), &g_blamdll };

	if (T::match(scan)) {


		return *value;

	}
	return nullptr;
}
template<unsigned NTH, typename T>
MH_FORCEINLINE
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
MH_PURE
static workgroup_result_t scanbehavior_pointer_scan(unsigned i) {
	if((i&0xF))
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


}

template<workgroup_result_t (*sub_behavior)(unsigned i)>
MH_FORCEINLINE
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
			if (m_result_receiver)
				*m_result_receiver = res;
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

		virtual workgroup_result_t execute_on_block(unsigned displ) override {
			mh_assume_m( (((SCANNER_WORKGROUP_BLOCK_SIZE) - 1) & displ) == 0);
			unsigned end = displ + SCANNER_WORKGROUP_BLOCK_SIZE;
			end = end > g_blamdll.image_size ? g_blamdll.image_size : end;

			for (unsigned i = displ; i < end; ++i) {
				void* res = behavior(i);
				if (res)
					return res;

				//return behavior(displ);
			}
			return nullptr;
		}
		virtual workgroup_result_t execute_on_block_prefetching(unsigned displ) override {

			mh_assume_m( (((SCANNER_WORKGROUP_BLOCK_SIZE) - 1) & displ) == 0);

#if !defined(DISABLE_PREFETCHING_SCANNER_VARIANTS)

			unsigned end = displ + SCANNER_WORKGROUP_BLOCK_SIZE;
			end = end > g_blamdll.image_size ? g_blamdll.image_size : end;


			unsigned prefetchiters = (end - displ) / 64;

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

	}

};



