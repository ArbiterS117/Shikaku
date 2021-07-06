//=============================================================================
//
// パーティクル処理 [particle.h]
// Author : GP11B132 14 ゴショウケン
//
//=============================================================================
#pragma once
#include "model.h"
#include "sprite.h"

enum particleType {
	Particletype_LightBall,                // 火の効果
	Particletype_Smoke,                    // ジャンプ, 移動, 衝突....
	Particletype_DashSmoke,                // ダッシュ....
	Particletype_CardMove,                 // カード進む
	Particletype_EnemyDead,                
	Particletype_BumpSoftThings,                
	Particletype_WaterSplash,                
	Particletype_WaterAbilityEffect,       // カードアビリティアピール         
	Particletype_WalkOnGrass,                  
	Particletype_WalkOnSand,                   
	particleTypesNum
};

//*****************************************************************************
// 構造体定義
//*****************************************************************************
// パーティクル構造体
struct PARTICLE
{
	bool		 Use;			 // true:使っている  false:未使用

	int          SpriteID;      // g_Sprite or g_Model ID
	int			 nIdxShadow;	 // 影ID

	float        sprite_w;       // スプライト幅さ
	float        sprite_h;       // スプライト高さ
	float        gravityScale;   // 重力
	float        timer;          // Timer 
	float        liveTime;       // liveTime

	particleType particletype;

	D3DXVECTOR3	 pos;			 // バレットの座標
	D3DXVECTOR3	 rot;			 // バレットの回転量
	D3DXVECTOR3  scl;			 // スケール
	D3DXVECTOR3  speed;			 // バレットの移動量 // speed			
				 
	DX11_SPRITE  sprite;         // スプライト情報
	DX11_MODEL	 model;			 // モデル情報

};


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitParticle(void);
void UninitParticle(void);
void UpdateParticle(void);
void DrawParticle(void);

void SetParticleContinuous(D3DXVECTOR3 pos, D3DXVECTOR3 speed, D3DXCOLOR col, particleType type, float fSizeX, float fSizeY, int nLife);
void SetParticle(D3DXVECTOR3 pos, D3DXVECTOR3 dir, particleType type, int particlenum);

void SetColorParticle(int nIdxParticle, D3DXCOLOR col);

