//=============================================================================
//
// collider処理 [Collider3D.h]
// Author : GP11B132 14 ゴショウケン
//
//=============================================================================
#pragma once
#include "renderer.h"
#include "model.h"

#define	MAX_COLLIDER3D			    (2048)    // 最大数
#define RANGE_COLLIDER_SIZE_RATE    (1.0f)

enum enumCollider3DType
{
	Collider3DType_box,
	Collider3DType_circle
};

enum enumCollider3DTag
{
	collider3DTag_none,
	collider3DTag_player,
	collider3DTag_CircleRange,      // For every Circle Range to Chek if need to do more accurate intersection detection
	collider3DTag_PlatCircleRange,  // For every Circle Range to Chek if need to do more accurate intersection detection
	collider3DTag_EnemyCircleRange, // For every Circle Range to Chek if need to do more accurate intersection detection
	collider3DTag_enemy,
	collider3DTag_ground,
	collider3DTag_groundCheck,
	collider3DTag_cellingCheck,
	collider3DTag_bullet,      // player's bullet ex: wind, water
	collider3DTag_card,
	collider3DTag_normalPlatform,
	collider3DTag_moveablePlatform,
	collider3DTag_fallDownArea,
	collider3DTag_platEditRaycast,
	collider3DTag_GroundArea_HeartWorld,
	collider3DTag_GroundArea_HtoW,
	collider3DTag_GroundArea_WonderLand,
	collider3DTag_GroundArea_WonderLand2,
	collider3DTag_GroundArea_WtoS,
	collider3DTag_GroundArea_SandWorld,
	collider3DTag_GroundArea_StoS,
	collider3DTag_GroundArea_SnowWorld,
	collider3DTag_EnemyChaseCheck,

};
//*****************************************************************************
// 構造体定義
//*****************************************************************************
typedef struct
{
	bool  Use;		   // 使用しているかどうか
	bool  enable;      // 判定できるか
	bool  canDraw;
	bool  isCollision;

	int   masterID;    // father class OBJ's ID (用繼承更好) : COOP with TAG

	float wx;          // x軸 横   -> box
	float hy;          // y軸 高さ
	float lz;          // z軸 長さ
	float r;           // 半径     -> circle

	//for collision system
	enumCollider3DType type;
	enumCollider3DTag  tag;

	//for render
	D3DXMATRIX	mtxWorld;	// ワールドマトリックス
	D3DXVECTOR3 pos;		// 位置
	D3DXVECTOR3 rot;		// 回転
	D3DXVECTOR3 scl;		// スケール
	D3DXVECTOR3 normalDirect[3];     // OBB方向ベクトル(標準化)  0:X 1:Y 2:Z

	MATERIAL	material;	// マテリアル
	
} COLLIDER3D;
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitCollider3D(void);
void UninitCollider3D(void);
void UpdateCollider3D(void);
void DrawCollider3D(void);

int CreateCollider3DBox(enumCollider3DTag tag, D3DXVECTOR3 pos, float wx, float hy, float lz, int masterID);
int CreateCollider3DCircle(enumCollider3DTag tag, D3DXVECTOR3 pos, float r, int masterID);

void ReleaseCollider3D(int nIdxCollider3D);
void SetPositionCollider3D(int nIdxCollider3D, D3DXVECTOR3 pos = { 0.0f, 0.0f, 0.0f }, D3DXVECTOR3 rot = { 0.0f, 0.0f, 0.0f });
void SetScaleCollider3D(int nIdxCollider3D, D3DXVECTOR3 scl = { 1.0f, 1.0f, 1.0f });
void SetScaleCollider3D(int nIdxCollider3D, float r = 1.0f);
void SetColorCollider3D(int nIdxCollider3D, D3DXCOLOR col); // for render

//collision system
bool CheckHitByTag(int selfID, int tag);
bool CheckHitByTagReturnMasterID(int selfID, int tag, int *TargetMasterID); // return TargetID 被打到者ID
bool CheckHitByTagReturnCID(int selfID, int tag, int *TargetColliderID); // return TargetColliderID 被打到者的collider的ID
bool CheckHitByTagOBBTEST(int selfID, int tag, int *TargetColliderID);
bool CheckHitByTagOBBTESTReturnLen(int selfID, int tag, int * TargetColliderID, D3DXVECTOR3 *Len);
bool CheckHitByTagOBBPointReturnCID(int selfID, int tag, D3DXVECTOR3 *minPonit, int *TargetColliderID);

bool CheckHitByID(int selfID, int targetID);
bool CheckHitByIDOBB(int selfID, int targetID);
bool CheckHitByIDOBBToPoint(int selfID, int targetID, D3DXVECTOR3 *minPonit);

bool RayHitPlat(D3DXVECTOR3 pos, int TargetCID,D3DXVECTOR3 *HitPosition, D3DXVECTOR3 *Normal);
bool RayHitPlatWall(D3DXVECTOR3 pos, int TargetCID, D3DXVECTOR3 forwardXY, D3DXVECTOR3 *HitPosition, D3DXVECTOR3 *Normal);
bool RayHitGround(D3DXVECTOR3 pos, D3DXVECTOR3 *HitPosition, D3DXVECTOR3 *Normal, int id);
bool RayHitGroundWall(D3DXVECTOR3 pos, D3DXVECTOR3 forwardXY,D3DXVECTOR3 *HitPosition, D3DXVECTOR3 *Normal, int id);

// other
COLLIDER3D *GetCollider(void);

void SetCollider3DEnable(int id, bool b);
float dotProduct(D3DXVECTOR3 *v1, D3DXVECTOR3 *v2);
void crossProduct(D3DXVECTOR3 *ret, D3DXVECTOR3 *v1, D3DXVECTOR3 *v2);

// calculate 3 power bezier curve
D3DXVECTOR3 CalculateThreePowerBezierPoint(float t, D3DXVECTOR3 p0, D3DXVECTOR3 p1, D3DXVECTOR3 p2, D3DXVECTOR3 p3);
D3DXVECTOR3* GetThreePowerBeizerList(D3DXVECTOR3 startPoint, D3DXVECTOR3 controlPoint1, D3DXVECTOR3 controlPoint2, D3DXVECTOR3 endPoint, int segmentNum);