#pragma once
#include "mh_defs.hpp"
#include <map>
#include <string_view>
using located_feature_t = void*;
//need 1080 getlevelmap


extern std::map<std::string_view, void*> g_str_to_rrti_type_descr;

namespace descan {
#define		SCANNED_PTR_FEATURE(name, ...)		extern located_feature_t name;
#define		SCANNED_UINT_FEATURE(name,...)		extern unsigned name;
#include "xmacro_scanned_features.hpp"
#undef SCANNED_PTR_FEATURE
#undef SCANNED_UINT_FEATURE



	MH_NOINLINE
		void locate_critical_features();

	MH_NOINLINE
		void run_late_scangroups();

	/*
	at this point we have access to things like typeinfotools, 
	runs on very first frame of idCommonLocal::Frame
*/
	MH_NOINLINE
		void run_gamelib_postinit_scangroups();
}

struct scanner_ops_t {
	void (*locate_critical_features)();
	void (*run_late_scangroups)();
	void (*run_gamelib_postinit_scangroups)();
};

extern scanner_ops_t g_scanops_generic;
extern scanner_ops_t g_scanops_avx2;
extern scanner_ops_t g_scanops_avx512;