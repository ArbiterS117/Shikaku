#pragma once
#include "main.h"
#define ANIMATION_CURVE_POINT_COUNT		(10)

enum UIAnimationType
{
	TransForm,
	Scaling,
};

enum UIAnimationMoveType
{
	Line,
	Curve,
};

struct UIAnimation
{
	D3DXVECTOR3 stpos;
	D3DXVECTOR3 edpos;
	D3DXVECTOR3 cp1pos;
	D3DXVECTOR3 cp2pos;

	D3DXVECTOR3 workpos1;
	D3DXVECTOR3 workpos2;

	D3DXVECTOR3 workscale1;
	D3DXVECTOR3 workscale2;

	float time;
	float worktime;
	float timestep;
	bool hasanimation;
	bool loop;

	UIAnimationType at;
	UIAnimationMoveType amt;
	D3DXVECTOR3 curve[ANIMATION_CURVE_POINT_COUNT];
};