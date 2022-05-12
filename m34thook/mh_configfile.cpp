#include "mh_defs.hpp"
#include "mh_configfile.hpp"
#include <Windows.h>
#include <Shlobj.h> 
#include <string>
#include <filesystem>
#include "mhutil/mh_filesystem.hpp"
std::string get_mh_file_path(bool* success, const char* subpath) {
	char path[MAX_PATH];
	if (!SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_PROFILE, NULL, 0, path))) {
		*success = false;
		return "";
	}

	std::filesystem::path p = path;
	p.concat("/meathook/");


	if (!std::filesystem::is_directory(p)) {
		std::filesystem::create_directory(p);
	}
	if (subpath)
		p.concat(subpath);
	*success = true;


	return p.string();
}



struct token_indices_t {
	unsigned m_start_of_key;
	unsigned m_end_of_key;
	unsigned m_start_of_value;
	unsigned m_end_of_value;
};

static bool skip_white(const char* s, unsigned* index) {


	unsigned local_index = *index;

	while (true) {
		unsigned currchar = (unsigned char)s[local_index];
		if (currchar == '\n' || !currchar) {
			*index = local_index;
			return false; //malformed
		}
		if (currchar == ' ' || currchar == '\t' || currchar == '\r') {
			++local_index;
		}
		else {
			*index = local_index;
			return true;
		}
	}

}
//extended ascii, support special characters that appear in spanish, french, german, polish
//but not greek, too much work
static bool is_european_char(unsigned c) {
	return c == 'Ç' || c == 'ç'; //todo:add umlaut, accented vowels, the nino thingy
}

static bool is_identifier_char(unsigned c) {

	return
		(c >= ((unsigned)'a') && c <= ((unsigned)'z'))
		| (c >= ((unsigned)'A') && c <= ((unsigned)'Z'))
		| ((c - (unsigned)'0') <= 9) //allow numbers in identifiers
		| (c == (unsigned)'_');
}
static bool consume_identifier(const char* s, unsigned* index) {


	unsigned local_index = *index;

	while (true) {

		unsigned currchar = (unsigned char)s[local_index];

		
		if (!is_identifier_char(currchar)) {

			

			*index = local_index;
			return currchar != 0;
		}
		else {
			++local_index;

		}

	}

}

static bool consume_any_but_lineend(const char* s, unsigned* position) {

	unsigned local_position = *position;

	while (true) {

		unsigned currchar = (unsigned char)s[local_position];

		if ((currchar == '\n') | (currchar == 0)) {
			*position = local_position;
			return true;
		}
		else {
			++local_position;
		}
	}
}

static bool mh_config_file_find_token_indices(const char* s, token_indices_t* indices, unsigned* inout_position) 
{
	unsigned position = *inout_position;
	if (!skip_white(s, &position)) {
		return false;
	}


	
	unsigned start_identifier = position;


	if (!consume_identifier(s, &position)) {
		return false;
	}

	unsigned end_identifier = position;

	//skip whitespace, if any, between identifier and = 

	if (!skip_white(s, &position)) {
		return false;
	}

	if (s[position] != '=') {
		return false;
	}

	++position;

	//skip whitespace, if any, between = and value

	if (!skip_white(s, &position)) {
		return false;
	}

	unsigned value_start = position;


	if (!consume_any_but_lineend(s, &position)) {
		return false;
	}

	unsigned value_end = position;


	indices->m_start_of_key = start_identifier;
	indices->m_end_of_key = end_identifier;
	indices->m_start_of_value = value_start;

	if (s[value_end] == '\r') {
		indices->m_end_of_value = value_end - 1;
	}
	else {
		indices->m_end_of_value = value_end;
	}

	if (s[position] == 0) {


	}
	else { //advance past newline
		
		position++;

	}

	*inout_position = position;
	return true;
}


static char g_configvar_string_mem[MAX_CONFIGVAR_STRING_MEM];

static unsigned g_configvar_string_mem_pos = 0;

static void mh_configvar_set_from_span(const char* s, token_indices_t* indices, const mh_configvar_header_t* cfg) {

	switch (cfg->m_type) {
	case mh_configvar_type_t::INTEGER64:
	{
		char* endptr = nullptr;
		long long v = strtoll(&s[indices->m_start_of_value], &endptr, 10);

		cfg->set_payload(v);
	

		return;
	}

	case mh_configvar_type_t::FLOAT64: {
		char* endptr = nullptr;
		double result = strtod(&s[indices->m_start_of_value], &endptr);
		cfg->set_payload(result);

		return;
	}
	case mh_configvar_type_t::STRING_VAL: {
		

		unsigned length = (indices->m_end_of_value - indices->m_start_of_value) + 1;

		char* strpos = &g_configvar_string_mem[g_configvar_string_mem_pos];

		g_configvar_string_mem_pos += length;
		cs_assert(g_configvar_string_mem_pos < MAX_CONFIGVAR_STRING_MEM);
			
		
		memset(strpos, 0, length);
		memcpy(strpos, &s[indices->m_start_of_value], length - 1);
		cfg->set_payload(strpos);

		return;
		


	}
	}
	
}

static const mh_configvar_header_t* mh_select_cfgvar_for_key(
	const char* s,
	token_indices_t* indices,
	const mh_configvar_header_t* cfgarr,
	unsigned nconfigvars
) {

	unsigned key_token_length = indices->m_end_of_key - indices->m_start_of_key;
	for (unsigned i = 0; i < nconfigvars; ++i) {

		if (cfgarr[i].m_keylength == key_token_length) {

			if (!strncmp(cfgarr[i].m_key, &s[indices->m_start_of_key], key_token_length)) {
				return cfgarr + i;
			}
		}
	}
	return nullptr;
}

static bool mh_configline_process(const char* s, unsigned* inout_position, const mh_configvar_header_t* cfgarr, unsigned nconfigvars) {
	unsigned pos = *inout_position;
	token_indices_t indices;
	if (!mh_config_file_find_token_indices(s, &indices, &pos)) {
		return false;
	}

	const mh_configvar_header_t* var_to_set = mh_select_cfgvar_for_key(s, &indices, cfgarr, nconfigvars);

	cs_assert(var_to_set);

	mh_configvar_set_from_span(s, &indices, var_to_set);

	*inout_position = pos;

	return true;

}


void mh_configfile_load(
	const mh_configvar_header_t* cfgarr, 
	unsigned nconfigvars
) {
	bool success = false;
	std::string path_to_config = get_mh_file_path(&success, "config.txt");

	if (!success) {
		return;
	}

	unsigned cfg_file_size = 0;
	void* cfg_file_contents = filesys::get_file_contents(path_to_config.c_str(), &cfg_file_size);

	if (!cfg_file_contents) {


		return;
	}

	unsigned pos = 0;
	while (mh_configline_process((const char*)cfg_file_contents, &pos, cfgarr, nconfigvars)) {

	}

	filesys::free_file_contents(cfg_file_contents);
}