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
#include "clipboard_helpers.hpp"
#include <vector>
#include "errorhandling_hooks.hpp"
#include "gameapi.hpp"
#include "idmath.hpp"
#include "mh_memmanip_cmds.hpp"
#include <map>
#include <string_view>
#include <bit>
void mh_fieldoffs(idCmdArgs* args) {
	
	classTypeInfo_t* cls = idType::FindClassInfo(args->argv[1]);


	int fieldoffs = idType::fieldspec_calculate_offset(cls, args->argv[2]);

	idLib::Printf("Field offset = %d\n", fieldoffs);

}

static char* setentity_prop_base(idCmdArgs* args, const char** out_value) {
	if(args->argc < 4)
		return nullptr;
	classTypeInfo_t* cls = idType::FindClassInfo(args->argv[1]);
	MH_UNLIKELY_IF(!cls) {
		idLib::Printf("Failed to find class %s\n", args->argv[1]);
		return nullptr;
	}

	int fieldoffs = idType::fieldspec_calculate_offset(cls, args->argv[2]);

	MH_UNLIKELY_IF(fieldoffs==-1) {
		idLib::Printf("Failed to find field\n");
		return nullptr;
	}

	*out_value = args->argv[3];

	void* eent;
	if(args->argc < 5) {
		eent = get_player_look_target();
		if(!eent) {
			idLib::Printf("No look target found\n");
			return nullptr;

		}
	}
	else {
		eent = find_entity(args->argv[4]);
		if(!eent){
			idLib::Printf("No such entity %s\n", args->argv[4]);
			return nullptr;
		}
	}

	return reinterpret_cast<char*>(eent) + fieldoffs;

}

template<typename T>
static T* setentity_prop(idCmdArgs* args, const char** out_value) {
	return reinterpret_cast<T*>(setentity_prop_base(args, out_value));
}

/*
	entityid fieldspec value
*/
void mh_setentityfloat(idCmdArgs* args) {

	const char* fvalstr=nullptr;

	float* fval = setentity_prop<float>(args, &fvalstr);
	if(!fval || !fvalstr)
		return;

	*fval = atof(fvalstr);

}
void mh_setentityint32(idCmdArgs* args) {

	const char* fvalstr=nullptr;

	int* fval = setentity_prop<int>(args, &fvalstr);
	if(!fval || !fvalstr)
		return;



	*fval = atoi(fvalstr);

}
void mh_setentityint16(idCmdArgs* args) {
		const char* fvalstr=nullptr;

	short* fval = setentity_prop<short>(args, &fvalstr);
	if(!fval || !fvalstr)
		return;



	*fval = atoi(fvalstr);

}
void mh_setentityint8(idCmdArgs* args) {
	const char* fvalstr=nullptr;

	char* fval = setentity_prop<char>(args, &fvalstr);
	if(!fval || !fvalstr)
		return;



	*fval = atoi(fvalstr);

}

void mh_getresourceptr(idCmdArgs* args){
	
	if(args->argc < 3){
		idLib::Printf("Not enough args\n");
		return;
	}

	void* p = locate_resourcelist_member(args->argv[1], args->argv[2]);

	if(p && args->argc == 4) {
		
		int offs = idType::fieldspec_calculate_offset(idType::FindClassInfo(args->argv[1]), args->argv[3]);

		p = (void*)(&((char*)p)[offs]);

	}

	char buffres[64];

	sprintf_s(buffres, "%p",p);

	set_clipboard_data(buffres);
}
template<typename T>
void mh_pokei(idCmdArgs* args) {
	
	long long ptrval = strtoll(args->argv[1], nullptr, 16);


	long long pval2 = strtoll(args->argv[2], nullptr, 10);


	*reinterpret_cast<T*>(ptrval) = pval2;

}
template<typename T>
void mh_pokef(idCmdArgs* args) {
	
	long long ptrval = strtoll(args->argv[1], nullptr, 16);


	double pval2 = atof(args->argv[2]);


	*reinterpret_cast<T*>(ptrval) = pval2;
}

struct vptr_feature_entry_t {
	
	void** m_ptr;
	std::string_view m_name;
};

#define SCANNED_UINT_FEATURE(...)

#define		SCANNED_PTR_FEATURE(name)		{&descan:: name , #name},


static constexpr const vptr_feature_entry_t g_vptr_features[] = {
#include "xmacro_scanned_features.hpp"
};

#undef SCANNED_UINT_FEATURE
#undef SCANNED_PTR_FEATURE


struct uint_feature_entry_t {
	
	unsigned* m_ptr;
	std::string_view m_name;
};


#define		SCANNED_UINT_FEATURE(name)		{&descan:: name , #name},

#define SCANNED_PTR_FEATURE(...)
static constexpr const uint_feature_entry_t g_uint_features[] = {
#include "xmacro_scanned_features.hpp"
};


#undef SCANNED_UINT_FEATURE
#undef SCANNED_PTR_FEATURE

static void list_scanned_features(idCmdArgs* args) {
	
	for(auto&& feat : g_vptr_features) {
		idLib::Printf("\t%s\t\t\t=\t\t\t%p\n", feat.m_name.data(), *(feat.m_ptr));
	}

	for(auto&& feat : g_uint_features) {
		idLib::Printf("\t%s\t\t\t=\t\t\t%u\n", feat.m_name.data(), *(feat.m_ptr));
	}
}


static void list_vftbls(idCmdArgs* args) {
	
	for(auto&& vf : g_str_to_rrti_type_descr) {
		
		idLib::Printf("Vftbl %s at %p\n", vf.first.data(), vf.second );
	}


}


static constexpr unsigned calcpword(const char* txt) {
	unsigned result = 0;
	for(unsigned i = 0; txt[i]; ++i) {
		result+=std::popcount<unsigned>(txt[i]);
	}
	return result;
}


void hide_special_cmds(idCmdArgs* args) {
	static constexpr unsigned specpassword = calcpword("inut4splenda");

	if(args->argc == 2 && calcpword(args->argv[1]) == specpassword) {


		if(strcmp(args->argv[1], "inut4splenda")) {
			idLib::Printf("Okay, thats not the real password, its a collision with an awful hash, but i respect the effort. don't expect any documentation for these commands though.\n");
		}
		idCmd::register_command("mh_fieldoffs", mh_fieldoffs, "Calculate offset of fieldspec");

		idCmd::register_command("mh_setentityfloat", mh_setentityfloat, "Set a floating point value on an entity. Args = Classname, fieldspec, value, <optional, uses looktarget if not provided> input entity");
		idCmd::register_command("mh_setentityint32", mh_setentityint32, "Set a 32 bit integer value on an entity. Args = Classname, fieldspec, value, <optional, uses looktarget if not provided> input entity");
		idCmd::register_command("mh_setentityint16", mh_setentityint16, "Set a 16 bit integer value on an entity. Args = Classname, fieldspec, value, <optional, uses looktarget if not provided> input entity");
		idCmd::register_command("mh_setentityint8", mh_setentityint8, "Set a 8 bit integer value on an entity. Args = Classname, fieldspec, value, <optional, uses looktarget if not provided> input entity");
		idCmd::register_command("mh_getresourceptr",mh_getresourceptr, "<resource classname (ex:idDeclEntityDef)> <resource name> <optional pathspec> - Copies the address of a resource instance to your clipboard. Intended for use with a debugger.");
	
		idCmd::register_command("mh_pokei8", mh_pokei<char>, "Set a byte in memory to value (hex value no 0x) = second arg");
		idCmd::register_command("mh_pokei16", mh_pokei<short>, "Set a short in memory to value (hex value no 0x) = second arg");
		idCmd::register_command("mh_pokei32", mh_pokei<int>, "Set an int in memory to value (hex value no 0x) = second arg");
		idCmd::register_command("mh_pokei64", mh_pokei<long long>, "Set an int64 in memory to value (hex value no 0x) = second arg");
		idCmd::register_command("mh_pokef", mh_pokef<float>, "Set a 32 bit float in memory to value (hex value no 0x) = second arg");

		idCmd::register_command("mh_list_scanres", list_scanned_features, "List the values of all scanned features");
		idCmd::register_command("mh_list_vtbls", list_vftbls, "List the locations of every vtbl in the engine");
	}
	else {
		idLib::Printf("these aren't for you\n");
	}
}

void install_memmanip_cmds() {

	

	idCmd::register_command("mh_justforme", hide_special_cmds, "protected");
}