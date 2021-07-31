#include "mh_defs.hpp"

#include "game_exe_interface.hpp"
#include "doomoffs.hpp"
#include "meathook.h"
#include "cmdsystem.hpp"
#include "idtypeinfo.hpp"
#include "eventdef.hpp"
#include "scanner_core.hpp"
#include "idLib.hpp"
#include "idStr.hpp"
#include "idmath.hpp"
#include <cassert>
#include <stdlib.h>
#include <stdio.h>
#include "gameapi.hpp"
#include "snaphakalgo.hpp"
#include "mh_mainloop.hpp"
#include "mh_guirender.hpp"


template<typename T>
static T load8_no_cache(T* ptr) {
	
	return reinterpret_cast<T>(_InterlockedExchangeAdd64((volatile long long*)ptr, 0));
}

struct alignas(64) frame_callbacks_t {
	mh_mainloop::frame_cb_t m_cbs[MAX_FRAME_CALLBACKS];
	void* m_uds[MAX_FRAME_CALLBACKS];
	size_t m_numcallbacks;


	void init() {
		sh::memops::nt_zero_mem(this, sizeof(*this));
		m_numcallbacks=0;
		sh::memops::sfence_if();
	}

	unsigned add_callback(mh_mainloop::frame_cb_t cb, void* ud) {


		long long numcb = _InterlockedIncrement64((volatile long long*)&m_numcallbacks);
		MH_UNLIKELY_IF(numcb >= MAX_FRAME_CALLBACKS) {
			sh::fatal("M347h00k exceeded MAX_FRAME_CALLBACKS!");

		}
		
		InterlockedExchange64((volatile long long*)&m_cbs[numcb], (long long)(void*)cb);
		InterlockedExchange64((volatile long long*)&m_uds[numcb], (long long)ud);

		return (unsigned)numcb;
	}
	

	void run_callbacks() {
		unsigned numcb = _InterlockedExchangeAdd64((volatile long long*)&m_numcallbacks, 0);

		for(unsigned i = 0; i < numcb; ++i) {
			mh_mainloop::frame_cb_t cb = load8_no_cache(&m_cbs[i]);
			void* ud = load8_no_cache(&m_uds[i]);

			cb(ud);
		}
	}


};



static frame_callbacks_t g_preframe{};
static frame_callbacks_t g_postframe{};

static bool g_isfirstframe = true;


MH_NOINLINE
MH_REGFREE_CALL
CS_COLD_CODE
static void run_firstframe_code()  {
	g_isfirstframe = false;
	//run the late stage scanners, load all plugins
	meathook_final_init();	
	mh_gui::install_gui_hooks();
}

static uint64_t g_num_ticks = 0;

static __int64 meathook_game_frame(__int64 framearg) {
	MH_UNLIKELY_IF (g_isfirstframe) {
		run_firstframe_code();
		
	}
	g_preframe.run_callbacks();
	__int64 result = call_as<__int64>(descan::g_idCommonLocal_Frame, framearg);
	g_postframe.run_callbacks();
	g_num_ticks++;
	return result;
}
uint64_t mh_mainloop::num_ticks() {
	return g_num_ticks;
}

void mh_mainloop::install_mainloop_hooks() {
	*reinterpret_cast<void**>(descan::g_idCommonLocal_Frame_CallbackPtr) = (void*)meathook_game_frame;
	g_preframe.init();
	g_postframe.init();
}

void mh_mainloop::add_preframe_callback(frame_cb_t cb, void* ud) {
	g_preframe.add_callback(cb, ud);
}
void mh_mainloop::add_postframe_callback(frame_cb_t cb, void* ud) {
	g_postframe.add_callback(cb, ud);
}