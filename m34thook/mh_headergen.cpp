#include "mh_headergen.hpp"
#pragma comment(lib, "cabinet.lib")
#include "compressapi.h"
#define		PROPERTY_KEY_TABLE_COMPRESSION_ALGO		COMPRESS_ALGORITHM_LZMS

bvec_t pack_strset(strviewset_t& sset) {
	unsigned total_required_bytes = 0;
	for (auto&& chrs : sset) {

		total_required_bytes += chrs.length() + 1;


	}

	bvec_t bbuff{};
	bbuff.resize(total_required_bytes);

	unsigned writepos = 0;
	for (auto&& chrs : sset) {

		memcpy(&bbuff[writepos], chrs.data(), chrs.length() + 1);
		writepos += chrs.length() + 1;

	}

	return bbuff;
}
bvec_t compress_packet_strset(bvec_t& pss) {
	COMPRESSOR_HANDLE comp;
	CreateCompressor(PROPERTY_KEY_TABLE_COMPRESSION_ALGO, nullptr, &comp);


	bvec_t compout{};
	compout.resize(pss.size() * 2);
	SIZE_T decompres;
	Compress(comp, (LPCVOID)pss.data(), pss.size(), compout.data(), compout.size(), &decompres);

	CloseCompressor(comp);

	compout.resize(decompres);
	return compout;
}


void write_cfile(std::string txt, const char* name) {

	FILE* outf;
	fopen_s(&outf, name, "wb");



	/*for (auto&& name : allprops) {

		fprintf(outf, "%s\n", name.data());
	}
	*/

	fwrite(txt.data(), txt.length(), 1, outf);
	fclose(outf);

}

std::string expand_bytes_to_c_bytearray(bvec_t bvals) {

	std::string result = std::to_string(bvals[0]);

	for (unsigned i = 1; i < bvals.size(); ++i) {

		result += ",";
		result += std::to_string(bvals[i]);
	}
	return result;
}

bvec_t decompress_strset(unsigned char* values, unsigned numvalues, unsigned expected_decompress) {

	DECOMPRESSOR_HANDLE decomp;
	CreateDecompressor(PROPERTY_KEY_TABLE_COMPRESSION_ALGO, nullptr, &decomp);

	bvec_t decompdata;
	decompdata.resize(expected_decompress);

	SIZE_T decompressed_size;
	Decompress(decomp, values, numvalues, decompdata.data(), expected_decompress, &decompressed_size);


	CloseDecompressor(decomp);
	return decompdata;
}

strviewset_t unpack_strset(bvec_t& bvals, unsigned numstrs) {

	strviewset_t decompressed_set;

	const char* consumepos = (const char*)(bvals.data());
	for (unsigned i = 0; i < numstrs; ++i) {

		std::string_view current = consumepos;

		consumepos += current.length() + 1;
		decompressed_set.emplace_hint(decompressed_set.end(), std::move(current));


	}
	return decompressed_set;
}