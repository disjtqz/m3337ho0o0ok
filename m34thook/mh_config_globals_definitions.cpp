#include "mh_config_globals.hpp"
#include "mh_defs.hpp"
#include "mh_configfile.hpp"
char* g_loadmaponstart = nullptr;
char* g_overrides_subdir = (char*)"overrides";
char* g_overrides_absdir = nullptr;
char* g_console_key_swap = nullptr;
long long mh_devflags::g_devflags = 0;
#define		DEVFLAG_CFGVAR(name)		{&mh_devflags::g_devflags, #name, mh_devflags::name}
static const constexpr mh_configvar_header_t g_mh_configvars[] = {
	{&g_loadmaponstart, "loadmap_on_start"},
	{&g_overrides_absdir, "overrides_absdir"},
	{&g_overrides_subdir, "overrides_subdir"},
	{&g_console_key_swap, "console_key_swap"},
	DEVFLAG_CFGVAR(always_nut4splenda),
	DEVFLAG_CFGVAR(enable_hugepages_on_start)
};

static constexpr unsigned g_n_mh_configvars = sizeof(g_mh_configvars) / sizeof(g_mh_configvars[0]);

void mh_global_config_load() {
	mh_configfile_load(g_mh_configvars, g_n_mh_configvars);

}