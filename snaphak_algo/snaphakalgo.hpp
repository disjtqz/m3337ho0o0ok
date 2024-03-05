#pragma once
#include "snaphakalgo_predef.hpp"
#include <cstdint>
#include <cstddef>
#include <intrin.h>
#include <memory>
#include "snaphak_heap.hpp"
#include "snaphak_rbtree.hpp"
#ifndef SHALGO_DISABLE_BITMAP
#include "snaphak_bitmap.hpp"
#endif
#include "snaphak_string.hpp"
#include "snaphak_memops.hpp"
#include "snaphak_vmemops.hpp"
#include "snaphak_coro.hpp"
#include "snaphak_smt.hpp"
#include "snaphak_sortnsearch.hpp"
#ifndef SHALGO_DISABLE_INTBULK
#include "snaphak_intbulk.hpp"
#endif
#include "snaphak_fmath.hpp"
#include "snaphak_networking.hpp"
#include "snaphak_allocator.hpp"
#include "syscall_list.hpp"


#define		SNAPHAK_SHARED_SEG		CS_CODE_SEG(".common")
#define		SNAPHAKALGO_EXPORT		extern "C"
enum class snaphak_cpu_lvl_t {
	GenericCpu,
	AVX2Cpu,
	AVX512Cpu
};

enum class snaphak_cpu_vendor_t {
	AMD,
	INTEL,
	UNKNOWN

};
/*
	assign "profiles" based on thresholds of memory. in the future these will be taken into account with overrides, keeping the contents of the resources in memory instead of reading back from the disc

	4gb = constrained, eternal usually uses like 2.5gb. according to the steam hardware survey about 5% of users fall into this category, likely there are a very very small number of eternal users with this little

	8gb = light. less sacrifices need to be made. about 25% of users

	16gb = average. no sacrifices needed

	32gb = workstation. can use extra memory for caching shit

	128gb = chrispy. i have 256 gb lol, at this profile theres no reason not to cache everything

*/

#define		DECLARE_MEMCLASS(name, value)		name = value
#define		MEMSEP		,
enum class mh_memclass_e : std::uint32_t {
#include "memclasses.hpp"
};

#undef DECLARE_MEMCLASS
#undef MEMSEP


/*
	top level structure with all function pointers for implementations
*/
struct snaphak_algo_t {

#if defined(ENABLE_SYSCALL_STUFF)
	unsigned (*m_get_syscall_code)(unsigned swi);
	void* (*m_get_syscall_func)(unsigned swi);
#endif
	void (*m_print_cpu_info)(struct snaphak_algo_t* alg, char* buffer, size_t maxsize);
	snaphak_cpu_lvl_t m_cpulevel;
	struct {
		uint32_t m_has_clflushopt : 1;
		uint32_t m_has_clwb : 1;
		uint32_t m_has_smt : 1;
		uint32_t m_has_movbe : 1;
		uint32_t m_has_f16c : 1;
		uint32_t m_has_fast_longrep : 1;
		uint32_t m_has_fast_shortrep : 1;
		uint32_t m_is_under_wine : 1;
		/*
			on amd processors LOOP/LOOPE/LOOPNE are really fast single uop instructions
			on intel processors they're implemented in microcode and really slow
			jcxz/rcxz/ecxz are also single uop on amd, but on intel theyre only slightly slower at 2 uops
		*/
		uint32_t m_has_fast_loop_ops : 1;
		/*
			intel cpus have this, and amd cpus after zen 3
			zen1 - zen2 all emulate pdep and pext, so they're slow af
		*/
		uint32_t m_singlecycle_pdep_pext : 1;
	};
	void (*m_fatal_raise)(const char* fmt, ...);
	uint32_t m_ncores;
	uint32_t m_nprocgroups;
	uint32_t m_nthreads;
	uint32_t m_smt_width;
	mh_memclass_e m_memclass;
	snaphak_cpu_vendor_t m_vendor;
	//how many bytes of memory the system has installed
	uint64_t m_total_device_memory;
	snaphak_heaproutines_t m_heaproutines;
	snaphak_rbroutines_t m_rbroutines;
#ifndef SHALGO_DISABLE_BITMAP
	snaphak_bmproutines_t m_bmproutines;
#endif
	snaphak_memroutines_t m_memroutines;
	snaphak_virtmemroutines_t m_vmemroutines;
	snaphak_cororoutines_t m_cororoutines;
	snaphak_sroutines_t m_sroutines;
	snaphak_smtroutines_t m_smtroutines;
	snaphak_snsroutines_t m_snsroutines;
#ifndef SHALGO_DISABLE_INTBULK
	snaphak_ibulkroutines_t m_ibulkroutines;
#endif
	snaphak_realroutines_t m_realroutines;
	snaphak_netroutines_t m_netroutines;


};

#if !defined(SNAPHAKALGO_BUILDING)
extern snaphak_algo_t g_shalgo;


SNAPHAKALGO_EXPORT
void sh_algo_init(snaphak_algo_t* out_algo);

namespace sh {
	static inline bool avx2_available() {
		return g_shalgo.m_cpulevel == snaphak_cpu_lvl_t::AVX2Cpu || g_shalgo.m_cpulevel == snaphak_cpu_lvl_t::AVX512Cpu;
	}
	template<typename... Ts>
	[[noreturn]]
	static inline void fatal(const char* fmt, Ts... args) {
		g_shalgo.m_fatal_raise(fmt, args...);
	}
#if defined(ENABLE_SYSCALL_STUFF)
	static inline bool syscall_interface_available() {
		return !g_shalgo.m_is_under_wine;
	}
	template<unsigned swicode, typename RetType, typename... Argtypes>
	static RetType perform_syscall(Argtypes... args) {
		return reinterpret_cast<RetType(*)(Argtypes...)>(g_shalgo.m_get_syscall_func(swicode))(args...);
	}
#endif
}
namespace sh::heap {
#include "sh_heap_shared.hpp"
	static inline sh_heap_t create_heap_from_mem(void* mem, size_t memsize, unsigned heapflags) {
		return g_shalgo.m_heaproutines.m_create_heap_from_mem(mem, memsize, heapflags);
	}
	static inline void destroy_heap(sh_heap_t heap) {
		return g_shalgo.m_heaproutines.m_destroy_heap(heap);
	}

	static inline void* alloc_from_heap(sh_heap_t heap, size_t nbytes, unsigned heapflags) {
		return g_shalgo.m_heaproutines.m_alloc_from_heap(heap, nbytes, heapflags);
	}
	static inline void free_from_heap(sh_heap_t heap, const void* mem, unsigned heapflags) {
		return g_shalgo.m_heaproutines.m_free_from_heap(heap, const_cast<void*>(mem), heapflags);
	}

	static inline void* realloc_from_heap(sh_heap_t heap, void* oldmem, size_t nbytes_new, unsigned heapflags) {
		return g_shalgo.m_heaproutines.m_realloc_from_heap(heap, oldmem, nbytes_new, heapflags);
	}
	static inline bool lock_heap(sh_heap_t heap) {
		return g_shalgo.m_heaproutines.m_lock_heap(heap);
	}
	static inline bool unlock_heap(sh_heap_t heap) {
		return g_shalgo.m_heaproutines.m_unlock_heap(heap);
	}
	template <class T, sh_heap_t* static_heap_ptr, unsigned heapflags = 0>
	class sh_heap_based_allocator_t : public std::allocator<T>
	{
	public:
		typedef size_t    size_type;
		typedef ptrdiff_t difference_type;
		typedef T* pointer;
		typedef const T* const_pointer;
		typedef T& reference;
		typedef const T& const_reference;
		typedef T         value_type;

		inline sh_heap_based_allocator_t() {}
		inline sh_heap_based_allocator_t(const sh_heap_based_allocator_t&) {}



		static inline pointer   allocate(size_type n, const void* = 0) {

			//return (pointer)cn_threadpriv_allocator()->allocate(sizeof(T) * n);
			return (pointer)alloc_from_heap(*static_heap_ptr, n * sizeof(T), heapflags);
		}

		static inline void      deallocate(void* p, size_type hint) {
			//cn_threadpriv_allocator()->deallocate(p, hint);
			free_from_heap(*static_heap_ptr, p, heapflags);
		}

		static inline pointer           address(reference x) { return &x; }
		static inline const_pointer     address(const_reference x) { return &x; }
		sh_heap_based_allocator_t<T, static_heap_ptr, heapflags>& operator=(const sh_heap_based_allocator_t&) { return *this; }
		static inline void              construct(pointer p, const T& val)
		{
			new ((T*)p) T(val);
		}
		static inline void              destroy(pointer p) { p->~T(); }

		static inline size_type         max_size() { return size_t(~0ULL); }

		template <class U>
		struct rebind { typedef sh_heap_based_allocator_t<U, static_heap_ptr, heapflags> other; };

		template <class U>
		inline sh_heap_based_allocator_t(const sh_heap_based_allocator_t<U, static_heap_ptr, heapflags>&) {}

		template <class U>
		inline sh_heap_based_allocator_t& operator=(const sh_heap_based_allocator_t<U, static_heap_ptr, heapflags>&) { return *this; }
	};
}
#define	DISABLE_RB_ASM
namespace sh::rb {
	static inline rb_node* rb_next(const struct rb_node* n) {
		return g_shalgo.m_rbroutines.m_rb_next(n);
	}
	static inline rb_node* rb_prev(const struct rb_node* n) {
		return g_shalgo.m_rbroutines.m_rb_prev(n);
	}
	static inline rb_node* rb_first(const struct rb_root* r) {
		return g_shalgo.m_rbroutines.m_rb_first(r);
	}
	static inline rb_node* rb_last(const struct rb_root* r) {
		return g_shalgo.m_rbroutines.m_rb_last(r);
	}
	static inline void asm_rb_insert_color(struct rb_node* n, struct rb_root* r, low_gpregs_t* saverestore_area) {

		register rb_root* rpass asm("r14") = r;
		register low_gpregs_t* rsave asm("r10") = saverestore_area;

		__asm__ volatile(
			"call cs_rbnode_insert_and_color\n\t"
			:
		: "r"(rpass), "r"(rsave), "c" (n)
			: "r8", "r9"
			);
	}

	static inline void asm_rb_erase(struct rb_node* n, struct rb_root* r, low_gpregs_t* saverestore_area) {

		register rb_root* rpass asm("r14") = r;
		register low_gpregs_t* rsave asm("r10") = saverestore_area;

		__asm__ volatile(
			"call cs_rbnode_erase_from\n\t"
			:
			: "r"(rpass), "r"(rsave), "d" (n)
			: "r8", "r9"
			);
	}
	static inline void rb_insert_color(struct rb_node* n, struct rb_root* r) {
		low_gpregs_t saver;
#if !defined(DISABLE_RB_ASM)
		asm_rb_insert_color(n, r, &saver);
#else
		return g_shalgo.m_rbroutines.m_rb_insert_color(n, r);
#endif
	}

	static inline void rb_erase(struct rb_node* n, struct rb_root* r) {
#if !defined(DISABLE_RB_ASM)
		low_gpregs_t saver;
		asm_rb_erase(n, r, &saver);
#else
		g_shalgo.m_rbroutines.m_rb_erase(n, r);
#endif
	}
	struct insert_hint_t {
		rb_node** iterpos_hint;
		rb_node* parent_hint;

		void insert(rb_node* new_node, rb_root* into_tree) {
			rb_link_node(new_node, parent_hint, iterpos_hint);
			rb_insert_color(new_node, into_tree);
		}
	};

	template<typename T, unsigned delta_to_object, typename TR, typename TComp>
	static inline T* rbnode_find(rb_root* root, TR r, TComp&& cmp, insert_hint_t* hint) {
		rb_node** _new = &root->rb_node;

		rb_node* parent = nullptr;

		while (*_new) {
			auto n = *_new;



			auto cont = reinterpret_cast<T*>(reinterpret_cast<char*>(n) - delta_to_object);

			ptrdiff_t cmpres = cmp(cont, r);
			parent = *_new;
			if (!cmpres) {
				return cont;
			}

			_new = (&n->rb_right) + (cmpres > 0);//n->get_left_or_right_ptr(cmpres > 0);

		}

		if (hint) {
			hint->iterpos_hint = _new;
			hint->parent_hint = parent;
		}
		return nullptr;
	}
	template<typename TContainer, unsigned delt>
	struct rb_nodes_dummy_iter_t {
		rb_node* m_node;
		inline TContainer* operator *() {
			return reinterpret_cast<TContainer*>(reinterpret_cast<char*>(m_node) - delt);
		}
		inline rb_nodes_dummy_iter_t<TContainer, delt>& operator ++() {
			m_node = rb_next(m_node);
			return *this;
		}
		constexpr bool operator !=(std::nullptr_t p) const {
			return m_node != p;
		}
	};

	template<typename TContainer, unsigned delt>
	struct rb_nodes_iter_t {
		rb_root* m_current;

		rb_nodes_iter_t(rb_root& root) : m_current(&root) {}


		std::nullptr_t  end() {
			return nullptr;
		}

		const std::nullptr_t end() const {
			return nullptr;
		}

		rb_nodes_dummy_iter_t<TContainer, delt> begin() {
			return rb_nodes_dummy_iter_t<TContainer, delt>{.m_node = rb_first(m_current) };
		}

	};
	template<typename TContainer, unsigned delt>
	static inline rb_nodes_iter_t<TContainer, delt> rb_iterate(rb_root& root) {
		return rb_nodes_iter_t<TContainer, delt>{root};
	}


}
#ifndef SHALGO_DISABLE_BITMAP
namespace sh::bitmap {
	static inline bool bits_are_set(unsigned* bits, unsigned nbits_allocated_for, unsigned startidx, unsigned length) {
		return g_shalgo.m_bmproutines.m_bitmap_bits_are_set(bits, nbits_allocated_for, startidx, length);
	}
	static inline bool bits_are_clear(unsigned* bits, unsigned nbits_allocated_for, unsigned startidx, unsigned length) {
		return g_shalgo.m_bmproutines.m_bitmap_bits_are_clear(bits, nbits_allocated_for, startidx, length);
	}
	static inline void clear_bits(unsigned* bits, unsigned nbits_allocated_for, unsigned startidx, unsigned ntoclear) {
		return g_shalgo.m_bmproutines.m_bitmap_clear_bits(bits, nbits_allocated_for, startidx, ntoclear);
	}
	static inline unsigned find_n_clear_from(unsigned* bits, unsigned nbits_allocated_for, unsigned ntofind, unsigned start) {
		return g_shalgo.m_bmproutines.m_bitmap_find_n_clear_from(bits, nbits_allocated_for, ntofind, start);
	}
	static inline unsigned find_n_clear_from_and_set(unsigned* bits, unsigned nbits_allocated_for, unsigned ntofind, unsigned start) {
		return g_shalgo.m_bmproutines.m_bitmap_find_n_clear_from_and_set(bits, nbits_allocated_for, ntofind, start);
	}

	static inline void set_bits(unsigned* bits, unsigned nbits_allocated_for, unsigned startidx, unsigned ntoset) {
		return g_shalgo.m_bmproutines.m_bitmap_set_bits(bits, nbits_allocated_for, startidx, ntoset);

	}
	static inline unsigned find_n_set_from(unsigned* bits, unsigned nbits_allocated_for, unsigned ntofind, unsigned start) {
		return g_shalgo.m_bmproutines.m_bitmap_find_n_set_from(bits, nbits_allocated_for, ntofind, start);

	}
	static inline unsigned find_n_set_from_and_clear(unsigned* bits, unsigned nbits_allocated_for, unsigned ntofind, unsigned start) {

		return g_shalgo.m_bmproutines.m_bitmap_find_n_set_from_and_clear(bits, nbits_allocated_for, ntofind, start);

	}
}
#endif
namespace sh::memops {
	static inline void nt_move_mem(void* to, const void* from, size_t size) {
		return g_shalgo.m_memroutines.m_ntmovemem(to, from, size);
	}
	static inline void nt_zero_mem(void* to, size_t size) {
		return g_shalgo.m_memroutines.m_ntzeromem(to, size);
	}

	static inline bool flushcache(const void* mem, size_t size) {
		return g_shalgo.m_memroutines.m_flushcache(mem, size);
	}
	static inline bool cleancache(const void* mem, size_t size) {
		return g_shalgo.m_memroutines.m_cleancache(mem, size);
	}
	/*
		moves memory at from to to, flushing the contents of from from the cache
	*/
	static inline void nt_move_mem_flush(void* to, const void* from, size_t size) {
		return g_shalgo.m_memroutines.m_movemem_flush(to, from, size);
	}
	/*
		moves memory at from to to, cleaning (writing back) the cache for from

	*/
	static inline void nt_move_mem_clean(void* to, const void* from, size_t size) {
		return g_shalgo.m_memroutines.m_movemem_clean(to, from, size);
	}
	static inline void sfence_if(bool b = true) {
		if (b) {
			_mm_sfence();
		}
	}

	static inline void prefetchNT(const void* mem) {
		_mm_prefetch((const char*)mem, _MM_HINT_NTA);
	}
	static inline void smol_memcpy(void* to, const void* from, size_t size) {
		g_shalgo.m_memroutines.m_smol_memcpy(to, from, size);
	}

	static inline void smol_memzero(void* to, size_t size) {
		g_shalgo.m_memroutines.m_smol_memzero(to, size);
	}

	static inline void memcpy_with_strategy(void* to, const void* from, size_t size, memcpy_strategy_e strat) {
		switch (strat) {
		case memcpy_strategy_e::STANDARD_MEMCPY:
			memcpy(to, from, size);
			return;
		case memcpy_strategy_e::NT_COPY:
			nt_move_mem(to, from, size);
			return;
		case memcpy_strategy_e::NT_COPY_CLEAN:
			nt_move_mem_clean(to, from, size);
			return;
		case memcpy_strategy_e::NT_COPY_FLUSH:
			nt_move_mem_flush(to, from, size);
			return;
		case memcpy_strategy_e::SMOL_CPY:
			smol_memcpy(to, from, size);
			return;
		}
	}
}

namespace sh::vmem {
	static inline void* allocate_rw(size_t size) { return g_shalgo.m_vmemroutines.m_allocate_rw(size); }
	static inline void* allocate_rwx(size_t size) { return g_shalgo.m_vmemroutines.m_allocate_rwx(size); }
	static inline void* allocate_rwx_absolute(size_t size, void* where) { return g_shalgo.m_vmemroutines.m_allocate_rwx_absolute(size, where); }
	static inline void* allocate_rw_absolute(size_t size, void* where) { return g_shalgo.m_vmemroutines.m_allocate_rw_absolute(size, where); }
	static inline void release_rw(void* mem) { return g_shalgo.m_vmemroutines.m_release_rw(mem); }
	static inline void discard_contents(void* mem, size_t size) { return g_shalgo.m_vmemroutines.m_discard_contents(mem, size); }
	static inline void prefetch_for_seq_access(void* mem, size_t size) { return g_shalgo.m_vmemroutines.m_prefetch_for_seq_access(mem, size); }
	static inline int try_enable_hugepages() {
		return g_shalgo.m_vmemroutines.m_try_enable_hugepages();
	}
}
namespace sh::coros {

	static inline uintptr_t coroswitch(coro_base_t* from, coro_base_t* to) {
		return g_shalgo.m_cororoutines.m_coroswitch(from, to);
	}
	struct coro_t : public coro_base_t {
		uintptr_t yield_to(coro_t* other) {
			return coroswitch(this, other);
		}
		template<typename TYield>
		uintptr_t yield_to(coro_t* other, TYield valto) {
			static_assert(sizeof(TYield) == 8);
			other->m_rax = (uintptr_t)valto;
			return coroswitch(this, other);
		}
	};
}

namespace sh::string {
	MH_LEAF
		static inline bool streq(MH_NOESCAPE const char* s1, MH_NOESCAPE const char* s2) { return g_shalgo.m_sroutines.m_streq(s1, s2); }
	MH_LEAF
		static inline bool strieq(MH_NOESCAPE const char* s1, MH_NOESCAPE const char* s2) { return g_shalgo.m_sroutines.m_strieq(s1, s2); }
	MH_LEAF
		static inline unsigned strlength(MH_NOESCAPE const char* s1) { return g_shalgo.m_sroutines.m_strlen(s1); }
	MH_LEAF
		static inline int str_to_long(const char* str, const char** out_endptr, unsigned base) { return g_shalgo.m_sroutines.m_str_to_long(str, out_endptr, base); }
	MH_LEAF
		static inline int atoi_fast(MH_NOESCAPE const char* str) { return g_shalgo.m_sroutines.m_atoi_fast(str); }
	MH_LEAF
		static inline unsigned int2str_base10(unsigned int val, MH_NOESCAPE char* dstbuf, unsigned size) { return g_shalgo.m_sroutines.m_int2str_base10(val, dstbuf, size); }
	MH_LEAF
		static inline unsigned int2str_base16(unsigned int val, MH_NOESCAPE char* dstbuf, unsigned size) { return g_shalgo.m_sroutines.m_int2str_base16(val, dstbuf, size); }
	MH_LEAF
		static inline unsigned uint2str_base10(unsigned int val, MH_NOESCAPE char* dstbuf, unsigned size) { return g_shalgo.m_sroutines.m_uint2str_base10(val, dstbuf, size); }

	//precision default=4
	MH_LEAF
		static inline unsigned float2str_fast(float fvalue, MH_NOESCAPE char* dstbuf, unsigned precision = 4) { return g_shalgo.m_sroutines.m_float2str_fast(fvalue, dstbuf, precision); }
	//out_endpos default = nullptr
	MH_LEAF
		static inline double fast_atof(const char* p, const char** out_endpos = nullptr) { return g_shalgo.m_sroutines.m_fast_atof(p, out_endpos); }
	MH_LEAF
		static inline int strcmp(MH_NOESCAPE const char* s1, MH_NOESCAPE const char* s2) {
		return g_shalgo.m_sroutines.m_strcmp(s1, s2);
	}
	MH_LEAF
		static inline const char* strstr(const char* s1, MH_NOESCAPE const char* needle) {
		return g_shalgo.m_sroutines.m_strstr(s1, needle);
	}
	MH_LEAF
		static inline const char* strchr(const char* s1, char c) {
		return g_shalgo.m_sroutines.m_strchr(s1, c);
	}
	MH_LEAF
		static inline char* strcpy(MH_NOESCAPE char* destbuf, MH_NOESCAPE const char* srcb) {
		return g_shalgo.m_sroutines.m_strcpy(destbuf, srcb);
	}

	MH_LEAF
		static inline unsigned to_unicode(MH_NOESCAPE wchar_t* destbuf, MH_NOESCAPE const char* srcb) {
		return g_shalgo.m_sroutines.m_to_unicode(destbuf, srcb);
	}
	MH_LEAF
		static inline unsigned from_unicode(MH_NOESCAPE char* destbuf, MH_NOESCAPE const wchar_t* srcb) {
		return g_shalgo.m_sroutines.m_from_unicode(destbuf, srcb);
	}

	MH_LEAF
		static inline unsigned find_string_insens(const char* haystack, const char* needle) {
		return g_shalgo.m_sroutines.m_find_str_insens(haystack, needle);
	}
	static inline
		unsigned strlen_smol(const char* s) {
#if defined(MAY_USE_INLINE_ASM)
		unsigned output = 0;
		unsigned findval = 0;
		const char* savedstart = s;
		unsigned ecx = 0;
		//bitnot is done to generate 0xFFFFFFFF in 4 bytes instead of five
		__asm__(
			"notl %1\n\t"
			"repne scasb"
			: "+D"(s), "+c"(ecx)
			: "a"(findval)
			: );
		return ~ecx;
#else
		return strlen(s);
#endif
	}
}

namespace sh::smt {
	static inline bool cmpxchg16b(volatile uint64_t* destination, uint64_t xhigh, uint64_t xlow, uint64_t* cmpres) { return g_shalgo.m_smtroutines.m_cmpxchg16b(destination, xhigh, xlow, cmpres); }
	static inline int xadd32(volatile int* value, int addend) { return g_shalgo.m_smtroutines.m_xadd32(value, addend); }
	static inline int64_t xadd64(volatile int64_t* value, int64_t addend) { return g_shalgo.m_smtroutines.m_xadd64(value, addend); }

	static inline bool bts64(volatile uint64_t* m_ptr, unsigned m_bitidx) { return g_shalgo.m_smtroutines.m_bts64(m_ptr, m_bitidx); }
	static inline bool btr64(volatile uint64_t* m_ptr, unsigned m_bitidx) { return g_shalgo.m_smtroutines.m_btr64(m_ptr, m_bitidx); }
	static inline bool btc64(volatile uint64_t* m_ptr, unsigned m_bitidx) { return g_shalgo.m_smtroutines.m_btc64(m_ptr, m_bitidx); }

	static inline uintptr_t create_thread(sh_thrdproc_t function, void* parms, unsigned procnum, size_t stackSize, bool suspended, int* out_threadid) { return g_shalgo.m_smtroutines.m_create_thread(function, parms, procnum, stackSize, suspended, out_threadid); }

	static inline void yield_thread() { return g_shalgo.m_smtroutines.m_yield_thread(); }
	static inline void sleep_thread(size_t ms) { return g_shalgo.m_smtroutines.m_sleep_thread(ms); }


	static inline snaphak_signal_t create_signal() { return g_shalgo.m_smtroutines.m_create_signal(); }
	static inline void destroy_signal(snaphak_signal_t s) { return g_shalgo.m_smtroutines.m_destroy_signal(s); }
	static inline void signal_raise(snaphak_signal_t sig) { return g_shalgo.m_smtroutines.m_signal_raise(sig); }
	static inline bool signal_wait(snaphak_signal_t sig, int64_t* timeout) { return g_shalgo.m_smtroutines.m_signal_wait(sig, timeout); }
	static inline bool signal_raise_and_wait(snaphak_signal_t raise, snaphak_signal_t wait, int64_t* timeout) { return g_shalgo.m_smtroutines.m_signal_raise_and_wait(raise, wait, timeout); }
}

namespace sh::sns {
	static inline void sort4ptr(void** ptrs) { return g_shalgo.m_snsroutines.m_sort4ptr(ptrs); }
	//static inline void sort16ptr_with_scratch(void** ptrs, void** scratchbuf) { return g_shalgo.m_snsroutines.m_sort16ptr_with_scratch(ptrs, scratchbuf); }
	static inline void sort16ptr_unrolled(void** ptrs) { return g_shalgo.m_snsroutines.m_sort16ptr_unrolled(ptrs); }
	static inline void sort16ptr(void* ptrs[8]) { return g_shalgo.m_snsroutines.m_sort16ptr(ptrs); }
}
#ifndef SHALGO_DISABLE_INTBULK
namespace sh::ibulk {
	static inline unsigned find_first_equal32(unsigned* values, unsigned nvalues, unsigned tofind) { return g_shalgo.m_ibulkroutines.m_find_first_equal32(values, nvalues, tofind); }
	static inline unsigned find_first_equal16(unsigned short* values, unsigned nvalues, unsigned tofind) { return g_shalgo.m_ibulkroutines.m_find_first_equal16(values, nvalues, tofind); }

	static inline unsigned find_first_notequal32(unsigned* values, unsigned nvalues, unsigned tofind) { return g_shalgo.m_ibulkroutines.m_find_first_notequal32(values, nvalues, tofind); }
	static inline void addscalar_32(unsigned* values, unsigned nvalues, unsigned addend) { return g_shalgo.m_ibulkroutines.m_addscalar_32(values, nvalues, addend); }
	static inline void subscalar_32(unsigned* values, unsigned nvalues, unsigned addend) { return g_shalgo.m_ibulkroutines.m_subscalar_32(values, nvalues, addend); }
	static inline void mulscalar_32(unsigned* values, unsigned nvalues, unsigned mult) { return g_shalgo.m_ibulkroutines.m_mulscalar_32(values, nvalues, mult); }
	//if the value does not exist then you're in for a crash
	MH_LEAF
		static inline unsigned find_indexof_u64(unsigned long long* MH_NOESCAPE values, unsigned long long findval) {

		return g_shalgo.m_ibulkroutines.m_indexof_u64(values, findval);
	}
}
#endif
namespace sh::math {

	static constexpr double PI = 3.14159265358979323846;

	static constexpr double	M_DEG2RAD = PI / 180.0;
	static constexpr double	M_RAD2DEG = 180.0 / PI;
	static constexpr double DEG2RAD(double a) {
		return (a)*M_DEG2RAD;
	}
	static double precise_normalize3d(double* valuesxyz) {
		return g_shalgo.m_realroutines.m_precise_normalize(valuesxyz);
	}
	struct real_t {
		sh_real_t m_data;

		static real_t from_int64(int64_t v) {
			return real_t{ .m_data = g_shalgo.m_realroutines.m_real_from_i64(v) };
		}
		static real_t from_int32(int32_t v) {
			return real_t{ .m_data = g_shalgo.m_realroutines.m_real_from_i32(v) };
		}
		static real_t from_float(float v) {
			return real_t{ .m_data = g_shalgo.m_realroutines.m_real_from_double(v) };
		}

		static real_t floorr(real_t r) {
			return real_t{ .m_data = g_shalgo.m_realroutines.m_floor(r.m_data) };
		}
		static real_t from_double(double v) {
			return real_t{ .m_data = g_shalgo.m_realroutines.m_real_from_double(v) };
		}

		inline real_t operator +(real_t other) const {
			return real_t{ .m_data = g_shalgo.m_realroutines.m_add_real(this->m_data, other.m_data) };
		}
		inline real_t operator -(real_t other) const {
			return real_t{ .m_data = g_shalgo.m_realroutines.m_sub_real(this->m_data, other.m_data) };
		}
		inline real_t operator *(real_t other) const {
			return real_t{ .m_data = g_shalgo.m_realroutines.m_mul_real(this->m_data, other.m_data) };
		}
		inline real_t operator /(real_t other) const {
			return real_t{ .m_data = g_shalgo.m_realroutines.m_div_real(this->m_data, other.m_data) };
		}
		inline real_t& operator +=(real_t other) {
			*this = *this + other;
			return *this;
		}
		inline real_t& operator -=(real_t other) {
			*this = *this - other;
			return *this;
		}
		inline real_t& operator *=(real_t other) {
			*this = *this * other;
			return *this;
		}
		inline real_t& operator /=(real_t other) {
			*this = *this / other;
			return *this;
		}
		inline real_t operator -() const {
			return real_t{ .m_data = g_shalgo.m_realroutines.m_neg_real(this->m_data) };
		}

		static real_t sqrtr(real_t value) {
			return real_t{ .m_data = g_shalgo.m_realroutines.m_sqrt_real(value.m_data) };
		}


		static void sincos(real_t value, real_t* out_sin, real_t* out_cos) {
			g_shalgo.m_realroutines.m_sincos(value.m_data, &out_sin->m_data, &out_cos->m_data);
		}
		explicit
			inline operator double() const {
			return g_shalgo.m_realroutines.m_double_from_real(this->m_data);
		}
		explicit
			inline operator float() const {
			return g_shalgo.m_realroutines.m_float_from_real(this->m_data);
		}
		static real_t atan2r(real_t first, real_t second) {
			return real_t{ .m_data = g_shalgo.m_realroutines.m_atan2(first.m_data, second.m_data) };
		}
	};

	static void sincos(double d, double& s, double& c) {

		sh::math::real_t dr = real_t::from_double(d);
		real_t ss, cc;

		dr.sincos(dr, &ss, &cc);

		s = (double)ss;
		c = (double)cc;
	}
	struct realvec3_t {
		real_t x, y, z;

		realvec3_t() : x{ 0 }, y{ 0 }, z{ 0 }
		{}
		realvec3_t(real_t inx, real_t iny, real_t inz) : x(inx), y(iny), z(inz) {}

		inline realvec3_t operator +(realvec3_t other) const {
			return realvec3_t{ x + other.x, y + other.y, z + other.z };
		}
		inline realvec3_t operator -(realvec3_t other) const {
			return realvec3_t{ x - other.x, y - other.y, z - other.z };
		}
		inline realvec3_t operator /(realvec3_t other) const {
			return realvec3_t{ x / other.x, y / other.y, z / other.z };
		}
		inline realvec3_t operator *(realvec3_t other) const {
			return realvec3_t{ x * other.x, y * other.y, z * other.z };
		}

		inline real_t normalize() {
			real_t sqrlen = (x * x) + (y * y) + (z * z);
			real_t invlen = real_t::sqrtr(sqrlen);
			x /= invlen;
			y /= invlen;
			z /= invlen;
			return sqrlen * invlen;
		}

		inline realvec3_t cross(realvec3_t a) const {
			return realvec3_t{ y * a.z - z * a.y, z * a.x - x * a.z, x * a.y - y * a.x };
		}
		inline real_t length_sqr() const {
			return x * x + y * y + z * z;

		}
		inline real_t length() const {
			return real_t::sqrtr(length_sqr());
		}
		inline real_t dot(realvec3_t other) {
			realvec3_t tmp = *this * other;
			return tmp.x + tmp.y + tmp.z;
		}
	};

	

	struct snaphak_rendermatrix_t {
		real_t m[16];

		static snaphak_rendermatrix_t multiply(snaphak_rendermatrix_t& a, snaphak_rendermatrix_t& b) {

			snaphak_rendermatrix_t out;
			out.m[0 * 4 + 0] = a.m[0 * 4 + 0] * b.m[0 * 4 + 0] + a.m[0 * 4 + 1] * b.m[1 * 4 + 0] + a.m[0 * 4 + 2] * b.m[2 * 4 + 0] + a.m[0 * 4 + 3] * b.m[3 * 4 + 0];
			out.m[0 * 4 + 1] = a.m[0 * 4 + 0] * b.m[0 * 4 + 1] + a.m[0 * 4 + 1] * b.m[1 * 4 + 1] + a.m[0 * 4 + 2] * b.m[2 * 4 + 1] + a.m[0 * 4 + 3] * b.m[3 * 4 + 1];
			out.m[0 * 4 + 2] = a.m[0 * 4 + 0] * b.m[0 * 4 + 2] + a.m[0 * 4 + 1] * b.m[1 * 4 + 2] + a.m[0 * 4 + 2] * b.m[2 * 4 + 2] + a.m[0 * 4 + 3] * b.m[3 * 4 + 2];
			out.m[0 * 4 + 3] = a.m[0 * 4 + 0] * b.m[0 * 4 + 3] + a.m[0 * 4 + 1] * b.m[1 * 4 + 3] + a.m[0 * 4 + 2] * b.m[2 * 4 + 3] + a.m[0 * 4 + 3] * b.m[3 * 4 + 3];

			out.m[1 * 4 + 0] = a.m[1 * 4 + 0] * b.m[0 * 4 + 0] + a.m[1 * 4 + 1] * b.m[1 * 4 + 0] + a.m[1 * 4 + 2] * b.m[2 * 4 + 0] + a.m[1 * 4 + 3] * b.m[3 * 4 + 0];
			out.m[1 * 4 + 1] = a.m[1 * 4 + 0] * b.m[0 * 4 + 1] + a.m[1 * 4 + 1] * b.m[1 * 4 + 1] + a.m[1 * 4 + 2] * b.m[2 * 4 + 1] + a.m[1 * 4 + 3] * b.m[3 * 4 + 1];
			out.m[1 * 4 + 2] = a.m[1 * 4 + 0] * b.m[0 * 4 + 2] + a.m[1 * 4 + 1] * b.m[1 * 4 + 2] + a.m[1 * 4 + 2] * b.m[2 * 4 + 2] + a.m[1 * 4 + 3] * b.m[3 * 4 + 2];
			out.m[1 * 4 + 3] = a.m[1 * 4 + 0] * b.m[0 * 4 + 3] + a.m[1 * 4 + 1] * b.m[1 * 4 + 3] + a.m[1 * 4 + 2] * b.m[2 * 4 + 3] + a.m[1 * 4 + 3] * b.m[3 * 4 + 3];

			out.m[2 * 4 + 0] = a.m[2 * 4 + 0] * b.m[0 * 4 + 0] + a.m[2 * 4 + 1] * b.m[1 * 4 + 0] + a.m[2 * 4 + 2] * b.m[2 * 4 + 0] + a.m[2 * 4 + 3] * b.m[3 * 4 + 0];
			out.m[2 * 4 + 1] = a.m[2 * 4 + 0] * b.m[0 * 4 + 1] + a.m[2 * 4 + 1] * b.m[1 * 4 + 1] + a.m[2 * 4 + 2] * b.m[2 * 4 + 1] + a.m[2 * 4 + 3] * b.m[3 * 4 + 1];
			out.m[2 * 4 + 2] = a.m[2 * 4 + 0] * b.m[0 * 4 + 2] + a.m[2 * 4 + 1] * b.m[1 * 4 + 2] + a.m[2 * 4 + 2] * b.m[2 * 4 + 2] + a.m[2 * 4 + 3] * b.m[3 * 4 + 2];
			out.m[2 * 4 + 3] = a.m[2 * 4 + 0] * b.m[0 * 4 + 3] + a.m[2 * 4 + 1] * b.m[1 * 4 + 3] + a.m[2 * 4 + 2] * b.m[2 * 4 + 3] + a.m[2 * 4 + 3] * b.m[3 * 4 + 3];

			out.m[3 * 4 + 0] = a.m[3 * 4 + 0] * b.m[0 * 4 + 0] + a.m[3 * 4 + 1] * b.m[1 * 4 + 0] + a.m[3 * 4 + 2] * b.m[2 * 4 + 0] + a.m[3 * 4 + 3] * b.m[3 * 4 + 0];
			out.m[3 * 4 + 1] = a.m[3 * 4 + 0] * b.m[0 * 4 + 1] + a.m[3 * 4 + 1] * b.m[1 * 4 + 1] + a.m[3 * 4 + 2] * b.m[2 * 4 + 1] + a.m[3 * 4 + 3] * b.m[3 * 4 + 1];
			out.m[3 * 4 + 2] = a.m[3 * 4 + 0] * b.m[0 * 4 + 2] + a.m[3 * 4 + 1] * b.m[1 * 4 + 2] + a.m[3 * 4 + 2] * b.m[2 * 4 + 2] + a.m[3 * 4 + 3] * b.m[3 * 4 + 2];
			out.m[3 * 4 + 3] = a.m[3 * 4 + 0] * b.m[0 * 4 + 3] + a.m[3 * 4 + 1] * b.m[1 * 4 + 3] + a.m[3 * 4 + 2] * b.m[2 * 4 + 3] + a.m[3 * 4 + 3] * b.m[3 * 4 + 3];
			return out;
		}

		void from_float_matrix(float vals[16]) {
			for (unsigned i = 0; i < 16; ++i)
				m[i] = real_t::from_float(vals[i]);
		}

		void to_float_matrix(float vals[16]) {
			for (unsigned i = 0; i < 16; ++i)
				vals[i] = static_cast<float>(m[i]);
		}

		static void inverse(snaphak_rendermatrix_t& out, snaphak_rendermatrix_t& src) {
			const int FRL = 4;

			// 84+4+16 = 104 multiplications
			//			   1 division

			// 2x2 sub-determinants required to calculate 4x4 determinant
			const real_t det2_01_01 = src.m[0 * FRL + 0] * src.m[1 * FRL + 1] - src.m[0 * FRL + 1] * src.m[1 * FRL + 0];
			const real_t det2_01_02 = src.m[0 * FRL + 0] * src.m[1 * FRL + 2] - src.m[0 * FRL + 2] * src.m[1 * FRL + 0];
			const real_t det2_01_03 = src.m[0 * FRL + 0] * src.m[1 * FRL + 3] - src.m[0 * FRL + 3] * src.m[1 * FRL + 0];
			const real_t det2_01_12 = src.m[0 * FRL + 1] * src.m[1 * FRL + 2] - src.m[0 * FRL + 2] * src.m[1 * FRL + 1];
			const real_t det2_01_13 = src.m[0 * FRL + 1] * src.m[1 * FRL + 3] - src.m[0 * FRL + 3] * src.m[1 * FRL + 1];
			const real_t det2_01_23 = src.m[0 * FRL + 2] * src.m[1 * FRL + 3] - src.m[0 * FRL + 3] * src.m[1 * FRL + 2];

			// 3x3 sub-determinants required to calculate 4x4 determinant
			const real_t det3_201_012 = src.m[2 * FRL + 0] * det2_01_12 - src.m[2 * FRL + 1] * det2_01_02 + src.m[2 * FRL + 2] * det2_01_01;
			const real_t det3_201_013 = src.m[2 * FRL + 0] * det2_01_13 - src.m[2 * FRL + 1] * det2_01_03 + src.m[2 * FRL + 3] * det2_01_01;
			const real_t det3_201_023 = src.m[2 * FRL + 0] * det2_01_23 - src.m[2 * FRL + 2] * det2_01_03 + src.m[2 * FRL + 3] * det2_01_02;
			const real_t det3_201_123 = src.m[2 * FRL + 1] * det2_01_23 - src.m[2 * FRL + 2] * det2_01_13 + src.m[2 * FRL + 3] * det2_01_12;

			const real_t det = (-det3_201_123 * src.m[3 * FRL + 0] + det3_201_023 * src.m[3 * FRL + 1] - det3_201_013 * src.m[3 * FRL + 2] + det3_201_012 * src.m[3 * FRL + 3]);

			/*if (idMath::Fabs(det) < RENDER_MATRIX_INVERSE_EPSILON)
			{
				return false;
			}*/

			//	const real_t rcpDet = 1.0f / det;

				// remaining 2x2 sub-determinants
			const real_t det2_03_01 = src.m[0 * FRL + 0] * src.m[3 * FRL + 1] - src.m[0 * FRL + 1] * src.m[3 * FRL + 0];
			const real_t det2_03_02 = src.m[0 * FRL + 0] * src.m[3 * FRL + 2] - src.m[0 * FRL + 2] * src.m[3 * FRL + 0];
			const real_t det2_03_03 = src.m[0 * FRL + 0] * src.m[3 * FRL + 3] - src.m[0 * FRL + 3] * src.m[3 * FRL + 0];
			const real_t det2_03_12 = src.m[0 * FRL + 1] * src.m[3 * FRL + 2] - src.m[0 * FRL + 2] * src.m[3 * FRL + 1];
			const real_t det2_03_13 = src.m[0 * FRL + 1] * src.m[3 * FRL + 3] - src.m[0 * FRL + 3] * src.m[3 * FRL + 1];
			const real_t det2_03_23 = src.m[0 * FRL + 2] * src.m[3 * FRL + 3] - src.m[0 * FRL + 3] * src.m[3 * FRL + 2];

			const real_t det2_13_01 = src.m[1 * FRL + 0] * src.m[3 * FRL + 1] - src.m[1 * FRL + 1] * src.m[3 * FRL + 0];
			const real_t det2_13_02 = src.m[1 * FRL + 0] * src.m[3 * FRL + 2] - src.m[1 * FRL + 2] * src.m[3 * FRL + 0];
			const real_t det2_13_03 = src.m[1 * FRL + 0] * src.m[3 * FRL + 3] - src.m[1 * FRL + 3] * src.m[3 * FRL + 0];
			const real_t det2_13_12 = src.m[1 * FRL + 1] * src.m[3 * FRL + 2] - src.m[1 * FRL + 2] * src.m[3 * FRL + 1];
			const real_t det2_13_13 = src.m[1 * FRL + 1] * src.m[3 * FRL + 3] - src.m[1 * FRL + 3] * src.m[3 * FRL + 1];
			const real_t det2_13_23 = src.m[1 * FRL + 2] * src.m[3 * FRL + 3] - src.m[1 * FRL + 3] * src.m[3 * FRL + 2];

			// remaining 3x3 sub-determinants
			const real_t det3_203_012 = src.m[2 * FRL + 0] * det2_03_12 - src.m[2 * FRL + 1] * det2_03_02 + src.m[2 * FRL + 2] * det2_03_01;
			const real_t det3_203_013 = src.m[2 * FRL + 0] * det2_03_13 - src.m[2 * FRL + 1] * det2_03_03 + src.m[2 * FRL + 3] * det2_03_01;
			const real_t det3_203_023 = src.m[2 * FRL + 0] * det2_03_23 - src.m[2 * FRL + 2] * det2_03_03 + src.m[2 * FRL + 3] * det2_03_02;
			const real_t det3_203_123 = src.m[2 * FRL + 1] * det2_03_23 - src.m[2 * FRL + 2] * det2_03_13 + src.m[2 * FRL + 3] * det2_03_12;

			const real_t det3_213_012 = src.m[2 * FRL + 0] * det2_13_12 - src.m[2 * FRL + 1] * det2_13_02 + src.m[2 * FRL + 2] * det2_13_01;
			const real_t det3_213_013 = src.m[2 * FRL + 0] * det2_13_13 - src.m[2 * FRL + 1] * det2_13_03 + src.m[2 * FRL + 3] * det2_13_01;
			const real_t det3_213_023 = src.m[2 * FRL + 0] * det2_13_23 - src.m[2 * FRL + 2] * det2_13_03 + src.m[2 * FRL + 3] * det2_13_02;
			const real_t det3_213_123 = src.m[2 * FRL + 1] * det2_13_23 - src.m[2 * FRL + 2] * det2_13_13 + src.m[2 * FRL + 3] * det2_13_12;

			const real_t det3_301_012 = src.m[3 * FRL + 0] * det2_01_12 - src.m[3 * FRL + 1] * det2_01_02 + src.m[3 * FRL + 2] * det2_01_01;
			const real_t det3_301_013 = src.m[3 * FRL + 0] * det2_01_13 - src.m[3 * FRL + 1] * det2_01_03 + src.m[3 * FRL + 3] * det2_01_01;
			const real_t det3_301_023 = src.m[3 * FRL + 0] * det2_01_23 - src.m[3 * FRL + 2] * det2_01_03 + src.m[3 * FRL + 3] * det2_01_02;
			const real_t det3_301_123 = src.m[3 * FRL + 1] * det2_01_23 - src.m[3 * FRL + 2] * det2_01_13 + src.m[3 * FRL + 3] * det2_01_12;

			out.m[0 * FRL + 0] = -det3_213_123 / det;
			out.m[1 * FRL + 0] = det3_213_023 / det;
			out.m[2 * FRL + 0] = -det3_213_013 / det;
			out.m[3 * FRL + 0] = det3_213_012 / det;

			out.m[0 * FRL + 1] = det3_203_123 / det;
			out.m[1 * FRL + 1] = -det3_203_023 / det;
			out.m[2 * FRL + 1] = det3_203_013 / det;
			out.m[3 * FRL + 1] = -det3_203_012 / det;

			out.m[0 * FRL + 2] = det3_301_123 / det;
			out.m[1 * FRL + 2] = -det3_301_023 / det;
			out.m[2 * FRL + 2] = det3_301_013 / det;
			out.m[3 * FRL + 2] = -det3_301_012 / det;

			out.m[0 * FRL + 3] = -det3_201_123 / det;
			out.m[1 * FRL + 3] = det3_201_023 / det;
			out.m[2 * FRL + 3] = -det3_201_013 / det;
			out.m[3 * FRL + 3] = det3_201_012 / det;
		}
	};
}
struct mh_string_view_t {
	const char* m_text;
	unsigned m_length;
	unsigned m_weak_hashcode;
};
//this might be handy, but im worried about it breaking msvc compatability
#if 1
template<template<snaphak_cpu_lvl_t lvlarg> class TImpl>
struct sh_multiversioner_t {
	template<typename... TArgs>

#if !defined(__clang__)
	template<typename... TArgs>
	static auto exec(TArgs... args) {

		return TImpl<snaphak_cpu_lvl_t::GenericCpu>::Run(args...);
	}
#else
	__attribute__((target("avx2,avx,bmi,bmi2")))
		__attribute__((flatten))
		static auto fnavx2(TArgs... args) {
		auto result = TImpl<snaphak_cpu_lvl_t::AVX2Cpu>::Run(args...);
		//__asm__ volatile ("vzeroupper");
		return result;
	}


	template<typename... TArgs>
	__attribute__((target("avx,avx2,avx512f,avx512bw,avx512dq,avx512cd,avx512vl")))
		__attribute__((flatten))
		static auto fnavx512(TArgs... args) {

		auto result = TImpl<snaphak_cpu_lvl_t::AVX512Cpu>::Run(args...);
	//	__asm__ volatile ("vzeroupper");
		return result;
	}
	//mark this as noinline so that the code is never fetched on cpus that support avx2/avx512
	template<typename... TArgs>
	__attribute__((noinline))
	static auto fngeneric(TArgs... args) {
		return TImpl<snaphak_cpu_lvl_t::GenericCpu>::Run(args...);
	}


	template<typename... TArgs>
	static auto exec(TArgs... args) {

		switch (g_shalgo.m_cpulevel) {

		case snaphak_cpu_lvl_t::AVX2Cpu:
			return fnavx2(args...);

		case snaphak_cpu_lvl_t::AVX512Cpu:
			return fnavx512(args...);

		default:
			return fngeneric(args...);
		}
	}
#endif
#endif







};
//multiversioner example:
/*
template<snaphak_cpu_lvl_t lvl>
struct my_test {
	MH_FORCEINLINE
		static double Run(double* input, unsigned n) {

		double result = 0;

		for (unsigned i = 0; i < n; ++i) {

			result += input[i];
		}
		return result;
	}
};


double sum_values(double* input, unsigned n) {
	return sh_multiversioner_t<my_test>::exec(input, n);
}*/
#endif