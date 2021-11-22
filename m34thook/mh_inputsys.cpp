#include "xbyak/xbyak.h"
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
#include "idmath.hpp"
#include <cassert>
#include <stdlib.h>
#include <stdio.h>
#include "gameapi.hpp"
#include "snaphakalgo.hpp"
#include "mh_inputsys.hpp"
#include "mh_mainloop.hpp"
#include <mutex>
static void* g_original_queueevent = nullptr;

static enumTypeInfo_t* g_eventtype_enum = nullptr;

static enumTypeInfo_t* g_keynum_enum = nullptr;
MH_NOINLINE
MH_REGFREE_CALL
static void init_inputeventqueue_hook_globals() {
	g_eventtype_enum = idType::FindEnumInfo("inputEventType_t");
	g_keynum_enum = idType::FindEnumInfo("keyNum_t");
}
enum inputEventType_t
{
	SE_NONE = 0x0,
	SE_KEY = 0x1,
	SE_CHAR = 0x2,
	SE_MOUSE = 0x3,
	SE_MOUSE_LEAVE = 0x4,
	SE_JOYSTICK = 0x5,
	SE_CONSOLE = 0x6,
	SE_TOUCHPAD = 0x7,
	SE_VR = 0x8,
};

enum keyNum_t {
	K_NONE = 0x0,
	K_ESCAPE = 0x1,
	K_1 = 0x2,
	K_2 = 0x3,
	K_3 = 0x4,
	K_4 = 0x5,
	K_5 = 0x6,
	K_6 = 0x7,
	K_7 = 0x8,
	K_8 = 0x9,
	K_9 = 0xA,
	K_0 = 0xB,
	K_MINUS = 0xC,
	K_EQUALS = 0xD,
	K_BACKSPACE = 0xE,
	K_TAB = 0xF,
	K_Q = 0x10,
	K_W = 0x11,
	K_E = 0x12,
	K_R = 0x13,
	K_T = 0x14,
	K_Y = 0x15,
	K_U = 0x16,
	K_I = 0x17,
	K_O = 0x18,
	K_P = 0x19,
	K_LBRACKET = 0x1A,
	K_RBRACKET = 0x1B,
	K_ENTER = 0x1C,
	K_LCTRL = 0x1D,
	K_A = 0x1E,
	K_S = 0x1F,
	K_D = 0x20,
	K_F = 0x21,
	K_G = 0x22,
	K_H = 0x23,
	K_J = 0x24,
	K_K = 0x25,
	K_L = 0x26,
	K_SEMICOLON = 0x27,
	K_APOSTROPHE = 0x28,
	K_GRAVE = 0x29,
	K_LSHIFT = 0x2A,
	K_BACKSLASH = 0x2B,
	K_Z = 0x2C,
	K_X = 0x2D,
	K_C = 0x2E,
	K_V = 0x2F,
	K_B = 0x30,
	K_N = 0x31,
	K_M = 0x32,
	K_COMMA = 0x33,
	K_PERIOD = 0x34,
	K_SLASH = 0x35,
	K_RSHIFT = 0x36,
	K_KP_STAR = 0x37,
	K_LALT = 0x38,
	K_SPACE = 0x39,
	K_CAPSLOCK = 0x3A,
	K_F1 = 0x3B,
	K_F2 = 0x3C,
	K_F3 = 0x3D,
	K_F4 = 0x3E,
	K_F5 = 0x3F,
	K_F6 = 0x40,
	K_F7 = 0x41,
	K_F8 = 0x42,
	K_F9 = 0x43,
	K_F10 = 0x44,
	K_PAUSE = 0x45,
	K_SCROLL = 0x46,
	K_KP_7 = 0x47,
	K_KP_8 = 0x48,
	K_KP_9 = 0x49,
	K_KP_MINUS = 0x4A,
	K_KP_4 = 0x4B,
	K_KP_5 = 0x4C,
	K_KP_6 = 0x4D,
	K_KP_PLUS = 0x4E,
	K_KP_1 = 0x4F,
	K_KP_2 = 0x50,
	K_KP_3 = 0x51,
	K_KP_0 = 0x52,
	K_KP_DOT = 0x53,
	K_F11 = 0x57,
	K_F12 = 0x58,
	K_F13 = 0x64,
	K_F14 = 0x65,
	K_F15 = 0x66,
	K_KANA = 0x70,
	K_CONVERT = 0x79,
	K_NOCONVERT = 0x7B,
	K_YEN = 0x7D,
	K_KP_EQUALS = 0x8D,
	K_CIRCUMFLEX = 0x90,
	K_AT = 0x91,
	K_COLON = 0x92,
	K_UNDERLINE = 0x93,
	K_KANJI = 0x94,
	K_STOP = 0x95,
	K_AX = 0x96,
	K_UNLABELED = 0x97,
	K_KP_ENTER = 0x9C,
	K_RCTRL = 0x9D,
	K_KP_COMMA = 0xB3,
	K_KP_SLASH = 0xB5,
	K_PRINTSCREEN = 0xB7,
	K_RALT = 0xB8,
	K_NUMLOCK = 0xC5,
	K_HOME = 0xC7,
	K_UPARROW = 0xC8,
	K_PGUP = 0xC9,
	K_LEFTARROW = 0xCB,
	K_RIGHTARROW = 0xCD,
	K_END = 0xCF,
	K_DOWNARROW = 0xD0,
	K_PGDN = 0xD1,
	K_INS = 0xD2,
	K_DEL = 0xD3,
	K_LWIN = 0xDB,
	K_RWIN = 0xDC,
	K_APPS = 0xDD,
	K_POWER = 0xDE,
	K_SLEEP = 0xDF,
	K_JOY1 = 0x100,
	K_JOY_A = 0x100,
	K_JOY2 = 0x101,
	K_JOY_B = 0x101,
	K_JOY3 = 0x102,
	K_JOY_X = 0x102,
	K_JOY4 = 0x103,
	K_JOY_Y = 0x103,
	K_JOY5 = 0x104,
	K_JOY_LB = 0x104,
	K_JOY6 = 0x105,
	K_JOY_RB = 0x105,
	K_JOY7 = 0x106,
	K_JOY_LS = 0x106,
	K_JOY8 = 0x107,
	K_JOY_RS = 0x107,
	K_JOY9 = 0x108,
	K_JOY_START = 0x108,
	K_JOY10 = 0x109,
	K_JOY_BACK = 0x109,
	K_JOY11 = 0x10A,
	K_JOY12 = 0x10B,
	K_JOY13 = 0x10C,
	K_JOY14 = 0x10D,
	K_JOY15 = 0x10E,
	K_JOY16 = 0x10F,
	K_JOY_STICK1_UP = 0x110,
	K_JOY_STICK1_DOWN = 0x111,
	K_JOY_STICK1_LEFT = 0x112,
	K_JOY_STICK1_RIGHT = 0x113,
	K_JOY_STICK2_UP = 0x114,
	K_JOY_STICK2_DOWN = 0x115,
	K_JOY_STICK2_LEFT = 0x116,
	K_JOY_STICK2_RIGHT = 0x117,
	K_JOY_TRIGGER1 = 0x118,
	K_JOY_TRIGGER2 = 0x119,
	K_JOY_DPAD_UP = 0x11A,
	K_JOY_DPAD_DOWN = 0x11B,
	K_JOY_DPAD_LEFT = 0x11C,
	K_JOY_DPAD_RIGHT = 0x11D,
	K_MOUSE1 = 0x11E,
	K_MOUSE2 = 0x11F,
	K_MOUSE3 = 0x120,
	K_MOUSE4 = 0x121,
	K_MOUSE5 = 0x122,
	K_MOUSE6 = 0x123,
	K_MOUSE7 = 0x124,
	K_MOUSE8 = 0x125,
	K_MWHEELDOWN = 0x126,
	K_MWHEELUP = 0x127,
	K_JOY_SIXAXIS_UP = 0x128,
	K_JOY_SIXAXIS_DOWN = 0x129,
	K_JOY_SIXAXIS_LEFT = 0x12A,
	K_JOY_SIXAXIS_RIGHT = 0x12B,
	K_VR_START_INDEX = 0x12C,
	K_STEAMVR_PRIMARY_START_INDEX = 0x12C,
	K_STEAMVR_PRIMARY_SYSTEM = 0x12C,
	K_STEAMVR_PRIMARY_APPLICATION = 0x12D,
	K_STEAMVR_PRIMARY_GRIP = 0x12E,
	K_STEAMVR_PRIMARY_DPAD_LEFT = 0x12F,
	K_STEAMVR_PRIMARY_DPAD_UP = 0x130,
	K_STEAMVR_PRIMARY_DPAD_RIGHT = 0x131,
	K_STEAMVR_PRIMARY_DPAD_DOWN = 0x132,
	K_STEAMVR_PRIMARY_A = 0x133,
	K_STEAMVR_PRIMARY_AXIS0 = 0x134,
	K_STEAMVR_PRIMARY_TOUCHPAD = 0x134,
	K_STEAMVR_PRIMARY_AXIS1 = 0x135,
	K_STEAMVR_PRIMARY_TRIGGER = 0x135,
	K_STEAMVR_PRIMARY_AXIS2 = 0x136,
	K_STEAMVR_PRIMARY_AXIS3 = 0x137,
	K_STEAMVR_PRIMARY_AXIS4 = 0x138,
	K_STEAMVR_SECONDARY_START_INDEX = 0x13C,
	K_STEAMVR_SECONDARY_SYSTEM = 0x13C,
	K_STEAMVR_SECONDARY_APPLICATION = 0x13D,
	K_STEAMVR_SECONDARY_GRIP = 0x13E,
	K_STEAMVR_SECONDARY_DPAD_LEFT = 0x13F,
	K_STEAMVR_SECONDARY_DPAD_UP = 0x140,
	K_STEAMVR_SECONDARY_DPAD_RIGHT = 0x141,
	K_STEAMVR_SECONDARY_DPAD_DOWN = 0x142,
	K_STEAMVR_SECONDARY_A = 0x143,
	K_STEAMVR_SECONDARY_AXIS0 = 0x144,
	K_STEAMVR_SECONDARY_TOUCHPAD = 0x144,
	K_STEAMVR_SECONDARY_AXIS1 = 0x145,
	K_STEAMVR_SECONDARY_TRIGGER = 0x145,
	K_STEAMVR_SECONDARY_AXIS2 = 0x146,
	K_STEAMVR_SECONDARY_AXIS3 = 0x147,
	K_STEAMVR_SECONDARY_AXIS4 = 0x148,
	K_PSMOVE_PRIMARY_START_INDEX = 0x14C,
	K_PSMOVE_PRIMARY_SELECT = 0x14C,
	K_PSMOVE_PRIMARY_START = 0x14E,
	K_PSMOVE_PRIMARY_TRIANGLE = 0x14F,
	K_PSMOVE_PRIMARY_SQUARE = 0x150,
	K_PSMOVE_PRIMARY_CIRCLE = 0x151,
	K_PSMOVE_PRIMARY_CROSS = 0x152,
	K_PSMOVE_PRIMARY_MOVE = 0x154,
	K_PSMOVE_PRIMARY_TRIGGER = 0x155,
	K_PSMOVE_SECONDARY_START_INDEX = 0x15C,
	K_PSMOVE_SECONDARY_SELECT = 0x15C,
	K_PSMOVE_SECONDARY_START = 0x15E,
	K_PSMOVE_SECONDARY_TRIANGLE = 0x15F,
	K_PSMOVE_SECONDARY_SQUARE = 0x160,
	K_PSMOVE_SECONDARY_CIRCLE = 0x161,
	K_PSMOVE_SECONDARY_CROSS = 0x162,
	K_PSMOVE_SECONDARY_MOVE = 0x164,
	K_PSMOVE_SECONDARY_TRIGGER = 0x165,
	K_PSAIM_START_INDEX = 0x16C,
	K_PSAIM_TRIANGLE = 0x16C,
	K_PSAIM_SQUARE = 0x16D,
	K_PSAIM_CIRCLE = 0x16E,
	K_PSAIM_CROSS = 0x16F,
	K_PSAIM_L1 = 0x170,
	K_PSAIM_L2 = 0x171,
	K_PSAIM_L3 = 0x172,
	K_PSAIM_R1 = 0x173,
	K_PSAIM_R2 = 0x174,
	K_PSAIM_R3 = 0x175,
	K_PSAIM_DPAD_LEFT = 0x176,
	K_PSAIM_DPAD_UP = 0x177,
	K_PSAIM_DPAD_RIGHT = 0x178,
	K_PSAIM_DPAD_DOWN = 0x179,
	K_PSAIM_TOUCHPAD = 0x17A,
	K_PSAIM_OPTIONS = 0x17B,
	K_VR_END_INDEX = 0x17C,
	K_LAST_KEY = 0x17C,
};
#if 0
struct kbd_macro_entry_t {
	unsigned m_sendkey;
	bool m_up;
	unsigned m_num_frames_til_next;
	uint64_t m_frameplay;

	kbd_macro_entry_t(unsigned k, bool up, unsigned numframe) : m_sendkey(k), m_up(up), m_num_frames_til_next(numframe), m_frameplay(0) {}

};

struct macro_state_t {
	std::vector<kbd_macro_entry_t> m_entries;
	unsigned m_sentdevice;
	unsigned m_current_playing;
	unsigned m_bind;
	bool m_playing;
	void start(unsigned device) {
		uint64_t tickstart = mh_mainloop::num_ticks();

		for (auto&& entry : m_entries) {
			entry.m_frameplay = entry.m_num_frames_til_next + tickstart;
			tickstart += entry.m_num_frames_til_next;
		}
		m_playing = true;
		m_sentdevice = device;
		m_current_playing = 0;
	}

	void update(__int64 a1) {
		if (!m_playing)
			return;
		uint64_t currframe = mh_mainloop::num_ticks();


		for (; m_current_playing < m_entries.size() && m_entries[m_current_playing].m_frameplay <= currframe; ++m_current_playing) {
			call_as<__int64>(g_original_queueevent, a1, 1, m_entries[m_current_playing].m_sendkey, (unsigned)m_entries[m_current_playing].m_up, 0, nullptr, m_sentdevice);
		}
		if (m_current_playing >= m_entries.size()) {
			m_playing = false;
		}
	}
};


static macro_state_t g_testmacro{};

static void init_testmacro() {

	g_testmacro.m_entries.emplace_back(K_BACKSPACE, false, 1);
	g_testmacro.m_entries.emplace_back(K_ENTER, false, 1);
	g_testmacro.m_entries.emplace_back(K_ENTER, true, 1);
	g_testmacro.m_entries.emplace_back(K_BACKSPACE, true, 1);

}
#endif
static std::mutex g_input_handler_mutex{};

//static std::vector<mh_input_handler_t*> g_input_handlers{};

struct input_handler_node_t {
	rb_node m_srchnode;
	mh_input_handler_t* m_handler;

};
static rb_root g_input_handler_set{};


void mh_input::install_input_handler(mh_input_handler_t* handler) {
	g_input_handler_mutex.lock();
	sh::rb::insert_hint_t hint{};
	auto node = sh::rb::rbnode_find<input_handler_node_t, 0, mh_input_handler_t*>(&g_input_handler_set,
		handler, [](input_handler_node_t* node, mh_input_handler_t* handler) {
			return node->m_handler - handler;
		}, &hint);

	if (!node) {
		input_handler_node_t* result = new input_handler_node_t();
		rb_init_node(&result->m_srchnode);
		result->m_handler = handler;


		hint.insert(&result->m_srchnode, &g_input_handler_set);
	}

	g_input_handler_mutex.unlock();
}
void mh_input::remove_input_handler(mh_input_handler_t* handler) {
	g_input_handler_mutex.lock();
	sh::rb::insert_hint_t hint{};
	auto node = sh::rb::rbnode_find<input_handler_node_t, 0, mh_input_handler_t*>(&g_input_handler_set,
		handler, [](input_handler_node_t* node, mh_input_handler_t* handler) {
			return node->m_handler - handler;
		}, &hint);

	if (node) {
		sh::rb::rb_erase(&node->m_srchnode, &g_input_handler_set);
		delete node;
	}
	g_input_handler_mutex.unlock();
}

static event_consumed_e run_events(int evtype, int evval, int evvalue2) {
	g_input_handler_mutex.lock();
	event_consumed_e consumed = event_consumed_e::UNCONSUMED;
	for (auto&& handlernode : sh::rb::rb_iterate<input_handler_node_t, 0>(g_input_handler_set)) {
		auto handler = handlernode->m_handler;
		switch (evtype) {
		case SE_CHAR: {
			consumed = handler->receive_char_event(evval);
	check_consumed_and_break:
			if (consumed == event_consumed_e::CONSUMED)
				goto done;
			break;
		}
		case SE_KEY:{
			//skip K_
			const char* kname = idType::get_enum_member_name_for_value(g_keynum_enum, evval) + 2;

			consumed = evvalue2 != 0 ? handler->receive_keyup_event(kname) : handler->receive_keydown_event(kname);
			goto check_consumed_and_break;
		}
		case SE_MOUSE:
			handler->receive_mouse_move_event(evval, evvalue2);
			break;
		}
	}
done:
	g_input_handler_mutex.unlock();
	return consumed;
}

static __int64  idInputEventQueue_QueueEvent(
	__int64 a1,
	int evtype,
	int evval,
	int evvalue2,
	int evptrlength,
	void* evptr,
	unsigned int inpdevice) {

	MH_UNLIKELY_IF(!g_eventtype_enum) {

		init_inputeventqueue_hook_globals();
		//	init_testmacro();
	}
#if 0
	if (idType::enum_member_is(g_eventtype_enum, evtype, "SE_CHAR")) {
		char shit[2] = { 0,0 };
		shit[0] = evval;
		//evval = the char
		//idLib::Printf("se_char, val = %s, val2 = %d\n", shit, evvalue2);
	}
	//g_testmacro.update(a1);
	if (idType::enum_member_is(g_eventtype_enum, evtype, "SE_KEY")) {

		/*if(evval == 0x29) grave , this code disables opening the console via `
			return 0;
			*/
		const char* keyname = idType::get_enum_member_name_for_value(g_keynum_enum, evval);

		if (keyname == nullptr) {
			keyname = "NULL";
		}

		/*if(evval == K_F1 && evvalue2 != 0) {
			g_testmacro.start(inpdevice);
		}*/
		/*
			this all works and spams keystrokes in a really annoying fashion

		*/

		//idLib::Printf("Got key %s\n", keyname);

	}
#else
	if(run_events(evtype, evval, evvalue2) == event_consumed_e::CONSUMED)
		return 0;
#endif
	return call_as<__int64>(g_original_queueevent, a1, evtype, evval, evvalue2, evptrlength, evptr, inpdevice);
}


void mh_input::install_input_hooks() {
	//same in v1 and v6


	void* queue_event_func = get_class_vtbl(".?AVidInputLocalWin32@@")[0x60 / 8];

	if (reinterpret_cast<unsigned char*>(queue_event_func)[1] == 0x8D) {//check if lea eventqueue jmp queuevent, which is done for v1 
		//jit it so we can call both possible versions seamlessly
		void* orig = mh_disassembler_t::first_jump_target(queue_event_func);

		void* event_queue_ptr = mh_lea<char>(queue_event_func, 7) + *mh_lea<int>(queue_event_func, 3);

		Xbyak::CodeGenerator e{};

		e.mov(e.rcx, (uintptr_t)event_queue_ptr);
		e.mov(e.rax, (uintptr_t)orig);
		e.jmp(e.rax);

		void* thunkmem = alloc_execmem(e.getSize());

		memcpy(thunkmem, e.getCode(), e.getSize());


		g_original_queueevent = thunkmem;
		redirect_to_func(idInputEventQueue_QueueEvent, (uintptr_t)queue_event_func, true);
	}
	else {
		g_original_queueevent = detour_with_thunk_for_original(queue_event_func, (void*)idInputEventQueue_QueueEvent);

	}

}