#pragma once



extern char* g_loadmaponstart;
extern char* g_overrides_subdir;
extern char* g_overrides_absdir;
extern char* g_console_key_swap;

namespace mh_devflags {

	extern long long g_devflags;

	enum : unsigned char {
		always_nut4splenda, //install the dev commands that normally require mh_justforme inut4splenda on startup
		enable_hugepages_on_start, //acquire SeLockPagesInMemory on startup
	};



}
static bool mh_test_devflag(unsigned flag) {
	return (mh_devflags::g_devflags & (1ULL << flag)) != 0;
}
void mh_global_config_load();
