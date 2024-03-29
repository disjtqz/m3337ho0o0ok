#pragma once
struct idColor {
	float r, g, b, a;
	int PackColor() const
	{
		int v1; // edx
		unsigned __int8 v2; // al
		int r_packed; // er9
		int v4; // edx
		unsigned __int8 v5; // al
		int g_packed; // er8
		int v7; // edx
		unsigned __int8 v8; // al
		int b_packed; // edx
		int v10; // ecx
		unsigned __int8 a_packed; // al

		v1 = (int)(float)(this->r * 255.0);
		if (v1 >= 0)
		{
			v2 = (int)(float)(this->r * 255.0);
			if (v1 > 255)
				v2 = -1;
		}
		else
		{
			v2 = 0;
		}
		r_packed = v2;
		v4 = (int)(float)(this->g * 255.0);
		if (v4 >= 0)
		{
			v5 = (int)(float)(this->g * 255.0);
			if (v4 > 255)
				v5 = -1;
		}
		else
		{
			v5 = 0;
		}
		g_packed = v5;
		v7 = (int)(float)(this->b * 255.0);
		if (v7 >= 0)
		{
			v8 = (int)(float)(this->b * 255.0);
			if (v7 > 255)
				v8 = -1;
		}
		else
		{
			v8 = 0;
		}
		b_packed = v8;
		v10 = (int)(float)(this->a * 255.0);
		if (v10 >= 0)
		{
			a_packed = v10;
			if (v10 > 255)
				a_packed = -1;
		}
		else
		{
			a_packed = 0;
		}
		return r_packed | ((g_packed | ((b_packed | (a_packed << 8)) << 8)) << 8);
	}
};

struct idVec3 {
	float x, y, z;

	float Distance(idVec3& other) {
		
		float xx = x - other.x;
		float yy = y - other.y;
		float zz = z - other.z;
		xx *= xx;
		yy *= yy;
		zz *= zz;

		return sqrtf(xx + yy + zz);

	}

	void Set(float val) {
		x = val;
		y = val;
		z = val;
	}
};
struct idVec2 {
	float x, y;
};
struct idVec4 {
	float x, y, z, w;
};
struct  idAngles
{
	float pitch;
	float yaw;
	float roll;
};

struct  idDrawVert
{
	idVec3 pos;
	idVec2 lightmapUV;
	unsigned __int8 normal[4];
	unsigned __int8 tangent[4];
	unsigned __int8 color[4];
	idVec2 materialUV;
	unsigned __int16 materials[2];
	unsigned __int16 unused[2];
	//some of this is copied from d3bfg
	void idDrawVert::Clear()
	{
		this->pos.Set(0);
		*reinterpret_cast<uint64_t*>(&this->lightmapUV) = 0;
		*reinterpret_cast<uint64_t*>(&this->materialUV) = 0;
		*reinterpret_cast<unsigned*>(this->normal) = 0x00FF8080;	// x=0, y=0, z=1
		*reinterpret_cast<unsigned*>(this->tangent) = 0xFF8080FF;	// x=1, y=0, z=0
		*reinterpret_cast<unsigned*>(this->color) = 0;
	}
	idDrawVert() {
		Clear();
	}
	void set_color(idColor c) {
		*reinterpret_cast<int*>(&color[0]) = c.PackColor();
	}

	void set_color(mh_color_t mhcol) {
		*reinterpret_cast<int*>(&color[0]) = mhcol.m_color;
	}
};


struct idMat3 {
	idVec3 mat[3];
};
/*
extern	idColor colorBlack;
extern	idColor colorWhite;
extern	idColor colorRed;
extern	idColor colorGreen;
extern	idColor colorBlue;
extern	idColor colorYellow;
extern	idColor colorMagenta;
extern	idColor colorCyan;
extern	idColor colorOrange;
extern	idColor colorPurple;
extern	idColor colorPink;
extern	idColor colorBrown;
extern	idColor colorLtGrey;
extern	idColor colorMdGrey;
extern	idColor colorDkGrey;*/
static constexpr idColor 	colorBlack = idColor{ 0.00f, 0.00f, 0.00f, 1.00f };
static constexpr idColor 	colorWhite = idColor{ 1.00f, 1.00f, 1.00f, 1.00f };
static constexpr idColor 	colorRed = idColor{ 1.00f, 0.00f, 0.00f, 1.00f };
static constexpr idColor 	colorGreen = idColor{ 0.00f, 1.00f, 0.00f, 1.00f };
static constexpr idColor 	colorBlue = idColor{ 0.00f, 0.00f, 1.00f, 1.00f };
static constexpr idColor 	colorYellow = idColor{ 1.00f, 1.00f, 0.00f, 1.00f };
static constexpr idColor 	colorMagenta = idColor{ 1.00f, 0.00f, 1.00f, 1.00f };
static constexpr idColor 	colorCyan = idColor{ 0.00f, 1.00f, 1.00f, 1.00f };
static constexpr idColor 	colorOrange = idColor{ 1.00f, 0.50f, 0.00f, 1.00f };
static constexpr idColor 	colorPurple = idColor{ 0.60f, 0.00f, 0.60f, 1.00f };
static constexpr idColor 	colorPink = idColor{ 0.73f, 0.40f, 0.48f, 1.00f };
static constexpr idColor 	colorBrown = idColor{ 0.40f, 0.35f, 0.08f, 1.00f };
static constexpr idColor 	colorLtGrey = idColor{ 0.75f, 0.75f, 0.75f, 1.00f };
static constexpr idColor 	colorMdGrey = idColor{ 0.50f, 0.50f, 0.50f, 1.00f };
static constexpr idColor 	colorDkGrey = idColor{ 0.25f, 0.25f, 0.25f, 1.00f };

