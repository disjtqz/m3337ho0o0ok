#pragma once



extern char* g_loadmaponstart;
extern char* g_overrides_subdir;
extern char* g_overrides_absdir;
extern char* g_console_key_swap;


extern long long g_preallocated_ghost_heap_size;
namespace mh_devflags {

	extern long long g_devflags;

	enum : unsigned char {
		always_nut4splenda, //install the dev commands that normally require mh_justforme inut4splenda on startup
		enable_hugepages_on_start, //acquire SeLockPagesInMemory on startup
		preallocate_entire_game_heap, //if true, we will swap out the global heap on the peb with a heap created from a single massive block of preallocated memory. (plan is to add various dev things that can easily check if its a heap pointer via >= < etc)
	};



}
static bool mh_test_devflag(unsigned flag) {
	return (mh_devflags::g_devflags & (1ULL << flag)) != 0;
}
void mh_global_config_load();
