#include "mh_defs.hpp"

#include "eventdef.hpp"
#include <string>
#include <vector>
#include "idtypeinfo.hpp"
#include "gameapi.hpp"
idEventDefInterfaceLocal* idEventDefInterfaceLocal::Singleton() {
	return reinterpret_cast<idEventDefInterfaceLocal*>(get_eventdef_interface());
}

static std::string event_to_string(idEventDefInterfaceLocal* iface, unsigned evnum, bool as_enum) {

	const char* name = iface->GetEventNameForNum(evnum);

	if (!as_enum) {
		struct arginfo_t {
			std::string name;
			std::string type;
		};

		std::vector<arginfo_t> allargs{};

		unsigned narg = iface->GetNumEventArgs(evnum);
		allargs.reserve(narg);


		for (unsigned i = 0; i < narg; ++i) {
			arginfo_t info{};
			{
				idStr temp{};
				iface->GetEventArgName(evnum, i, &temp);
				info.name = temp.data;
			}
			{
				idStr temp{};
				iface->GetEventArgTypeName(evnum, i, &temp);
				info.type = temp.data;
			}

			allargs.push_back(std::move(info));
		}


		std::string result = name;
		result += ":\n";

		for (auto&& arg : allargs) {
			result += "\t";
			result += arg.name;
			result += " : ";
			result += arg.type;
			result += "\n";

		}

		return result;
	}
	else {
		return "EVID_" + std::string{ name } + " = " + std::to_string(evnum) + ",";
	}
}

void DumpEventDefs(bool as_enum) {
	auto iface = idEventDefInterfaceLocal::Singleton();

	unsigned nevent = iface->GetNumEvents();

	std::string result_text = as_enum ? "enum evnum_t {\n" : "";


	for (unsigned i = 0; i < nevent; ++i) {
		result_text += event_to_string(iface, i, as_enum);
		result_text += "\n";
	}

	if(as_enum) {
		result_text += "};";
	}
	FILE* outfile = nullptr;
	fopen_s(&outfile, "eternalevents.txt", "w");
	fputs(result_text.c_str(), outfile);

	fclose(outfile);



}