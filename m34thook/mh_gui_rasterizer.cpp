//yes, i know. technically by definition this isnt rasterization, the engine/the gpu driver is doing the actual rasterization
//i like the word, okay?

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

#include "mh_guirender.hpp"

#include "mhgui.hpp"
#include "GLState.hpp"
#include <mutex>
#include "mh_gui_rasterizer.hpp"

static idDrawVert push_center_into_edge(double centerx, double centery, double radius, double rads) {
	double sine, cosine;
	sh::math::sincos(rads, sine, cosine);

	double xlatx = cosine * radius;
	double xlaty = sine * radius;

	xlatx += centerx;
	xlaty += centery;

	idDrawVert result;
	result.pos.x = xlatx;
	result.pos.y = xlaty;

	return result;
}

void make_partial_triangle_fan(
	mh_ui_vector_t<idDrawVert>* verts,
	mh_ui_vector_t<unsigned short>* indices,
	float centerx,
	float centery,
	float radius,
	unsigned numsegm,
	mh_color_t color) {


	idDrawVert dvcenter;
	dvcenter.pos.x = centerx;
	dvcenter.pos.y = centery;
	dvcenter.set_color(color);

	verts->push_back(dvcenter);


	double radianstep = (sh::math::PI * 2.0) / (double)numsegm;
	double currrad = .0;



	for (unsigned i = 0; i < numsegm; ++i) {

		idDrawVert pt = push_center_into_edge(centerx, centery, radius, currrad);
		if (i & 1) {
			indices->push_back(verts->size());

		}
		else {
			indices->push_back(0);
			indices->push_back(verts->size());

		}
		pt.set_color(color);
		verts->push_back(pt);
		currrad += radianstep;
	}



}

void make_circle(
	mh_ui_vector_t<idDrawVert>* verts,
	mh_ui_vector_t<unsigned short>* indices,
	float centerx,
	float centery,
	float radius,
	unsigned numsegm,
	mh_color_t color) {


	idDrawVert dvcenter;
	dvcenter.pos.x = centerx;
	dvcenter.pos.y = centery;
	dvcenter.set_color(color);

	verts->push_back(dvcenter);


	double radianstep = (sh::math::PI * 2.0) / (double)numsegm;
	double currrad = .0;



	for (unsigned i = 0; i < numsegm; ++i) {

		idDrawVert pt = push_center_into_edge(centerx, centery, radius, currrad);

		indices->push_back(0);
		indices->push_back(verts->size());
		//next vert or first vert. add one to skip the center vert if we're wrapping around

		//yucky modulus, could use magu
		indices->push_back(((i + 1) % (numsegm)) + 1);

		pt.set_color(color);
		verts->push_back(pt);
		currrad += radianstep;
	}



}

void submit_rect(mh_uigeo_builder_t* ub, float upperx, float uppery, float width, float height) {

	idDrawVert upl, upr, bl, br;

	unsigned i = ub->idxbase();
	upl.pos.x = upperx;
	upl.pos.y = uppery;

	upr = upl;
	upr.pos.x += width;


	bl.pos.x = upperx;
	bl.pos.y = uppery + height;

	br = bl;
	br.pos.x += width;

	auto triul = ub->addvert(upl), triur = ub->addvert(upr), trill = ub->addvert(bl), trilr = ub->addvert(br);


	ub->addidx(triul, triur, trill, trill, trilr, triur);
}
static  float useScale = 1.0f, oneOverWidth = 1.0f, oneOverHeight = 1.0f;
static  float unk_144F39260 = 1.0f;//this one actually doesnt seem constant

/*
void __fastcall idRenderModelGui::DrawChar(idRenderModelGui *this, float x, float y, int character, float scale)
{
  float v7; // xmm15_4
  glyphInfo_t *Glyph; // rax
  float v9; // xmm8_4
  float v10; // xmm13_4
  float s1; // xmm14_4
  float t1; // xmm11_4
  float v13; // xmm12_4
  float s2; // xmm10_4
  float t2; // xmm9_4
  float v16; // xmm6_4
  float v17; // xmm7_4
  const idMaterial *material; // rax

  if ( __PAIR64__(
		 idFont::GetMaterialHeight((idFont *)monospaceFont.r),
		 idFont::GetMaterialWidth((idFont *)monospaceFont.r)) != __PAIR64__(fontHeight, fontWidth) )
	idRenderModelGui::SetupMonospaceFont(this, SMALLCHAR_WIDTH);
  v7 = y + SMALLCHAR_HEIGHT;
  Glyph = idFont::GetGlyph((idFont *)monospaceFont.r, character);
  v9 = useScale * scale;
  v10 = (float)Glyph->width + 1.0;
  s1 = (float)((float)Glyph->s - 0.5) * oneOverWidth;
  t1 = (float)((float)Glyph->t - 0.5) * oneOverHeight;
  v13 = (float)Glyph->height + 1.0;
  s2 = (float)((float)(Glyph->s + Glyph->width) + 0.5) * oneOverWidth;
  t2 = (float)((float)(Glyph->t + Glyph->height) + 0.5) * oneOverHeight;
  v16 = (float)Glyph->top * (float)(useScale * scale);
  v17 = (float)((float)((float)((float)(SMALLCHAR_WIDTH / useScale) - (float)Glyph->xSkip) * 0.5) + (float)Glyph->left)
	  * (float)(useScale * scale);
  material = idFont::GetMaterial((idFont *)monospaceFont.r);
  idRenderModelGui::DrawStretchPic(this, v17 + x, v7 - v16, 0.0, v9 * v10, v9 * v13, s1, t1, s2, t2, material);
}
*/
static unsigned int fontHeight = 1;
static unsigned int fontWidth = 1;
float SMALLCHAR_WIDTH = 10.0f;
float SMALLCHAR_HEIGHT;
static void compute_glyphinfo(glyphInfo_t* Glyph, 
	/*float& x,
	float& y,
	float& w,
	float& h,*/
	float& s1,
	float&t1,
	float& s2,
	float& t2,
	float scale = 1.0f
) {
	float v10 = useScale * scale;
	s1 = (float)((float)Glyph->s - 0.5) * oneOverWidth;
	t1 = (float)((float)Glyph->t - 0.5) * oneOverHeight;
	s2 = (float)((float)(Glyph->s + Glyph->width) + 0.5) * oneOverWidth;
	float v14 = (float)Glyph->width + 1.0;
	t2 = (float)((float)(Glyph->t + Glyph->height) + 0.5) * oneOverHeight;
	float v16 = (float)Glyph->height + 1.0;
	float v17 = (float)((float)((float)((float)(useScale * SMALLCHAR_WIDTH) - (float)Glyph->xSkip) * 0.5) + (float)Glyph->left)
		* (float)(useScale * scale);
	float v18 = (float)Glyph->top * (float)(useScale * scale);

	
//	material = (const idMaterial*)idFont::GetMaterial(debugGUIFont);
}
//this seems to be the same in all versions
static __int64  idFont_GetPointSize(__int64 a1)
{
	__int64 i; // rax
	__int64 result; // rax

	for (i = *(__int64*)(a1 + 88); i; i = *(__int64*)(i + 88))
		a1 = i;
	result = *(__int64*)(a1 + 96);
	if (result)
		return (unsigned int)*(__int16*)(result + 28);
	return result;
}

static void* g_dbgfont = nullptr;
void* g_dbgfont_mtr = nullptr;

static glyphInfo_t g_glyphcache[128];
void ensure_text_rasterization_initialized() {
	MH_UNLIKELY_IF(!g_dbgfont)
		g_dbgfont = get_debuggui_font();

	unsigned mtrwidth = call_as<unsigned>(descan::g_idFont_GetMaterialWidth, g_dbgfont);
	unsigned mtrheight = call_as<unsigned>(descan::g_idFont_GetMaterialHeight, g_dbgfont);
	MH_UNLIKELY_IF (mtrheight != fontHeight || mtrwidth != fontWidth) {
		g_dbgfont_mtr = call_as<void*>(descan::g_idFont_GetMaterial, g_dbgfont);

		glyphInfo_t* Glyph; // rbx

		SMALLCHAR_WIDTH = SMALLCHAR_WIDTH;
		Glyph = call_as<glyphInfo_t*>(descan::g_idFont_GetGlyph, g_dbgfont, 0x41u); //idFont::GetGlyph((idFont*)monospaceFont.r, 0x41u);
		SMALLCHAR_HEIGHT = (float)((float)(int)idFont_GetPointSize((__int64)g_dbgfont) * SMALLCHAR_WIDTH)
			/ (float)Glyph->xSkip;
		useScale = SMALLCHAR_WIDTH / (float)Glyph->xSkip;
		fontWidth = mtrwidth;
		fontHeight = mtrheight;
		oneOverWidth = 1.0 / (float)(int)fontWidth;
		oneOverHeight = 1.0 / (float)(int)fontHeight;
#pragma clang loop unroll(disable)
		for (unsigned i = 0; i < 128; ++i) {
			g_glyphcache[i] = *call_as<glyphInfo_t*>(descan::g_idFont_GetGlyph, g_dbgfont, i);
		}
	}

}
void draw_char(mh_uigeo_builder_t* ub, unsigned character, float xpos, float ypos, mh_color_t color, float scale) {


	glyphInfo_t glyph = g_glyphcache[character];

	float s1, t1, s2, t2;

	
		
	float v10 = useScale * scale;
	s1 = (float)((float)glyph.s - 0.5) * oneOverWidth;
	t1 = (float)((float)glyph.t - 0.5) * oneOverHeight;
	s2 = (float)((float)(glyph.s + glyph.width) + 0.5) * oneOverWidth;
	float v14 = (float)glyph.width + 1.0;
	t2 = (float)((float)(glyph.t + glyph.height) + 0.5) * oneOverHeight;
	float v16 = (float)glyph.height + 1.0;
	float v17 = (float)((float)((float)((float)(useScale * SMALLCHAR_WIDTH) - (float)glyph.xSkip) * 0.5) + (float)glyph.left)
		* (float)(useScale * scale);
	float v18 = (float)glyph.top * (float)(useScale * scale);

	
	//compute_glyphinfo(&glyph, s1, t1, s2, t2, scale);

	float upperx = xpos;
	float uppery = ypos;
	float width = v10 * v14;
	float height = v10 * v16;

	idDrawVert upl, upr, bl, br;

	unsigned i = ub->idxbase();
	upl.pos.x = upperx;
	upl.pos.y = uppery;

	upl.materialUV.x = s1;
	upl.materialUV.y = t1;



	upr = upl;
	upr.pos.x += width;

	upr.materialUV.x = s2;

	

	bl.pos.x = upperx;
	bl.pos.y = uppery + height;


	bl.materialUV.x = s1;
	bl.materialUV.y = t2;


	br = bl;


	br.pos.x += width;

	br.materialUV.x = s2;
	br.materialUV.y = t2;
	upl.set_color(color);
	upr.set_color(color);
	bl.set_color(color);
	br.set_color(color);
	auto triul = ub->addvert(upl), triur = ub->addvert(upr), trill = ub->addvert(bl), trilr = ub->addvert(br);


	ub->addidx(triul, triur, trill, trill, trilr, triur);
}
void submit_line(mh_uigeo_builder_t* ub, float x1, float y1, float x2, float y2) {



	float slope = (y1 - y2) / (x1 - x2);


}