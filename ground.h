//=============================================================================
//
// ground���� [ground.h]
// Author : GP11B132 14 �S�V���E�P��
//
//=============================================================================
#pragma once
#include "model.h"

enum enumGroundType
{
	GroundType_HeartWorld,
	GroundType_HeartWorld_D,
	GroundType_HtoW,
	GroundType_HtoW_D,
	GroundType_Wonderland,
	GroundType_Wonderland2,
	GroundType_Wonderland_D,
	GroundType_WtoS,
	GroundType_SandWorld,
	GroundType_SandWorld_D,
	GroundType_StoS,
	GroundType_SnowWorld,
	GroundType_Sky1,
	GroundType_Sky2,
	GroundType_Sky3,
	GroundType_MAX,
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitGround(void);
void UninitGround(void);
void UpdateGround(void);
void DrawGround(void);

DX11_MODEL getGroundModel(int id);
D3DXVECTOR3 *getGroundNormal(int id);