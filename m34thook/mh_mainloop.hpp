#pragma once

#define	MAX_FRAME_CALLBACKS		64

namespace mh_mainloop {
	using frame_cb_t = void (*)(void*);

	void install_mainloop_hooks();
	void add_preframe_callback(frame_cb_t cb, void* ud = nullptr);
	void add_postframe_callback(frame_cb_t cb, void* ud = nullptr);

	uint64_t num_ticks();
}