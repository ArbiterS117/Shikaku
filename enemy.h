//=============================================================================
//
// エネミー処理 [enemy.h]
// Author : GP11B132 14 ゴショウケン
//
//=============================================================================
#pragma once

#include "main.h"
#include "model.h"
enum enumEnemyType
{
	EnemyType_mushroom,
	EnemyType_wind,
	EnemyType_water,
	EnemyType_gem,
	EnemyType_star,
	EnemyType_coin,
	EnemyType_quickSand,
	EnemyType_mushroomSpring,
	EnemyType_Sign,
	EnemyType_MAX,
};

static D3DXVECTOR3 EnemyColliderSize[EnemyType_MAX] = {
	{7.0f, 7.0f, 7.0f},    // mushroom
	{5.0f, 5.0f, 5.0f},    // wind
	{20.0f, 20.0f, 20.0f}, // water
	{5.0f, 5.0f, 5.0f},    // gem
	{5.0f, 5.0f, 5.0f},    // heart
	{5.0f, 5.0f, 5.0f},    // coin
	{20.0f, 20.0f, 20.0f}, // quickSand
	{7.0f, 7.0f, 7.0f},    // mushroomSpring	
	{30.0f, 30.0f, 30.0f},    // sign	
};

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_ENEMY		           (256)				// プレイヤーの数
#define ENEMY_GROUND_CHECK_THIN    (0.1f)				// 地面チェックの高さ

//*****************************************************************************
// 構造体定義
//*****************************************************************************
struct ENEMY
{
	bool		    Use;			        // 使用しているかどうか
	bool            Canfly;
	bool            CanChase;
	bool            CanMove;
	bool            isGround;
	bool            On_Moving_Plat;
	bool            On_Moving_PlatPre;
	bool            Exit_Moving_Plat;

	int             SPID;                   // the "ONLY ONE" ID : manual setting (手動設定) 0 = default : can not use for searching  """""""under constuction""""""
	int             enemyID;
	int             BodyColliderIdx;        // colliderのインデックス番号
	int             ChaseColliderIdx;       // colliderのインデックス番号
	int             SearchColliderIdx;       // colliderのインデックス番号
	int             GroundCheckColliderIdx; // colliderのインデックス番号
	int				shadowIdx;			    // 影のインデックス番号
	int             enemyType;
	int             Status;
	int             scaleAxis;              // 0: X  1: Y  2: Z  -1: all
	int             canmoveAxis;            // 0: X  1: Y  2: Z  -1: none
	int             deadToDestroyTimer;
	int             On_Moving_PlatID;

	float           maxMoveSpeed;
	float           decelerate;
	float           gravityScale;
	float           LMtime;                 //Liner move time 線形移動 

	D3DXMATRIX			mtxWorld;			// ワールドマトリックス
	D3DXVECTOR3			pos;				// モデルの位置
	D3DXVECTOR3			rot;				// モデルの向き(回転)
	D3DXVECTOR3			scl;				// モデルの大きさ(スケール)
	D3DXVECTOR3         speed;              // 移動スピード
	D3DXVECTOR3         hitDirXZ;
	D3DXVECTOR3         CaculatedPos;				       
	D3DXVECTOR3         ChaseDir;           // 追尾時向く方向
	D3DXVECTOR3         On_Moving_Plat_Pos;
	D3DXVECTOR3         Moving_Plat_PrePos;
	D3DXVECTOR3         Moving_Plat_Speed;
	D3DXVECTOR3         ChaceArea;          // 追尾範囲
	D3DXVECTOR3         SearchArea;          // 追尾範囲
	D3DXVECTOR3         OriginPos;          // 元の位置
	
	D3DXVECTOR3         movingOriginPos;    // 動ける元の位置

	DX11_MODEL			model;				// モデル情報


	D3DXVECTOR3			Reborn_pos;
	clock_t				move_time;
};

enum ENUMEnemyStatus {
	EnemyStatus_Idle,
	EnemyStatus_Move,
	EnemyStatus_Air,
	EnemyStatus_Chase,
	EnemyStatus_Back,  // 元の位置に戻る
	EnemyStatus_Dead,  // 打っ飛ばす=>destroy
	EnemyStatus_Hit,   // 打っ飛ばす=>destroy
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitEnemy(void);
void UninitEnemy(void);
void UpdateEnemy(void);
void DrawEnemy(void);

ENEMY *GetEnemy(void);

int SetEnemy(D3DXVECTOR3 pos, D3DXVECTOR3 rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f),
	         D3DXVECTOR3 scl = D3DXVECTOR3(0.0f,0.0f,0.0f), int enemyType = 0, D3DXVECTOR3 reborn_pos);
int SetEnemyToID(int num, DX11_MODEL model, D3DXVECTOR3 pos, D3DXVECTOR3 rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f),
	D3DXVECTOR3 scl = D3DXVECTOR3(0.0f, 0.0f, 0.0f), int EnemyType = 0);

void ReleaseEnemy(int id);
void ReleaseEnemy(ENEMY &enemy);

void HitEnemy(int id, D3DXVECTOR3 hitDirXZ);

// Replace
void RebuildAllEnemyAfterLoad();

//Update Model
void UpdateEnemyModel(int ID);

void ChangeEnemyType(int ID,int Type);

void SetEnemyAnimOriScl(int ID, D3DXVECTOR3 scl);
void SetEnemyAnimOriRot(int ID, D3DXVECTOR3 rot);
void SetEnemyAnimOriPos(int ID, D3DXVECTOR3 pos);
