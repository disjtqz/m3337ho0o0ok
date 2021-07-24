#include "mh_defs.hpp"

#include "game_exe_interface.hpp"
#include "doomoffs.hpp"
#include "meathook.h"
#include "cmdsystem.hpp"
#include "idtypeinfo.hpp"
#include "eventdef.hpp"
#include "scanner_core.hpp"
#include "idLib.hpp"
#include "idStr.hpp"
#include "idmath.hpp"
#include <cassert>
#include <stdlib.h>
#include <stdio.h>
#include "gameapi.hpp"
#define     GET_VERTEXCOLOR()       *mh_lea<unsigned>(this, RENDERMODELGUI_VERTEXCOLOR_OFFSET)
void idRenderModelGui::DrawFilled(const idColor& color, float x, float y, float w, float h) {

	unsigned old_color = GET_VERTEXCOLOR();

	GET_VERTEXCOLOR() = color.PackColor();

	void* whitemat = get_material("_white");
	DrawStretchPic(x, y, 0.0, w, h, 0.0, 0.0, 1.0, 1.0, whitemat);
	GET_VERTEXCOLOR() = old_color;
}
void idRenderModelGui::DrawStretchPic(
	float x,
	float y,
	float z,
	float w,
	float h,
	float s1,
	float t1,
	float s2,
	float t2,
	const void* material) {
	call_as<void>(descan::g_idRenderModelGui__DrawStretchPic, this, x, y, z, w, h, s1, t1, s2, t2, material);
}
idDrawVert* idRenderModelGui::AllocTris(int numVerts, unsigned short* indexes, int numIndexes, void* material) {
	return call_as<idDrawVert*>(descan::g_idRenderModelGui_AllocTris, this, numVerts, indexes, numIndexes, material);
}

void idRenderModelGui::DrawChar(float x, float y, int character, float scale) {
	call_as<void>(descan::g_idRenderModelGui__DrawChar, this, x, y, character, scale);
}
void idRenderModelGui::DrawString(
	float x,
	float y,
	const char* string,
	const idColor* defaultColor,
	bool forceColor,
	const float scale) {
	call_as<void>(descan::g_idRenderModelGui__DrawString, this, x, y, string, defaultColor, forceColor, scale);
}