//=============================================================================
//
// プラットフォーム処理 [platform.cpp]
// Author : GP11B132 14 ゴショウケン
//
//=============================================================================
#include "main.h"
#include "gameScene.h"
#include "input.h"
#include "camera.h"
#include "platform.h"
#include "renderer.h"
#include "Collider3D.h"
#include "shadow.h"
#include "groundFile.h"
#include "debugproc.h"
#include "EditTool.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	MODEL_PLATFORM_NORMAL	    "data/MODEL/Block2x2.obj"	// 読み込むモデル名
#define	MODEL_PLATFORM_ICE          "data/MODEL/icePlat.obj"   
#define	MODEL_PLATFORM_T1           "data/MODEL/tree1.obj"     
#define	MODEL_PLATFORM_T2           "data/MODEL/tree2.obj"     
#define	MODEL_PLATFORM_T3           "data/MODEL/tree3.obj"     
#define	MODEL_PLATFORM_BLOCK        "data/MODEL/stoneBlock.obj"     
#define	MODEL_PLATFORM_PYRAMID      "data/MODEL/pyramid.obj"     
#define	MODEL_PLATFORM_BIGTREE      "data/MODEL/BigTree.obj"     
#define	MODEL_PLATFORM_BIGSNOWMAN   "data/MODEL/BigSnowMan.obj"     
#define	MODEL_PLATFORM_MOUNTAIN     "data/MODEL/mountain.obj"     
#define	MODEL_PLATFORM_T4           "data/MODEL/tree4.obj"     
#define	MODEL_PLATFORM_CURVEG       "data/MODEL/CurveG.obj"     
#define	MODEL_PLATFORM_GRASSBLOCK   "data/MODEL/grassBlock.obj"     
#define	MODEL_PLATFORM_HILL         "data/MODEL/hill.obj"     
#define	MODEL_PLATFORM_POKERBLOCK   "data/MODEL/pokerBlock.obj"     
#define	MODEL_PLATFORM_SLIDER       "data/MODEL/slider.obj"     
#define	MODEL_PLATFORM_SNOWMOUNTAIN "data/MODEL/snowMountain.obj"     
#define	VALUE_ROTATE		(D3DX_PI * 0.02f)		// 回転量
#define GROUND_DIS          (800.0f)				// 地面との距離 
#define SELECT_SHINE_TIME   (60)  				    // 選択物件ピカピカタイムマ 
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
static void InputUpdate();
static void PhysicsUpdate(PLATFORM &platfom);
static void CollisionUpdate(PLATFORM &platfom);
static void DebugProc();
//*****************************************************************************
// グローバル変数
//*****************************************************************************
static PLATFORM				g_Platform[MAX_PLATFORM];				// プラットフォームの数

static DX11_MODEL			g_Model[PlatformType_MAX];						// モデル情報

static D3DXVECTOR3 g_MoveTblX[2]; // linear move
static float       g_MoveSpdX[2]; // linear move
static D3DXVECTOR3 g_MoveTblY[2]; // linear move
static float       g_MoveSpdY[2]; // linear move
static D3DXVECTOR3 g_MoveTblZ[2]; // linear move
static float       g_MoveSpdZ[2]; // linear move

static int debugCtrlID = -1;       // Now controlling plat ID

D3DXCOLOR ColorPenBlock   = D3DXCOLOR(0.35f, 0.85f, 0.75f, 0.0f);
D3DXCOLOR ColorPenBlockE  = D3DXCOLOR(0.35f, 0.85f, 0.75f, 0.5f);
D3DXCOLOR ColorRedBlock   = D3DXCOLOR(0.9f, 0.35f, 0.35f, 1.0f);
D3DXCOLOR ColorWhiteBlock = D3DXCOLOR(0.85f, 0.85f, 0.85f, 1.0f);
D3DXCOLOR ColorBlackBlock = D3DXCOLOR(0.25f, 0.25f, 0.25f, 1.0f);

D3DXCOLOR ColorICEBlock = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);

static int SelectShineTimer = 0.0f;
static D3DXCOLOR  OriSelectShineColor;
static D3DXCOLOR  SelectShineColoer = D3DXCOLOR(2.0f, 2.0f, 0.4f, 0.7f);
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlatform(void)
{

	LoadModel(MODEL_PLATFORM_NORMAL, &g_Model[PlatformType_normal]);
	LoadModel(MODEL_PLATFORM_ICE, &g_Model[PlatformType_ice]);
	LoadModel(MODEL_PLATFORM_T1, &g_Model[PlatformType_t1]);
	LoadModel(MODEL_PLATFORM_T2, &g_Model[PlatformType_t2]);
	LoadModel(MODEL_PLATFORM_T3, &g_Model[PlatformType_t3]);
	LoadModel(MODEL_PLATFORM_BLOCK, &g_Model[PlatformType_block]);
	LoadModel(MODEL_PLATFORM_PYRAMID, &g_Model[PlatformType_pyramid]);
	LoadModel(MODEL_PLATFORM_BIGTREE, &g_Model[PlatformType_bigTree]);
	LoadModel(MODEL_PLATFORM_BIGSNOWMAN, &g_Model[PlatformType_bigSnowMan]);
	LoadModel(MODEL_PLATFORM_MOUNTAIN, &g_Model[PlatformType_mountain]);
	LoadModel(MODEL_PLATFORM_T4, &g_Model[PlatformType_t4]);
	LoadModel(MODEL_PLATFORM_CURVEG, &g_Model[PlatformType_CurveG]);
	LoadModel(MODEL_PLATFORM_GRASSBLOCK, &g_Model[PlatformType_grassBlock]);
	LoadModel(MODEL_PLATFORM_HILL, &g_Model[PlatformType_hill]);
	LoadModel(MODEL_PLATFORM_POKERBLOCK, &g_Model[PlatformType_pokerBlock]);
	LoadModel(MODEL_PLATFORM_SLIDER, &g_Model[PlatformType_slider]);
	LoadModel(MODEL_PLATFORM_SNOWMOUNTAIN, &g_Model[PlatformType_snowMountain]);

	//=========Init
	for (int i = 0; i < MAX_PLATFORM; i++) {
		g_Platform[i].Use = false;
		g_Platform[i].model = g_Model[0];
		// 位置・回転・スケールの初期設定
		g_Platform[i].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Platform[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Platform[i].scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

		//physics
		g_Platform[i].speed = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Platform[i].maxMoveSpeed = 15.0f;
		g_Platform[i].decelerate = 0.4f;

		//system
		g_Platform[i].platformID  = -1;   
		g_Platform[i].colliderIdx = -1;              // -1 => no data of collider
		g_Platform[i].CircleRangeColliderIdx = -1;   // -1 => no data of collider

		//edit
		g_Platform[i].scaleAxis = -1;     // 0: X  1: Y  2: Z  -1 : all
		g_Platform[i].colorType =  0;     // 0: red  1:white  2:black

		//線形移動 tmep
		g_Platform[i].canMove = false;
		g_Platform[i].canmoveAxis = -1;   // 0: X  1: Y  2: Z  -1 : not active
		g_Platform[i].LMtime = 0.0f;      //Liner move time

		g_Platform[i].canSpin = false;
		g_Platform[i].canspinAxis = -1;   // 0: X  1: Y  2: Z  -1 : not active

	}
	//============Load Edited data
	bool result;
	result = LoadGroundData();
	//===========if no save data  => these ground will appear
	if (result == false) {
		//linemoving ground
		SetPlatform(D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(60.0f, 60.0f, 60.0f), PlatformType_normal);
		//main ground
		SetPlatform(D3DXVECTOR3(0.0f, -1100.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(200.0f, 200.0f, 200.0f), PlatformType_normal);

		SetPlatform(D3DXVECTOR3(0.0f, -180.0f, 3400.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(100.0f, 100.0f, 100.0f), PlatformType_normal);
		SetPlatform(D3DXVECTOR3(1500.0f, 80.0f, -1500.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(40.0f, 40.0f, 40.0f), PlatformType_normal);
	}
	if (result == true) { // have data
		//rebuild data & collider
		RebuildAllPlatformAfterLoad();
	}

	//============線形移動 tmep
	g_MoveSpdX[0] = 0.0032f;
	g_MoveSpdX[1] = 0.005f;
	g_MoveTblX[0] = D3DXVECTOR3(-1000.0f,  0.0f,  0.0f);
	g_MoveTblX[1] = D3DXVECTOR3( 1000.0f, 0.0f, 0.0f);
	g_MoveSpdY[0] = 0.0032f;
	g_MoveSpdY[1] = 0.005f;
	g_MoveTblY[0] = D3DXVECTOR3(0.0f, -1000.0f, 0.0f);
	g_MoveTblY[1] = D3DXVECTOR3(0.0f,  1000.0f, 0.0f);
	g_MoveSpdZ[0] = 0.0032f;
	g_MoveSpdZ[1] = 0.005f;
	g_MoveTblZ[0] = D3DXVECTOR3(0.0f, 0.0f, -1000.0f);
	g_MoveTblZ[1] = D3DXVECTOR3(0.0f, 0.0f,  1000.0f);

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPlatform(void)
{
	// モデルの解放処理
	for (int i = 0; i < PlatformType_MAX; i++) {
		UnloadModel(&g_Model[i]);
	}

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdatePlatform(void)
{
	//================Input
	InputUpdate();

	for (int i = 0; i < MAX_PLATFORM; i++) {

		if (g_Platform[i].Use) {
			//================Physics
			PhysicsUpdate(g_Platform[i]);
			

			//================collider detect
			CollisionUpdate(g_Platform[i]);

			//g_Platform[i].rot.x += 0.001 * D3DX_PI;
		}

	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPlatform(void)
{
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;
	int debugCtrlCount = 0; // reset SelectShineTimer

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_PLATFORM; i++) {
		if (g_Platform[i].Use) {
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_Platform[i].mtxWorld);

			// スケールを反映
			D3DXMatrixScaling(&mtxScl, g_Platform[i].scl.x, g_Platform[i].scl.y, g_Platform[i].scl.z);
			D3DXMatrixMultiply(&g_Platform[i].mtxWorld, &g_Platform[i].mtxWorld, &mtxScl);

			// 回転を反映
			D3DXMatrixRotationYawPitchRoll(&mtxRot, g_Platform[i].rot.y, g_Platform[i].rot.x, g_Platform[i].rot.z);
			D3DXMatrixMultiply(&g_Platform[i].mtxWorld, &g_Platform[i].mtxWorld, &mtxRot);

			// 移動を反映
			if (g_Platform[i].canMove) {
				D3DXMatrixTranslation(&mtxTranslate, g_Platform[i].CaculatedPos.x, g_Platform[i].CaculatedPos.y, g_Platform[i].CaculatedPos.z);
				D3DXMatrixMultiply(&g_Platform[i].mtxWorld, &g_Platform[i].mtxWorld, &mtxTranslate);
			}
			else {
				D3DXMatrixTranslation(&mtxTranslate, g_Platform[i].pos.x, g_Platform[i].pos.y, g_Platform[i].pos.z);
				D3DXMatrixMultiply(&g_Platform[i].mtxWorld, &g_Platform[i].mtxWorld, &mtxTranslate);
			}
			// ワールドマトリックスの設定
			SetWorldMatrix(&g_Platform[i].mtxWorld);

			// モデル描画
			//Editmode
			if (GetGameSceneEditMode()) {
				if (g_Platform[i].Use) {
					if (i == GetPlatFormDebugCtrlID()) { // selected Platform shine
						if (g_Platform[i].colorType == 0) OriSelectShineColor = ColorPenBlockE;
						else if (g_Platform[i].colorType == 1) OriSelectShineColor = ColorWhiteBlock;
						else if (g_Platform[i].colorType == 2) OriSelectShineColor = ColorBlackBlock;
						else if (g_Platform[i].colorType == 3) OriSelectShineColor = ColorRedBlock;
						if (g_Platform[i].platformType == PlatformType_ice)OriSelectShineColor = ColorICEBlock;

						SelectShineTimer += 1;
						if (SelectShineTimer >= 2 * SELECT_SHINE_TIME) {
							SelectShineTimer = 0;
						}
						D3DXCOLOR color;
						float r, g, b, a;
						if (SelectShineTimer <= SELECT_SHINE_TIME) {
							r = (SelectShineColoer.r - OriSelectShineColor.r) / SELECT_SHINE_TIME * (float)(SelectShineTimer);
							g = (SelectShineColoer.g - OriSelectShineColor.g) / SELECT_SHINE_TIME * (float)(SelectShineTimer);
							b = (SelectShineColoer.b - OriSelectShineColor.b) / SELECT_SHINE_TIME * (float)(SelectShineTimer);
							a = (SelectShineColoer.a - OriSelectShineColor.a) / SELECT_SHINE_TIME * (float)(SelectShineTimer);
							color = D3DXCOLOR(r, g, b, a);
							for (unsigned short j = 0; j < g_Platform[i].model.SubsetNum; j++)
							{
								g_Platform[i].model.SubsetArray[j].Material.Material.Diffuse = OriSelectShineColor + color;
							}
						}
						else {
							r = (OriSelectShineColor.r - SelectShineColoer.r) / SELECT_SHINE_TIME * (float)(SelectShineTimer - SELECT_SHINE_TIME);
							g = (OriSelectShineColor.g - SelectShineColoer.g) / SELECT_SHINE_TIME * (float)(SelectShineTimer - SELECT_SHINE_TIME);
							b = (OriSelectShineColor.b - SelectShineColoer.b) / SELECT_SHINE_TIME * (float)(SelectShineTimer - SELECT_SHINE_TIME);
							a = (OriSelectShineColor.a - SelectShineColoer.a) / SELECT_SHINE_TIME * (float)(SelectShineTimer - SELECT_SHINE_TIME);
							color = D3DXCOLOR(r, g, b, a);
							for (unsigned short j = 0; j < g_Platform[i].model.SubsetNum; j++)
							{
								g_Platform[i].model.SubsetArray[j].Material.Material.Diffuse = SelectShineColoer + color;
							}
						}
						debugCtrlCount++;

						DrawModel(&g_Platform[i].model);
					}
					else { //COLOR
						D3DXCOLOR color;
						if (g_Platform[i].colorType == 0) color = ColorPenBlockE;
						else if (g_Platform[i].colorType == 1) color = ColorWhiteBlock;
						else if (g_Platform[i].colorType == 2) color = ColorBlackBlock;
						else if (g_Platform[i].colorType == 3) color = ColorRedBlock;
						if (g_Platform[i].platformType == PlatformType_ice)color = ColorICEBlock;
						for (unsigned short j = 0; j < g_Platform[i].model.SubsetNum; j++)
						{
							g_Platform[i].model.SubsetArray[j].Material.Material.Diffuse = color;
						}
						DrawModel(&g_Platform[i].model);
					}
				}
			}
			else {//COLOR
				D3DXCOLOR color;
				if (g_Platform[i].colorType == 0) color = ColorPenBlock;
				else if (g_Platform[i].colorType == 1) color = ColorWhiteBlock;
				else if (g_Platform[i].colorType == 2) color = ColorBlackBlock;
				else if (g_Platform[i].colorType == 3) color = ColorRedBlock;
				if (g_Platform[i].platformType == PlatformType_ice)color = ColorICEBlock;

				for (unsigned short j = 0; j < g_Platform[i].model.SubsetNum; j++)
				{
					g_Platform[i].model.SubsetArray[j].Material.Material.Diffuse = color;
				}
				if (color.a != 0.0f)DrawModel(&g_Platform[i].model);
			}
			
		}

	}
	if (debugCtrlCount == 0)SelectShineTimer = 0;


	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}


//=============================================================================
// 情報を取得
//=============================================================================
PLATFORM *GetPlatform(void)
{
	return &g_Platform[0];
}

//=============================================================================
// エネミーのパラメータをセット
//=============================================================================
int SetPlatform(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scl, int platformType)
{
	int IdxPlatform = -1;

	for (int i = 0; i < MAX_PLATFORM; i++)
	{
		if (!g_Platform[i].Use)
		{
			IdxPlatform = i;
			g_Platform[i].platformID = i;

			g_Platform[i].Use = true;
			g_Platform[i].model = g_Model[platformType];
			g_Platform[i].pos = pos;
			g_Platform[i].rot = rot;
			g_Platform[i].scl = scl;
			g_Platform[i].colorType = 0;

			//collider 設定
			g_Platform[i].platformType = platformType;
			
			g_Platform[i].colliderIdx = CreateCollider3DBox(collider3DTag_normalPlatform, pos, g_Platform[i].scl.x * PlatformColliderSize[platformType].x,
				g_Platform[i].scl.y* PlatformColliderSize[platformType].y, g_Platform[i].scl.z* PlatformColliderSize[platformType].z, g_Platform[i].platformID);

			float maxR = max((max((g_Platform[i].scl.x* PlatformColliderSize[platformType].x), (g_Platform[i].scl.y* PlatformColliderSize[platformType].y))), (g_Platform[i].scl.z* PlatformColliderSize[platformType].z));
			g_Platform[i].CircleRangeColliderIdx = CreateCollider3DCircle(collider3DTag_PlatCircleRange, pos, maxR * RANGE_COLLIDER_SIZE_RATE, g_Platform[i].platformID);
			
			break;
		}
	}

	return IdxPlatform;
}

int SetPlatformToID(int num, DX11_MODEL model, D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scl, int platformType, D3DXCOLOR clr)
{
	int IdxPlatform = -1;

	IdxPlatform = num;
	g_Platform[num].platformID = num;

	g_Platform[num].Use = true;
	g_Platform[num].model = model;
	g_Platform[num].pos = pos;
	g_Platform[num].rot = rot;
	g_Platform[num].scl = scl;
	g_Platform[num].color = clr;
	g_Platform[num].model.SubsetArray->Material.Material.Diffuse = clr;

	//collider 設定
	g_Platform[num].platformType = platformType;

	g_Platform[num].colliderIdx = CreateCollider3DBox(collider3DTag_normalPlatform, pos, g_Platform[num].scl.x * PlatformColliderSize[platformType].x,
		g_Platform[num].scl.y* PlatformColliderSize[platformType].y, g_Platform[num].scl.z* PlatformColliderSize[platformType].z, g_Platform[num].platformID);

	float maxR = max((max((g_Platform[num].scl.x* PlatformColliderSize[platformType].x), (g_Platform[num].scl.y* PlatformColliderSize[platformType].y))), (g_Platform[num].scl.z* PlatformColliderSize[platformType].z));
	g_Platform[num].CircleRangeColliderIdx = CreateCollider3DCircle(collider3DTag_PlatCircleRange, pos, maxR * RANGE_COLLIDER_SIZE_RATE, g_Platform[num].platformID);
	

	return IdxPlatform;
}

void ReleasePlatform(int id)
{
	if (g_Platform[id].Use) {
		g_Platform[id].Use = false;
		ReleaseCollider3D(g_Platform[id].colliderIdx);
		ReleaseCollider3D(g_Platform[id].CircleRangeColliderIdx);
		//ReleaseShadow(g_Platform[id].shadowIdx);
	}
}

void InputUpdate() {

}

void PhysicsUpdate(PLATFORM &platfom) {
	////==============ライン移動
	//=線形移動 
	if (platfom.canmoveAxis == 0) { // X
		platfom.canMove = true;
		int nowNo = (int)platfom.LMtime; //Liner move time
		int maxNo = (sizeof(g_MoveTblX) / sizeof(D3DXVECTOR3));
		int nextNo = (nowNo + 1) % maxNo;
		D3DXVECTOR3 pos = g_MoveTblX[nextNo] - g_MoveTblX[nowNo];
		float nowTime = platfom.LMtime - nowNo;
		pos *= nowTime;
		platfom.CaculatedPos = platfom.pos + g_MoveTblX[nowNo] + pos;
		platfom.LMtime += g_MoveSpdX[nowNo];
		if ((int)platfom.LMtime >= maxNo) {
			platfom.LMtime = 0.0f;
		}
	}
	else if (platfom.canmoveAxis == 1) { // Y
		platfom.canMove = true;
		int nowNo = (int)platfom.LMtime; //Liner move time
		int maxNo = (sizeof(g_MoveTblY) / sizeof(D3DXVECTOR3));
		int nextNo = (nowNo + 1) % maxNo;
		D3DXVECTOR3 pos = g_MoveTblY[nextNo] - g_MoveTblY[nowNo];
		float nowTime = platfom.LMtime - nowNo;
		pos *= nowTime;
		platfom.CaculatedPos = platfom.pos + g_MoveTblY[nowNo] + pos;
		platfom.LMtime += g_MoveSpdX[nowNo];
		if ((int)platfom.LMtime >= maxNo) {
			platfom.LMtime = 0.0f;
		}
	}
	else if (platfom.canmoveAxis == 2) { // Z
		platfom.canMove = true;
		int nowNo = (int)platfom.LMtime; //Liner move time
		int maxNo = (sizeof(g_MoveTblZ) / sizeof(D3DXVECTOR3));
		int nextNo = (nowNo + 1) % maxNo;
		D3DXVECTOR3 pos = g_MoveTblZ[nextNo] - g_MoveTblZ[nowNo];
		float nowTime = platfom.LMtime - nowNo;
		pos *= nowTime;
		platfom.CaculatedPos = platfom.pos + g_MoveTblZ[nowNo] + pos;
		platfom.LMtime += g_MoveSpdX[nowNo];
		if ((int)platfom.LMtime >= maxNo) {
			platfom.LMtime = 0.0f;
		}
	}
	else platfom.canMove = false;

	if (platfom.canspinAxis == 0) { // X
		platfom.canSpin = true;
		platfom.rot.x += 0.02f;
		platfom.rot.y = 0.0f;
		platfom.rot.z = 0.0f;
	}
	else if (platfom.canspinAxis == 1) { // Y
		platfom.canSpin = true;
		platfom.rot.y += 0.02f;
		platfom.rot.x = 0.0f;
		platfom.rot.z = 0.0f;
	}
	else if (platfom.canspinAxis == 2) { // Z
		platfom.canSpin = true;
		platfom.rot.z += 0.02f;
		platfom.rot.y = 0.0f;
		platfom.rot.x = 0.0f;
	}
	else {
		platfom.canSpin = false;
	}
	
}

void CollisionUpdate(PLATFORM & platfom)
{
	//collider position
	if (platfom.canMove) {
		SetPositionCollider3D(platfom.colliderIdx, platfom.CaculatedPos, platfom.rot);
		SetPositionCollider3D(platfom.CircleRangeColliderIdx, platfom.CaculatedPos);
	}
	else {
		SetPositionCollider3D(platfom.colliderIdx, platfom.pos, platfom.rot);
		SetPositionCollider3D(platfom.CircleRangeColliderIdx, platfom.pos);
	}

	SetCollider3DEnable(platfom.colliderIdx,false);
	//collision

}

void RebuildAllPlatformAfterLoad() {
	//rebuild data & collider
	for (int i = 0; i < MAX_PLATFORM; i++) {
		if (g_Platform[i].Use == true) { //have save data ==> replace
			DX11_MODEL model = g_Model[g_Platform[i].platformType];
			D3DXVECTOR3 pos = g_Platform[i].pos;
			D3DXVECTOR3 rot = g_Platform[i].rot;
			D3DXVECTOR3 scl = g_Platform[i].scl;
			D3DXCOLOR clr = g_Platform[i].color;
			enumPlatformType platformType = (enumPlatformType)g_Platform[i].platformType;

			SetPlatformToID(i, model, pos, rot, scl, platformType, clr);
		}
	}
}

void UpdatePlatformModel(int id)
{
	g_Platform[id].model = g_Model[g_Platform[id].platformType];
}