#include "mh_defs.hpp"

#include "game_exe_interface.hpp"
#include "memscan.hpp"
#include "scanner_core.hpp"
#include <map>
#include <string_view>
#include "snaphakalgo.hpp"
#include <cstdio>
#include "idtypeinfo.hpp"
#include "gameapi.hpp"


#include "scanners/scanner_core_incl.hpp"

scanner_ops_t g_scanops_avx2{ .locate_critical_features = descan_locate_critical_features,
.run_late_scangroups = descan_run_late_scangroups, .run_gamelib_postinit_scangroups = descan_run_gamelib_postinit_scangroups };