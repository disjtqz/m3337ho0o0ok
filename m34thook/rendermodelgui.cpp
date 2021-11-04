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
#include "snaphakalgo.hpp"

#define		SMALLCHAR_WIDTH_SCALEFACTOR		1.0f
#define		SMALLCHAR_HEIGHT_SCALEFACTOR	1.0f
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

void idRenderModelGui::DrawStretchPic(
	const idVec4* topLeft,
	const idVec4* topRight,
	const idVec4* bottomRight,
	const idVec4* bottomLeft,
	const void* material,
	float z) {
	call_as<void>(descan::g_idRenderModelGui__DrawStretchPicVec4Version, this, topLeft, topRight, bottomRight, bottomLeft, material, z);

}
MH_NOINLINE
void idRenderModelGui::DrawRectMaterial(
	float xstart, float ystart, float width, float height, const char* material
) {

	void* mtr = get_material(material);

	MH_UNLIKELY_IF(!mtr) {
		sh::fatal("Failed to find material %s in DrawRectMaterial!", material);
	}
	DrawRectMaterial(xstart, ystart, width, height, mtr);
}
MH_NOINLINE
void idRenderModelGui::DrawRectMaterial(
	float xstart, float ystart, float width, float height, void* material
) {

	idVec4 topl, topr, botr, botl;

	topl.x = xstart;
	topl.y = ystart;
	topl.z = .0f;
	topl.w = .0f;
	auto make_texcoords = [xstart, ystart, width, height](idVec4& v) {
		v.z = (v.x - xstart) / width;
		v.w = (v.y - ystart) / height;
	};

	make_texcoords(topl);

	topr.x = xstart + width;
	topr.y = ystart;
	topr.z = .0f;
	topr.w = .0f;
	make_texcoords(topr);
	botr.x = xstart + width;
	botr.y = ystart + height;
	botr.z = .0f;
	botr.w = .0f;
	make_texcoords(botr);
	botl.x = xstart;
	botl.y = ystart + height;
	botl.z = .0f;
	botl.w = .0f;
	make_texcoords(botl);
	DrawStretchPic(&topl, &topr, &botr, &botl, material, 1.0f);
}

void idRenderModelGui::set_current_vertexcolor(unsigned vcolor) {
	GET_VERTEXCOLOR() = vcolor;
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

void idRenderModelGui::SetViewport(int x,
    int y,
    int width,
    int height) {
	call_as<void>(descan::g_idRenderModelGui_SetViewPort, x, y, width, height);
}

#define		DEFAULT_SMALLCHAR_WIDTH		48.0f
#define		DEFAULT_SMALLCHAR_HEIGHT	48.0f
float idRenderModelGui::get_SMALLCHAR_WIDTH() {
	MH_UNLIKELY_IF(!descan::g_SMALLCHAR_WIDTH) {
		return DEFAULT_SMALLCHAR_WIDTH;
	}
	else {
		return *reinterpret_cast<float*>(descan::g_SMALLCHAR_WIDTH) * SMALLCHAR_WIDTH_SCALEFACTOR;
	}
}

float idRenderModelGui::get_SMALLCHAR_HEIGHT(){
	MH_UNLIKELY_IF(!descan::g_SMALLCHAR_HEIGHT) {
		return DEFAULT_SMALLCHAR_HEIGHT;
	}
	else {
		return *reinterpret_cast<float*>(descan::g_SMALLCHAR_HEIGHT) * SMALLCHAR_HEIGHT_SCALEFACTOR;
	}
}


float idRenderModelGui::GetVirtualWidth() {

	return call_as<float>(descan::g_idRenderModelGui_GetVirtualWidth, this);

}
float idRenderModelGui::GetVirtualHeight() {
	return call_as<float>(descan::g_idRenderModelGui_GetVirtualHeight, this);
}

unsigned  idRenderModelGui::GetStringWidth(
	const char* string,
	const float scale)
{
	int i; // er8
	char v4; // cl
	__int64 v5; // rax

	for (i = 0; *string; string += v5)
	{
		if (*string == 94 && ((v4 = string[1], (unsigned __int8)(v4 - 48) <= 9u) || (unsigned __int8)(v4 - 97) <= 3u))
		{
			v5 = 2i64;
		}
		else
		{
			++i;
			v5 = 1i64;
		}
	}
	return (unsigned int)(int)(float)((float)((float)i * get_SMALLCHAR_WIDTH()) * scale);
}