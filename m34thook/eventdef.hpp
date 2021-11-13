#pragma once
struct idEventDefInterfaceLocal;
#include "idStr.hpp"

struct idEventDef;
struct idEventDefInterfaceLocalVftbl
{
  void *(__fastcall  *dctor)(void *Memory, char a2);
  void *_ZN24idEventDefInterfaceLocal8AddEventE11eventType_tPKcS2_cP10idEventDef;
  void (__fastcall  *nullsub_1)();
  void* (__fastcall *_ZNK24idEventDefInterfaceLocal15GetEventCommandEi)(idEventDefInterfaceLocal * thiz, int);
  char **(__fastcall  *_ZNK24idEventDefInterfaceLocal9FindEventEPKc)(idEventDefInterfaceLocal* a1, __int64 a2);
  __int64 (__fastcall *_ZNK24idEventDefInterfaceLocal18GetEventNumForNameEPKc)(idEventDefInterfaceLocal* a1);
  const char *(__fastcall  *_ZNK24idEventDefInterfaceLocal18GetEventNameForNumEi)(idEventDefInterfaceLocal* a1, unsigned);
  unsigned (*_ZNK24idEventDefInterfaceLocal12GetNumEventsEv)(idEventDefInterfaceLocal*);
  unsigned (__fastcall  *_ZNK24idEventDefInterfaceLocal15GetNumEventArgsEi)(idEventDefInterfaceLocal* a1, int evt);
  void (__fastcall *_ZNK24idEventDefInterfaceLocal19GetEventArgTypeNameEiiR5idStr)(idEventDefInterfaceLocal * thiz, int, int, idStr *);
  void (__fastcall *_ZNK24idEventDefInterfaceLocal15GetEventArgNameEiiR5idStr)(idEventDefInterfaceLocal * thiz, int, int, idStr *);
  void *field_58;
  void (__fastcall  *anonymous_0)();
  void *field_68;
};

struct idEventDefInterfaceLocal {
	idEventDefInterfaceLocalVftbl* vftbl;

	unsigned GetNumEvents() {
		return vftbl->_ZNK24idEventDefInterfaceLocal12GetNumEventsEv(this);
	}

	const char* GetEventNameForNum(unsigned evtnum) {
		return vftbl->_ZNK24idEventDefInterfaceLocal18GetEventNameForNumEi(this, evtnum);
	}



	unsigned GetNumEventArgs(unsigned evtnum) {
		return vftbl->_ZNK24idEventDefInterfaceLocal15GetNumEventArgsEi(this, evtnum);
	}
	void GetEventArgTypeName(int evt, int arg, idStr* outstr) {
		vftbl->_ZNK24idEventDefInterfaceLocal19GetEventArgTypeNameEiiR5idStr(this, evt, arg, outstr);
	}
	void GetEventArgName(int evt, int arg, idStr* outstr) {
		vftbl->_ZNK24idEventDefInterfaceLocal15GetEventArgNameEiiR5idStr(this, evt, arg, outstr);
	}


	idEventDef* FindEvent(const char* name) {

		return (idEventDef*) vftbl->_ZNK24idEventDefInterfaceLocal9FindEventEPKc(this, (long long)name);
	}
	idEventDef* GetEventForNum(unsigned idx) {
		//ewww 
		return FindEvent(GetEventNameForNum(idx));
	}
	static idEventDefInterfaceLocal* Singleton();
};


void DumpEventDefs(bool as_enum);
enum eventType_t
{
	EVENT_GAME = 0x0,
	EVENT_ACTION = 0x1,
	EVENT_ANIMEVENT = 0x2,
	EVENT_FSMEVENT = 0x3,
	EVENT_NOTICE = 0x4,
	EVENT_SSACTION = 0x5,
};

struct idEventDef {
	char* name;
	char* formatspec;
	char* argTypes;
	char* argNames;
	char* argDefaultValues;
	int returnType;
	int numargs;
	int numstrings;
	int eventnum;
	int flags;
	eventType_t type;
	char* timelineEventGroup;
	char* comment;
	idEventDef* noticeEvent;
	idEventDef* next;
	//copied from decompilation, for reimplementing scriptcmdent
	bool GetArgTypeName(int arg, std::string* tname);
};