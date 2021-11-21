#include "mh_defs.hpp"
#include "game_exe_interface.hpp"
#include "doomoffs.hpp"
#include "idtypeinfo.hpp"
#include "scanner_core.hpp"
#include "snaphakalgo.hpp"
#include <string>
#include <set>
#include <vector>
#include <algorithm>
#include "eventdef.hpp"
#include "cmdsystem.hpp"


static bool check_string_against_query(const char* inputstr, const char** args, unsigned argc) {
	if (!inputstr || inputstr[0] == 0)
		return false;
	for (unsigned i = 1; i < argc; ++i) {
		unsigned idx = sh::string::find_string_insens(inputstr, args[i]);

		
		if (!~idx)
			continue;
		return true;
	}
	return false;
}
class keyword_search_results_t {
	std::string result_buffer{};
	char tmpbuffer[8192];

public:
	template<typename... Ts>
	void fmt(const char* fmt, Ts... args) {

		sprintf(tmpbuffer, fmt, args...);
		result_buffer += (&tmpbuffer[0]);
		result_buffer += "\n";
	}


	void get_results(std::string& out) {

		out = std::move(result_buffer);
	}
};

struct keyword_search_param_t {

	keyword_search_results_t* m_results;
	unsigned which;
	const char** argv;
	unsigned argc;



};

static DWORD process_enumtypes_kwsearch(void* ud) {
	keyword_search_param_t* params = (keyword_search_param_t*)ud;



	auto args = params->argv;
	auto argc = params->argc;
	keyword_search_results_t& b = *(params->m_results);

	unsigned nenums;

	enumTypeInfo_t* enums = idType::EnumTypes(nenums, params->which);

	for (unsigned i = 0; i < nenums; ++i) {
		if (!enums[i].name) {
			continue;
		}
		if (check_string_against_query(enums[i].name, args, argc)) {

			b.fmt("Enum %s", enums[i].name);
		}


		enumTypeInfo_t* currenum = &enums[i];

		auto vals = currenum->values;

		while (vals->name && vals->name[0]) {

			if (check_string_against_query(vals->name, args, argc)) {

				b.fmt("Enum %s member %s", currenum->name, vals->name);
			}
			++vals;
		}
	}
	return 0;
}

static DWORD process_classtypes_kwsearch(void* ud) {

	keyword_search_param_t* params = (keyword_search_param_t*)ud;

	unsigned nclasses;
	classTypeInfo_t* iterclasses = idType::ClassTypes(nclasses, params->which);

	auto args = params->argv;
	auto argc = params->argc;
	keyword_search_results_t& b = *(params->m_results);

	//add the structures
	for (unsigned i = 0; i<nclasses; ++i) {

		classTypeInfo_t* cltype = &iterclasses[i];

		if (!cltype->name) {
			continue;
		}

		if (cltype->metaData && cltype->metaData->metaData) {

			if (check_string_against_query(cltype->metaData->metaData, args, argc)) {


				b.fmt("Class %s metaData->metadata = %s", cltype->name, cltype->metaData->metaData);

			}
		}

		if (check_string_against_query(cltype->name, args, argc)) {

			b.fmt("Class %s", cltype->name);
		}
		if (check_string_against_query(cltype->superType, args, argc)) {

			b.fmt("Class %s, superclass %s", cltype->name, cltype->superType);
		}

		classVariableInfo_t* vars = cltype->templateParms;
		if (vars) {
			while (vars->name && vars->name[0]) {


				if (check_string_against_query(vars->name, args, argc)) {
					b.fmt("Class %s template parm %s", cltype->name, vars->name);
				}
				if (check_string_against_query(vars->type, args, argc)) {

					b.fmt("Class %s template parm %s of type %s", cltype->name, vars->name, vars->type);
				}

				if (check_string_against_query(vars->comment, args, argc)) {

					b.fmt("Class %s template parm %s comment %s", cltype->name, vars->name, vars->comment);

				}
				vars++;

			}
		}


		vars = cltype->variables;
		if (vars) {
			while (vars->name && vars->name[0]) {


				if (check_string_against_query(vars->name, args, argc)) {
					b.fmt("Class %s var %s", cltype->name, vars->name);
				}
				if (check_string_against_query(vars->type, args, argc)) {

					b.fmt("Class %s var %s of type %s", cltype->name, vars->name, vars->type);
				}

				if (check_string_against_query(vars->comment, args, argc)) {

					b.fmt("Class %s var %s comment %s", cltype->name, vars->name, vars->comment);

				}
				vars++;

			}
		}

	}
	return 0;
}

static DWORD process_typedefs_kwsearch(void* ud) {

	keyword_search_param_t* params = (keyword_search_param_t*)ud;



	auto args = params->argv;
	auto argc = params->argc;
	keyword_search_results_t& b = *(params->m_results);
	unsigned ntypedefs;
	typedefInfo_t* typd = idType::TypedefTypes(ntypedefs, params->which);

	for (unsigned i = 0; i < ntypedefs; ++i) {

		typedefInfo_t* cty = &typd[i];
		if (!cty->name) {
			continue;
		}

		if (check_string_against_query(cty->name, args, argc)) {

			b.fmt("Typedef %s", cty->name);
		}

		if (check_string_against_query(cty->type, args, argc)) {

			b.fmt("Typedef %s type %s", cty->name, cty->type);
		}
	}
	return 0;
}

static DWORD process_vtbls_kwsearch(void* ud) {
	keyword_search_param_t* params = (keyword_search_param_t*)ud;

	auto args = params->argv;
	auto argc = params->argc;
	keyword_search_results_t& b = *(params->m_results);

	//g_str_to_rrti_type_descr

	for (auto&& memb : *get_str_to_rtti_type_map()) {
		if (check_string_against_query(memb.first.data(), args, argc)) {
			b.fmt("Vtbl name %s", memb.first.data());
		}
	}
	return 1;
}
static DWORD process_eventdefs_kwsearch(void* ud) {
	keyword_search_param_t* params = (keyword_search_param_t*)ud;

	auto args = params->argv;
	auto argc = params->argc;
	keyword_search_results_t& b = *(params->m_results);

	auto ev = idEventDefInterfaceLocal::Singleton();

	unsigned numev = ev->GetNumEvents();

	for (unsigned i = 0; i < numev; ++i) {
		idEventDef* evdef = ev->GetEventForNum(i);


		if (!evdef)
			continue;
		const char* n = evdef->name;
		if (check_string_against_query(n, args, argc)) {
			b.fmt("Event %s", n);
		}

		if (check_string_against_query(evdef->comment, args, argc)) {
			b.fmt("Event %s comment: %s", n, evdef->comment);
		}
		if (check_string_against_query(evdef->argNames, args, argc)) {
			b.fmt("Event %s arg names: %s", n, evdef->argNames);
		}
		if (check_string_against_query(evdef->argDefaultValues, args, argc)) {
			b.fmt("Event %s arg default values : %s", n, evdef->argDefaultValues);
		}
	}
	return 1;
}

static DWORD process_cvars_kwsearch(void* ud) {
	keyword_search_param_t* params = (keyword_search_param_t*)ud;

	auto args = params->argv;
	auto argc = params->argc;
	keyword_search_results_t& b = *(params->m_results);
	unsigned n_cvars;
	idCVar** cvars = idCVar::GetList(n_cvars);

	for (unsigned i = 0; i < n_cvars; ++i) {
	
		idCVar* c = cvars[i];
		auto cd = c->data;
		auto name = cd->name;

		if (check_string_against_query(name, args, argc)) {
			b.fmt("CVar name %s", name);
		}
		if (check_string_against_query(cd->description, args, argc)) {
			b.fmt("CVar %s description: %s", name, cd->description);
		}

		if (check_string_against_query(cd->valueString, args, argc)) {
			b.fmt("CVar %s valueString: %s", name, cd->valueString);

		}
	}
	return 1;

}
struct kwsearch_dispatcher_t {
	std::vector<HANDLE> m_enqueued_threads;
	keyword_search_param_t m_params[9];
	keyword_search_results_t m_splitres[9];
	const char** argv;
	unsigned argc;
	void add_thread(DWORD(*fn)(void*), unsigned which) {

		keyword_search_param_t* parm = &m_params[m_enqueued_threads.size()];
		parm->m_results = &m_splitres[m_enqueued_threads.size()];
		parm->which = which;
		parm->argc = argc;
		parm->argv = argv;
		void* usd =(void*)parm;


		DWORD tid;
		m_enqueued_threads.push_back(CreateThread(nullptr, 65536, fn, usd, 0, &tid));


	}


};


std::string idType::keyword_search(const char** args, unsigned argc) {

unsigned unused1;
//ensure that typeinfogenerated is initialized before we thread out
	volatile void* rwhe = idType::ClassTypes(unused1, 0);

	kwsearch_dispatcher_t dispatcher{};
	dispatcher.argc = argc;
	dispatcher.argv = args;
	for (unsigned i = 0; i < 2; ++i) {
		dispatcher.add_thread(process_enumtypes_kwsearch, i);
		dispatcher.add_thread(process_classtypes_kwsearch, i);
		dispatcher.add_thread(process_typedefs_kwsearch, i);
	}

	dispatcher.add_thread(process_vtbls_kwsearch, 0);
	dispatcher.add_thread(process_eventdefs_kwsearch, 0);
	dispatcher.add_thread(process_cvars_kwsearch, 0);
	WaitForMultipleObjects(dispatcher.m_enqueued_threads.size(), dispatcher.m_enqueued_threads.data(), TRUE, INFINITE);

	FlushProcessWriteBuffers();

	std::string concatres;

	for (auto&& v : dispatcher.m_splitres) {
		std::string tmp;
		v.get_results(tmp);
		concatres += tmp;
	}
	return concatres;

}