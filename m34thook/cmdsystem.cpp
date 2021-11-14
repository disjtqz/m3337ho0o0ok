#include "mh_defs.hpp"

#include "game_exe_interface.hpp"
#include "cmdsystem.hpp"

//#include "doomoffs.hpp"
#include "scanner_core.hpp"
#include "idtypeinfo.hpp"
#include "gameapi.hpp"
#include "snaphakalgo.hpp"
#include "idLib.hpp"
#include "mh_headergen.hpp"
#include "idStr.hpp"
void idCmd::register_command(const char* name, cmdcb_t cb, const char* description) {
	//auto cmdSystem = *doomsym<char**>(doomoffs::cmdSystem);

	auto cmdSystem = *(char**)descan::g_idcmdsystem;
	(*(void(__fastcall**)(__int64, const char*, cmdcb_t, const char*, unsigned long long, unsigned))(*(unsigned long long*)cmdSystem + 24i64))(
		(__int64)cmdSystem,
		name,
		cb,
		description,
		0i64,
		0);

}
struct idCmdSystemLocal;


using _QWORD = unsigned  long long;
struct idCmdSystemLocalVftbl
{
  void* first; // 0
  void *field_8; // 8
  __int64 (__fastcall *field_10)(__int64 a1, int a2); // 10
  void *_ZN16idCmdSystemLocal10AddCommandEPKcPFvRK9idCmdArgsES1_PFvR14idAutoCompleteEi; // 18
  const char *(__fastcall *_ZN16idCmdSystemLocal21GetCommandDescriptionEPKc)(__int64 a1, __int64 a2); // 20
  _QWORD (__fastcall *_ZN16idCmdSystemLocal13CommandExistsEPKcb)(idCmdSystemLocal * thiz, const char *, bool); // 28
  void *_ZN16idCmdSystemLocal12FindCommandsEPKcR6idListI5idStrL8memTag_t5ELb0EE; // 30
  void *field_38; // 38 
  _QWORD (__fastcall *_ZN16idCmdSystemLocal18ExecuteCommandTextEPKc)(idCmdSystemLocal * thiz, const char *); // 40
  void *field_48;
  //void *field_50;
  _QWORD(__fastcall* _AppendCommandHack)(idCmdSystemLocal* thiz, const char*); // 48
  void *_GenerateTestCases__$TestDummyTemplatedParametricTestFixture_V_$hash_map_H_NU_$hash_H_core__U_$equal_to_H_std___core___SuiteTemplatedTestkUnitTestCategory__SAXAEAU_$TestCaseEmitter_HXXXX_Testing___Z;
  void *field_60;
  __int64 (__fastcall  *field_68)(_QWORD *a1);
  idListVoid* (__fastcall *GetCommands)(idCmdSystemLocal *a1);
  _QWORD (*_ZN16idCmdSystemLocal20ExecuteCommandBufferEv)(idCmdSystemLocal * thiz);
  void *field_80;
};


struct idCmdSystemLocal {
	idCmdSystemLocalVftbl* vftbl;
};

static idCmdSystemLocal* cmdSystem_get() {
	return *reinterpret_cast<idCmdSystemLocal**>(descan::g_idcmdsystem);
}

void idCmd::execute_command_text(const char* txt) {
	auto cmdsys = cmdSystem_get();
	cmdsys->vftbl->_ZN16idCmdSystemLocal18ExecuteCommandTextEPKc(cmdsys, txt);
}

void idCmd::add_command(const char* txt)
{
	auto cmdsys = cmdSystem_get();
	cmdsys->vftbl->_AppendCommandHack(cmdsys, txt);
}

void idCmd::execute_command_buffer() {
	auto cmdsys = cmdSystem_get();
	cmdsys->vftbl->_ZN16idCmdSystemLocal20ExecuteCommandBufferEv(cmdsys);
}

idCVar* idCVar::Find(const char* name) {
	void* sys = get_cvarsystem();
	void* tocall = *mh_lea<void*>(reinterpret_cast<void**>(sys)[0], 32);

	return call_as<idCVar*>(tocall, sys, name);
}

static mh_fieldcached_t<const char*> g_idCommandLink_cmdName{};
static mh_fieldcached_t<void*> g_idCommandLink_function{};
static mh_fieldcached_t<void*> g_idCommandLink_next{};
enum cmdFlags_t
{
	CMD_NOCHEAT = 0x2,
	CMD_EXPOSE = 0x4,
	CMD_SHIPPINGDISABLED = 0x8,
};

//from v1
struct idAutoComplete;
struct __declspec(align(8)) used_for_commands_t
{
	const char* name;
	void(__fastcall* function)(const idCmdArgs*);
	void(__fastcall* autocomplete)(idAutoComplete*);
	const char* description;
	int flags; //cmdFlags_t
};
struct __declspec(align(8)) idCmdSystem_buffer
{
	idListVoid all_cmds;
	idListVoid nocheat_cmds;
	int framewait;
	idStrStatic<65536> buffer1;
	idStrStatic<65536> buffer2;
};

void* idCmd::find_command_by_name(const char* name) {

	auto cmdsystem = cmdSystem_get();


	idCmdSystem_buffer* cmdinfo = *mh_lea<idCmdSystem_buffer*>(cmdsystem, 16);



	//while (cmds) {

	idListVoid* cmds_lists = &cmdinfo->all_cmds;


	for(unsigned i = 0; i < cmds_lists->num; ++i) {
		used_for_commands_t* cmds = reinterpret_cast<used_for_commands_t*>(cmds_lists->list[i]);


		const char* cmdname = *g_idCommandLink_cmdName(cmds, "idCommandLink", "cmdName");

		if (sh::string::streq(cmds->name, name)) {
			return cmds->function;
		}

	}
	return nullptr;
}

idCVar** idCVar::GetList(unsigned& out_n) {
	void* sys = get_cvarsystem();
	
	idListVoid* entries = *mh_lea<idListVoid*>(sys, 8);


	out_n = entries->num;


	return (idCVar**)entries->list;


}



static strviewset_t generate_cvar_name_set() {
	strviewset_t res;
	unsigned num_cvars = 0;
	idCVar** cvs = idCVar::GetList(num_cvars);


	for (unsigned i = 0; i < num_cvars; ++i) {
		idCVar* current = cvs[i];

		auto data = current->data;
		res.insert(data->name);
	}

	return res;

}

MH_NOINLINE
void idCVar::generate_name_table() {

	strviewset_t cvset = generate_cvar_name_set();
	std::vector<unsigned char> bbuff = pack_strset(cvset);
	unsigned total_required_bytes = bbuff.size();
	std::vector<unsigned char> compout = compress_packet_strset(bbuff);

	std::string header_txt = "#pragma once\nenum de_cvar_e : unsigned short {";

	for (auto&& prop : cvset) {

		header_txt += "cvr_";
		std::string_view tmpprop = prop;
		while (isspace(tmpprop[0])) {
			tmpprop = tmpprop.substr(1);
		}
		header_txt += tmpprop;
		header_txt += ",";
	}
	header_txt += "};static constexpr unsigned DE_NUMCVARS = ";

	header_txt += std::to_string(cvset.size());


	header_txt += ";static constexpr unsigned ALLCVARS_COMPRESSED_SIZE = ";
	header_txt += std::to_string(compout.size());
	header_txt += ";static constexpr unsigned ALLCVARS_DECOMPRESSED_SIZE = ";

	header_txt += std::to_string(bbuff.size());

	header_txt += ";\n__declspec(allocate(\"cmptbl\")) extern unsigned char ALLCVARS_COMPRESSED_DATA[ALLCVARS_COMPRESSED_SIZE];";

	write_cfile(std::move(header_txt), "doom_eternal_cvars_generated.hpp");


	std::string cvartxt = "#include \"doom_eternal_cvars_generated.hpp\"\n";

	cvartxt += "__declspec(allocate(\"cmptbl\")) unsigned char ALLCVARS_COMPRESSED_DATA[ALLCVARS_COMPRESSED_SIZE] = {";

	cvartxt += expand_bytes_to_c_bytearray(compout);

	cvartxt += "};";
	write_cfile(std::move(cvartxt), "doom_eternal_cvars_generated.cpp");
}
unsigned g_cvardata_rvas[DE_NUMCVARS];

void idCVar::get_cvardata_rvas() {
	bvec_t hugebuffer_decompress1 = decompress_strset(ALLCVARS_COMPRESSED_DATA, ALLCVARS_COMPRESSED_SIZE, ALLCVARS_DECOMPRESSED_SIZE);


	strviewset_t decompressed_set = unpack_strset(hugebuffer_decompress1, DE_NUMCVARS);

	std::map<std::string_view, idCVar::cvarData_t*> cvar_datas;
	unsigned num_cvars;
	idCVar** cvs = idCVar::GetList(num_cvars);


	for (unsigned i = 0; i < num_cvars; ++i) {
		idCVar* current = cvs[i];

		auto data = current->data;
		cvar_datas[data->name] = data;
	}

	strviewset_t::iterator current_findpos = decompressed_set.begin();

	auto cvardataend = cvar_datas.end();
	for (unsigned i = 0; i < DE_NUMCVARS; ++i) {

		auto loc = cvar_datas.find(*current_findpos);
		if (loc == cvardataend) {

			g_cvardata_rvas[i] == 0;
		}
		else {
			g_cvardata_rvas[i] = to_de_rva(loc->second);
		}
		++current_findpos;
	}


}

MH_NOINLINE
void idCVar::cvarData_t::call_onchange_functions() {
	for (auto i = this->onChange; i; i = i->next)
		i->callback->Call();
}