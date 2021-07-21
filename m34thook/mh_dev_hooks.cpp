#if 1


#include "xbyak/xbyak.h"
#include "mh_defs.hpp"

#include "game_exe_interface.hpp"
#include "memscan.hpp"
#include "scanner_core.hpp"

//one granula 
#define		EXECMEM_NEEDED		65536


struct register_ringbuff_element_t {
	uintptr_t g_calltime;
	uintptr_t gpregs[15];
};

//17*8 = 136b per
//there are about 100k functions, lets assume 80k total actually are large enough to have a check
// assume machine has 256 gb, about 10-15% used up by ida instances+the game running
// 
// **** this math has changed somewhat, cut gpregs down to 15 registers+calltime so that the memory is always 32 bytes aligned and can be written with nt writes
// 
//so lets say 220gb
//220gb / 80k = 2952790 per call
//so 21711 ringbuffer elements per call, but to make wrapping around easier, we can round down to 16384
//so we use only around 166 gb total, but we will probably need a good chunk for the lookup table


static uintptr_t g_current_calltime = 0;


#define		NUM_RESERVED_RINGBUFFS		10000ULL
#define		NUM_ELEMENTS_PER_RINGBUFF	16384ULL


static uintptr_t get_actual_returnaddr(uintptr_t addr){
	return addr - 5; //points to 0xE8 now
}
/*
	writing the logic out, but this should be jitted
*/
static unsigned get_returnaddr_key(uintptr_t addr) {
	
	addr = get_actual_returnaddr(addr);
	addr -= (uintptr_t)g_blamdll.image_base;
	/*
		all functions starts get aligned to 16 bytes, so we will always still be in the function if we align down
	*/

	return (addr & ~15ULL) >> 4;

}


using ringbuffer_list_ele_t = std::array<register_ringbuff_element_t, NUM_ELEMENTS_PER_RINGBUFF>;
/*
	//todo: allocate using hugepages
*/
static ringbuffer_list_ele_t* g_ringbuffers= nullptr;

static uintptr_t g_current_free_ringbuff = 0;

/*
	a huge lookup table that we use to look up our recordkeeping object for a given function call

	

*/
static void* g_retaddr2context_table = nullptr;


static size_t g_execmem_current_size = 0;
static char* g_execmem_region = nullptr;

static void allocate_execmem_takelist() {
	
	g_execmem_current_size = EXECMEM_NEEDED;
	g_execmem_region =(char*) VirtualAlloc(nullptr, EXECMEM_NEEDED, MEM_COMMIT|MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	__stosb((unsigned char*)g_execmem_region, 0, g_execmem_current_size);
}


static void allocate_retaddr2context_table() {
	
	size_t total_bytes_for_table = static_cast<size_t>(g_blamdll.image_size >> 4) * sizeof(void*);


	void* table = VirtualAlloc(nullptr, total_bytes_for_table, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE);
	//prefault the pages so we arent getting pagefaults while playing
	__stosb((unsigned char*)table, 0, total_bytes_for_table);
	g_retaddr2context_table = table;


}


static void callinstrument_logic(uintptr_t retaddr) {
	auto key = get_returnaddr_key(retaddr);
	auto ringbufferpos_for = &reinterpret_cast<ringbuffer_list_ele_t**>(g_retaddr2context_table)[key];

	ringbuffer_list_ele_t* ringbuff =  *ringbufferpos_for;

	if(!ringbuff) {
		size_t newringbuffpos = _interlockedincrement64((volatile long long*) &g_current_free_ringbuff);
		if(newringbuffpos >= NUM_RESERVED_RINGBUFFS) {
			return;
		}
		ringbuff = &g_ringbuffers[newringbuffpos];

		_mm_stream_si64(reinterpret_cast<long long*>(ringbufferpos_for), (long long)ringbuff);
	}
	

	uintptr_t decode_buffer_pos = (uintptr_t)ringbuff;


	uintptr_t rgbuff_base = decode_buffer_pos >> 48;




	size_t currtime = _interlockedincrement64((volatile long long*)&g_current_calltime);


	/*
		build 4 ymm values from regs, stream out
	*/	

}
static void* emit_function(Xbyak::CodeGenerator& cgen) {
	
	if(g_execmem_current_size < cgen.getSize()) {
		MessageBoxA(0, "Out of exec mem!", "Exec mem exhausted",0);
		exit(1);
	}

	char* emitdest = g_execmem_region;
	g_execmem_region += cgen.getSize();

	__movsb((unsigned char*)emitdest, cgen.getCode(), cgen.getSize());
	return emitdest;
}



/*
	we can get information on the values in registers at the end of a function by hooking security_check_cookie

*/


void mh_install_security_cookie_regval_hook() {
	
}

#endif