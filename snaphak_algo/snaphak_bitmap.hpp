#pragma once
/*
	todo: _mm512_lzcnt_epi64 might be pretty handy (AVX512CD)
*/

struct snaphak_bmproutines_t {
	bool (*m_bitmap_bits_are_set)(unsigned* bits, unsigned nbits_allocated_for, unsigned startidx, unsigned length);
	bool (*m_bitmap_bits_are_clear)(unsigned* bits, unsigned nbits_allocated_for, unsigned startidx, unsigned length);
	void (*m_bitmap_clear_bits)(unsigned* bits, unsigned nbits_allocated_for, unsigned startidx, unsigned ntoclear);
	unsigned (*m_bitmap_find_n_clear_from)(unsigned* bits, unsigned nbits_allocated_for, unsigned ntofind, unsigned start);
	unsigned (*m_bitmap_find_n_clear_from_and_set)(unsigned* bits, unsigned nbits_allocated_for, unsigned ntofind, unsigned start);

	void (*m_bitmap_set_bits)(unsigned* bits, unsigned nbits_allocated_for, unsigned startidx, unsigned ntoset);
	unsigned (*m_bitmap_find_n_set_from)(unsigned* bits, unsigned nbits_allocated_for, unsigned ntofind, unsigned start);
	unsigned (*m_bitmap_find_n_set_from_and_clear)(unsigned* bits, unsigned nbits_allocated_for, unsigned ntofind, unsigned start);
};