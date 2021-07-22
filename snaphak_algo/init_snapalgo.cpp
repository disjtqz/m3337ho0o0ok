#include <cstdint>
#include "snaphakalgo_predef.hpp"
#include "snaphakalgo.hpp"

#include <intrin.h>
#include <Windows.h>

//#define		FORCE_GENERIC
CS_NOINLINE
CS_COLD_CODE
void init_impls_generic(snaphak_algo_t* algo);

CS_NOINLINE
CS_COLD_CODE
void init_impls_avx2(snaphak_algo_t* algo);
CS_NOINLINE
CS_COLD_CODE
void init_impls_avx512(snaphak_algo_t* algo);

static const char* get_isa_level(snaphak_cpu_lvl_t cpulvl) {
	switch(cpulvl) {
	case snaphak_cpu_lvl_t::GenericCpu:
		return "Generic (base x64)";
	case snaphak_cpu_lvl_t::AVX2Cpu:
		return "AVX2";
	case snaphak_cpu_lvl_t::AVX512Cpu:
		return "AVX512(VL,CD,DQ,BW)";

	}
}
CS_COLD_CODE
static
void _fatal_raise(const char* fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	char buffer[4096];

	vsprintf_s(buffer,4096, fmt,ap );
	MessageBoxA(nullptr, buffer, "SnapHak Fatal Error", MB_ICONERROR | MB_OK);

	exit(1);
}
CS_COLD_CODE
static
void print_cpu_info(snaphak_algo_t* alg, char* buffer, size_t maxsize) {
	sprintf_s(buffer, maxsize, 
		"Snaphak Algo CpuInfo\n"
		"ISA Profile:%s\n\t"
		"Has SMT: %d\n\t"
		"SMT width:%d\n\t"
		"NCores:%d\n\t"
		"NThreads %d\n\t"
		"NProcGroups %d\n\t"
		"Has clflushopt:%d\n\t"
		"Has clwb:%d\n\t"
		"Has movbe: %d\n\t"
		"Has f16c: %d\n\t"
		"Has Fast Long Rep: %d\n\t"
		"Has Fast Short Rep: %d\n\t"
		"Is running under Wine: %d\n\t",
		get_isa_level(alg->m_cpulevel), 
		alg->m_has_smt, 
		alg->m_smt_width, 
		alg->m_ncores, 
		alg->m_nthreads,
		alg->m_nprocgroups, 
		alg->m_has_clflushopt, 
		alg->m_has_clwb, 
		alg->m_has_movbe, 
		alg->m_has_f16c,
		alg->m_has_fast_longrep,
		alg->m_has_fast_shortrep,
		alg->m_is_under_wine);
}

extern "C"
__declspec(dllexport)
void sh_algo_init(snaphak_algo_t* out_algo) {
	int extendedfeat[4] = { 0 };
	__cpuidex(extendedfeat, 7, 0);

	union {
		uint32_t extendedflags;
		struct {
			uint32_t unused5 : 5;
			uint32_t bAVX2 : 1;
			uint32_t unused25 : 22;
			uint32_t bAVX512CD : 1;
			uint32_t bIntelSha : 1;
			uint32_t bAVX512BW : 1;
			uint32_t bAVX512VL : 1;

		};
	}flagtest;
	out_algo->m_fatal_raise=_fatal_raise;
	out_algo->m_is_under_wine = (GetProcAddress(GetModuleHandleA("ntdll.dll"), "wine_server_call") != nullptr) ;
		
	
	int cpuflags[4] = {0};
	//EAX, EBX, ECX, and EDX
	__cpuid(cpuflags, 1);
#ifdef FORCE_GENERIC

	out_algo->m_print_cpu_info = print_cpu_info;
	out_algo->m_has_smt = false;
	out_algo->m_has_movbe = false;
	out_algo->m_has_f16c = false;
	out_algo->m_has_fast_longrep = false;
	out_algo->m_has_fast_shortrep = false;
	out_algo->m_has_clflushopt = false;
	out_algo->m_has_clwb = false;
	out_algo->m_smt_width = false;
	out_algo->m_cpulevel = snaphak_cpu_lvl_t::GenericCpu;
	init_impls_generic(out_algo);
#else
	out_algo->m_print_cpu_info = print_cpu_info;
	out_algo->m_has_smt = (cpuflags[3] & (1U << 28)) != 0;
	out_algo->m_has_movbe = (cpuflags[2] & (1U << 22)) != 0;
	out_algo->m_has_f16c = (cpuflags[2]&(1U << 29)) != 0;
	out_algo->m_has_fast_longrep = (extendedfeat[1] & (1U << 9)) != 0;
	out_algo->m_has_fast_shortrep = (extendedfeat[3] & (1U << 4)) != 0;

	flagtest.extendedflags = extendedfeat[1];

	out_algo->m_has_clflushopt = (flagtest.extendedflags & (1U << 23)) != 0;
	out_algo->m_has_clwb = (flagtest.extendedflags & (1U << 23)) != 0;
	out_algo->m_smt_width = (cpuflags[1] >>15) & 0x3f;

	if(flagtest.bAVX512VL && flagtest.bAVX512BW && flagtest.bAVX512CD && (flagtest.extendedflags & (1U << 16)) && (flagtest.extendedflags & (1U << 17))) {
		/*
			our avx512 feature level - vl, bw, dq, cd, foundation
		*/
		out_algo->m_cpulevel = snaphak_cpu_lvl_t::AVX512Cpu;
		init_impls_avx512(out_algo);
	}
	else if(flagtest.bAVX2) {
		out_algo->m_cpulevel = snaphak_cpu_lvl_t::AVX2Cpu;
		init_impls_avx2(out_algo);
	}
	else {
		out_algo->m_cpulevel = snaphak_cpu_lvl_t::GenericCpu;
		init_impls_generic(out_algo);
	}

#endif


}