#pragma once

template<typename TTemp = void>
struct qstrviewset_t {

	struct strviewnode_t {
		std::string_view m_str;
		rb_node m_srchnode;

	};

	rb_root m_srchroot;

	
	strviewnode_t* find_node(std::string_view sv, sh::rb::insert_hint_t* out_hint) {

		return sh::rb::rbnode_find< strviewnode_t, cs_offsetof_m(strviewnode_t, m_srchnode), std::string_view>(&m_srchroot, sv, [](strviewnode_t* svn, std::string_view svo) {

			return sh::string::strcmp(svn->m_str.data(), svo.data());
			}, out_hint);

	}

};