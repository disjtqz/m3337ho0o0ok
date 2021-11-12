#pragma once
struct idCmdArgs {
	//offset 0 , size 4
	int argc;
	//offset 8 , size 1024
	char* argv[128];
	//offset 1032 , size 2048
	char tokenized[2048];
};

using cmdcb_t = void (*)(idCmdArgs*);
namespace idCmd {
	void register_command(const char* name, cmdcb_t, const char* description);


	void execute_command_text(const char* txt);

	void add_command(const char* txt);
	void execute_command_buffer();
	/*
		added this so we can scan command implementations
	*/
	void* find_command_by_name(const char* name);
}

struct __declspec(align(8)) idCVar
{
	using cvarCallback_t = void;

	struct __declspec(align(8)) cvarData_t
	{
		char* valueString;
		int valueInteger;
		float valueFloat;
		long long valueGameTime;
		__declspec(align(8)) float valueSeconds;
		int valueMilliseconds;
		long long valueMicroseconds;
		char* name;
		char* resetString;
		char* description;
		int flags;
		float valueMin;
		float valueMax;
		char** valueStrings;
		void* valueCompletion;
		idCVar::cvarCallback_t* onChange;
	};

	idCVar::cvarData_t* data;
	idCVar::cvarData_t dataStorage;
	idCVar* next;


	static idCVar* Find(const char* name);

	static idCVar** GetList(unsigned& out_n);


	static void generate_name_table();

	static void get_cvardata_rvas();
};

#include "pregenerated/doom_eternal_cvars_generated.hpp"


//array of rvas to each de_cvar_e's location
extern unsigned g_cvardata_rvas[DE_NUMCVARS];


MH_PURE
static idCVar::cvarData_t* cvar_data(de_cvar_e cv) {

	return from_de_rva<idCVar::cvarData_t>(g_cvardata_rvas[cv]);
}

