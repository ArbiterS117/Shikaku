//=============================================================================
//
// バレット処理 [bullet.h]
// Author : GP11B132 14 ゴショウケン
//
//=============================================================================
#pragma once

#include "model.h"
#include "sprite.h"
#include "player.h"

enum enumBulletType
{
	BulletType_tree,
	BulletType_box,
	BulletType_card,
	BulletType_windStorm,
	BulletType_waterBob,
	BulletType_waterBobFloat, // stick on player
};

enum enumCardAbility
{
	CardAbility_none,
	CardAbility_wind,
	CardAbility_water,
};

enum ENUMBulletStatus {
	BulletStatus_Idle,
	BulletStatus_Hit,   // 打っ飛ばす=>destroy
	BulletStatus_Destroy,   // 打っ飛ばす=>destroy
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************
typedef struct
{
	bool		Use;			       // 使用しているかどうか
	bool        cardIsForward;         // same speed forward status
	bool        canSummonBack;
	bool        isSummonBack;

	int         liveTimer;
	int         Status;
	int         PreStatus;

	int         SPID;                  // the "ONLY ONE" ID : manual setting (マニュアル設定) 0 = default : can not use for searching """""""under constuction""""""
	int         bulletID;
	int         colliderIdx;           // colliderのインデックスID
	int			shadowIdx;		       // 影ID

	int         cardForwardTimer;
	int         cardAbility;           // enumCardAbility

	enumBulletType bulletType;
	enumBulletDrawType drawType;

	D3DXMATRIX	mtxWorld;		       // ワールドマトリックス
	D3DXVECTOR3 pos;			       // 位置
	D3DXVECTOR3	rot;			       // モデルの向き(回転)
	D3DXVECTOR3 scl;			       // スケール
	D3DXVECTOR3 speed;			       // バレットの移動量 // speed	
	D3DXVECTOR3 dirXZ;
	D3DXVECTOR3     playerPos;
								       
	DX11_MODEL	model;			       // モデル情報
	DX11_SPRITE sprite;			       
				      					    
} BULLET;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitBullet(void);
void UninitBullet(void);
void UpdateBullet(void);
void DrawBullet(void);

int SetBullet(enumBulletType bulletType,  D3DXVECTOR3 pos, D3DXVECTOR3 dir = D3DXVECTOR3(0.0f, 0.0f, 0.0f), int cardAbility = CardAbility_none);
void ReleaseBullet(int id);
// other
BULLET *GetBullet(void);
