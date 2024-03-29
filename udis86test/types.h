/* udis86 - libudis86/types.h
 *
 * Copyright (c) 2002-2013 Vivek Thampi
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright notice,
 *       this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright notice,
 *       this list of conditions and the following disclaimer in the documentation
 *       and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef UD_TYPES_H
#define UD_TYPES_H

#ifdef __KERNEL__
 /* -D__KERNEL__ is automatically passed on the command line when
	building something as part of the Linux kernel */
# include <linux/kernel.h>
# include <linux/string.h>
# ifndef __UD_STANDALONE__
#  define __UD_STANDALONE__ 1
#endif
#endif /* __KERNEL__ */

#if defined(_MSC_VER) || defined(__BORLANDC__)
# include <stdint.h>
# include <stdio.h>

#elif !defined(__UD_STANDALONE__)
# include <stdio.h>
# include <inttypes.h>
#endif /* !__UD_STANDALONE__ */

	/* gcc specific extensions */
#ifdef __GNUC__
# define UD_ATTR_PACKED __attribute__((packed))
#else
# define UD_ATTR_PACKED
#endif /* UD_ATTR_PACKED */


//#define     DISABLE_ASM_TEXT
#define     SPECIALIZE_FOR_ETERNAL


#ifdef SPECIALIZE_FOR_ETERNAL


#define     FORCE_64BIT_MODE

#define     DONT_DECODE_MMX 
#define     GS_IS_ONLY_SEG
#define     DONT_DECODE_X87
#define     FORCE_INTEL_VENDOR
#define     DISABLE_PAUSE_REP_NOP_DECODING
#define     ONLY_USE_INPUT_BUF
#define     INP_READ_FULL_TYPE

#include <array>
#include <intrin.h>
#include <bit>

//make the bytes that ud uses on ud_state full 32 bit to eliminate need for zero ext
//and enable memory mirroring on zen
//using udstate_uint8_fast_t = unsigned;
/*
	map values in range 0-32 to continuous indices
*/
template<unsigned... vals>
struct popcnt_table_mapper_t {

	static constexpr unsigned gentable() {
		unsigned valsa[] = { vals... };

		unsigned res = 0;
		for (auto&& val : valsa) {
			res |= 1U << val;
		}
		return res;
	}

	static constexpr unsigned m_pop_word = gentable();

	static constexpr unsigned indexfor(unsigned value) {

		unsigned tmp = ((1U << value) - 1) & m_pop_word;

		if (std::is_constant_evaluated()) {
			return std::popcount(tmp);
		}
		else
			return _popcnt32(tmp);
	}
	static bool isset(unsigned value) {
		return ((1U << value) & m_pop_word) != 0;
	}
};


#else
#endif
using udstate_uint8_fast_t = uint8_t;

#define     mh_dis_assume(...)      if(!(__VA_ARGS__))__builtin_unreachable()

/* -----------------------------------------------------------------------------
 * All possible "types" of objects in udis86. Order is Important!
 * -----------------------------------------------------------------------------
 */
enum ud_type
{
	UD_NONE,

	/* 8 bit GPRs */
	UD_R_AL, UD_R_CL, UD_R_DL, UD_R_BL,
	UD_R_AH, UD_R_CH, UD_R_DH, UD_R_BH,
	UD_R_SPL, UD_R_BPL, UD_R_SIL, UD_R_DIL,
	UD_R_R8B, UD_R_R9B, UD_R_R10B, UD_R_R11B,
	UD_R_R12B, UD_R_R13B, UD_R_R14B, UD_R_R15B,

	/* 16 bit GPRs */
	UD_R_AX, UD_R_CX, UD_R_DX, UD_R_BX,
	UD_R_SP, UD_R_BP, UD_R_SI, UD_R_DI,
	UD_R_R8W, UD_R_R9W, UD_R_R10W, UD_R_R11W,
	UD_R_R12W, UD_R_R13W, UD_R_R14W, UD_R_R15W,

	/* 32 bit GPRs */
	UD_R_EAX, UD_R_ECX, UD_R_EDX, UD_R_EBX,
	UD_R_ESP, UD_R_EBP, UD_R_ESI, UD_R_EDI,
	UD_R_R8D, UD_R_R9D, UD_R_R10D, UD_R_R11D,
	UD_R_R12D, UD_R_R13D, UD_R_R14D, UD_R_R15D,

	/* 64 bit GPRs */
	UD_R_RAX, UD_R_RCX, UD_R_RDX, UD_R_RBX,
	UD_R_RSP, UD_R_RBP, UD_R_RSI, UD_R_RDI,
	UD_R_R8, UD_R_R9, UD_R_R10, UD_R_R11,
	UD_R_R12, UD_R_R13, UD_R_R14, UD_R_R15,

	/* segment registers */
	UD_R_ES, UD_R_CS, UD_R_SS, UD_R_DS,
	UD_R_FS, UD_R_GS,

	/* control registers*/
	UD_R_CR0, UD_R_CR1, UD_R_CR2, UD_R_CR3,
	UD_R_CR4, UD_R_CR5, UD_R_CR6, UD_R_CR7,
	UD_R_CR8, UD_R_CR9, UD_R_CR10, UD_R_CR11,
	UD_R_CR12, UD_R_CR13, UD_R_CR14, UD_R_CR15,

	/* debug registers */
	UD_R_DR0, UD_R_DR1, UD_R_DR2, UD_R_DR3,
	UD_R_DR4, UD_R_DR5, UD_R_DR6, UD_R_DR7,
	UD_R_DR8, UD_R_DR9, UD_R_DR10, UD_R_DR11,
	UD_R_DR12, UD_R_DR13, UD_R_DR14, UD_R_DR15,

	/* mmx registers */
	UD_R_MM0, UD_R_MM1, UD_R_MM2, UD_R_MM3,
	UD_R_MM4, UD_R_MM5, UD_R_MM6, UD_R_MM7,

	/* x87 registers */
	UD_R_ST0, UD_R_ST1, UD_R_ST2, UD_R_ST3,
	UD_R_ST4, UD_R_ST5, UD_R_ST6, UD_R_ST7,

	/* extended multimedia registers */
	UD_R_XMM0, UD_R_XMM1, UD_R_XMM2, UD_R_XMM3,
	UD_R_XMM4, UD_R_XMM5, UD_R_XMM6, UD_R_XMM7,
	UD_R_XMM8, UD_R_XMM9, UD_R_XMM10, UD_R_XMM11,
	UD_R_XMM12, UD_R_XMM13, UD_R_XMM14, UD_R_XMM15,

	UD_R_RIP,

	/* Operand Types */
	UD_OP_REG, UD_OP_MEM, UD_OP_PTR, UD_OP_IMM,
	UD_OP_JIMM, UD_OP_CONST
};

#include "itab.h"

union ud_lval {
	int8_t     sbyte;
	uint8_t    ubyte;
	int16_t    sword;
	uint16_t   uword;
	int32_t    sdword;
	uint32_t   udword;
	int64_t    sqword;
	uint64_t   uqword;
	struct {
		uint16_t seg;
		uint32_t off;
	} ptr;
};
struct ud_operand_data_t {
	enum ud_type    type;
	uint8_t         size;
	enum ud_type    base;
	enum ud_type    index;
	uint8_t         scale;
	uint8_t         offset;
	union ud_lval   lval;
};
/* -----------------------------------------------------------------------------
 * struct ud_operand - Disassembled instruction Operand.
 * -----------------------------------------------------------------------------
 */
struct ud_operand : public ud_operand_data_t {

	/*
	 * internal use only
	 */
	uint64_t        _legacy; /* this will be removed in 1.8 */
	uint8_t         _oprcode;
};
#define UD_VENDOR_AMD         0
#define UD_VENDOR_INTEL       1
#define UD_VENDOR_ANY         2
/* -----------------------------------------------------------------------------
 * struct ud - The udis86 object.
 * -----------------------------------------------------------------------------
 */
struct ud
{
	/*
	 * input buffering
	 */

#ifndef SPECIALIZE_FOR_ETERNAL
	int       (*inp_hook) (struct ud*);
#ifndef __UD_STANDALONE__
	FILE* inp_file;
#endif
#endif
	const uint8_t* inp_buf;
	struct ud_itab_entry* itab_entry;
	struct ud_lookup_table_list_entry* le;
#ifndef SPECIALIZE_FOR_ETERNAL
	size_t    inp_buf_size;
#endif
	size_t    inp_buf_index;
	udstate_uint8_fast_t   inp_curr;
	size_t    inp_ctr;

#ifndef SPECIALIZE_FOR_ETERNAL
	uint8_t   inp_sess[64];
	int       inp_end;

#else
	static constexpr int inp_end = 0;
#endif
	uint64_t  insn_offset;




	/*
	 * Symbol resolver for use in the translation phase.
	 */
#ifndef SPECIALIZE_FOR_ETERNAL
	const char* (*sym_resolver)(struct ud*, uint64_t addr, int64_t* offset);
#else
	static constexpr const char* (*const sym_resolver)(struct ud*, uint64_t addr, int64_t* offset) = nullptr;
#endif
#ifndef FORCE_64BIT_MODE
	uint8_t   dis_mode;
#else
	static constexpr unsigned dis_mode = 64;
#endif
	uint64_t  pc;
#ifndef FORCE_INTEL_VENDOR
	uint8_t   vendor;
#else
	static constexpr unsigned vendor = UD_VENDOR_INTEL;
#endif
	enum ud_mnemonic_code mnemonic;
	struct ud_operand operand[3];
	udstate_uint8_fast_t   error;
	udstate_uint8_fast_t   pfx_rex;
	udstate_uint8_fast_t pfx_reg_gpr_flag; //2 or 0
	udstate_uint8_fast_t   pfx_seg;
	udstate_uint8_fast_t   pfx_opr;
	udstate_uint8_fast_t   pfx_adr;
	udstate_uint8_fast_t   pfx_lock;
	udstate_uint8_fast_t   pfx_str;
	udstate_uint8_fast_t   pfx_rep;
	udstate_uint8_fast_t   pfx_repe;
	udstate_uint8_fast_t   pfx_repne;
	udstate_uint8_fast_t   opr_mode;
	udstate_uint8_fast_t   adr_mode;
	udstate_uint8_fast_t   br_far;
	udstate_uint8_fast_t   br_near;
	udstate_uint8_fast_t   have_modrm;
	udstate_uint8_fast_t   modrm;
	udstate_uint8_fast_t   primary_opcode;



	inline void set_opr_mode(unsigned mode) {
		opr_mode = mode;
	}
#ifndef SPECIALIZE_FOR_ETERNAL
	void* user_opaque_data;
#endif

	/*
	 * Assembly output buffer
	 */
	char* asm_buf;
	size_t    asm_buf_size;
	size_t    asm_buf_fill;
	char      asm_buf_int[128];
	void      (*translator)(struct ud*);
	char      insn_hexcode[64];
};

/* -----------------------------------------------------------------------------
 * Type-definitions
 * -----------------------------------------------------------------------------
 */
typedef enum ud_type          ud_type_t;
typedef enum ud_mnemonic_code ud_mnemonic_code_t;

typedef struct ud             ud_t;
typedef struct ud_operand     ud_operand_t;

#define UD_SYN_INTEL          ud_translate_intel
#define UD_SYN_ATT            ud_translate_att
#define UD_EOI                (-1)
#define UD_INP_CACHE_SZ       32


#endif

/*
vim: set ts=2 sw=2 expandtab
*/
