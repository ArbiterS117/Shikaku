//=============================================================================
//
// パーティクル処理 [particle.h]
// Author : GP11A132_16_佐久間和也
//
//=============================================================================
#pragma once

//*****************************************************************************
// マクロ定義
//*****************************************************************************

#define	MAX_PARTICLE		(2000)		// パーティクル最大数
#define MAX_BEZ				(256)

//*****************************************************************************
// 構造体定義
//*****************************************************************************
typedef struct
{
	D3DXVECTOR3		pos;			// 位置
	D3DXVECTOR3		rot;			// 回転
	D3DXVECTOR3		scale;			// スケール
	D3DXVECTOR3		move;			// 移動量
	MATERIAL		material;		// マテリアル
	float			fSizeX;			// 幅
	float			fSizeY;			// 高さ
	int				nIdxShadow;		// 影ID
	int				nLife;			// 寿命
	bool			bUse;			// 使用しているかどうか
	int				hitNo;			// 当たり判定
	float			count;			// カウント

} PARTICLE;

typedef struct
{
	int bez;
	D3DXVECTOR3 finish;
	D3DXVECTOR3 start;
	D3DXVECTOR3 P1;
	D3DXVECTOR3 P2;
	D3DXCOLOR color;

	float x;
	float y;
	float z;
	float x2;
	float y2;
	float z2;
	float x3;
	float y3;
	float z3;

} BEZ;

enum
{
	EFFECT_DEFAULT,
	EFFECT_LISSAJOUS,
	EFFECT_FIRE,
	EFFECT_SMOKE,
	EFFECT_STAR,
	EFFECT_CIRCLE,
	EFFECT_BEZIER,
	EFFECT_ALL,
	EFFECT_MAX,
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitParticle(void);
void UninitParticle(void);
void UpdateParticle(void);
void DrawParticle(void);

int SetParticle(D3DXVECTOR3 pos, D3DXCOLOR col, int nLife);
void SetColorParticle(int nIdxParticle, D3DXCOLOR col);
PARTICLE *GetParticle();
BEZ *GetBez(void);

void effectLissajous(D3DXVECTOR3 pos, int sinX, int sinY, int sinZ, float sizeX, float sizeY, float sizeZ, int Quantity);
void effectStar(D3DXVECTOR3 pos);
int effectFire(D3DXVECTOR3 pos, D3DXCOLOR col, float fSizeX, float fSizeY, int count);
int effectSmoke(D3DXVECTOR3 pos, D3DXCOLOR col, float fSizeX, float fSizeY, int count);
int effectCircle(D3DXVECTOR3 pos);
void effectBezier(D3DXVECTOR3 pos);
void ParticleSwitch(D3DXVECTOR3 pos,int No);