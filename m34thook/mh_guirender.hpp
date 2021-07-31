#pragma once

class mh_dom_t;

namespace mh_gui {
	void install_gui_hooks();

	void show_test_gui();

	mh_dom_t* new_named_dom(const char* name);

}