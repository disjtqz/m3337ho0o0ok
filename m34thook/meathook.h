#pragma once


void meathook_init();
/*
	* executes on first tick of idCommonLocal::Frame
	run late-stage scanners, and load plugins
*/
void meathook_final_init();

