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
MH_NOINLINE
unsigned scanner_late_get_field_offset(const char* cls, const char* fld) {
	/*
		this is all written out like this because most of our advanced typeinfo stuff
		isnt initialized when this runs
	*/
	auto clstype = idType::FindClassInfo(cls);
	cs_assert(clstype != nullptr);

	auto namehash = idType::calculate_field_name_hash(fld, sh::string::strlength(fld));

	while (clstype) {

		auto vrs = clstype->variables;

		auto vrhashes = clstype->variableNameHashes;

		unsigned i = 0;
		for (; vrhashes[i]; ++i) {
			if (vrhashes[i] == namehash) {
				break;
			}
		}
		if (vrhashes[i] == 0) {

			if (clstype->superType && clstype->superType[0]) {
				clstype = idType::FindClassInfo(clstype->superType);
			}
			else
				clstype = nullptr;
		}
		else {

			return vrs[i].offset;
		}
		
	}
}
MH_NOINLINE
void* scanner_late_get_vtbl(const char* name) {
	return get_class_vtbl(name);
}


MH_NOINLINE
MH_REGFREE_CALL
void scanner_find_and_extract_next_call_within_range(
	scanstate_t* scstate,
	void** out_call,
	unsigned max_distance
) {

	*out_call = nullptr;
	mh_disassembler_t dis{};
	dis.setup_for_addr(scstate->addr);
	/*
		todo: technically, we dont need to disassemble, we can just decode
		and check the first byte to see if its 0xE8
	*/
	if (!dis.find_next_of_mnem<UD_Icall>())
		return;

	void* target_res = dis.extract_pcrel_value(0);

	if (!target_res || !scstate->dll->is_in_image(target_res)) {
		return;
	}
	//did we go too far?
	if (mh_ptrdelta(dis.current_pc(), scstate->addr) > max_distance) {
		return;
	}

	*out_call = target_res;
	scstate->addr = (unsigned char*)dis.current_pc();
}