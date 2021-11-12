#pragma once
#include <set>
#include <string_view>

#include <vector>
#include <string>

using strviewset_t = std::set<std::string_view>;
using bvec_t = std::vector<unsigned char>;

bvec_t pack_strset(strviewset_t& sset);
bvec_t compress_packet_strset(bvec_t& pss);
void write_cfile(std::string txt, const char* name);
std::string expand_bytes_to_c_bytearray(bvec_t bvals);


bvec_t decompress_strset(unsigned char* values, unsigned numvalues, unsigned expected_decompress);

strviewset_t unpack_strset(bvec_t& bvals, unsigned numstrs);