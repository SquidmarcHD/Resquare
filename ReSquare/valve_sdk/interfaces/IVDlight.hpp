#pragma once
#include "../math/Vector.hpp"
struct ColorRGBExp32
{
	byte r, g, b;
	signed char exponent;
};
struct C_Dlight
{
	int		flags;
	Vector	origin;
	float	radius;
	ColorRGBExp32 color;
	float	die;
	float	decay;
	float	minlight;
	int		key;
	int		style;
	Vector	m_Direction;
	float	m_InnerAngle;
	float	m_OuterAngle;
	float GetRadius() const
	{
		return radius;
	}
	float GetRadiusSquared() const
	{
		return radius * radius;
	}
	float IsRadiusGreaterThanZero() const
	{
		return radius > 0.0f;
	}
};
class IVEffects
{
public:
	C_Dlight* CL_AllocDlight(int key)
	{
		typedef C_Dlight* (__thiscall* OriginalFn)(PVOID, int);
		return CallVFunction<OriginalFn>(this, 4)(this, key);
	}
	C_Dlight* CL_AllocElight(int key)
	{
		typedef C_Dlight* (__thiscall* OriginalFn)(PVOID, int);
		return CallVFunction<OriginalFn>(this, 5)(this, key);
	}
	C_Dlight* GetElightByKey(int key)
	{
		typedef C_Dlight* (__thiscall* OriginalFn)(PVOID, int);
		return CallVFunction<OriginalFn>(this, 8)(this, key);
	}
};