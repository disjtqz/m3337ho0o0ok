#pragma once
#include "extern.h"


using eternal_ea_t = unsigned;


struct disasm_opnd_t {
	ud_operand_data_t m_data;
	eternal_ea_t m_ea; //the effective address of the instruction (add to g_blamdll.image_base to get the real addr)
	unsigned short m_opcode; //ud_mnemonic_code

};