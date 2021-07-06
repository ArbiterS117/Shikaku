//=============================================================================
//
// プラットフォーム処理 [platform.h]
// Author : GP11B132 14 ゴショウケン
//
//=============================================================================
#pragma once

#include "main.h"
#include "model.h"

enum enumPlatformType
{
	PlatformType_normal,
	PlatformType_ice,
	PlatformType_t1,
	PlatformType_t2,
	PlatformType_t3,
	PlatformType_block,
	PlatformType_pyramid,
	PlatformType_bigTree,
	PlatformType_bigSnowMan,
	PlatformType_mountain,
	PlatformType_t4,
	PlatformType_CurveG,
	PlatformType_grassBlock,
	PlatformType_hill,
	PlatformType_pokerBlock,
	PlatformType_slider,
	PlatformType_snowMountain,
	PlatformType_MAX,
};

static D3DXVECTOR3 PlatformColliderSize[PlatformType_MAX] = {
	{18.5f, 11.0f, 18.5f},    // normal
	{20.0f, 20.0f, 20.0f},    // ice
	{50.0f, 50.0f, 50.0f},    // tree1
	{50.0f, 50.0f, 50.0f},    // tree2
	{10.0f, 30.0f, 10.0f},    // tree3
	{20.0f, 20.0f, 20.0f},    // block
	{150.0f, 150.0f, 150.0f}, // pyramid
	{150.0f, 150.0f, 150.0f}, // bigTree
	{200.0f, 200.0f, 200.0f}, // bigSnowman
	{50.0f, 50.0f, 50.0f},    // mountain
	{50.0f, 50.0f, 50.0f},    // t4
	{80.0f, 80.0f, 80.0f},    // CurveG
	{20.0f, 20.0f, 20.0f},    // grassBlock
	{30.0f, 30.0f, 30.0f},    // hill
	{20.0f, 20.0f, 20.0f},    // pokerBlock
	{150.0f, 150.0f, 150.0f}, // slider
	{250.0f, 250.0f, 250.0f}, // snowMountain
};

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_PLATFORM		(256)					// プレイヤーの数

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct PLATFORM
{
	bool		    Use;			        // 使用しているかどうか
	bool            canMove;
	bool            canSpin;

	int             SPID;                   // the "ONLY ONE" ID : manual setting (手動設定) 0 = default : can not use for searching  """""""under constuction""""""
	int             platformID;
	int             colliderIdx;            // colliderのインデックス番号
	int             CircleRangeColliderIdx; //  CircleRangeColliderのインデックス番号
	int             platformType;
	int             canmoveAxis;            // 0: X  1: Y  2: Z  -1: none
	int             canspinAxis;            // 0: X  1: Y  2: Z  -1: none
	int             scaleAxis;              // 0: X  1: Y  2: Z  -1: all
	int             colorType;              // 0: red  1:white  2:black

	float           maxMoveSpeed;
	float           decelerate;
	float           LMtime;                 //Liner move time 線形移動

	D3DXMATRIX		mtxWorld;			// ワールドマトリックス

	D3DXVECTOR3		pos;				    // モデルの位置
	D3DXVECTOR3		rot;				    // モデルの向き(回転)
	D3DXVECTOR3		scl;				    // モデルの大きさ(スケール)
	D3DXVECTOR3     speed;                  // 移動スピード
	D3DXVECTOR3     CaculatedPos;

	DX11_MODEL		model;				// モデル情報
	D3DXCOLOR       color;

};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitPlatform(void);
void UninitPlatform(void);
void UpdatePlatform(void);
void DrawPlatform(void);

PLATFORM *GetPlatform(void);

// Replace
void RebuildAllPlatformAfterLoad();

//New
int SetPlatform(D3DXVECTOR3 pos, D3DXVECTOR3 rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f),
	D3DXVECTOR3 scl = D3DXVECTOR3(0.0f, 0.0f, 0.0f), int PlatformType = PlatformType_normal);

int SetPlatformToID(int num, DX11_MODEL model, D3DXVECTOR3 pos, D3DXVECTOR3 rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f),
	D3DXVECTOR3 scl = D3DXVECTOR3(0.0f, 0.0f, 0.0f), int PlatformType = PlatformType_normal,
	D3DXCOLOR clr = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

void ReleasePlatform(int id);

void UpdatePlatformModel(int id);
