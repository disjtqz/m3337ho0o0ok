#pragma once
// enable various features that are only useful to me
//#define			MH_DEV_BUILD

#include <intrin.h>
#include <array>
#define		MH_FORCEINLINE		__forceinline
#define		MH_CODE_SEG(name)	__declspec(code_seg(name)) 

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
#define			mh_unreachable_m()			__builtin_unreachable()
#define			MH_NOCLONE					__attribute__((noclone))

#endif

#define		mh_assume_m(...)			if(!(__VA_ARGS__)) mh_unreachable_m()


#define		MH_HEADER_NOINLINE		  MH_NOINLINE

#define		MH_NOALIAS		__declspec(noalias)
template<typename T>
concept mh_ptr_sized_c = sizeof(T) == sizeof(void*);

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
