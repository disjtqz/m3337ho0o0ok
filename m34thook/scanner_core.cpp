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

#define		SCANNED_PTR_FEATURE(name, ...)		located_feature_t descan:: name = 0;
#define		SCANNED_UINT_FEATURE(name,...)		 unsigned descan::  name = 0;
#include "xmacro_scanned_features.hpp"
#undef SCANNED_PTR_FEATURE
#undef SCANNED_UINT_FEATURE

cs_uninit_t< vtblmap_t>g_str_to_rtti_type_descr;

vtblmap_t* get_str_to_rtti_type_map() {

	return &g_str_to_rtti_type_descr.ref();
}
MH_NOINLINE
MH_PURE
static scanner_ops_t* get_scanops_for_curr_cpu() {
	switch(g_shalgo.m_cpulevel) {
	case snaphak_cpu_lvl_t::AVX2Cpu:
		return &g_scanops_avx2;

	case snaphak_cpu_lvl_t::AVX512Cpu:
		return &g_scanops_avx512;

	default:
		return &g_scanops_generic;
	}
}

MH_NOINLINE
void descan::locate_critical_features(){
	get_scanops_for_curr_cpu()->locate_critical_features();
}

MH_NOINLINE
void descan::run_late_scangroups(){
	get_scanops_for_curr_cpu()->run_late_scangroups();
}


MH_NOINLINE
void descan::run_gamelib_postinit_scangroups() {
	get_scanops_for_curr_cpu()->run_gamelib_postinit_scangroups();
}