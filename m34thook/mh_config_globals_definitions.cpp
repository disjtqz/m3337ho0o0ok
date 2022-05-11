#include "mh_config_globals.hpp"
#include "mh_defs.hpp"
#include "mh_configfile.hpp"
char* g_loadmaponstart = nullptr;


static const constexpr mh_configvar_header_t g_mh_configvars[] = {
	{&g_loadmaponstart, "loadmap_on_start"}
};

static constexpr unsigned g_n_mh_configvars = sizeof(g_mh_configvars) / sizeof(g_mh_configvars[0]);

void mh_global_config_load() {
	mh_configfile_load(g_mh_configvars, g_n_mh_configvars);

}