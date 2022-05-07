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
	//das right, im a sneaky fucker i am
	cmdcb_t swap_command_impl(const char* name, cmdcb_t function);
}

struct __declspec(align(8)) idCVar
{
	struct cvarCallback_t
	{
		struct idCallback* callback;
		idCVar::cvarCallback_t* next;
	};


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
		MH_NOINLINE
		void call_onchange_functions();
	};

	idCVar::cvarData_t* data;
	idCVar::cvarData_t dataStorage;
	idCVar* next;


	static idCVar* Find(const char* name);

	static idCVar** GetList(unsigned& out_n);


	static void generate_name_table();


};


MH_PURE
static void set_cvar_integer(const char* cv, int value) {
	auto cvx = idCVar::Find(cv);

	if (!cvx)
		return;
	auto cvd = cvx->data;
	if (!cvd)
		return;
	cvd->flags |= 0x40000; // CVAR_MODIFIED
	cvd->valueInteger = value;
	cvd->valueFloat = value;
	cvd->call_onchange_functions();
}