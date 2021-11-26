#pragma once
// enable various features that are only useful to me
//#define			MH_DEV_BUILD

#include <intrin.h>
#include <array>
#include "snaphakalgo.hpp"
//fuck it lets just pre-include this stuff here. tired of duplicating these includes
#include <map>
#include <vector>
#include <string>
#include <set>

//#define     DISABLE_MH_NATIVE_API   

//#define     MH_DISABLE_ALL_DEV_STUFF
#define		MH_FORCEINLINE		__forceinline
#define		MH_CODE_SEG(name)	__declspec(code_seg(name)) 

//64 mb for mh heap
#define     MH_HEAP_SIZE        (16ULL*(1024*1024))

//the base of our dll
extern char* g_mh_module_base;




//for static noinline functions in header files so compiler is aware of what they do unlike with extern at early phases
//but cant bloat our shit
#define		MH_SELECTANY			__declspec(selectany)
#if !defined(__clang__)
#define		MH_NOINLINE			__declspec(noinline)
#define		MH_REGFREE_CALL		
#define		MH_FLATTEN

#define			MH_UNLIKELY_IF(...)			if(__VA_ARGS__)	[[unlikely]]
#define			MH_LIKELY_IF(...)			if(__VA_ARGS__)	[[likely]]

#define			MH_SEMIPURE						
#define			MH_PURE							
#define			MH_NODUPLICATE	

#define			mh_unreachable_m()			__assume(false)
#define			MH_NOCLONE		
#else
#define		MH_NOINLINE			__attribute__((noinline))
#define		MH_FLATTEN			__attribute__((flatten))

//prevents func call duplication
#define			MH_NODUPLICATE	__attribute__((noduplicate))
/*
	call that preserves all registers.
	use for rarely called functions and the like (initializing an offset from typeinfo system ?)
*/
#define		MH_REGFREE_CALL			__attribute__((no_caller_saved_registers)) __attribute__((disable_tail_calls))  

#define			MH_UNLIKELY_IF(...)			if(__builtin_expect(!!(__VA_ARGS__), false))
#define			MH_LIKELY_IF(...)			if(__builtin_expect(!!(__VA_ARGS__), true))

#define			MH_SEMIPURE						__attribute__((pure)) 
#define			MH_PURE							__attribute__((const))
//#define			MH_SEMIPURE						
//#define			MH_PURE	
#define			mh_unreachable_m()			__builtin_unreachable()
#define			MH_NOCLONE					__attribute__((noclone))

#endif

#define		mh_assume_m(...)			if(!(__VA_ARGS__)) mh_unreachable_m()


#define		MH_HEADER_NOINLINE		  MH_NOINLINE

#define		MH_NOALIAS		__declspec(noalias)
template<typename T>
concept mh_ptr_sized_c = sizeof(T) == sizeof(void*);

extern
sh_heap_t g_mh_heap;


/*
    MH should use its own heap, so that when it pointer scans the global heap it doesnt have hits on itself
    we use this heap just for basic bookkeeping information that mh uses, like the vtbl set
*/
/*MH_SELECTANY
extern sh_heap_t g_mh_heap;*/

template<typename T>
using mh_allocator_t = sh::heap::sh_heap_based_allocator_t<T, &g_mh_heap, 0>;

template<typename TKey, typename TValue>
using mh_map_t = std::map < TKey, TValue, std::less<TKey>, mh_allocator_t<std::pair<const TKey, TValue>>>;

template<typename TElement>
using mh_vector_t = std::vector<TElement, mh_allocator_t<TElement>>;

template<typename TElement>
using mh_set_t = std::set<TElement, std::less<TElement>, mh_allocator_t<TElement>>;

using mh_str_t = std::basic_string<char, std::char_traits<char>, mh_allocator_t<char>>;
/*
	pointer arithmetic helper
*/
template<typename TTo, mh_ptr_sized_c TFrom>
static inline TTo* mh_lea(TFrom shouldbptr, ptrdiff_t displacement) {
	return (TTo*)(&((char*)shouldbptr)[displacement]);
}

MH_NOINLINE
void mh_error_message(const char* fmt, ...);

#define		mh_tmpl_strarg_m(...)		decltype([](){return __VA_ARGS__;})

#include "snaphakalgo_predef.hpp"


template<typename TRet, typename... Ts>
MH_PURE
static TRet mh_pure_call(TRet (*fn)(Ts...), Ts... args) {
	return fn(args...);
}
//ancient macro version
#if 0
template<char... s>
struct _yuckystring_t {
	static constexpr unsigned yucky_length() {
		char tmpbuf[] = { s... };

		unsigned i = 0;
		for (; tmpbuf[i]; ++i) {

		}
		return i;
	}

	static constexpr unsigned temp_length = yucky_length();
	using str_storage_type_t = std::array<char, temp_length + 1>;

	static constexpr str_storage_type_t get_actual_string_trimmed() {
		str_storage_type_t result{ 0 };

		char tmpbuf[] = { s... };

		for (unsigned i = 0; i < temp_length; ++i) {
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

#else
//clang version
template<size_t N>
struct _cstr_t {
    char m_vals[N];
    constexpr _cstr_t(char const (&vals)[N]) {

        for (unsigned i = 0; i < N; ++i) {
            m_vals[i] = vals[i];
        }
    }
    constexpr _cstr_t() {


    }
};
#if defined(__clang__)

template<_cstr_t s>
static constexpr auto operator ""_cstr() {

    return s;

}

#define     MAKE_CONSTANT_STR(txt)      txt##_cstr
#else
//msvc version
template<typename T>
struct _const_str_evaluator {
    static constexpr T g_callable{};
    static constexpr const char* g_text = g_callable();

    static constexpr unsigned get_slen() {

        unsigned i = 0;
        for (; g_text[i]; ++i)
            ;
        return i;
    }
    static constexpr unsigned g_text_len = get_slen();

    static constexpr _cstr_t<g_text_len> make_result_arr() {
        _cstr_t<g_text_len> result{};
        for (unsigned i = 0; i < g_text_len; ++i) {
            result.m_vals[i] = g_text[i];
        }
        return result;
    }

    static constexpr _cstr_t<g_text_len> value = make_result_arr();

};

#define     MAKE_CONSTANT_STR(txt)      (_const_str_evaluator<decltype([](){return txt;})>::value)
#endif
template<auto obj>
struct intern_const_str_t {

    static constexpr const char* value = &obj.m_vals[0];
};

#define		yuckystring_m(...)		intern_const_str_t<MAKE_CONSTANT_STR(__VA_ARGS__)>
#define		yuckystring_str_from_type_m(...)			(&__VA_ARGS__::value[0])

#define		yuckystr_parampack_unp_m(ts)				(&ts::value[0])...

#define		YS(...)		yuckystring_m(__VA_ARGS__)
#endif


template<typename T>
struct cs_uninit_t {
	alignas(T)
		char m_storage[sizeof(T)];

	inline T* operator ->() {
		return reinterpret_cast<T*>(&m_storage[0]);
	}
	inline const T* operator ->() const {
		return reinterpret_cast<const T*>(&m_storage[0]);
	}
	inline T* operator &() {
		return reinterpret_cast<T*>(&m_storage[0]);
	}
	inline const T* operator &()const {
		return reinterpret_cast<const T*>(&m_storage[0]);
	}

	inline T& ref() {
		return *reinterpret_cast<T*>(&m_storage[0]);
	}

};

template<typename T, unsigned N>
struct cs_uninit_array_t {
	alignas(T)
		char m_storage[sizeof(T) * N];


	inline const T& operator [](unsigned idx) const {
		return reinterpret_cast<const T*>(&m_storage[0])[idx];
	}
	inline T& operator [](unsigned idx) {
		return reinterpret_cast<T*>(&m_storage[0])[idx];
	}
};
namespace cs {
	struct cs_assert_fail_descr_t {
		const char* _err_msg;
		const char* _file_name;
		const char* fn_name;
	};
	CS_COLD_CODE
	CS_NOINLINE
	CS_NORETURN
	void _cs_assert_fail_raise(const cs_assert_fail_descr_t* descr);

}
namespace cs::_cs_assert_impl {

	template<typename T/*, unsigned line_no*/>
	struct _cs_assert_fail {
		static constexpr cs_assert_fail_descr_t _descr{ T::__msg(), T::__file() , T::fn() };

		CS_NOINLINE
			CS_NORETURN
			CS_CODE_SEG(".assert_failures")
			static void raise_failure() {
			_cs_assert_fail_raise(&_descr);
		}

	};
}
#if 0
#define cs_assert(...)		\
	do {\
		static  const char* __assert_msg = #__VA_ARGS__;\
		static  const char* __assert_failure_file = __FILE__;\
		using __failure_handler = cs::_cs_assert_impl::_cs_assert_fail<&__assert_msg,&__assert_failure_file, __LINE__>;\
		if(!(__VA_ARGS__)) \
			__failure_handler::raise_failure();\
	}while(false)
#else
#define cs_assert(...)		\
	do {\
		static constexpr const char* __funcy = __FUNCTION__;\
		class _arg_passer {\
		public:\
			static constexpr const char* __msg() {\
				return #__VA_ARGS__; \
			}\
			static constexpr const char* __file() {\
				return __FILE__;\
			}\
static constexpr const char* fn(){return __funcy;}\
		};\
\
		using __failure_handler = cs::_cs_assert_impl::_cs_assert_fail<_arg_passer>;\
		if(!(__VA_ARGS__)) \
			__failure_handler::raise_failure();\
	}while(false)
#endif


/*
    convince the compiler that the function is pure or super pure even if it actually isnt. useful in some scenarios when a function is affected by changes in the global state,
    but only under certain conditions that are currently known to not be true
*/
template<typename TRet, typename... TArgs>
MH_PURE
static TRet call_as_super_pure(MH_NOESCAPE TRet(*func)(TArgs...), TArgs... args) {
    return func(args...);
}

template<typename TRet, typename... TArgs>
MH_SEMIPURE
static TRet call_as_pure(MH_NOESCAPE TRet(*func)(TArgs...), TArgs... args) {
    return func(args...);
}
template<typename T>
concept is_domain_ptr_t = requires(T a) {
    { a.extract_displ() } -> std::same_as<uint32_t>;
};

template<typename T>
static inline bool checknull_for_jrcxz(T value) {
    if constexpr (is_domain_ptr_t<T>) {
        return value == T::make_from_u32(0);

    }
    else {
        return value == static_cast<T>(0);

    }

}
static constexpr unsigned X86_CF = 0x0100,
X86_PF = 0x0004 << 8,
X86_ZF = 0x0040 << 8,
X86_SF = 0x0080 << 8,
X86_NONE = 0;

#if defined(__clang__)

template<typename T>
MH_PURE
static inline
bool cs_x86_jrcxz_impl(T vv) {
    asm goto (
        "jrcxz %l1"
        :
    : "c" (vv)
        :
        : iszero);

    return 0;

iszero:
    return 1;
}
template<typename T>
MH_PURE
static inline bool cs_x86_jrcxz(T vv) {

    if (__builtin_constant_p(checknull_for_jrcxz<T>(vv))) {
        return checknull_for_jrcxz<T>(vv);


    }
    else {
        bool result = cs_x86_jrcxz_impl(vv);

        if (result != checknull_for_jrcxz<T>(vv))
            __builtin_unreachable();

        return result;
    }
}

static inline bool cs_x86_loop(unsigned long long& vv) {
    const unsigned long long orig_value = vv;
    //bool result = cs_x86_loop_impl(vv);


    bool result = false;

    register unsigned long long valinput asm("rcx") = orig_value;


    asm goto (
        "loop %l1"
        :"+r" (valinput)
        :
        :
        : iszero);

    // result = 0;
    goto gotresult;
iszero:
    result = true;

gotresult:
    unsigned long long localres = valinput;


    /*
    * this assume causes some bad codegen (moves to rcx and back from rcx repeatedly :/


    if (localres != (orig_value - 1) || result != (localres == 0))
         __builtin_unreachable();*/

    vv = localres;
    return result;

}

MH_PURE
static inline unsigned sahf_cpzs_switch(unsigned flagsval) {


    __asm__ goto(
        "sahf\n\t"
        "jb %l1\n\t"
        "jp %l2\n\t"
        "jz %l3\n\t"
        "js %l4\n\t"
        :
    : "a"(flagsval)
        : "cc"
        : cf_set, pf_set, zf_set, sf_set
        );

    return X86_NONE;
cf_set:
    return X86_CF;
pf_set:
    return X86_PF;
zf_set:
    return X86_ZF;
sf_set:
    return X86_SF;
}
MH_PURE
static inline unsigned sahf_cpz_switch(unsigned flagsval) {


    __asm__ goto(
        "sahf\n\t"
        "jb %l1\n\t"
        "jp %l2\n\t"
        "jz %l3\n\t"
        :
    : "a"(flagsval)
        : "cc"
        : cf_set, pf_set, zf_set
        );

    return X86_NONE;
cf_set:
    return X86_CF;
pf_set:
    return X86_PF;
zf_set:
    return X86_ZF;
}
MH_PURE
static inline unsigned sahf_cp_switch(unsigned flagsval) {


    __asm__ goto(
        "sahf\n\t"
        "jb %l1\n\t"
        "jp %l2\n\t"
        :
    : "a"(flagsval)
        : "cc"
        : cf_set, pf_set
        );

    return X86_NONE;
cf_set:
    return X86_CF;
pf_set:
    return X86_PF;

}
#else

template<typename T>
MH_PURE
static inline bool cs_x86_jrcxz(T vv) {

    return checknull_for_jrcxz<T>(vv);

}
static inline unsigned sahf_cpzs_switch(unsigned flagsval) {

    if (flagsval & X86_CF)
        goto cf_set;
    if (flagsval & X86_PF)
        goto pf_set;
    if (flagsval & X86_ZF)
        goto zf_set;
    if (flagsval & X86_SF)
        goto sf_set;

    return X86_NONE;
cf_set:
    return X86_CF;
pf_set:
    return X86_PF;
zf_set:
    return X86_ZF;
sf_set:
    return X86_SF;
}
static inline unsigned sahf_cpz_switch(unsigned flagsval) {

    if (flagsval & X86_CF)
        goto cf_set;
    if (flagsval & X86_PF)
        goto pf_set;
    if (flagsval & X86_ZF)
        goto zf_set;

    return X86_NONE;
cf_set:
    return X86_CF;
pf_set:
    return X86_PF;
zf_set:
    return X86_ZF;
}
static inline unsigned sahf_cp_switch(unsigned flagsval) {

    if (flagsval & X86_CF)
        goto cf_set;
    if (flagsval & X86_PF)
        goto pf_set;

    return X86_NONE;
cf_set:
    return X86_CF;
pf_set:
    return X86_PF;

}
static inline bool cs_x86_loop(unsigned long long& vv) {

    bool result = false;


    if (--vv == 0)
        goto iszero;
    goto gotresult;
iszero:
    result = true;

gotresult:


    return result;

}

#endif
template<typename T>
MH_PURE
static inline bool cs_x86_jrcxnz(T vv) {

    return !cs_x86_jrcxz<T>(vv);

}
#define     CS_DECLARE_GETSET(propname, type, getter, setter)   __declspec(property(get = getter, put = setter)) type propname


static constexpr unsigned pack_color(unsigned r, unsigned g, unsigned b, unsigned a = 255) {
    return r | ((g | ((b | (a << 8)) << 8)) << 8);
}

struct MH_TRIVIAL_ABI mh_color_t {
    unsigned m_color;

    constexpr mh_color_t() : m_color(~0u) {}

    constexpr mh_color_t(unsigned r, unsigned g, unsigned b, unsigned a = 255) : m_color(pack_color(r, g, b, a)) {
    
    }



};

static constexpr mh_color_t mh_colorWhite{};

static constexpr mh_color_t mh_colorBlack{ 0,0,0 };
static constexpr mh_color_t mh_colorRed{ 255, 0, 0 };
static constexpr mh_color_t mh_colorGreen{ 0, 255, 0 };
static constexpr mh_color_t mh_colorBlue{ 0, 0, 255 };


MH_PURE
static inline const char* get_mh_base() {
    return g_mh_module_base;
}
template<typename T>
MH_PURE
static inline unsigned to_mh_rva(T* p) {
    return reinterpret_cast<char*>(p) - get_mh_base();
}

template<typename T>
static inline T* from_mh_rva(unsigned rva) {
    return mh_lea<T>(get_mh_base(), static_cast<uintptr_t>(rva));
}