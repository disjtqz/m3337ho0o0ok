#pragma once


class mh_editor_interface_t {
public:
	virtual void grab(void* entity) = 0;

	//grabs the entity the player is looking at
	virtual void grab_player_focus() = 0;
	virtual void ungrab() = 0;
	
	virtual void editor_spawn_entitydef(void* entitydef) = 0;

	virtual void init_for_session() = 0;

	virtual bool is_initialized_for_sess() = 0;
	virtual void set_angle_increment(double inc) = 0;
};

mh_editor_interface_t* get_current_editor();