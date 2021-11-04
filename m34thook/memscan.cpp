#include "mh_defs.hpp"

#include "memscan.hpp"
#include "cmdsystem.hpp"
#include "game_exe_interface.hpp"
#include "scanner_core.hpp"
#include "idtypeinfo.hpp"
#include "gameapi.hpp"
#include "eventdef.hpp"
static std::mutex g_failure_message_mutex{};

__declspec(noinline)
void scanner_failure_message(const char* scanner_name) {
	g_failure_message_mutex.lock();
	{
		char tmpbuf[1024];
		sprintf_s(tmpbuf, "Blockscan entry %s failed to locate its required patterns. The DLL may still function normally, but some features may not work. If the game does not start up, go into your doom eternal install directory and delete XINPUT1_3.dll. Check the Doom 2016+ Mods discord for an updated version of MH for this version of Eternal.", scanner_name);
		MessageBoxA(nullptr, tmpbuf, "M347h00k (XINPUT1_3.dll, in your Doom Eternal directory) ->  Scanner Failed", 0);
	}
	g_failure_message_mutex.unlock();
}

void* scanner_late_get_cvar(const char* s) {
	return idCVar::Find(s);
}

unsigned scanner_late_get_struct_size(const char* s) {
	return idType::FindClassInfo(s)->size;
}
void* scanner_late_get_eventdef(const char* name) {

	return idEventDefInterfaceLocal::Singleton()->FindEvent(name);
}