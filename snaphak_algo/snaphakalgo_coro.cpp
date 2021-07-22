#include "snaphakalgo_predef.hpp"
#include "snaphakalgo.hpp"
extern "C"
uintptr_t coroswitch(coro_base_t* from, coro_base_t* to);

void coroalgos_init(snaphak_cororoutines_t* routines) {
	routines->m_coroswitch = coroswitch;
}