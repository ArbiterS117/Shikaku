//=============================================================================
//
// ゲーム画面処理 [gameScene.cpp]
// Author : GP11B132 14 ゴショウケン
//
//=============================================================================
#include "gameScene.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "fade.h"
#include "saveFile.h"
#include "player.h"
#include "enemy.h"
#include "Collider3D.h"
#include "shadow.h"
#include "bullet.h"
#include "score.h"
#include "sound.h"
#include "particle.h"
#include "sprite.h"
#include "platform.h"
#include "debugproc.h"
#include "light.h"
#include "EditTool.h"
#include "ground.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define STOPFRAMECDTIME		 (10)		
#define FINISHFADETIME		 (120)		
#define BLINKCDTIMER         (10)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
static void InputUpdate();
static void DebugProc();
//*****************************************************************************
// グローバル変数
//*****************************************************************************
bool bIsStoppingFrame = false;
int  StoppingFrameLeft = 0;
int  StopFrameCD = 0;
int  gameTime = 0;

////======Edit Mode
////Edit Menu
//enum EditMenuID
//{
//	EditMenu_Platform,
//	EditMenu_Enemy,
//	EditMenu_MAX,
//
//};
//static bool EditMode = false;
//static int editMenuSelectedID = 0; //(-1 ~ .....)  -1 : deActive 
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitGameScene(void)
{
	InitLight();

	// カメラ処理の初期化 (3D)
	InitCamera();

	// 影の初期化処理  // player , enemy の前
	InitShadow();

	// colliderの初期化処理 // player , enemy の前
	InitCollider3D();

	// プレイヤーの初期化
	InitPlayer();

	// エネミーの初期化
	InitEnemy();

	InitPlatform();

	//バレット
	InitBullet();

	//Ground
	InitGround();

	//other
	InitScore();

	// ライトを有効化
	SetLightEnable(true);
	SetDepthEnable(true);

	// 背面ポリゴンをカリング
	SetCullingMode(CULL_MODE_BACK);

	// ゲーム音楽の再生
	PlaySound(SOUND_LABEL_BGM_2);

	//===================SYS
	InitParticle();

	//fallDawnArea
	CreateCollider3DBox(collider3DTag_fallDownArea, D3DXVECTOR3(0.0f, -6000.0f, 0.0f), 240000.0f, 4000.0f, 120000.0f, 0);

	CreateCollider3DBox(collider3DTag_GroundArea_HeartWorld,  D3DXVECTOR3(0.0f, 0.0f, 2000.0f), 8000.0f, 8000.0f, 8000.0f, 0);
	CreateCollider3DBox(collider3DTag_GroundArea_HtoW,        D3DXVECTOR3(0.0f, 0.0f, 10500.0f), 10000.0f, 8000.0f, 9000.0f, 0);
	CreateCollider3DBox(collider3DTag_GroundArea_WonderLand,  D3DXVECTOR3(0.0f, 0.0f, 24500.0f), 9000.0f, 8000.0f, 19000.0f, 0);
	CreateCollider3DBox(collider3DTag_GroundArea_WonderLand2, D3DXVECTOR3(-8000.0f, 0.0f, 24500.0f), 9000.0f, 10000.0f, 19000.0f, 0);
	CreateCollider3DBox(collider3DTag_GroundArea_WtoS,        D3DXVECTOR3(-18000.0f, 0.0f, 24500.0f), 11000.0f, 10000.0f, 5000.0f, 0);
	CreateCollider3DBox(collider3DTag_GroundArea_SandWorld,   D3DXVECTOR3(-34500.0f, 0.0f, 24500.0f), 22000.0f, 10000.0f, 20000.0f, 0);
	CreateCollider3DBox(collider3DTag_GroundArea_StoS,        D3DXVECTOR3(-50250.0f, 0.0f, 24500.0f), 10650.0f, 10000.0f, 10000.0f, 0);
	CreateCollider3DBox(collider3DTag_GroundArea_SnowWorld,   D3DXVECTOR3(-61000.0f, 0.0f, 24500.0f), 12000.0f, 20000.0f, 20000.0f, 0);
	

	InitEditTool();

	gameTime = 0;
	//===================DEBUG

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitGameScene(void)
{
	// プレイヤーの終了処理
	UninitPlayer();

	// エネミーの終了処理
	UninitEnemy();

	// バレット
	UninitBullet();

	UninitPlatform();

	UninitGround();

	// 影の終了処理
	UninitShadow();

	// colliderの終了処理
	UninitCollider3D();

	//other
	UninitScore();

	//Particle
	UninitParticle();

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateGameScene(void)
{
	DebugProc();

	UpdateLight();

	//========== Timer
	//Stop Frame
	if (StoppingFrameLeft > 0) {
		bIsStoppingFrame = true;
		StoppingFrameLeft -= 1;
	}
	else {
		bIsStoppingFrame = false;
	}

	if (StopFrameCD > 0)StopFrameCD -= 1;

	//game Time
	gameTime += 1;
	if (gameTime >= 60) {
		AddScoreTime(1); // 60 frame 1 sec
		gameTime = 0;
	}

	//========== セーブ＆ロードのテスト
#ifdef _DEBUG	// デバッグ情報を表示する
	
	if (GetKeyboardTrigger(DIK_S))
	{
		SaveData();
}

	if (GetKeyboardTrigger(DIK_L))
	{
		LoadData();
	}

#endif
	

	//========== game
	// プレイヤーの更新処理
	UpdatePlayer();

	// エネミーの更新処理
	UpdateEnemy();

	//バレット
	UpdateBullet();


	UpdatePlatform();

	// colliderの更新処理
	UpdateCollider3D();

	UpdateParticle();

	// 影の更新処理
	UpdateShadow();

	//===== 当たり判定
	//CheckHit();

	//other
	UpdateScore();


	//========== Input
	InputUpdate();

	//========== EditTool
	UpdateEditTool();

	//========== debug
#ifdef _DEBUG
	if (GetKeyboardTrigger(DIK_RETURN)) {
		SetFade(FADE_OUT, MODE_RESULT);
	}
#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawGameScene(void)
{
	// 背景の描画処理
	//DrawBG();
	SetCamera(); // reset camera projection after drawing bg

	// 影の描画処理
	DrawShadow(); // DepthEnable(false) => 上の描画を無視して必ず描画

	// colliderの描画処理
	DrawCollider3D(); // DepthEnable(false) => 上の描画を無視して必ず描画

	// エネミーの描画処理
	DrawEnemy();

	// プレイヤーの描画処理
	DrawPlayer();

	DrawGround();   // draw before plat // 透明

	DrawPlatform(); // 透明

	//========半透明
	//バレット
	DrawBullet();

	//other
	// 2D

	DrawParticle();// DepthEnable(false) => 上の描画を無視して必ず描画

	DrawScore();

	//=====EditTool
	DrawEditTool();

}

//================ Stop frame

void EffectStopFrame(int frame) {
	if (!bIsStoppingFrame && StopFrameCD == 0) {
		StoppingFrameLeft = frame;
		StopFrameCD = STOPFRAMECDTIME;
	}
}

bool IsStoppingFrame()
{
	return bIsStoppingFrame;
}

void InputUpdate() {

}

void DebugProc() {

}