//=============================================================================
//
// エネミー処理処理 [enemy.cpp]
// Author : GP11B132 14 ゴショウケン
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "enemy.h"
#include "renderer.h"
#include "Collider3D.h"
#include "shadow.h"
#include "EditTool.h"
#include "EnemyFile.h"
#include "debugproc.h"
#include "particle.h"
#include "sound.h"

#include <time.h>
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_MODEL		(EnemyType_MAX)		        // モデルの数
#define	MODEL_ENEMY		"data/MODEL/ghost.obj"		// 読み込むモデル名
#define	MODEL_ENEMY2    "data/MODEL/dimond.obj"        
#define	MODEL_ENEMY3	"data/MODEL/WaterCrys.obj"	
#define	MODEL_ENEMY4	"data/MODEL/star.obj"	    
#define	MODEL_ENEMY5	"data/MODEL/coin.obj"	       
#define	MODEL_ENEMY6	"data/MODEL/quickSand.obj"   
#define	MODEL_ENEMY7	"data/MODEL/mushroomSpring.obj"	   
#define	MODEL_ENEMY8	"data/MODEL/Sign.obj"	   

#define	VALUE_MOVE			(2.0f)						// 移動量
#define	VALUE_ROTATE		(D3DX_PI * 0.02f)			// 回転量
#define	VALUE_GRAVITY		(0.8f)		                // 重力
#define VALUE_DECELERATE    (0.6f)

#define VALUE_MAX_MOVE_SPEED       (10.0f)
#define VALUE_MAX_FALLEN_SPEED     (-30.0f)

#define ENEMY_SHADOW_SIZE	       (20.0f)				// 影の大きさ
#define ENEMY_GROUND_DIS           (400.0f)				// 地面との距離
						           
#define SELECT_SHINE_TIME          (60)  				// 選択物件ピカピカタイムマ 

#define CHASE_DETECT_AREA_X        (1000.0f)
#define CHASE_DETECT_AREA_Y        (500.0f)
#define CHASE_DETECT_AREA_Z        (1000.0f)
#define CHASEING_AREA_X            (4000.0f)
#define CHASEING_AREA_Y            (5000.0f)
#define CHASEING_AREA_Z            (4000.0f)
#define CHASE_SPEED                (10.0f)
#define VALUE_CHASE_OBJ_LIMIT      (200.0f)
#define VALUE_CHASE_TURN           (0.04f)             // 1 => turn easily
#define VALUE_DEAD_FLY_SPEED       (30.0f)             
#define VALUE_DEAD_MOVE_SPEED      (15.0f)             
#define DEAD_TO_DESTROY_TIME       (45)         
#define	VALUE_DEAD_GRAVITY		   (1.3f)

//===============Anim Time (Start to Finish => no matter how many states it has) (frame)
#define AnimTime_MUSHROOM_HIT      (30.0f)
#define AnimTime_WATER_IDLE        (300.0f)
#define AnimTime_WATER_HIT         (180.0f)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
static void InputUpdate(ENEMY &enemy);
static void PhysicsUpdate(ENEMY &enemy);
static void CollisionUpdate(ENEMY &enemy);
static void StatusUpdate(ENEMY &enemy);
static void AnimUpdate(ENEMY &enemy);
//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ENEMY				g_Enemy[MAX_ENEMY];				// プレイヤー

static PLAYER				*g_Player = GetPlayer();

static DX11_MODEL			g_Model[MAX_MODEL];						// モデル情報

static D3DXVECTOR3 g_MoveTblX[3]; // linear move
static float       g_MoveSpdX[3]; // linear move
static D3DXVECTOR3 g_MoveTblY[3]; // linear move
static float       g_MoveSpdY[3]; // linear move
static D3DXVECTOR3 g_MoveTblZ[3]; // linear move
static float       g_MoveSpdZ[3]; // linear move

static int SelectShineTimer = 0.0f;
static D3DXCOLOR  OriSelectShineColor;
static D3DXCOLOR  SelectShineColoer = D3DXCOLOR(2.0f, 2.0f, 0.4f, 0.7f);

static float             g_AnimTimer[MAX_ENEMY]; // AnimTimer
static D3DXVECTOR3       g_AnimOriScl[MAX_ENEMY]; // AnimTimer
static D3DXVECTOR3       g_AnimOriRot[MAX_ENEMY]; // AnimTimer
static D3DXVECTOR3       g_AnimOriPos[MAX_ENEMY]; // AnimTimer
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEnemy(void)
{
	LoadModel(MODEL_ENEMY, &g_Model[EnemyType_mushroom]);
	LoadModel(MODEL_ENEMY2, &g_Model[EnemyType_wind]);
	LoadModel(MODEL_ENEMY3, &g_Model[EnemyType_water]);
	LoadModel(MODEL_ENEMY2, &g_Model[EnemyType_gem]);
	LoadModel(MODEL_ENEMY4, &g_Model[EnemyType_star]);
	LoadModel(MODEL_ENEMY5, &g_Model[EnemyType_coin], false);
	LoadModel(MODEL_ENEMY6, &g_Model[EnemyType_quickSand]);
	LoadModel(MODEL_ENEMY7, &g_Model[EnemyType_mushroomSpring]);
	LoadModel(MODEL_ENEMY8, &g_Model[EnemyType_Sign]);

	//=====Init
	for (int i = 0; i < MAX_ENEMY; i++) {
		g_Enemy[i].Use = false;
		g_Enemy[i].model = g_Model[0];
		// 位置・回転・スケールの初期設定
		g_Enemy[i].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Enemy[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Enemy[i].scl = D3DXVECTOR3(100.0f, 100.0f, 100.0f);
		g_Enemy[i].OriginPos = g_Enemy[i].pos;

		g_Enemy[i].Reborn_pos = g_Enemy[i].pos;
		//physics
		g_Enemy[i].speed = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Enemy[i].maxMoveSpeed = VALUE_MAX_MOVE_SPEED;
		g_Enemy[i].decelerate = VALUE_DECELERATE;
		g_Enemy[i].gravityScale = VALUE_GRAVITY;
		g_Enemy[i].BodyColliderIdx = -1;
		g_Enemy[i].GroundCheckColliderIdx = -1;
		g_Enemy[i].ChaseColliderIdx = -1;
		g_Enemy[i].ChaceArea = D3DXVECTOR3(CHASE_DETECT_AREA_X, CHASE_DETECT_AREA_Y, CHASE_DETECT_AREA_Z);
		g_Enemy[i].deadToDestroyTimer = 0;
		//system
		g_Enemy[i].enemyID  = 0;   // 0 = default : can not use for searching
		g_Enemy[i].CanChase = false;   
		g_Enemy[i].Canfly = false;   

		g_Enemy[i].move_time = 0.0f;
		////線形移動 tmep
		//g_Enemy[i].CanMove = false;
		//g_Enemy[i].canmoveAxis = -1;   // 0: X  1: Y  2: Z  -1 : not active
		//g_Enemy[i].LMtime = 0.0f;//Liner move time
		//g_Enemy[i].Status = EnemyStatus_Idle;

		g_AnimTimer[i] = 0.0f;
	}
	
	//============Load Edited data
	bool result;
	result = LoadEnemyData();
	//result = UpgradeLoadEnemyData();
	//===========if no save data  => these ground will appear
	if (result == false) {
		//SetEnemy(D3DXVECTOR3(1500.0f, ENEMY_GROUND_DIS, 0.0f), D3DXVECTOR3(0.0f, D3DX_PI * -0.5f, 0.0f), D3DXVECTOR3(60.0f, 60.0f, 60.0f), EnemyType_mushroom);
		//SetEnemy(D3DXVECTOR3(-1500.0f, ENEMY_GROUND_DIS - 125.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(40.0f, 40.0f, 40.0f), EnemyType_wind);
		//SetEnemy(D3DXVECTOR3(-400.0f, 80.0f, -400.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(10.0f, 10.0f, 10.0f), EnemyType_water);
		//SetEnemy(D3DXVECTOR3(1500.0f, ENEMY_GROUND_DIS - 125.0f, -1500.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(40.0f, 40.0f, 40.0f), EnemyType_wind);
	}
	if (result == true) { // have data
		//rebuild data & collider
		RebuildAllEnemyAfterLoad();
	}
	
	//線形移動 tmep
	/*g_MoveSpdX[0] = 0.01f;
	g_MoveSpdX[1] = 0.005f;
	g_MoveSpdX[2] = 0.01f;
	g_MoveTblX[0] = D3DXVECTOR3(    0.0f, 0.0f, 0.0f);
	g_MoveTblX[1] = D3DXVECTOR3(-1000.0f, 0.0f, 0.0f);
	g_MoveTblX[2] = D3DXVECTOR3( 1000.0f, 0.0f, 0.0f);
	g_MoveSpdY[0] = 0.01f;
	g_MoveSpdY[1] = 0.005f;
	g_MoveSpdY[2] = 0.01f;
	g_MoveTblY[0] = D3DXVECTOR3(0.0f,     0.0f, 0.0f);
	g_MoveTblY[1] = D3DXVECTOR3(0.0f, -1000.0f, 0.0f);
	g_MoveTblY[2] = D3DXVECTOR3(0.0f,  1000.0f, 0.0f);
	g_MoveSpdZ[0] = 0.01f;
	g_MoveSpdZ[1] = 0.005f;
	g_MoveSpdZ[2] = 0.01f;
	g_MoveTblZ[0] = D3DXVECTOR3(0.0f, 0.0f,     0.0f);
	g_MoveTblZ[1] = D3DXVECTOR3(0.0f, 0.0f, -1000.0f);
	g_MoveTblZ[2] = D3DXVECTOR3(0.0f, 0.0f,  1000.0f);*/

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEnemy(void)
{
	// モデルの解放処理
	for (int i = 0; i < MAX_MODEL; i++) {
		UnloadModel(&g_Model[i]);
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateEnemy(void)
{
	for (int i = 0; i < MAX_ENEMY; i++) {
		if (g_Enemy[i].Use) {
			//================Input
			//InputUpdate(g_Enemy[i]);

			//================Physics
			PhysicsUpdate(g_Enemy[i]);

			//================collider detect
			CollisionUpdate(g_Enemy[i]);

			//================Status
			StatusUpdate(g_Enemy[i]);

			//================Anim
			AnimUpdate(g_Enemy[i]);

			// 影もプレイヤーの位置に合わせる
			D3DXVECTOR3 pos = g_Enemy[i].pos;
			pos.y = 0.0f;
			//SetPositionShadow(g_Enemy[i].shadowIdx, pos);
		}
	}

	// ============respawn
	int nokori = 0;
	for (int i = 0; i < MAX_ENEMY; i++) {
		if (g_Enemy[i].Use == false)nokori++;
	}
	if (nokori == MAX_ENEMY) {
		//SetEnemy(D3DXVECTOR3(1500.0f, ENEMY_GROUND_DIS, 0.0f), D3DXVECTOR3(0.0f, D3DX_PI * -0.5f, 0.0f), D3DXVECTOR3(60.0f, 60.0f, 60.0f));
		//SetEnemy(D3DXVECTOR3(-1500.0f, ENEMY_GROUND_DIS - 125.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(40.0f, 40.0f, 40.0f), EnemyType_wind);
		//SetEnemy(D3DXVECTOR3(-400.0f, 80.0f, -400.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(10.0f, 10.0f, 10.0f), EnemyType_water);
		//SetEnemy(D3DXVECTOR3(1500.0f, ENEMY_GROUND_DIS - 125.0f, -1500.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(40.0f, 40.0f, 40.0f), EnemyType_wind);
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEnemy(void)
{
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;
	int debugCtrlCount = 0; // reset SelectShineTimer

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	for (int i = 0; i < MAX_ENEMY; i++) {
		
		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&g_Enemy[i].mtxWorld);

		// スケールを反映
		D3DXMatrixScaling(&mtxScl, g_Enemy[i].scl.x, g_Enemy[i].scl.y, g_Enemy[i].scl.z);
		D3DXMatrixMultiply(&g_Enemy[i].mtxWorld, &g_Enemy[i].mtxWorld, &mtxScl);

		// 回転を反映
		D3DXMatrixRotationYawPitchRoll(&mtxRot, g_Enemy[i].rot.y + D3DX_PI, g_Enemy[i].rot.x, g_Enemy[i].rot.z);
		D3DXMatrixMultiply(&g_Enemy[i].mtxWorld, &g_Enemy[i].mtxWorld, &mtxRot);

		// 移動を反映
		if (g_Enemy[i].CanMove) {
			D3DXMatrixTranslation(&mtxTranslate, g_Enemy[i].CaculatedPos.x, g_Enemy[i].CaculatedPos.y, g_Enemy[i].CaculatedPos.z);
			D3DXMatrixMultiply(&g_Enemy[i].mtxWorld, &g_Enemy[i].mtxWorld, &mtxTranslate);
		}
		else {
			D3DXMatrixTranslation(&mtxTranslate, g_Enemy[i].pos.x, g_Enemy[i].pos.y, g_Enemy[i].pos.z);
			D3DXMatrixMultiply(&g_Enemy[i].mtxWorld, &g_Enemy[i].mtxWorld, &mtxTranslate);
		}

		// ワールドマトリックスの設定
		SetWorldMatrix(&g_Enemy[i].mtxWorld);

		// モデル描画
		//Editmode
		if (g_Enemy[i].Use) {
			if (GetGameSceneEditMode()) {
				if (i == GetEnemyDebugCtrlID()) { // selected Enemy shine
					OriSelectShineColor = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

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
						for (unsigned short j = 0; j < g_Enemy[i].model.SubsetNum; j++)
						{
							g_Enemy[i].model.SubsetArray[j].Material.Material.Diffuse = OriSelectShineColor + color;
						}
					}
					else {
						r = (OriSelectShineColor.r - SelectShineColoer.r) / SELECT_SHINE_TIME * (float)(SelectShineTimer - SELECT_SHINE_TIME);
						g = (OriSelectShineColor.g - SelectShineColoer.g) / SELECT_SHINE_TIME * (float)(SelectShineTimer - SELECT_SHINE_TIME);
						b = (OriSelectShineColor.b - SelectShineColoer.b) / SELECT_SHINE_TIME * (float)(SelectShineTimer - SELECT_SHINE_TIME);
						a = (OriSelectShineColor.a - SelectShineColoer.a) / SELECT_SHINE_TIME * (float)(SelectShineTimer - SELECT_SHINE_TIME);
						color = D3DXCOLOR(r, g, b, a);
						for (unsigned short j = 0; j < g_Enemy[i].model.SubsetNum; j++)
						{
							g_Enemy[i].model.SubsetArray[j].Material.Material.Diffuse = SelectShineColoer + color;
						}

					}
					debugCtrlCount++;

					DrawModel(&g_Enemy[i].model);
				}
				else { //COLOR
					for (unsigned short j = 0; j < g_Enemy[i].model.SubsetNum; j++)
					{
						g_Enemy[i].model.SubsetArray[j].Material.Material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
					}
					DrawModel(&g_Enemy[i].model);
				}
			}
			else {//COLOR
				for (unsigned short j = 0; j < g_Enemy[i].model.SubsetNum; j++)
				{
					g_Enemy[i].model.SubsetArray[j].Material.Material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
				}
				DrawModel(&g_Enemy[i].model);
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
ENEMY *GetEnemy(void)
{
	return &g_Enemy[0];
}

//=============================================================================
// エネミーのパラメータをセット
//=============================================================================
int SetEnemy(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scl, int enemyType, D3DXVECTOR3 reborn_pos)
{
	int IdxEnemy = -1;

	for (int i = 0; i < MAX_ENEMY; i++)
	{
		if (!g_Enemy[i].Use)
		{
			IdxEnemy = i;
			g_Enemy[i].enemyID = i;

			g_Enemy[i].Use = true;
			g_Enemy[i].model = g_Model[enemyType];
			g_Enemy[i].pos = pos;
			g_Enemy[i].rot = rot;
			g_AnimOriRot[i] = rot;
			g_AnimOriPos[i] = pos;
			g_Enemy[i].scl = scl;
			g_AnimOriScl[i] = scl;
			g_Enemy[i].OriginPos = pos;
			g_Enemy[i].Reborn_pos = reborn_pos;
			g_Enemy[i].Status = EnemyStatus_Idle;
			g_Enemy[i].speed = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

			g_AnimTimer[i] = 0.0f;


			g_Enemy[i].move_time = clock();
			// 影の設定
			//g_Enemy[i].shadowIdx = CreateShadow(pos, ENEMY_SHADOW_SIZE, ENEMY_SHADOW_SIZE);

			g_Enemy[i].enemyType = enemyType;

			D3DXVECTOR3 groundCheckPos;
			//collider 設定
			switch (g_Enemy[i].enemyType)
			{
			case EnemyType_mushroom:
				g_Enemy[i].BodyColliderIdx = CreateCollider3DBox(collider3DTag_enemy, pos, g_Enemy[i].scl.x * EnemyColliderSize[enemyType].x,
					g_Enemy[i].scl.y* EnemyColliderSize[enemyType].y, g_Enemy[i].scl.z* EnemyColliderSize[enemyType].z, g_Enemy[i].enemyID);
				groundCheckPos = D3DXVECTOR3 (pos.x,pos.y - g_Enemy[i].scl.y* EnemyColliderSize[enemyType].y * 0.5f,pos.z);
				g_Enemy[i].GroundCheckColliderIdx = CreateCollider3DBox(collider3DTag_groundCheck, groundCheckPos, g_Enemy[i].scl.x * EnemyColliderSize[enemyType].x,
					g_Enemy[i].scl.y* EnemyColliderSize[enemyType].y * ENEMY_GROUND_CHECK_THIN, g_Enemy[i].scl.z* EnemyColliderSize[enemyType].z, g_Enemy[i].enemyID);
				g_Enemy[i].ChaseColliderIdx = CreateCollider3DBox(collider3DTag_EnemyChaseCheck, pos, g_Enemy[i].ChaceArea.x, 
					g_Enemy[i].ChaceArea.y, g_Enemy[i].ChaceArea.z, g_Enemy[i].enemyID);
				g_Enemy[i].Canfly = true;
				break;
			case EnemyType_wind:
				g_Enemy[i].BodyColliderIdx = CreateCollider3DBox(collider3DTag_enemy, pos, g_Enemy[i].scl.x * EnemyColliderSize[enemyType].x,
					g_Enemy[i].scl.y* EnemyColliderSize[enemyType].y, g_Enemy[i].scl.z* EnemyColliderSize[enemyType].z, g_Enemy[i].enemyID);
				groundCheckPos = D3DXVECTOR3(pos.x, pos.y - g_Enemy[i].scl.y* EnemyColliderSize[enemyType].y * 0.5f, pos.z);
				g_Enemy[i].GroundCheckColliderIdx = CreateCollider3DBox(collider3DTag_groundCheck, groundCheckPos, g_Enemy[i].scl.x * EnemyColliderSize[enemyType].x,
					g_Enemy[i].scl.y* EnemyColliderSize[enemyType].y * ENEMY_GROUND_CHECK_THIN, g_Enemy[i].scl.z* EnemyColliderSize[enemyType].z, g_Enemy[i].enemyID);
				g_Enemy[i].ChaseColliderIdx = CreateCollider3DBox(collider3DTag_EnemyChaseCheck, pos, g_Enemy[i].ChaceArea.x,
					g_Enemy[i].ChaceArea.y, g_Enemy[i].ChaceArea.z, g_Enemy[i].enemyID);
				g_Enemy[i].Canfly = true;
				break;
			case EnemyType_water:
				g_Enemy[i].BodyColliderIdx = CreateCollider3DBox(collider3DTag_enemy, pos, g_Enemy[i].scl.x * EnemyColliderSize[enemyType].x,
					g_Enemy[i].scl.y* EnemyColliderSize[enemyType].y, g_Enemy[i].scl.z* EnemyColliderSize[enemyType].z, g_Enemy[i].enemyID);
				groundCheckPos = D3DXVECTOR3(pos.x, pos.y - g_Enemy[i].scl.y* EnemyColliderSize[enemyType].y * 0.5f, pos.z);
				g_Enemy[i].GroundCheckColliderIdx = CreateCollider3DBox(collider3DTag_groundCheck, groundCheckPos, g_Enemy[i].scl.x * EnemyColliderSize[enemyType].x,
					g_Enemy[i].scl.y* EnemyColliderSize[enemyType].y * ENEMY_GROUND_CHECK_THIN, g_Enemy[i].scl.z* EnemyColliderSize[enemyType].z, g_Enemy[i].enemyID);
				g_Enemy[i].ChaseColliderIdx = CreateCollider3DBox(collider3DTag_EnemyChaseCheck, pos, g_Enemy[i].ChaceArea.x,
					g_Enemy[i].ChaceArea.y, g_Enemy[i].ChaceArea.z, g_Enemy[i].enemyID);
				g_Enemy[i].Canfly = true;
				break;
			case EnemyType_gem:
				g_Enemy[i].BodyColliderIdx = CreateCollider3DBox(collider3DTag_enemy, pos, g_Enemy[i].scl.x * EnemyColliderSize[enemyType].x,
					g_Enemy[i].scl.y* EnemyColliderSize[enemyType].y, g_Enemy[i].scl.z* EnemyColliderSize[enemyType].z, g_Enemy[i].enemyID);
				groundCheckPos = D3DXVECTOR3(pos.x, pos.y - g_Enemy[i].scl.y* EnemyColliderSize[enemyType].y * 0.5f, pos.z);
				g_Enemy[i].GroundCheckColliderIdx = CreateCollider3DBox(collider3DTag_groundCheck, groundCheckPos, g_Enemy[i].scl.x * EnemyColliderSize[enemyType].x,
					g_Enemy[i].scl.y* EnemyColliderSize[enemyType].y * ENEMY_GROUND_CHECK_THIN, g_Enemy[i].scl.z* EnemyColliderSize[enemyType].z, g_Enemy[i].enemyID);
				g_Enemy[i].ChaseColliderIdx = CreateCollider3DBox(collider3DTag_EnemyChaseCheck, pos, g_Enemy[i].ChaceArea.x,
					g_Enemy[i].ChaceArea.y, g_Enemy[i].ChaceArea.z, g_Enemy[i].enemyID);
				g_Enemy[i].Canfly = true;
				break;
			case EnemyType_star:
				g_Enemy[i].BodyColliderIdx = CreateCollider3DBox(collider3DTag_enemy, pos, g_Enemy[i].scl.x * EnemyColliderSize[enemyType].x,
					g_Enemy[i].scl.y* EnemyColliderSize[enemyType].y, g_Enemy[i].scl.z* EnemyColliderSize[enemyType].z, g_Enemy[i].enemyID);
				groundCheckPos = D3DXVECTOR3(pos.x, pos.y - g_Enemy[i].scl.y* EnemyColliderSize[enemyType].y * 0.5f, pos.z);
				g_Enemy[i].GroundCheckColliderIdx = CreateCollider3DBox(collider3DTag_groundCheck, groundCheckPos, g_Enemy[i].scl.x * EnemyColliderSize[enemyType].x,
					g_Enemy[i].scl.y* EnemyColliderSize[enemyType].y * ENEMY_GROUND_CHECK_THIN, g_Enemy[i].scl.z* EnemyColliderSize[enemyType].z, g_Enemy[i].enemyID);
				g_Enemy[i].ChaseColliderIdx = CreateCollider3DBox(collider3DTag_EnemyChaseCheck, pos, g_Enemy[i].ChaceArea.x,
					g_Enemy[i].ChaceArea.y, g_Enemy[i].ChaceArea.z, g_Enemy[i].enemyID);
				g_Enemy[i].Canfly = true;
				break;
			case EnemyType_coin:
				g_Enemy[i].BodyColliderIdx = CreateCollider3DBox(collider3DTag_enemy, pos, g_Enemy[i].scl.x * EnemyColliderSize[enemyType].x,
					g_Enemy[i].scl.y* EnemyColliderSize[enemyType].y, g_Enemy[i].scl.z* EnemyColliderSize[enemyType].z, g_Enemy[i].enemyID);
				groundCheckPos = D3DXVECTOR3(pos.x, pos.y - g_Enemy[i].scl.y* EnemyColliderSize[enemyType].y * 0.5f, pos.z);
				g_Enemy[i].GroundCheckColliderIdx = CreateCollider3DBox(collider3DTag_groundCheck, groundCheckPos, g_Enemy[i].scl.x * EnemyColliderSize[enemyType].x,
					g_Enemy[i].scl.y* EnemyColliderSize[enemyType].y * ENEMY_GROUND_CHECK_THIN, g_Enemy[i].scl.z* EnemyColliderSize[enemyType].z, g_Enemy[i].enemyID);
				g_Enemy[i].ChaseColliderIdx = CreateCollider3DBox(collider3DTag_EnemyChaseCheck, pos, g_Enemy[i].ChaceArea.x,
					g_Enemy[i].ChaceArea.y, g_Enemy[i].ChaceArea.z, g_Enemy[i].enemyID);
				g_Enemy[i].Canfly = true;
				break;
			case EnemyType_quickSand:
				g_Enemy[i].BodyColliderIdx = CreateCollider3DBox(collider3DTag_enemy, pos, g_Enemy[i].scl.x * EnemyColliderSize[enemyType].x,
					g_Enemy[i].scl.y* EnemyColliderSize[enemyType].y, g_Enemy[i].scl.z* EnemyColliderSize[enemyType].z, g_Enemy[i].enemyID);
				groundCheckPos = D3DXVECTOR3(pos.x, pos.y - g_Enemy[i].scl.y* EnemyColliderSize[enemyType].y * 0.5f, pos.z);
				g_Enemy[i].GroundCheckColliderIdx = CreateCollider3DBox(collider3DTag_groundCheck, groundCheckPos, g_Enemy[i].scl.x * EnemyColliderSize[enemyType].x,
					g_Enemy[i].scl.y* EnemyColliderSize[enemyType].y * ENEMY_GROUND_CHECK_THIN, g_Enemy[i].scl.z* EnemyColliderSize[enemyType].z, g_Enemy[i].enemyID);
				g_Enemy[i].ChaseColliderIdx = CreateCollider3DBox(collider3DTag_EnemyChaseCheck, pos, g_Enemy[i].ChaceArea.x,
					g_Enemy[i].ChaceArea.y, g_Enemy[i].ChaceArea.z, g_Enemy[i].enemyID);
				g_Enemy[i].Canfly = true;
				break;
			case EnemyType_mushroomSpring:
				g_Enemy[i].BodyColliderIdx = CreateCollider3DBox(collider3DTag_enemy, pos, g_Enemy[i].scl.x * EnemyColliderSize[enemyType].x,
					g_Enemy[i].scl.y* EnemyColliderSize[enemyType].y, g_Enemy[i].scl.z* EnemyColliderSize[enemyType].z, g_Enemy[i].enemyID);
				groundCheckPos = D3DXVECTOR3(pos.x, pos.y - g_Enemy[i].scl.y* EnemyColliderSize[enemyType].y * 0.5f, pos.z);
				g_Enemy[i].GroundCheckColliderIdx = CreateCollider3DBox(collider3DTag_groundCheck, groundCheckPos, g_Enemy[i].scl.x * EnemyColliderSize[enemyType].x,
					g_Enemy[i].scl.y* EnemyColliderSize[enemyType].y * ENEMY_GROUND_CHECK_THIN, g_Enemy[i].scl.z* EnemyColliderSize[enemyType].z, g_Enemy[i].enemyID);
				g_Enemy[i].ChaseColliderIdx = CreateCollider3DBox(collider3DTag_EnemyChaseCheck, pos, g_Enemy[i].ChaceArea.x,
					g_Enemy[i].ChaceArea.y, g_Enemy[i].ChaceArea.z, g_Enemy[i].enemyID);
				g_Enemy[i].Canfly = true;
				break;
			case EnemyType_Sign:
				g_Enemy[i].BodyColliderIdx = CreateCollider3DBox(collider3DTag_enemy, pos, g_Enemy[i].scl.x * EnemyColliderSize[enemyType].x,
					g_Enemy[i].scl.y* EnemyColliderSize[enemyType].y, g_Enemy[i].scl.z* EnemyColliderSize[enemyType].z, g_Enemy[i].enemyID);
				groundCheckPos = D3DXVECTOR3(pos.x, pos.y - g_Enemy[i].scl.y* EnemyColliderSize[enemyType].y * 0.5f, pos.z);
				g_Enemy[i].GroundCheckColliderIdx = CreateCollider3DBox(collider3DTag_groundCheck, groundCheckPos, g_Enemy[i].scl.x * EnemyColliderSize[enemyType].x,
					g_Enemy[i].scl.y* EnemyColliderSize[enemyType].y * ENEMY_GROUND_CHECK_THIN, g_Enemy[i].scl.z* EnemyColliderSize[enemyType].z, g_Enemy[i].enemyID);
				g_Enemy[i].ChaseColliderIdx = CreateCollider3DBox(collider3DTag_EnemyChaseCheck, pos, g_Enemy[i].ChaceArea.x,
					g_Enemy[i].ChaceArea.y, g_Enemy[i].ChaceArea.z, g_Enemy[i].enemyID);
				g_Enemy[i].Canfly = true;
				break;
			default:
				break;
			}
			break;
		}
	}

	return IdxEnemy;
}

int SetEnemyToID(int num, DX11_MODEL model, D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scl, int enemyType, D3DXVECTOR3 reborn_pos)
{
	int IdxEnemy = -1;

	IdxEnemy = num;
	g_Enemy[num].enemyID = num;

	g_Enemy[num].Use = true;
	g_Enemy[num].model = model;
	g_Enemy[num].pos = pos;
	g_Enemy[num].rot = rot;
	g_AnimOriRot[num] = rot;
	g_AnimOriPos[num] = pos;
	g_Enemy[num].scl = scl;
	g_AnimOriScl[num] = scl;
	g_Enemy[num].OriginPos = pos;
	g_Enemy[num].Reborn_pos = reborn_pos;
	g_Enemy[num].Status = EnemyStatus_Idle;


	g_Enemy[num].move_time = clock();

	g_AnimTimer[num] = 0.0f;

	//collider 設定
	g_Enemy[num].enemyType = enemyType;
	D3DXVECTOR3 groundCheckPos;

	switch (g_Enemy[num].enemyType)
	{
	case EnemyType_mushroom:
		g_Enemy[num].BodyColliderIdx = CreateCollider3DBox(collider3DTag_enemy, pos, g_Enemy[num].scl.x * EnemyColliderSize[enemyType].x,
			g_Enemy[num].scl.y* EnemyColliderSize[enemyType].y, g_Enemy[num].scl.z* EnemyColliderSize[enemyType].z, g_Enemy[num].enemyID);
		groundCheckPos = D3DXVECTOR3(pos.x, pos.y - g_Enemy[num].scl.y* EnemyColliderSize[enemyType].y * 0.5f, pos.z);
		g_Enemy[num].GroundCheckColliderIdx = CreateCollider3DBox(collider3DTag_groundCheck, groundCheckPos, g_Enemy[num].scl.x * EnemyColliderSize[enemyType].x,
			g_Enemy[num].scl.y* EnemyColliderSize[enemyType].y * ENEMY_GROUND_CHECK_THIN, g_Enemy[num].scl.z* EnemyColliderSize[enemyType].z, g_Enemy[num].enemyID);
		g_Enemy[num].ChaseColliderIdx = CreateCollider3DBox(collider3DTag_EnemyChaseCheck, pos, g_Enemy[num].ChaceArea.x,
			g_Enemy[num].ChaceArea.y, g_Enemy[num].ChaceArea.z, g_Enemy[num].enemyID);
		g_Enemy[num].Canfly = true;
		break;
	case EnemyType_wind:
		g_Enemy[num].BodyColliderIdx = CreateCollider3DBox(collider3DTag_enemy, pos, g_Enemy[num].scl.x * EnemyColliderSize[enemyType].x,
			g_Enemy[num].scl.y* EnemyColliderSize[enemyType].y, g_Enemy[num].scl.z* EnemyColliderSize[enemyType].z, g_Enemy[num].enemyID);
		groundCheckPos = D3DXVECTOR3(pos.x, pos.y - g_Enemy[num].scl.y* EnemyColliderSize[enemyType].y * 0.5f, pos.z);
		g_Enemy[num].GroundCheckColliderIdx = CreateCollider3DBox(collider3DTag_groundCheck, groundCheckPos, g_Enemy[num].scl.x * EnemyColliderSize[enemyType].x,
			g_Enemy[num].scl.y* EnemyColliderSize[enemyType].y * ENEMY_GROUND_CHECK_THIN, g_Enemy[num].scl.z* EnemyColliderSize[enemyType].z, g_Enemy[num].enemyID);
		g_Enemy[num].ChaseColliderIdx = CreateCollider3DBox(collider3DTag_EnemyChaseCheck, pos, g_Enemy[num].ChaceArea.x,
			g_Enemy[num].ChaceArea.y, g_Enemy[num].ChaceArea.z, g_Enemy[num].enemyID);
		g_Enemy[num].Canfly = true;
		break;
	case EnemyType_water:
		g_Enemy[num].BodyColliderIdx = CreateCollider3DBox(collider3DTag_enemy, pos, g_Enemy[num].scl.x * EnemyColliderSize[enemyType].x,
			g_Enemy[num].scl.y* EnemyColliderSize[enemyType].y, g_Enemy[num].scl.z* EnemyColliderSize[enemyType].z, g_Enemy[num].enemyID);
		groundCheckPos = D3DXVECTOR3(pos.x, pos.y - g_Enemy[num].scl.y* EnemyColliderSize[enemyType].y * 0.5f, pos.z);
		g_Enemy[num].GroundCheckColliderIdx = CreateCollider3DBox(collider3DTag_groundCheck, groundCheckPos, g_Enemy[num].scl.x * EnemyColliderSize[enemyType].x,
			g_Enemy[num].scl.y* EnemyColliderSize[enemyType].y * ENEMY_GROUND_CHECK_THIN, g_Enemy[num].scl.z* EnemyColliderSize[enemyType].z, g_Enemy[num].enemyID);
		g_Enemy[num].ChaseColliderIdx = CreateCollider3DBox(collider3DTag_EnemyChaseCheck, pos, g_Enemy[num].ChaceArea.x,
			g_Enemy[num].ChaceArea.y, g_Enemy[num].ChaceArea.z, g_Enemy[num].enemyID);
		g_Enemy[num].Canfly = true;
		break;
	case EnemyType_gem:
		g_Enemy[num].BodyColliderIdx = CreateCollider3DBox(collider3DTag_enemy, pos, g_Enemy[num].scl.x * EnemyColliderSize[enemyType].x,
			g_Enemy[num].scl.y* EnemyColliderSize[enemyType].y, g_Enemy[num].scl.z* EnemyColliderSize[enemyType].z, g_Enemy[num].enemyID);
		groundCheckPos = D3DXVECTOR3(pos.x, pos.y - g_Enemy[num].scl.y* EnemyColliderSize[enemyType].y * 0.5f, pos.z);
		g_Enemy[num].GroundCheckColliderIdx = CreateCollider3DBox(collider3DTag_groundCheck, groundCheckPos, g_Enemy[num].scl.x * EnemyColliderSize[enemyType].x,
			g_Enemy[num].scl.y* EnemyColliderSize[enemyType].y * ENEMY_GROUND_CHECK_THIN, g_Enemy[num].scl.z* EnemyColliderSize[enemyType].z, g_Enemy[num].enemyID);
		g_Enemy[num].ChaseColliderIdx = CreateCollider3DBox(collider3DTag_EnemyChaseCheck, pos, g_Enemy[num].ChaceArea.x,
			g_Enemy[num].ChaceArea.y, g_Enemy[num].ChaceArea.z, g_Enemy[num].enemyID);
		g_Enemy[num].Canfly = true;
		break;
	case EnemyType_star:
		g_Enemy[num].BodyColliderIdx = CreateCollider3DBox(collider3DTag_enemy, pos, g_Enemy[num].scl.x * EnemyColliderSize[enemyType].x,
			g_Enemy[num].scl.y* EnemyColliderSize[enemyType].y, g_Enemy[num].scl.z* EnemyColliderSize[enemyType].z, g_Enemy[num].enemyID);
		groundCheckPos = D3DXVECTOR3(pos.x, pos.y - g_Enemy[num].scl.y* EnemyColliderSize[enemyType].y * 0.5f, pos.z);
		g_Enemy[num].GroundCheckColliderIdx = CreateCollider3DBox(collider3DTag_groundCheck, groundCheckPos, g_Enemy[num].scl.x * EnemyColliderSize[enemyType].x,
			g_Enemy[num].scl.y* EnemyColliderSize[enemyType].y * ENEMY_GROUND_CHECK_THIN, g_Enemy[num].scl.z* EnemyColliderSize[enemyType].z, g_Enemy[num].enemyID);
		g_Enemy[num].ChaseColliderIdx = CreateCollider3DBox(collider3DTag_EnemyChaseCheck, pos, g_Enemy[num].ChaceArea.x,
			g_Enemy[num].ChaceArea.y, g_Enemy[num].ChaceArea.z, g_Enemy[num].enemyID);
		g_Enemy[num].Canfly = true;
		break;
	case EnemyType_coin:
		g_Enemy[num].BodyColliderIdx = CreateCollider3DBox(collider3DTag_enemy, pos, g_Enemy[num].scl.x * EnemyColliderSize[enemyType].x,
			g_Enemy[num].scl.y* EnemyColliderSize[enemyType].y, g_Enemy[num].scl.z* EnemyColliderSize[enemyType].z, g_Enemy[num].enemyID);
		groundCheckPos = D3DXVECTOR3(pos.x, pos.y - g_Enemy[num].scl.y* EnemyColliderSize[enemyType].y * 0.5f, pos.z);
		g_Enemy[num].GroundCheckColliderIdx = CreateCollider3DBox(collider3DTag_groundCheck, groundCheckPos, g_Enemy[num].scl.x * EnemyColliderSize[enemyType].x,
			g_Enemy[num].scl.y* EnemyColliderSize[enemyType].y * ENEMY_GROUND_CHECK_THIN, g_Enemy[num].scl.z* EnemyColliderSize[enemyType].z, g_Enemy[num].enemyID);
		g_Enemy[num].ChaseColliderIdx = CreateCollider3DBox(collider3DTag_EnemyChaseCheck, pos, g_Enemy[num].ChaceArea.x,
			g_Enemy[num].ChaceArea.y, g_Enemy[num].ChaceArea.z, g_Enemy[num].enemyID);
		g_Enemy[num].Canfly = true;
		break;
	case EnemyType_quickSand:
		g_Enemy[num].BodyColliderIdx = CreateCollider3DBox(collider3DTag_enemy, pos, g_Enemy[num].scl.x * EnemyColliderSize[enemyType].x,
			g_Enemy[num].scl.y* EnemyColliderSize[enemyType].y, g_Enemy[num].scl.z* EnemyColliderSize[enemyType].z, g_Enemy[num].enemyID);
		groundCheckPos = D3DXVECTOR3(pos.x, pos.y - g_Enemy[num].scl.y* EnemyColliderSize[enemyType].y * 0.5f, pos.z);
		g_Enemy[num].GroundCheckColliderIdx = CreateCollider3DBox(collider3DTag_groundCheck, groundCheckPos, g_Enemy[num].scl.x * EnemyColliderSize[enemyType].x,
			g_Enemy[num].scl.y* EnemyColliderSize[enemyType].y * ENEMY_GROUND_CHECK_THIN, g_Enemy[num].scl.z* EnemyColliderSize[enemyType].z, g_Enemy[num].enemyID);
		g_Enemy[num].ChaseColliderIdx = CreateCollider3DBox(collider3DTag_EnemyChaseCheck, pos, g_Enemy[num].ChaceArea.x,
			g_Enemy[num].ChaceArea.y, g_Enemy[num].ChaceArea.z, g_Enemy[num].enemyID);
		g_Enemy[num].Canfly = true;
		break; 
	case EnemyType_mushroomSpring:
		g_Enemy[num].BodyColliderIdx = CreateCollider3DBox(collider3DTag_enemy, pos, g_Enemy[num].scl.x * EnemyColliderSize[enemyType].x,
			g_Enemy[num].scl.y* EnemyColliderSize[enemyType].y, g_Enemy[num].scl.z* EnemyColliderSize[enemyType].z, g_Enemy[num].enemyID);
		groundCheckPos = D3DXVECTOR3(pos.x, pos.y - g_Enemy[num].scl.y* EnemyColliderSize[enemyType].y * 0.5f, pos.z);
		g_Enemy[num].GroundCheckColliderIdx = CreateCollider3DBox(collider3DTag_groundCheck, groundCheckPos, g_Enemy[num].scl.x * EnemyColliderSize[enemyType].x,
			g_Enemy[num].scl.y* EnemyColliderSize[enemyType].y * ENEMY_GROUND_CHECK_THIN, g_Enemy[num].scl.z* EnemyColliderSize[enemyType].z, g_Enemy[num].enemyID);
		g_Enemy[num].ChaseColliderIdx = CreateCollider3DBox(collider3DTag_EnemyChaseCheck, pos, g_Enemy[num].ChaceArea.x,
			g_Enemy[num].ChaceArea.y, g_Enemy[num].ChaceArea.z, g_Enemy[num].enemyID);
		g_Enemy[num].Canfly = true;
		break;
	case EnemyType_Sign:
		g_Enemy[num].BodyColliderIdx = CreateCollider3DBox(collider3DTag_enemy, pos, g_Enemy[num].scl.x * EnemyColliderSize[enemyType].x,
			g_Enemy[num].scl.y* EnemyColliderSize[enemyType].y, g_Enemy[num].scl.z* EnemyColliderSize[enemyType].z, g_Enemy[num].enemyID);
		groundCheckPos = D3DXVECTOR3(pos.x, pos.y - g_Enemy[num].scl.y* EnemyColliderSize[enemyType].y * 0.5f, pos.z);
		g_Enemy[num].GroundCheckColliderIdx = CreateCollider3DBox(collider3DTag_groundCheck, groundCheckPos, g_Enemy[num].scl.x * EnemyColliderSize[enemyType].x,
			g_Enemy[num].scl.y* EnemyColliderSize[enemyType].y * ENEMY_GROUND_CHECK_THIN, g_Enemy[num].scl.z* EnemyColliderSize[enemyType].z, g_Enemy[num].enemyID);
		g_Enemy[num].ChaseColliderIdx = CreateCollider3DBox(collider3DTag_EnemyChaseCheck, pos, g_Enemy[num].ChaceArea.x,
			g_Enemy[num].ChaceArea.y, g_Enemy[num].ChaceArea.z, g_Enemy[num].enemyID);
		g_Enemy[num].Canfly = true;
		break;
	}

	return IdxEnemy;
}

void ReleaseEnemy(int id)
{
	if (g_Enemy[id].Use) {
		g_Enemy[id].Use = false;
		ReleaseCollider3D(g_Enemy[id].BodyColliderIdx);
		ReleaseCollider3D(g_Enemy[id].GroundCheckColliderIdx);
		ReleaseCollider3D(g_Enemy[id].ChaseColliderIdx);
		//ReleaseShadow(g_Enemy[id].shadowIdx);
	}
}
void ReleaseEnemy(ENEMY &enemy)
{
	ReleaseEnemy(enemy.enemyID);
}

void InputUpdate(ENEMY & enemy) {

}

void PhysicsUpdate(ENEMY & enemy) {
	srand((unsigned)time(NULL));
	if (enemy.Status != EnemyStatus_Dead ) 
	{
		////==============普通移動
		D3DXVECTOR3 HitPoint, normal;
		D3DXVECTOR3 gravity = D3DXVECTOR3(0.0f,-1.0f,0.0f);//エネミーの重力方向
		
		int rand_num= rand() % (100);
		int rand_num2 = rand() % (2);
		//回転の重力をここでgravity変数を変更
		clock_t end = clock();
		int move_time= (end - enemy.move_time) ;			//単位:1/1000秒
		int a;
		switch (a)
		{
		case 0:
			//重力設定
			gravity = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
			//回転の時のみ、エネミーのROTを初期化		モデルの方向を修正
			if (世界が回転の変数)
			{
				enemy.rot = D3DXVECTOR3(0.0f, 0.0f, D3DX_PI / 2);
				世界が回転の変数初期化;
			}
			
			//重力回転の後、新しい地面を判定
			RayHitEnemy(enemy.pos, &HitPoint, &normal, enemy.enemyID, gravity);
			//先ずは地面に立て
			enemy.pos = HitPoint;

			if ((move_time % 5000) == 0　|| 世界が回転の変数==true) //もし一段時間を進むだら移動方向を変更
			{
				//ランダムの移動方向を決定
				if (rand_num2 > 1)
				{
					enemy.rot.x = D3DX_PI / rand_num;
					enemy.speed = D3DXVECTOR3(0.0f, (-sinf(D3DX_PI / rand_num)), (-cosf(D3DX_PI / rand_num)));
				}
				else
				{
					enemy.rot.x = -D3DX_PI / rand_num;
					enemy.speed = D3DXVECTOR3(0.0f, (+sinf(D3DX_PI / rand_num)), (+cosf(D3DX_PI / rand_num)));
				}
			}
			
			break;
		case 1:
			//重力設定
			gravity = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
			//回転の時のみ、エネミーのROTを初期化		モデルの方向を修正
			if (世界が回転の変数)
			{
				enemy.rot = D3DXVECTOR3(0.0f, 0.0f, -D3DX_PI / 2);
				世界が回転の変数初期化;
			}

			//重力回転の後、新しい地面を判定
			RayHitEnemy(enemy.pos, &HitPoint, &normal, enemy.enemyID, gravity);
			//先ずは地面に立て
			enemy.pos = HitPoint;

			if ((move_time % 5000) == 0 || 世界が回転の変数 == true) //もし一段時間を進むだら移動方向を変更
			{
				//ランダムの移動方向を決定
				if (rand_num2 > 1)
				{
					enemy.rot.x = D3DX_PI / rand_num;
					enemy.speed = D3DXVECTOR3(0.0f, (-sinf(D3DX_PI / rand_num)), (-cosf(D3DX_PI / rand_num)));
				}
				else
				{
					enemy.rot.x = -D3DX_PI / rand_num;
					enemy.speed = D3DXVECTOR3(0.0f, (+sinf(D3DX_PI / rand_num)), (+cosf(D3DX_PI / rand_num)));
				}
			}
			break;
		case 2:
			//重力設定
			gravity = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
			//回転の時のみ、エネミーのROTを初期化		モデルの方向を修正
			if (世界が回転の変数)
			{
				enemy.rot = D3DXVECTOR3(D3DX_PI / 2, 0.0f, 0.0f);
				世界が回転の変数初期化;
			}
			
			//重力回転の後、新しい地面を判定
			RayHitEnemy(enemy.pos, &HitPoint, &normal, enemy.enemyID, gravity);
			//先ずは地面に立て
			enemy.pos = HitPoint;
			if ((move_time % 5000) == 0 || 世界が回転の変数 == true) //もし一段時間を進むだら移動方向を変更
			{
				//ランダムの移動方向を決定
				if (rand_num2 > 1)
				{
					enemy.rot.x = D3DX_PI / rand_num;
					enemy.speed = D3DXVECTOR3((-sinf(D3DX_PI / rand_num)), (-cosf(D3DX_PI / rand_num)), 0.0f);
				}
				else
				{
					enemy.rot.x = -D3DX_PI / rand_num;
					enemy.speed = D3DXVECTOR3((+sinf(D3DX_PI / rand_num)), (+cosf(D3DX_PI / rand_num)), 0.0f);
				}
			}
			break;
		case 3:
			//重力設定
			gravity = D3DXVECTOR3(-D3DX_PI / 2, 0.0f, -1.0f);
			//回転の時のみ、エネミーのROTを初期化		モデルの方向を修正
			if (世界が回転の変数)
			{
				enemy.rot = D3DXVECTOR3(0.0f, 0.0f, D3DX_PI / 2);
				世界が回転の変数初期化;
			}
			
			//重力回転の後、新しい地面を判定
			RayHitEnemy(enemy.pos, &HitPoint, &normal, enemy.enemyID, gravity);
			//先ずは地面に立て
			enemy.pos = HitPoint;
			if ((move_time % 5000) == 0 || 世界が回転の変数 == true) //もし一段時間を進むだら移動方向を変更　
			{
				//ランダムの移動方向を決定
				if (rand_num2 > 1)
				{
					enemy.rot.x = D3DX_PI / rand_num;
					enemy.speed = D3DXVECTOR3((-sinf(D3DX_PI / rand_num)), (-cosf(D3DX_PI / rand_num)), 0.0f);
				}
				else
				{
					enemy.rot.x = -D3DX_PI / rand_num;
					enemy.speed = D3DXVECTOR3((+sinf(D3DX_PI / rand_num)), (+cosf(D3DX_PI / rand_num)), 0.0f);
				}
			}
			break;
		case 4:
			//重力設定
			gravity = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
			//回転の時のみ、エネミーのROTを初期化		モデルの方向を修正
			if (世界が回転の変数)
			{
				enemy.rot = D3DXVECTOR3(0.0f, 0.0f, D3DX_PI);
				世界が回転の変数初期化;
			}
			
			//重力回転の後、新しい地面を判定
			RayHitEnemy(enemy.pos, &HitPoint, &normal, enemy.enemyID, gravity);
			//先ずは地面に立て
			enemy.pos = HitPoint;
			if ((move_time % 5000) == 0 || 世界が回転の変数 == true)//もし一段時間を進むだら移動方向を変更　
			{
				//ランダムの移動方向を決定
				if (rand_num2 > 1)
				{
					enemy.rot.x = D3DX_PI / rand_num;
					enemy.speed = D3DXVECTOR3((-sinf(D3DX_PI / rand_num)), 0.0f, (-cosf(D3DX_PI / rand_num)));
				}
				else
				{
					enemy.rot.x = -D3DX_PI / rand_num;
					enemy.speed = D3DXVECTOR3((+sinf(D3DX_PI / rand_num)), 0.0f, (+cosf(D3DX_PI / rand_num)));
				}
			}
			break;
		case 5:
			//重力設定
			gravity = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
			//回転の時のみ、エネミーのROTを初期化		モデルの方向を修正
			if (世界が回転の変数)
			{
				enemy.rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
				世界が回転の変数初期化;
			}
			//重力回転の後、新しい地面を判定
			RayHitEnemy(enemy.pos, &HitPoint, &normal, enemy.enemyID, gravity);
			//先ずは地面に立て
			enemy.pos = HitPoint;
			if ((move_time % 5000) == 0 || 世界が回転の変数 == true) //もし一段時間を進むだら移動方向を変更
			{
				//ランダムの移動方向を決定
				if (rand_num2 > 1)
				{
					enemy.rot.x = D3DX_PI / rand_num;
					enemy.speed = D3DXVECTOR3((-sinf(D3DX_PI / rand_num)), 0.0f, (-cosf(D3DX_PI / rand_num)));
				}
				else
				{
					enemy.rot.x = -D3DX_PI / rand_num;
					enemy.speed = D3DXVECTOR3((+sinf(D3DX_PI / rand_num)), 0.0f, (+cosf(D3DX_PI / rand_num)));
				}
			}
			break;
		default:
			break;
		}
		//エネミーの前の捜査COLLIDERを回転とPOS修正



		//Enemyの前のCOLLIDERの中に、プレイヤとOBB判定　もし当たったら移動方向を変更
		bool ans=CheckHitByIDOBB(g_Player->bodyColliderIdx, enemy.BodyColliderIdx);
		
		if (ans)
		{
			D3DXVECTOR3 dir2 = g_Player->pos - enemy.pos;
			switch (a)
			{
			case 0:
				//このタイプの重力はgravity = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
				dir2.x = 0.0f;
				
				break;
			case 1:
				//このタイプの重力はgravity = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
				dir2.x = 0.0f;
				break;
			case 2:
				//このタイプの重力はgravity = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
				dir2.z = 0.0f;
				break;
			case 3:
				//このタイプの重力はgravity = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
				dir2.z = 0.0f;
				break;
			case 4:
				//このタイプの重力はgravity = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
				dir2.y = 0.0f;
				break;
			case 5:
				//このタイプの重力はgravity = D3DXVECTOR3(0.0f, -1.0f, 0.0f);
				dir2.y = 0.0f;
				break;
			default:
				break;
			}
			//エネミーの移動方向を修正
			D3DXVec3Normalize(&dir2, &dir2);
			enemy.speed += dir2 * enemy.decelerate;
		}
		else
		{
			//エネミーの移動方向を修正
			D3DXVECTOR3 dir = enemy.speed;
			D3DXVec3Normalize(&dir, &dir);
			enemy.speed += dir * enemy.decelerate;
		}
		//新しい移動方向を加算
		enemy.pos += enemy.speed;

		//新しい壁を当たり判定




		//重生点を修正
		/*
		D3DXMATRIX mtxWorld;
		D3DXMATRIX mtxRot, mtxTranslate;
		D3DXVECTOR3 NewPos;
		D3DXVECTOR3 OldPos = enemy.Reborn_pos;
		// 移動を反映
		D3DXMatrixTranslation(&mtxTranslate, OldPos.x, OldPos.y, OldPos.z);
		D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTranslate);
		// 回転を反映
		D3DXMatrixRotationYawPitchRoll(&mtxRot, deltaRot.x, deltaRot.y, deltaRot.x);
		D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

		D3DXVec3TransformCoord(&NewPos, &NewPos, &mtxWorld);
		
		enemy.Reborn_pos = NewPos;
		*/
		





		//if (enemy.CanMove == false) {
		//	D3DXVECTOR3 dir = enemy.speed;
		//	D3DXVECTOR3 dirXZ = D3DXVECTOR3(enemy.speed.x, 0.0f, enemy.speed.z);
		//	D3DXVec3Normalize(&dir, &dir);
		//	D3DXVec3Normalize(&dirXZ, &dirXZ);
		//	D3DXVECTOR3 speedXZ = D3DXVECTOR3(enemy.speed.x, 0.0f, enemy.speed.z);

		//	if (enemy.Canfly) {
		//		if (D3DXVec3Length(&enemy.speed) > 0) {
		//			enemy.speed -= dir * enemy.decelerate;
		//		}
		//		if (D3DXVec3Length(&enemy.speed) <= enemy.decelerate) {
		//			enemy.speed = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		//		}
		//		if (D3DXVec3Length(&enemy.speed) >= enemy.maxMoveSpeed) {
		//			enemy.speed = dir * enemy.maxMoveSpeed;
		//		}
		//	}

		//	if (!enemy.Canfly) {
		//		if (D3DXVec3Length(&speedXZ) > 0) {
		//			enemy.speed -= dirXZ * enemy.decelerate;
		//		}
		//		if (D3DXVec3Length(&speedXZ) <= enemy.decelerate) {
		//			enemy.speed.x = 0.0f;
		//			enemy.speed.z = 0.0f;
		//		}

		//		if (D3DXVec3Length(&speedXZ) >= enemy.maxMoveSpeed) {
		//			enemy.speed.x = dirXZ.x * enemy.maxMoveSpeed;
		//			enemy.speed.z = dirXZ.z * enemy.maxMoveSpeed;
		//		}
		//		if (enemy.speed.y < VALUE_MAX_FALLEN_SPEED) {
		//			enemy.speed.y = VALUE_MAX_FALLEN_SPEED;
		//		}
		//	}
		//	if(enemy.Status == EnemyStatus_Chase)enemy.rot.y = (atan2f(dirXZ.z, dirXZ.x) - D3DX_PI * 0.5f) * -1.0f;   //coordinate(counter clockwise), obj rotation(clockwise) : different way => -1

		//}


		////==============ライン移動  =====> change to point to point accoring to obj speed? オブジェクトのスピードによって、点からの動くにするほうがいいかな？
		//=線形移動 
		//if (enemy.CanMove == true) {
		//	if (enemy.canmoveAxis == 0) { // X
		//		int nowNo = (int)enemy.LMtime; //Liner move time
		//		int maxNo = (sizeof(g_MoveTblX) / sizeof(D3DXVECTOR3));
		//		int nextNo = (nowNo + 1) % maxNo;
		//		D3DXVECTOR3 pos = g_MoveTblX[nextNo] - g_MoveTblX[nowNo];
		//		float nowTime = enemy.LMtime - nowNo;
		//		pos *= nowTime;
		//		if (nowNo == 0)enemy.rot.y = D3DX_PI * 1.5f;
		//		if (nowNo == 1)enemy.rot.y = D3DX_PI * 0.5f;
		//		if (nowNo == 2)enemy.rot.y = D3DX_PI * 1.5f;
		//		enemy.CaculatedPos = enemy.pos + g_MoveTblX[nowNo] + pos;
		//		enemy.LMtime += g_MoveSpdX[nowNo];
		//		if ((int)enemy.LMtime >= maxNo) {
		//			enemy.LMtime = 0.0f;
		//		}
		//	}
		//	else if (enemy.canmoveAxis == 1) { // Y
		//		int nowNo = (int)enemy.LMtime; //Liner move time
		//		int maxNo = (sizeof(g_MoveTblY) / sizeof(D3DXVECTOR3));
		//		int nextNo = (nowNo + 1) % maxNo;
		//		D3DXVECTOR3 pos = g_MoveTblY[nextNo] - g_MoveTblY[nowNo];
		//		float nowTime = enemy.LMtime - nowNo;
		//		pos *= nowTime;
		//		enemy.CaculatedPos = enemy.pos + g_MoveTblY[nowNo] + pos;
		//		enemy.LMtime += g_MoveSpdX[nowNo];
		//		if ((int)enemy.LMtime >= maxNo) {
		//			enemy.LMtime = 0.0f;
		//		}
		//	}
		//	else if (enemy.canmoveAxis == 2) { // Z
		//		int nowNo = (int)enemy.LMtime; //Liner move time
		//		int maxNo = (sizeof(g_MoveTblZ) / sizeof(D3DXVECTOR3));
		//		int nextNo = (nowNo + 1) % maxNo;
		//		D3DXVECTOR3 pos = g_MoveTblZ[nextNo] - g_MoveTblZ[nowNo];
		//		float nowTime = enemy.LMtime - nowNo;
		//		pos *= nowTime;
		//		if(nowNo == 0)enemy.rot.y = D3DX_PI * 1.0f;
		//		if(nowNo == 1)enemy.rot.y = D3DX_PI * 0.0f;
		//		if(nowNo == 2)enemy.rot.y = D3DX_PI * 1.0f;
		//		enemy.CaculatedPos = enemy.pos + g_MoveTblZ[nowNo] + pos;
		//		enemy.LMtime += g_MoveSpdX[nowNo];
		//		if ((int)enemy.LMtime >= maxNo) {
		//			enemy.LMtime = 0.0f;
		//		}
		//	}
		//	if (enemy.canmoveAxis == -1)enemy.CanMove = false;
		//}
		//else {
		//	if (enemy.canmoveAxis != -1 && enemy.Status == EnemyStatus_Idle)enemy.CanMove = true;
		//}
	}
	//==========Wall Detect \\ checkBB only
	//int targetCID;
	//if (CheckHitByTagReturnCID(enemy.BodyColliderIdx, collider3DTag_normalPlatform, &targetCID)) {
	//	COLLIDER3D *Collider = GetCollider();
	//	COLLIDER3D enemyC = Collider[enemy.BodyColliderIdx];
	//	COLLIDER3D targetC = Collider[targetCID];
	//	D3DXVECTOR3 speedDir;
	//	D3DXVECTOR3 speedXZ = D3DXVECTOR3(enemy.speed.x, 0.0f, enemy.speed.z);
	//	D3DXVec3Normalize(&speedDir, &speedXZ);
	//
	//	if ((enemy.pos.y - enemyC.hy*0.4f < targetC.pos.y + targetC.hy*0.5f) && (enemy.pos.y + enemyC.hy*0.4f > targetC.pos.y - targetC.hy*0.5f)) {//高さ=>地面になった 0.5 => 0.4 : prevent from wall detect
	//
	//		if (enemyC.pos.z + enemyC.lz*0.5f > targetC.pos.z + targetC.lz*0.5f &&
	//			(enemyC.pos.x - enemyC.wx*0.5f < (targetC.pos.x + targetC.wx*0.5f) && (enemyC.pos.x + enemyC.wx*0.5f > (targetC.pos.x - targetC.wx*0.5f)))) {
	//			if (speedDir.z < 0)enemy.speed.z = 0.0f;
	//			enemy.pos.z = Collider[targetCID].pos.z + Collider[targetCID].lz*0.5f + Collider[enemy.BodyColliderIdx].lz*0.5f;
	//		}
	//		else if (enemyC.pos.x + enemyC.wx*0.5f > targetC.pos.x + targetC.wx*0.5f &&
	//			(enemyC.pos.z - enemyC.lz*0.5f < (targetC.pos.z + targetC.lz*0.5f) && (enemyC.pos.z + enemyC.lz*0.5f > (targetC.pos.z - targetC.lz*0.5f)))) {
	//			if (speedDir.x < 0)enemy.speed.x = 0.0f;
	//			enemy.pos.x = Collider[targetCID].pos.x + Collider[targetCID].wx*0.5f + Collider[enemy.BodyColliderIdx].wx*0.5f;
	//		}
	//		else if (enemyC.pos.z - enemyC.lz*0.5f < targetC.pos.z - targetC.lz*0.5f &&
	//			(enemyC.pos.x - enemyC.wx*0.5f < (targetC.pos.x + targetC.wx*0.5f) && (enemyC.pos.x + enemyC.wx*0.5f > (targetC.pos.x - targetC.wx*0.5f)))) {
	//			if (speedDir.z > 0)enemy.speed.z = 0.0f;
	//			enemy.pos.z = Collider[targetCID].pos.z - Collider[targetCID].lz*0.5f - Collider[enemy.BodyColliderIdx].lz*0.5f;
	//		}
	//		else if (enemyC.pos.x - enemyC.wx*0.5f < targetC.pos.x - targetC.wx*0.5f &&
	//			(enemyC.pos.z - enemyC.lz*0.5f < (targetC.pos.z + targetC.lz*0.5f) && (enemyC.pos.z + enemyC.lz*0.5f > (targetC.pos.z - targetC.lz*0.5f)))) {
	//			if (speedDir.x > 0)enemy.speed.x = 0.0f;
	//			enemy.pos.x = Collider[targetCID].pos.x - Collider[targetCID].wx*0.5f - Collider[enemy.BodyColliderIdx].wx*0.5f;
	//		}
	//		if (enemy.On_Moving_Plat) {
	//			enemy.On_Moving_Plat_Pos.x = enemy.pos.x - Collider[enemy.On_Moving_PlatID].pos.x;
	//			enemy.On_Moving_Plat_Pos.z = enemy.pos.z - Collider[enemy.On_Moving_PlatID].pos.z;
	//		}
	//	}
	//}

	//==========ground Detect
	//if (!enemy.Canfly && enemy.Status != EnemyStatus_Dead) {
	//	COLLIDER3D *Collider = GetCollider();
	//	if (CheckHitByTagReturnCID(enemy.GroundCheckColliderIdx, collider3DTag_normalPlatform, &targetCID)) {
	//		if (enemy.speed.y <= 0.0f && enemy.pos.y + Collider[enemy.BodyColliderIdx].hy*0.5f >= (Collider[targetCID].pos.y + Collider[targetCID].hy*0.5f)) { // player's head higher than target's head
	//			if (GetEnemyDebugCtrlID() != enemy.enemyID) {
	//				//====== NOT On moving platform
	//				if (enemy.On_Moving_Plat == false) {
	//					enemy.On_Moving_Plat_Pos.x = enemy.pos.x - Collider[targetCID].pos.x;
	//					enemy.On_Moving_Plat_Pos.z = enemy.pos.z - Collider[targetCID].pos.z;
	//					enemy.On_Moving_Plat = true;
	//					enemy.On_Moving_PlatID = targetCID;
	//					if (enemy.Exit_Moving_Plat) {
	//						enemy.speed.x = 0.0f;
	//						enemy.speed.z = 0.0f;
	//					}
	//				}
	//				//====== On moving platform
	//				if (enemy.On_Moving_Plat == true) {
	//					enemy.Moving_Plat_Speed = Collider[targetCID].pos - enemy.Moving_Plat_PrePos;
	//				}
	//				//===================================
	//				enemy.Moving_Plat_PrePos = Collider[targetCID].pos;
	//				enemy.pos.x = Collider[targetCID].pos.x + enemy.On_Moving_Plat_Pos.x;
	//				enemy.pos.z = Collider[targetCID].pos.z + enemy.On_Moving_Plat_Pos.z;
	//				enemy.On_Moving_Plat_Pos.x += enemy.speed.x;
	//				enemy.On_Moving_Plat_Pos.z += enemy.speed.z;
	//				enemy.isGround = true;
	//			}
	//		}
	//	}
	//	else {
	//		enemy.isGround = false;
	//		enemy.On_Moving_Plat = false;
	//	}
	//	//プラットフォームから出る瞬間
	//	if (enemy.On_Moving_PlatPre == true && enemy.On_Moving_Plat == false) {
	//		enemy.speed.x += enemy.Moving_Plat_Speed.x;
	//		enemy.speed.y += enemy.Moving_Plat_Speed.y;
	//		enemy.speed.z += enemy.Moving_Plat_Speed.z;
	//		enemy.maxMoveSpeed += D3DXVec3Length(&enemy.Moving_Plat_Speed); // 暫時放大最高速度 => 在平台上跳躍使speed.xz正常變動
	//		if (enemy.maxMoveSpeed >= 100.0f) enemy.maxMoveSpeed = VALUE_MAX_MOVE_SPEED; // prevent extream speed
	//		enemy.Moving_Plat_PrePos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	//		if (enemy.Moving_Plat_Speed.x != 0.0f || enemy.Moving_Plat_Speed.z != 0.0f)enemy.Exit_Moving_Plat = true;
	//	}
	//}

	//Gravity
	//if (!enemy.Canfly && enemy.gravityScale > 0.0f  && enemy.Status != EnemyStatus_Dead) {
	//	if (!enemy.isGround)enemy.speed.y -= enemy.gravityScale;
	//	else enemy.speed.y = 0.0f;
	//}
	//Dead
	/*if (enemy.Status == EnemyStatus_Dead) {
		enemy.speed.y -= VALUE_DEAD_GRAVITY;
		if (enemy.speed.y < VALUE_MAX_FALLEN_SPEED) {
			enemy.speed.y = VALUE_MAX_FALLEN_SPEED;
		}
	}*/

	//Update Speed
	enemy.pos += enemy.speed;
	//rot
	if (enemy.enemyType == EnemyType_star || enemy.enemyType == EnemyType_coin)enemy.rot.y += D3DX_PI * 0.01f;

}

void CollisionUpdate(ENEMY & enemy)
{
	//collider position
	if (enemy.CanMove) { 
		SetPositionCollider3D(enemy.BodyColliderIdx, enemy.CaculatedPos); 
		D3DXVECTOR3 groundCheckPos = D3DXVECTOR3(enemy.CaculatedPos.x, enemy.CaculatedPos.y - enemy.scl.y* EnemyColliderSize[enemy.enemyType].y * 0.5f, enemy.CaculatedPos.z);
		SetPositionCollider3D(enemy.GroundCheckColliderIdx, groundCheckPos);
		SetPositionCollider3D(enemy.ChaseColliderIdx, enemy.CaculatedPos); 
	}
	else {
		SetPositionCollider3D(enemy.BodyColliderIdx, enemy.pos); 
		D3DXVECTOR3 groundCheckPos = D3DXVECTOR3(enemy.pos.x, enemy.pos.y - enemy.scl.y* EnemyColliderSize[enemy.enemyType].y * 0.5f, enemy.pos.z);
		SetPositionCollider3D(enemy.GroundCheckColliderIdx, groundCheckPos);
		SetPositionCollider3D(enemy.ChaseColliderIdx, enemy.pos);
	}
	//=====collision
	//Chase
	if (enemy.CanChase) {
		COLLIDER3D *collider = GetCollider();
		collider[enemy.ChaseColliderIdx].canDraw = true;
		if (CheckHitByTag(enemy.ChaseColliderIdx, collider3DTag_player)) {
			enemy.Status = EnemyStatus_Chase;
			D3DXVECTOR3 scl = { CHASEING_AREA_X, CHASEING_AREA_Y, CHASEING_AREA_Z };
			SetScaleCollider3D(enemy.ChaseColliderIdx, scl);
			if (enemy.CanMove) {
				enemy.pos = enemy.CaculatedPos;
				enemy.movingOriginPos = enemy.CaculatedPos;
			}
		}
		else {
			if (enemy.Status == EnemyStatus_Chase) {
				enemy.Status = EnemyStatus_Back;
				D3DXVECTOR3 scl = { CHASE_DETECT_AREA_X, CHASE_DETECT_AREA_Y, CHASE_DETECT_AREA_Z };
				SetScaleCollider3D(enemy.ChaseColliderIdx, scl);
			}
		}
	}
	else {
		COLLIDER3D *collider = GetCollider();
		collider[enemy.ChaseColliderIdx].canDraw = false;
		
	}
	//===fall Down Area
	if (CheckHitByTag(enemy.BodyColliderIdx, collider3DTag_fallDownArea)) {
		ReleaseEnemy(enemy);
	}

}

void StatusUpdate(ENEMY &enemy) {
	// Chacing target
	if (enemy.Status == EnemyStatus_Chase) {
		if (enemy.CanChase) {
			PLAYER *player = GetPlayer();
			D3DXVECTOR3 dir = player->pos - enemy.pos;
			D3DXVec3Normalize(&dir, &dir);
			enemy.ChaseDir += (dir - enemy.ChaseDir) * VALUE_CHASE_TURN;
			D3DXVec3Normalize(&enemy.ChaseDir, &enemy.ChaseDir);
			if (!enemy.Canfly) dir.y = 0.0f;
			dir.y = 0.0f;
			if (D3DXVec3Length(&(player->pos - enemy.pos)) > VALUE_CHASE_OBJ_LIMIT) {
				enemy.ChaseDir.y = 0.0f;
				enemy.speed += enemy.ChaseDir * VALUE_MOVE;
				enemy.CanMove = false;
			}
		}
		else {
			enemy.Status = EnemyStatus_Back;
			D3DXVECTOR3 scl = { CHASE_DETECT_AREA_X, CHASE_DETECT_AREA_Y, CHASE_DETECT_AREA_Z };
			SetScaleCollider3D(enemy.ChaseColliderIdx, scl);
		}
	}
	
	// Back to position
	if (enemy.Status == EnemyStatus_Back) {
		D3DXVECTOR3 dir;
		if (enemy.canmoveAxis == -1) dir = enemy.OriginPos - enemy.pos;
		else dir = enemy.movingOriginPos - enemy.pos;
		if (!enemy.Canfly) dir.y = 0.0f;
		float dis = D3DXVec3Length(&dir);
		D3DXVec3Normalize(&dir, &dir);
		enemy.speed += dir * VALUE_MOVE;
		if (dis <= CHASE_SPEED) {
			enemy.Status = EnemyStatus_Idle;
			if (enemy.canmoveAxis != -1)enemy.CanMove = true;
			enemy.speed = D3DXVECTOR3(0, 0, 0);
		}
	}

	//Dead => Destroy
	if (enemy.Status == EnemyStatus_Dead) {
		if (enemy.CanMove) {
			enemy.pos = enemy.CaculatedPos;
			enemy.CanMove = false;
		}
		enemy.deadToDestroyTimer += 1;
		if (enemy.deadToDestroyTimer >= DEAD_TO_DESTROY_TIME) {
			enemy.deadToDestroyTimer = 0.0f;
			SetParticle(enemy.pos, D3DXVECTOR3(enemy.hitDirXZ.x, 0.0f, enemy.hitDirXZ.z), Particletype_EnemyDead, 30);
			PlaySound(SOUND_LABEL_SE_sword);
			ReleaseEnemy(enemy);
		}
		enemy.rot.x += D3DX_PI * -0.02f;

	}
	
	// Print status
	//if(enemy.Status == EnemyStatus_Chase)PrintDebugProc("status: Chase\n", enemy.Status);
	//if(enemy.Status == EnemyStatus_Idle)PrintDebugProc("status: Idle\n", enemy.Status);
	//if(enemy.Status == EnemyStatus_Move)PrintDebugProc("status: Move\n", enemy.Status);
	//if(enemy.Status == EnemyStatus_Air)PrintDebugProc("status: Air\n", enemy.Status);
	//if(enemy.Status == EnemyStatus_Back)PrintDebugProc("status: Back\n", enemy.Status);
	//if(enemy.Status == EnemyStatus_Dead)PrintDebugProc("status: Dead\n", enemy.Status);

}

void AnimUpdate(ENEMY &enemy) {
	
	//====waterCrys
	if (enemy.enemyType == EnemyType_water) {
		if (enemy.Status == EnemyStatus_Idle) {
			g_AnimTimer[enemy.enemyID] ++;
			if (g_AnimTimer[enemy.enemyID] <= AnimTime_WATER_IDLE) { // rot
				enemy.rot.y += D3DXToRadian(360 / AnimTime_WATER_IDLE);
			}
			else {
				g_AnimTimer[enemy.enemyID] = 0.0f;
				enemy.rot = g_AnimOriRot[enemy.enemyID];
				enemy.pos = g_AnimOriPos[enemy.enemyID];
			}
		}
		if (enemy.Status == EnemyStatus_Hit) {
			g_AnimTimer[enemy.enemyID] ++;
			if (g_AnimTimer[enemy.enemyID] <= 1 * AnimTime_WATER_HIT / 3.0f) {//pos part1
				enemy.rot.y += D3DXToRadian(3*360 / AnimTime_WATER_HIT);
			}
			else if (g_AnimTimer[enemy.enemyID] <= 2 * AnimTime_WATER_HIT / 3.0f) { // part2
				enemy.rot.y += D3DXToRadian(2*360 / AnimTime_WATER_HIT);
			}
			else if (g_AnimTimer[enemy.enemyID] <= 3 * AnimTime_WATER_HIT / 3.0f) { // part2
				enemy.rot.y += D3DXToRadian(1*360 / AnimTime_WATER_HIT);
			}
			else { // finish
				enemy.Status = EnemyStatus_Idle;
				g_AnimTimer[enemy.enemyID] = 0.0f;
				enemy.rot = g_AnimOriRot[enemy.enemyID];
			}
		}
	}
	//====mushroom spring
	if (enemy.enemyType == EnemyType_mushroomSpring) {
		if (enemy.Status == EnemyStatus_Hit) {
			g_AnimTimer[enemy.enemyID] ++;
			if (g_AnimTimer[enemy.enemyID] <= AnimTime_MUSHROOM_HIT / 4.0f) { // part 1
				enemy.scl.x *= 1.1f;
				enemy.scl.z *= 1.1f;
				enemy.scl.y /= 1.1f;
			}
			else if (g_AnimTimer[enemy.enemyID] <= 2*AnimTime_MUSHROOM_HIT / 4.0f) {// part 2
				enemy.scl.x /= 1.1f;
				enemy.scl.z /= 1.1f;
				enemy.scl.y *= 1.1f;
			}
			else if (g_AnimTimer[enemy.enemyID] <= 3 * AnimTime_MUSHROOM_HIT / 4.0f) {// part 3
				enemy.scl.x *= 1.07f;
				enemy.scl.z *= 1.07f;
				enemy.scl.y /= 1.07f;
			}
			else if (g_AnimTimer[enemy.enemyID] <= 4 * AnimTime_MUSHROOM_HIT / 4.0f) {// part 4
				enemy.scl.x /= 1.07f;
				enemy.scl.z /= 1.07f;
				enemy.scl.y *= 1.07f;
			}
			else { // finish
				enemy.Status = EnemyStatus_Idle;
				g_AnimTimer[enemy.enemyID] = 0.0f;
				enemy.scl = g_AnimOriScl[enemy.enemyID];
			}
		}
	}
}

void RebuildAllEnemyAfterLoad()
{
	//rebuild data & collider
	for (int i = 0; i < MAX_ENEMY; i++) {
		if (g_Enemy[i].Use == true) { //have save data ==> replace
			DX11_MODEL model = g_Model[g_Enemy[i].enemyType];
			D3DXVECTOR3 pos = g_Enemy[i].pos;
			D3DXVECTOR3 rot = g_Enemy[i].rot;
			D3DXVECTOR3 scl = g_Enemy[i].scl;
			int enemyType = g_Enemy[i].enemyType;

			SetEnemyToID(i, model, pos, rot, scl, enemyType);
		}
	}
}

void UpdateEnemyModel(int ID)
{
	g_Enemy[ID].model = g_Model[g_Enemy[ID].enemyType];
}

void ChangeEnemyType(int ID, int Type) {
	g_Enemy[ID].enemyType = Type;
	switch (g_Enemy[ID].enemyType)
	{
	case EnemyType_mushroom:
		g_Enemy[ID].Canfly = true;
		break;
	case EnemyType_wind:
		g_Enemy[ID].Canfly = true;
		break;
	case EnemyType_water:
		g_Enemy[ID].Canfly = true;
		break;
	case EnemyType_gem:
		g_Enemy[ID].Canfly = true;
		break;
	case EnemyType_star:
		g_Enemy[ID].Canfly = true;
		break;
	case EnemyType_coin:
		g_Enemy[ID].Canfly = true;
		break;
	default:
		break;
	}
}

void HitEnemy(int id, D3DXVECTOR3 hitDirXZ)
{
	if (g_Enemy[id].enemyType == EnemyType_mushroom) {
		PlaySound(SOUND_LABEL_SE_hit2);
		g_Enemy[id].Status = EnemyStatus_Dead;
		//SetCollider3DEnable(g_Enemy[id].BodyColliderIdx, false);
		SetCollider3DEnable(g_Enemy[id].ChaseColliderIdx, false);
		SetCollider3DEnable(g_Enemy[id].GroundCheckColliderIdx, false);

		g_Enemy[id].hitDirXZ = hitDirXZ;
		g_Enemy[id].speed.x = VALUE_DEAD_MOVE_SPEED * hitDirXZ.x;
		g_Enemy[id].speed.y = VALUE_DEAD_FLY_SPEED;
		g_Enemy[id].speed.z = VALUE_DEAD_MOVE_SPEED * hitDirXZ.z;
		//==========Caculate Rot
		g_Enemy[id].rot.y = (atan2f(hitDirXZ.z, hitDirXZ.x) - D3DX_PI * 0.5f) * -1.0f;
		g_Enemy[id].scl.z *= -1.0f;
	}
	if (g_Enemy[id].enemyType == EnemyType_mushroomSpring ){
		g_Enemy[id].Status = EnemyStatus_Hit;
		g_AnimTimer[id] = 0.0f;
		g_Enemy[id].scl = g_AnimOriScl[id];
	}
	if (g_Enemy[id].enemyType == EnemyType_water) {
		g_Enemy[id].Status = EnemyStatus_Hit;
	}

}

void SetEnemyAnimOriScl(int ID, D3DXVECTOR3 scl) {
	g_AnimOriScl[ID] = scl;
}
void SetEnemyAnimOriRot(int ID, D3DXVECTOR3 rot) {
	g_AnimOriRot[ID] = rot;
}
void SetEnemyAnimOriPos(int ID, D3DXVECTOR3 pos) {
	g_AnimOriPos[ID] = pos;
}