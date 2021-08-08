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
};
