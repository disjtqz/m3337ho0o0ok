#include "snaphakalgo_predef.hpp"
#include "snaphakalgo.hpp"
#include <Windows.h>
//due to alignment, putting these into different segs is wasteful


#if 0
#define		SMTPROC		CS_CODE_SEG(".smt")
#define		NOSMTPROC	CS_CODE_SEG(".nosmt")
#else
#define		SMTPROC
#define		NOSMTPROC
#endif
SMTPROC
//__attribute__((naked))
static bool smt_cmpxchg16b(volatile uint64_t* destination, uint64_t xhigh, uint64_t xlow, uint64_t* cmpres) {


	uint64_t rdx = cmpres[1];
	uint64_t rax = cmpres[0];
	uint64_t rcx = xhigh;
	uint64_t rbx = xlow;
	bool bres = 0;

	__asm__ volatile(
		"CMPXCHG16B (%5)\n\t"
		:  "+d"(rdx), "+a"(rax), "=@ccz"(bres)
		: "c"(rcx), "b"(rbx), "r"(destination)
		:
		);

	cmpres[0] = rax;
	cmpres[1] = rdx;
	return bres;


}
SMTPROC
static int smt_xadd32(volatile int* dest, int addend) {
	int saved = addend;
	__asm__ volatile ("xadd %0, (%1)\n\t"
		: "+r" (addend)
		: "r" (dest)
		: );
	return addend + saved;
}
SMTPROC
static int64_t smt_xadd64(volatile int64_t* dest, int64_t addend) {
	int64_t saved = addend;
	__asm__ volatile ("xadd %0, (%1)\n\t"
		: "+r" (addend)
		: "r" (dest)
		: );
	return addend + saved;
}

SMTPROC
static
bool smt_bts64(volatile uint64_t* m_ptr, unsigned m_bitidx) {
	bool res;
	__asm__ volatile(
		"btsq %2, (%1)\n\t"
		: "=@ccb" (res)
		: "r"(m_ptr), "r"((uint64_t)m_bitidx)
		:
		);
	return res;
}
SMTPROC
static
bool smt_btr64(volatile uint64_t* m_ptr, unsigned m_bitidx) {
	bool res;
	__asm__ volatile(
		"btrq %2, (%1)\n\t"
		: "=@ccb" (res)
		: "r"(m_ptr), "r"((uint64_t)m_bitidx)
		:
		);
	return res;
}

SMTPROC
static
bool smt_btc64(volatile uint64_t* m_ptr, unsigned m_bitidx) {
	bool res;
	__asm__ volatile(
		"btcq %2, (%1)\n\t"
		: "=@ccb" (res)
		: "r"(m_ptr), "r"((uint64_t)m_bitidx)
		:
		);
	return res;
}

NOSMTPROC
static bool nosmt_cmpxchg16b(volatile uint64_t* destination, uint64_t xhigh, uint64_t xlow, uint64_t* cmpres) {
	return _InterlockedCompareExchange128((volatile long long*)destination, xhigh, xlow, (long long*)cmpres);

}

NOSMTPROC
static int nosmt_xadd32(volatile int* dest, int addend) {
	return _InterlockedExchangeAdd((volatile unsigned*)dest, addend);
}



NOSMTPROC
static int64_t nosmt_xadd64(volatile int64_t* dest, int64_t addend) {
	return _InterlockedExchangeAdd64(dest, addend);
}


NOSMTPROC
static
bool nosmt_bts64(volatile uint64_t* m_ptr, unsigned m_bitidx) {
	return _interlockedbittestandset64((volatile long long*)m_ptr, m_bitidx);
}
NOSMTPROC
static
bool nosmt_btr64(volatile uint64_t* m_ptr, unsigned m_bitidx) {
	return _interlockedbittestandreset64((volatile long long*)m_ptr, m_bitidx);
}
NOSMTPROC
static
bool nosmt_btc64(volatile uint64_t* m_ptr, unsigned m_bitidx) {
	bool res;
	__asm__ volatile(
		"lock btcq %2, (%1)\n\t"
		: "=@ccb" (res)
		: "r"(m_ptr), "r"((uint64_t)m_bitidx)
		:
		);
	return res;
}



__forceinline
static
DWORD CountSetBits(ULONG_PTR bitMask) {
	DWORD LSHIFT = sizeof(ULONG_PTR) * 8 - 1;
	DWORD bitSetCount = 0;
	ULONG_PTR bitTest = (ULONG_PTR)1 << LSHIFT;

	for (DWORD i = 0; i <= LSHIFT; i++) {
		bitSetCount += ((bitMask & bitTest) ? 1 : 0);
		bitTest /= 2;
	}

	return bitSetCount;
}

typedef BOOL(WINAPI* LPFN_GLPI)(PSYSTEM_LOGICAL_PROCESSOR_INFORMATION, PDWORD);

enum LOGICAL_PROCESSOR_RELATIONSHIP_LOCAL {
	localRelationProcessorCore,
	localRelationNumaNode,
	localRelationCache,
	localRelationProcessorPackage
};

struct cpuInfo_t {
	int processorPackageCount;
	int processorCoreCount;
	int logicalProcessorCount;
	int numaNodeCount;
	struct cacheInfo_t {
		int count;
		int associativity;
		int lineSize;
		int size;
	} cacheLevel[3];
	bool m_smt_detected;
};

/*
	from Doom3BFG
*/
/*
========================
GetCPUInfo
========================
*/
CS_NOINLINE
CS_COLD_CODE
static
bool GetCPUInfo(cpuInfo_t& cpuInfo) {
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = NULL;
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr = NULL;
	PCACHE_DESCRIPTOR Cache;
	LPFN_GLPI	glpi;
	BOOL		done = FALSE;
	DWORD		returnLength = 0;
	DWORD		byteOffset = 0;

	memset(&cpuInfo, 0, sizeof(cpuInfo));

	glpi = (LPFN_GLPI)GetProcAddress(GetModuleHandle(TEXT("kernel32")), "GetLogicalProcessorInformation");
	if (NULL == glpi) {
		//idLib::Printf( "\nGetLogicalProcessorInformation is not supported.\n" );
		return 0;
	}

	while (!done) {
		DWORD rc = glpi(buffer, &returnLength);

		if (FALSE == rc) {
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
				if (buffer) {
					free(buffer);
				}

				buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc(returnLength);
			}
			else {
				//	idLib::Printf( "Sys_CPUCount error: %d\n", GetLastError() );
				return false;
			}
		}
		else {
			done = TRUE;
		}
	}

	ptr = buffer;
	bool has_smt = false;
	while (byteOffset + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) <= returnLength) {
		switch ((LOGICAL_PROCESSOR_RELATIONSHIP_LOCAL)ptr->Relationship) {
		case localRelationProcessorCore:
			cpuInfo.processorCoreCount++;

			// A hyperthreaded core supplies more than one logical processor.
			cpuInfo.logicalProcessorCount += CountSetBits(ptr->ProcessorMask);

			has_smt = true;
			break;

		case localRelationNumaNode:
			// Non-NUMA systems report a single record of this type.
			cpuInfo.numaNodeCount++;
			break;

		case localRelationCache:
			// Cache data is in ptr->Cache, one CACHE_DESCRIPTOR structure for each cache. 
			Cache = &ptr->Cache;
			if (Cache->Level >= 1 && Cache->Level <= 3) {
				int level = Cache->Level - 1;
				if (cpuInfo.cacheLevel[level].count > 0) {
					cpuInfo.cacheLevel[level].count++;
				}
				else {
					cpuInfo.cacheLevel[level].associativity = Cache->Associativity;
					cpuInfo.cacheLevel[level].lineSize = Cache->LineSize;
					cpuInfo.cacheLevel[level].size = Cache->Size;
				}
			}
			break;

		case localRelationProcessorPackage:
			// Logical processors share a physical package.
			cpuInfo.processorPackageCount++;
			break;

		default:
			//idLib::Printf( "Error: Unsupported LOGICAL_PROCESSOR_RELATIONSHIP value.\n" );
			break;
		}
		byteOffset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
		ptr++;
	}

	free(buffer);

	return true;
}

static void register_smt_atomics(snaphak_smtroutines_t* smt_r) {
	smt_r->m_cmpxchg16b = smt_cmpxchg16b;
	smt_r->m_xadd32 = smt_xadd32;
	smt_r->m_xadd64 = smt_xadd64;
	smt_r->m_btr64 = smt_btr64;
	smt_r->m_bts64 = smt_bts64;
	smt_r->m_btc64 = smt_btc64;
}
static void register_nosmt_atomics(snaphak_smtroutines_t* smt_r) {
	smt_r->m_cmpxchg16b = nosmt_cmpxchg16b;
	smt_r->m_xadd32 = nosmt_xadd32;
	smt_r->m_xadd64 = nosmt_xadd64;
	smt_r->m_btr64 = nosmt_btr64;
	smt_r->m_bts64 = nosmt_bts64;
	smt_r->m_btc64 = nosmt_btc64;
}
struct sys_createthread_params_t {
	sh_thrdproc_t function;
	void* parms;
};
static unsigned int createthread_forwarder(void* ud) {
	//cs_threading_ensure_init();
	sys_createthread_params_t* parms = reinterpret_cast<sys_createthread_params_t*>(ud);
	void* true_ud = parms->parms;
	sh_thrdproc_t true_function = parms->function;
	free(ud);
	unsigned result = true_function(true_ud);

	//	cs_threading_ensure_teardown();
	return result;
}

static uintptr_t cs_create_thread(sh_thrdproc_t function, void* parms, unsigned procnum, size_t stackSize, bool suspended, int* out_threadid)
{


	DWORD flags = (suspended ? CREATE_SUSPENDED : 0);

	flags |= STACK_SIZE_PARAM_IS_A_RESERVATION;

	DWORD threadId;

	sys_createthread_params_t* true_parms = (sys_createthread_params_t*)malloc(sizeof(sys_createthread_params_t));
	true_parms->function = function;
	true_parms->parms = parms;
	HANDLE handle = CreateThread(NULL,
		stackSize,
		(LPTHREAD_START_ROUTINE)createthread_forwarder,
		true_parms,
		flags,
		&threadId);


	unsigned groupnum = procnum / 64;
	unsigned thrdnum = procnum % 64;

	PROCESSOR_NUMBER kernel_given{};
	GetThreadIdealProcessorEx(handle, &kernel_given);
	//ewww
	//this is the only way ive been able to reliably pin a thread to a specific hw thread across processor groups though :/
	while (kernel_given.Group != groupnum || kernel_given.Number != thrdnum) {
		SwitchToThread();
		PROCESSOR_NUMBER procnumstruct{};
		procnumstruct.Group = groupnum;
		procnumstruct.Number = thrdnum;
		SetThreadIdealProcessorEx(handle, &procnumstruct, nullptr);

		GROUP_AFFINITY grpsave{};
		GetThreadGroupAffinity(handle, &grpsave);
		grpsave.Group = groupnum;
		SetThreadGroupAffinity(handle, &grpsave, nullptr);
		SwitchToThread();

		GetThreadIdealProcessorEx(handle, &kernel_given);
	}

	if (handle == 0)
	{
		return (uintptr_t)0;
	}


	if (out_threadid)
		*out_threadid = threadId;

	return (uintptr_t)handle;
}

static void cs_yield_thread() {
	SwitchToThread();
}

static void cs_sleep_thread(size_t ms) {
	Sleep(ms);
}

static snaphak_signal_t cs_signal_create() {
	return reinterpret_cast<snaphak_signal_t>(CreateEventA(nullptr, false, false, nullptr));
}
static void cs_signal_destroy(snaphak_signal_t sig) {
	CloseHandle((HANDLE)sig);
}

static void cs_signal_raise(snaphak_signal_t sig) {
	SetEvent((HANDLE)sig);
}
static bool cs_signal_wait(snaphak_signal_t sig, int64_t* timeout=SNAPHAK_FOREVER) {
	int wintimeout;
	if(!timeout) {
		wintimeout = INFINITE;
	}
	else {
		wintimeout = *timeout / -10000i64;
	}

	return !WaitForSingleObject((HANDLE)sig, wintimeout);

}
static bool cs_signal_raise_and_wait(snaphak_signal_t raise, snaphak_signal_t wait, int64_t* timeout) {
	int wintimeout;
	if(!timeout) {
		wintimeout = INFINITE;
	}
	else {
		wintimeout = *timeout / -10000i64;
	}

	return !SignalObjectAndWait((HANDLE)raise,(HANDLE)wait, wintimeout, FALSE);

}
static void register_common_smt_routines(snaphak_smtroutines_t* smtrout) {
	smtrout->m_create_thread = cs_create_thread;
	smtrout->m_sleep_thread = cs_sleep_thread;
	smtrout->m_yield_thread = cs_yield_thread;
	smtrout->m_create_signal = cs_signal_create;
	smtrout->m_destroy_signal = cs_signal_destroy;
	smtrout->m_signal_raise = cs_signal_raise;
	smtrout->m_signal_wait = cs_signal_wait;
	smtrout->m_signal_raise_and_wait = cs_signal_raise_and_wait;
}

void smtalgos_init(snaphak_algo_t* algo) {
	cpuInfo_t cpuinfo{};
	GetCPUInfo(cpuinfo);
	algo->m_nprocgroups = GetActiveProcessorGroupCount();

	algo->m_smt_width = cpuinfo.logicalProcessorCount / cpuinfo.processorCoreCount;

//	algo->m_ncores = GetActiveProcessorCount(ALL_PROCESSOR_GROUPS);
	//algo->m_nthreads = algo->m_ncores * algo->m_smt_width;

	algo->m_nthreads = GetActiveProcessorCount(ALL_PROCESSOR_GROUPS);
	algo->m_ncores = algo->m_nthreads / algo->m_smt_width;
	algo->m_has_smt = algo->m_smt_width > 1;

	if (algo->m_has_smt) {
		register_smt_atomics(&algo->m_smtroutines);
	}
	else {
		register_nosmt_atomics(&algo->m_smtroutines);
	}

	register_common_smt_routines(&algo->m_smtroutines);
}