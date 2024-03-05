#include <cstdint>
#include <cassert>
#include <intrin.h>
#include "rb_impl.hpp"
#include "bmp_impl.hpp"
#include "str_impl.hpp"
#include "memop_impl.hpp"
#include "sortnsearch_impl.hpp"
#include "bulkint_impl.hpp"
extern void heapalgos_init(snaphak_heaproutines_t* routines);
extern void vmemalgos_init(snaphak_virtmemroutines_t* routines);
extern void coroalgos_init(snaphak_cororoutines_t* routines);
extern void smtalgos_init(snaphak_algo_t* algo);
extern void realnum_init(snaphak_realroutines_t* algo);
extern void netroutines_init(snaphak_netroutines_t* net);
#ifndef SHALGO_DISABLE_INTBULK
#define		INIT_IBULK(algos)		ibulkalgos_init(&algos->m_ibulkroutines)
#else
#define		INIT_IBULK(algos)
#endif

#ifndef SHALGO_DISABLE_BITMAP
#define		INIT_BMP_ROUTINES(algos)	bmp_algos_init(&algos->m_bmproutines);
#else
#define		INIT_BMP_ROUTINES(algos)
#endif

#define __INIT__IMPLS(suffix)	\
CS_NOINLINE \
CS_COLD_CODE \
void init_impls_##suffix (snaphak_algo_t* algos) {\
	rb_algos_init(&algos->m_rbroutines);\
	heapalgos_init(&algos->m_heaproutines);\
	INIT_BMP_ROUTINES(algos);\
	memop_algos_init(algos);\
	vmemalgos_init(&algos->m_vmemroutines);\
	coroalgos_init(&algos->m_cororoutines);\
	str_algos_init(&algos->m_sroutines);\
	smtalgos_init(algos);\
	snsalgos_init(&algos->m_snsroutines);\
	INIT_IBULK(algos);\
	realnum_init(&algos->m_realroutines);\
	netroutines_init(&algos->m_netroutines);\
}