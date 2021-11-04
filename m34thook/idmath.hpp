#pragma once
struct idColor {
	float r, g, b, a;
	int PackColor() const
	{
		int v1; // edx
		unsigned __int8 v2; // al
		int v3; // er9
		int v4; // edx
		unsigned __int8 v5; // al
		int v6; // er8
		int v7; // edx
		unsigned __int8 v8; // al
		int v9; // edx
		int v10; // ecx
		unsigned __int8 v11; // al

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
		v3 = v2;
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
		v6 = v5;
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
		v9 = v8;
		v10 = (int)(float)(this->a * 255.0);
		if (v10 >= 0)
		{
			v11 = v10;
			if (v10 > 255)
				v11 = -1;
		}
		else
		{
			v11 = 0;
		}
		return v3 | ((v6 | ((v9 | (v11 << 8)) << 8)) << 8);
	}
};

struct idVec3 {
	float x, y, z;
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
	idVec3 xyz;
	idVec2 color;
	char normal[4];
	char tangent[4];
	char _color2[4];
	idVec2 _tangent;
	char materials[4];
	char unused[4];
	idVec2 _st;
	idVec2 _st2;
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

