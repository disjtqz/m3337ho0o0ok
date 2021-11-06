#pragma once


class mh_editor_interface_t {
public:
	virtual void grab(void* entity) = 0;

	//grabs the entity the player is looking at
	virtual void grab_player_focus() = 0;
	virtual void ungrab() = 0;


};

mh_editor_interface_t* get_current_editor();