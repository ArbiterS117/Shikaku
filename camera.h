//=============================================================================
//
// カメラ処理 [camera.h]
// Author : GP11B132 14 ゴショウケン
//
//=============================================================================
#pragma once


//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include <d3dx9.h>
#include "renderer.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
//階層アニメーション or else
enum MOIVE_LOOKAT_TYPE
{
	onePoint,
	twoPointMove,
	twoPointLookAt,
	twoPointMoveandLookAt,
	oneObject, // D3DXVECTOR3 at => no use
	twoObject, // D3DXVECTOR3 at => no use
};

struct MOIVE_TRANSFORM
{
	float frame;    // 実行フレーム数
	D3DXVECTOR3 pos;
	D3DXVECTOR3 at;
	MOIVE_LOOKAT_TYPE type;//look at direction but not "Point" when moving
};

struct CAMERA
{
	bool                TPMode;
	bool                canMove;
	bool                MovieMode;

	int                 TDN;            //階層アニメーション

	float               maxMoveSpeed;
	float               decelerate;
	float               move_time;      //階層アニメーション
	float				len;			// カメラの視点と注視点の距離

	D3DXMATRIX			mtxView;		// ビューマトリックス
	D3DXMATRIX			mtxInvView;		// ビューマトリックス
	D3DXMATRIX			mtxProjection;	// プロジェクションマトリックス

	D3DXVECTOR3			pos;			// カメラの視点(位置)
	D3DXVECTOR3			at;				// カメラの注視点
	D3DXVECTOR3			up;				// カメラの上方向ベクトル
	D3DXVECTOR3			rot;			// カメラの回転
	D3DXVECTOR3         speed;          // 移動スピード

	MOIVE_TRANSFORM *tbl_adr;

};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitCamera(void);
void UninitCamera(void);
void UpdateCamera(void);
void SetCamera(void);

CAMERA *GetCamera(void);
