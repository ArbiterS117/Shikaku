//=============================================================================
//
// プレーヤ処理 [player.cpp]
// Author : GP11B132 14 ゴショウケン
//
//=============================================================================
#include "main.h"
#include "gameScene.h"
#include "input.h"
#include "camera.h"
#include "player.h"
#include "enemy.h"
#include "renderer.h"
#include "Collider3D.h"
#include "shadow.h"
#include "bullet.h"
#include "player_partsModelAnim.h"
#include "sound.h"
#include "particle.h"
#include "score.h"
#include "fade.h"
#include "platform.h"
#include "light.h"
#include "EditTool.h"
#include "debugproc.h"
#include "ground.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	VALUE_MOVE			   (1.5f)					// 移動量 (加速)
#define	VALUE_AIR_MOVE		   (0.6f)					// 空中移動量 (加速)
#define VALUE_DECELERATE       (0.8f)
#define VALUE_AIR_DECELERATE   (0.3f)
#define	VALUE_ROTATE		   (D3DX_PI * 0.02f)		// 回転量
#define	VALUE_GRAVITY		   (1.5f)		            // 重力
#define	VALUE_THROWINGGRAVITY  (0.5f)		            // カード投げる重力
#define	VALUE_CANSLOPECAL      (0.6f)		            // 1 => flat ; 0.99~0.6 => can walk on slope ; 0.6~ => like wall 

#define PLAYER_SHADOW_SIZE	   (3.0f)					// 影の大きさ

//COLLIDER
#define PLAYER_BODYCHECKC_XZ       (60.0f)	
#define PLAYER_BODYCHECKC_Y        (100.0f)	
#define PLAYER_GROUNDCHECKC_Y      (PLAYER_BODYCHECKC_Y * 0.5)		
#define PLAYER_GROUND_OFFSET_Y     (PLAYER_BODYCHECKC_Y * 0.5 + 30.0f)		
#define PLAYER_RANGE_COLLIDER_SIZE (500.0f)		

#define CAN_STAIR_HEIGHT                 (-20.0f)					
#define WALL_SLOPE_DEGREE                (0.7f) // 1 ~ 0  => normal Vector.y					
#define	VALUE_SLOPE_ADDSPD_DEGREE        (0.2f)		            
#define	PLAYER_MAX_SLOPEUP_MOVE_SPEED    (20.0f)		            
#define	PLAYER_MAX_SLOPEDOWN_MOVE_SPEED  (10.0f)		            

//SPEED
#define PLAYER_MAX_MOVE_SPEED           (15.0f)
#define PLAYER_MAX_FALLEN_SPEED         (-30.0f)
#define PLAYER_JUMP_SPEED               (40.0f)
#define PLAYER_DASH_SPEED               (25.0f)
#define PLAYER_AIRDASH_SPEED            (25.0f)
#define PLAYER_DASH_JUMP_SPEED          (25.0f)
#define PLAYER_THROWING_JUMP_SPEED      (17.0f)
#define PLAYER_STEPCARD_JUMP_SPEED      (22.0f)
#define PLAYER_STEPCARD_DASHJUMP_SPEED  (30.0f)
#define PLAYER_WIND_MOVE_SPEED          (25.0f)
#define PLAYER_WIND_JUMP_SPEED          (42.0f)
#define PLAYER_BOB_JUMP_SPEED           (5.0f)
#define PLAYER_MAX_BOB_MOVE_SPEED       (10.0f)
#define PLAYER_MAX_SAND_MOVE_SPEED      (4.0f)
#define PLAYER_SAND_JUMP_SPEED          (30.0f)
#define PLAYER_MUSH_JUMP_SPEED          (60.0f)
#define	VALUE_SNADGRAVITY               (0.5f)		            
#define	VALUE_SNADJUMP_GRAVITY          (7.5f)		            
#define	VALUE_BOB_GRAVITY               (0.15f)		            

#define PLAYER_MAX_MOVE_SPEED_ICE       (25.0f)
#define	VALUE_ICE_MOVE			        (0.2f)
#define VALUE_ICE_DECELERATE            (0.1f)

//TIMER
#define PLAYER_DASHTIME            (15)                      // ダッシュ時間
#define PLAYER_DASHCDTIME          (45)                      // ダッシュCD時間
#define PLAYER_THROWING_TIME       (15)                      // カード投げる時間
#define PLAYER_USEABILITYCD        (60)                      // カード能力を使うCD時間
#define PLAYER_WATERABILITYCD      (10)                       // カード能力アピールCD時間
#define PLAYER_BOBSTATETIME        (420)                     // 泡能力時間
#define PLAYER_SANDSTATETIME       (1000)                    // 砂状態時間
#define PLAYER_MOVESMOKECDTIME     (6)                       // 歩いての煙

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
static void InputUpdate(void);
static void PhysicsUpdate(PLAYER &player);
static void CollisionUpdate(PLAYER &player);
static void PlayerStatusUpdate(PLAYER &player);
static void AnimStatusUpdate(PLAYER &player);

static void ChangeForceY(float force);
static void MovingPlatPhysicsUpdate(PLAYER &player, int InRangeOBJNum, int *OBJInRangeList);
//*****************************************************************************
// グローバル変数
//*****************************************************************************
static PLAYER				g_Player;						// プレイヤー

static float                backToMAXMoveSpeed = 0.2f;

bool getFinalStar = false;

D3DXVECTOR3 playerRebornPos[12] = {
	{350.0f, 550.0f, 0.0f},
	{150.0f, 1550.0f, 7500.0f},
	{122.0, 1000.0f, 14420.0f},
	{1365.0f, 1050.0f, 24530.0f},
	{-8600.0f, 2030.f, 33440.0f},
	{-9220.0f, 6445.0f, 39965.0f},
	{-11470.0f, 1285.0f, 24560.0f},
	{-25168.0f, 1590.0f, 24418.0f},
	{-29347.0f, 1630.0f, 18731.0f},
	{-45594.0f, 1100.0f, 24053.0f},
	{-56177.0f, 1183.0f, 24131.0f},
	{-57627.0f, 6405.0f, 19265.0f},
};
static int Max_Birth_Point = sizeof(playerRebornPos) / sizeof(playerRebornPos[0]);
int CurPlayerRebornPos = 0;

static bool g_canCreateSPParticle = true;
static bool g_canCtrl = true;
static int  g_getStarAnimTimer = 0;
static int  waterAbilityTimer = 0;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitPlayer(void)
{
	g_Player.Use = true;

	// 位置・回転・スケールの初期設定
	g_Player.pos = playerRebornPos[0];
	g_Player.rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_Player.scl = D3DXVECTOR3(3.0f, 3.0f, 3.0f);
	g_Player.prePos = g_Player.pos;

	//physics
	g_Player.speed = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_Player.dirXZ = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
	g_Player.maxMoveSpeed = PLAYER_MAX_MOVE_SPEED;
	g_Player.decelerate   = VALUE_DECELERATE;
	g_Player.gravityScale = VALUE_GRAVITY;
	g_Player.dashSpeed    = PLAYER_DASH_SPEED;
	g_Player.dashDir      = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	////===system
	g_Player.isGround            = false;
	g_Player.isGroundPrev        = true;
	g_Player.canDash             = true;
	g_Player.dashCDTimer         = 0;
	g_Player.isDash              = false;
	g_Player.dashTimer           = 0;
	g_Player.isFloat             = false;
	g_Player.cardBulletID        = -1;
	g_Player.cardInHand          = true;
	g_Player.canThrowCard        = true;
	g_Player.isThrowing          = false;
	g_Player.throwingTimer       = 0;
	g_Player.moveSmokeTimer      = 0;
	g_Player.playerAbility       = CardAbility_none;
	g_Player.canUseCard          = true;
	g_Player.canUseCardTrigger   = false;
	g_Player.canUseCardTimer     = 0;
	g_Player.isInBobStatus       = false;
	g_Player.bobStateTimer       = 0;
	g_Player.isInSandStatus		 = false;
	g_Player.SandStateTimer		 = 0;
	g_Player.Anim_Status         = AnimStatus_Idel;
	g_Player.backToMAXMove       = false;
	g_Player.Anim_isJumping      = false;
	g_Player.Anim_isWindBoost    = false;
	g_Player.Anim_isCatchedCard  = false;
	g_Player.Anim_isStepOnMushroomSpring = false;
	g_Player.Anim_GetStar        = false;
	g_Player.isOnSlope           = false;
	g_Player.isInIceStatus       = false;

	////====moving plat
	g_Player.Exit_Moving_Plat  = false;

	////====階層アニメーション parts init
	InitPlayerPartAnim();

	////====collider
	g_Player.bodyColliderIdx         = CreateCollider3DBox(collider3DTag_player, g_Player.pos, PLAYER_BODYCHECKC_XZ, PLAYER_BODYCHECKC_Y, PLAYER_BODYCHECKC_XZ, 0);
	g_Player.CircleRangeColliderIdx = CreateCollider3DCircle(collider3DTag_CircleRange, g_Player.pos, PLAYER_RANGE_COLLIDER_SIZE, 0);
	g_Player.groundCheckColliderIdx  = CreateCollider3DBox(collider3DTag_groundCheck, D3DXVECTOR3(g_Player.pos.x, g_Player.pos.y - PLAYER_GROUNDCHECKC_Y - PLAYER_BODYCHECKC_XZ * 0.5f, g_Player.pos.z), 10.0f, 10.0f, 10.0f, 0);
	
	//shadow
	D3DXVECTOR3 pos = g_Player.pos;
	pos.y = 0.0f;
	//g_Player.shadowIdx = CreateShadow(pos, PLAYER_SHADOW_SIZE, PLAYER_SHADOW_SIZE);

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitPlayer(void)
{
	UninitPlayerPartAnim();

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdatePlayer(void)
{
	//================Input
	InputUpdate();

	//================Physics
	PhysicsUpdate(g_Player);

	//================collider detect
	CollisionUpdate(g_Player);

	//================PlayerStatus & other
	PlayerStatusUpdate(g_Player);

	// 影もプレイヤーの位置に合わせる
	D3DXVECTOR3 pos = g_Player.pos;
	pos.y = 0.0f;
	//SetPositionShadow(g_Player.shadowIdx, pos);

	////================階層アニメーション更新
	AnimStatusUpdate(g_Player);
	UpdatePlayerPartAnim();

	// ポイントライトのテスト
	{
		LIGHT *light = GetLightData(2);
		D3DXVECTOR3 pos = g_Player.pos;
		pos.y += 20.0f;

		light->Position = pos;
		light->Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		light->Ambient = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		light->Direction = D3DXVECTOR3(0.2f, 1.0f, 0.2f);
		light->Type = LIGHT_TYPE_POINT;
		light->Attenuation = 40.0f;	// 減衰距離
		light->Enable = true;
		SetLightData(1, light);
	}

	//================== debug
#ifdef _DEBUG	// デバッグ情報を表示する
	char* str = GetDebugStr();
	//sprintf(&str[strlen(str)], " posX : %.2f   posY : %.2f   posZ : %.2f ", g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
	//sprintf(&str[strlen(str)], " MAXSPD : %.2f ", g_Player.maxMoveSpeed);
	//sprintf(&str[strlen(str)], " spdX : %.2f   spdY : %.2f   spdZ : %.2f ", g_Player.speed.x, g_Player.speed.y, g_Player.speed.z);

#endif

	//=====debug
	if (GetKeyboardTrigger(DIK_0)) {
		if (GetGameSceneEditMode() == false) {
			g_Player.pos = playerRebornPos[CurPlayerRebornPos];
			AddMiss(1);
		}
	}

	if (fabs(g_Player.pos.x) > 120000.0f || fabs(g_Player.pos.y) > 120000.0f || fabs(g_Player.pos.z) > 120000.0f) {
		g_Player.pos = playerRebornPos[CurPlayerRebornPos];
		g_Player.speed = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}
	
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawPlayer(void)
{
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;

	//SetAlphaTestEnable(true);

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&g_Player.mtxWorld);

	// スケールを反映
	D3DXMatrixScaling(&mtxScl, g_Player.scl.x, g_Player.scl.y, g_Player.scl.z);
	D3DXMatrixMultiply(&g_Player.mtxWorld, &g_Player.mtxWorld, &mtxScl);

	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, g_Player.rot.y, g_Player.rot.x, g_Player.rot.z);
	//D3DXQUATERNION q;
	//D3DXMatrixRotationQuaternion(&mtxRot, &q);
	D3DXMatrixMultiply(&g_Player.mtxWorld, &g_Player.mtxWorld, &mtxRot);

	// 移動を反映
	D3DXMatrixTranslation(&mtxTranslate, g_Player.pos.x, g_Player.pos.y, g_Player.pos.z);
	D3DXMatrixMultiply(&g_Player.mtxWorld, &g_Player.mtxWorld, &mtxTranslate);

	// ワールドマトリックスの設定
	SetWorldMatrix(&g_Player.mtxWorld);

	// モデル描画
	//DrawModel(&g_Player.model);

	//階層アニメーション 親を描画してから、PARTS(子)の描画
	DrawPlayerPartAnim();

	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);

	//SetAlphaTestEnable(false);
}


//=============================================================================
// プレイヤー情報を取得
//=============================================================================
PLAYER *GetPlayer(void)
{
	return &g_Player;
}

void InputUpdate(void) {

	if (GetKeyboardTrigger(DIK_NUMPAD2))g_canCtrl = !g_canCtrl;
	if (GetKeyboardTrigger(DIK_TAB))g_canCtrl = !g_canCtrl;
	

	CAMERA *cam = GetCamera();
	// 移動させちゃう
	D3DXVECTOR3 dir = g_Player.pos - GetCamera()->pos;
	dir = D3DXVECTOR3{ dir.x, 0.0f, dir.z }; // don't want to fly (move according to dir.y)
	D3DXVECTOR3 Ndir = { dir.z, 0.0f, -dir.x };
	D3DXVec3Normalize(&dir, &dir);
	D3DXVec3Normalize(&Ndir, &Ndir);
	D3DXVECTOR3 InputDir = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	//if (!cam->TPMode) return;
	if (!g_canCtrl) return;

	float moveSpeed;
	moveSpeed = VALUE_MOVE;
	if (g_Player.isInIceStatus)moveSpeed = VALUE_ICE_MOVE;
	if (!g_Player.isGround) moveSpeed = VALUE_AIR_MOVE;
	if (!g_Player.isGround && g_Player.Exit_Moving_Plat) moveSpeed = VALUE_AIR_MOVE * 0.5f;

	if (GetKeyboardPress(DIK_W))
	{
		g_Player.speed += dir * moveSpeed;
		g_Player.dirXZ += dir * moveSpeed;
		D3DXVec3Normalize(&g_Player.dirXZ, &g_Player.dirXZ);
		InputDir += dir;
		D3DXVec3Normalize(&InputDir, &InputDir);
	}
	if (GetKeyboardPress(DIK_S))
	{
		g_Player.speed -= dir * moveSpeed;
		g_Player.dirXZ -= dir * moveSpeed;
		D3DXVec3Normalize(&g_Player.dirXZ, &g_Player.dirXZ);
		InputDir -= dir;
		D3DXVec3Normalize(&InputDir, &InputDir);
	}
	if (GetKeyboardPress(DIK_A))
	{
		g_Player.speed -= Ndir * moveSpeed;
		g_Player.dirXZ -= Ndir * moveSpeed;
		D3DXVec3Normalize(&g_Player.dirXZ, &g_Player.dirXZ);
		InputDir -= Ndir;
		D3DXVec3Normalize(&InputDir, &InputDir);
	}
	if (GetKeyboardPress(DIK_D))
	{
		g_Player.speed += Ndir * moveSpeed;
		g_Player.dirXZ += Ndir * moveSpeed;
		D3DXVec3Normalize(&g_Player.dirXZ, &g_Player.dirXZ);
		InputDir += Ndir;
		D3DXVec3Normalize(&InputDir, &InputDir);
	}


	//gravity
	if (GetKeyboardTrigger(DIK_SPACE) || IsButtonTriggered(0, BUTTON_A)) {
		if (g_Player.isGround && !g_Player.isInSandStatus) {
			COLLIDER3D *Collider = GetCollider();
			ChangeForceY(PLAYER_JUMP_SPEED); // jump
			SetParticle(D3DXVECTOR3(g_Player.pos.x, g_Player.pos.y - Collider[g_Player.bodyColliderIdx].hy * 0.5f, g_Player.pos.z), D3DXVECTOR3(0.0f, 1.0f, 0.0f), Particletype_Smoke, 10);
			PlaySound(SOUND_LABEL_SE_jump);
		}
		if (g_Player.isInBobStatus) {
			ChangeForceY(PLAYER_BOB_JUMP_SPEED);
			PlaySound(SOUND_LABEL_SE_jump);
		}
		if (g_Player.isInSandStatus) {
			ChangeForceY(PLAYER_SAND_JUMP_SPEED);
			PlaySound(SOUND_LABEL_SE_jump);
		}
	}

	if (GetKeyboardTrigger(DIK_LCONTROL))
	{
		if (!g_Player.isDash && g_Player.canDash && !g_Player.isInSandStatus) {
			ChangeForceY(PLAYER_DASH_JUMP_SPEED);
			g_Player.isDash = true;
			g_Player.canDash = false;
			if (D3DXVec3Length(&InputDir) != 0) {
				g_Player.dirXZ = InputDir;
				g_Player.dashDir = InputDir;
			}
			else g_Player.dashDir = g_Player.dirXZ;
			COLLIDER3D *Collider = GetCollider();
			SetParticle(D3DXVECTOR3(g_Player.pos.x, g_Player.pos.y - Collider[g_Player.bodyColliderIdx].hy * 0.5f, g_Player.pos.z), g_Player.dashDir, Particletype_DashSmoke, 10);
			//cancel throwing & bob status
			if (g_Player.isThrowing || g_Player.isInBobStatus) {
				g_Player.throwingTimer = 0;
				g_Player.bobStateTimer = 0;
				g_Player.isThrowing = false;
				g_Player.isInBobStatus = false;
				g_Player.gravityScale = VALUE_GRAVITY;
				g_Player.maxMoveSpeed = PLAYER_DASH_SPEED;
				g_Player.backToMAXMove = false;
			}
			PlaySound(SOUND_LABEL_SE_sword);
		}
		else {
			g_Player.bobStateTimer = 0;
			g_Player.isInBobStatus = false;
			g_Player.gravityScale = VALUE_GRAVITY;
			g_Player.maxMoveSpeed = PLAYER_MAX_MOVE_SPEED;
		}
	}

	//Fire
	if (IsMouseLeftTriggered() || GetKeyboardTrigger(DIK_T)) {
		//カードを投げる
		if (g_Player.cardInHand && g_Player.canThrowCard && !g_Player.isInSandStatus) {
			if (D3DXVec3Length(&InputDir) != 0) { // Turn back 180°& throw 
				g_Player.cardBulletID = SetBullet(BulletType_card, g_Player.pos, InputDir, g_Player.playerAbility);
				g_Player.dirXZ = InputDir;
			}
			else g_Player.cardBulletID = SetBullet(BulletType_card, g_Player.pos, g_Player.dirXZ, g_Player.playerAbility); // stop & throw
			if (g_Player.cardBulletID == -1) return; // if bullet overload or create false
			g_Player.speed = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			g_Player.isThrowing = true;
			if (g_Player.isGround == false)ChangeForceY(PLAYER_THROWING_JUMP_SPEED); // jump a bit is inAir
			g_Player.cardInHand = false;
			g_Player.playerAbility = CardAbility_none;
			g_Player.canThrowCard = false;
			// cancel bobstatus
			if (g_Player.isInBobStatus) {
				g_Player.bobStateTimer = 0;
				g_Player.isInBobStatus = false;
				g_Player.gravityScale = VALUE_GRAVITY;
				g_Player.maxMoveSpeed = PLAYER_MAX_MOVE_SPEED;
			}
			PlaySound(SOUND_LABEL_SE_throwcard);
		}
		//カードを戻させる
		else {
			BULLET *Bullet = GetBullet();
			if (Bullet[g_Player.cardBulletID].canSummonBack) {
				Bullet[g_Player.cardBulletID].isSummonBack = true;
			}
			g_Player.bobStateTimer = 0;
			g_Player.isInBobStatus = false;
			g_Player.gravityScale = VALUE_GRAVITY;
			g_Player.maxMoveSpeed = PLAYER_MAX_MOVE_SPEED;
		}
	}
	if (IsMouseRightTriggered() || GetKeyboardTrigger(DIK_G)) {
		// Card in Hand
		if (g_Player.cardInHand == true && g_Player.canUseCard && !g_Player.isInSandStatus) {
			switch (g_Player.playerAbility)
			{
			case CardAbility_wind:
				SetBullet(BulletType_windStorm, g_Player.pos, g_Player.dirXZ);
				g_Player.canUseCard = false;
				g_Player.playerAbility = CardAbility_none;
				break;
			case CardAbility_water:
				if (!g_Player.isInBobStatus) {
					SetBullet(BulletType_waterBobFloat, g_Player.pos, g_Player.dirXZ);
					ChangeForceY(PLAYER_BOB_JUMP_SPEED);
					PlaySound(SOUND_LABEL_SE_usecard);
					PlaySound(SOUND_LABEL_SE_aqua);
					g_Player.isInBobStatus = true;
					g_Player.canUseCard = false;
					g_Player.playerAbility = CardAbility_none;
				}
				break;
			}
		}
		// Card outside
		else if (g_Player.cardInHand == false && g_Player.canUseCard) {
			BULLET *Bullet = GetBullet();
			if (Bullet[g_Player.cardBulletID].Use) {
				switch (Bullet[g_Player.cardBulletID].cardAbility)
				{
				case CardAbility_wind:
					SetBullet(BulletType_windStorm, Bullet[g_Player.cardBulletID].pos, g_Player.dirXZ);
					g_Player.cardInHand = true;
					//g_Player.playerAbility = Bullet[g_Player.cardBulletID].cardAbility;
					ReleaseBullet(g_Player.cardBulletID);
					g_Player.canUseCard = false;
					break;
				case CardAbility_water:
					SetBullet(BulletType_waterBob, Bullet[g_Player.cardBulletID].pos, g_Player.dirXZ);
					g_Player.cardInHand = true;
					//g_Player.playerAbility = Bullet[g_Player.cardBulletID].cardAbility;
					ReleaseBullet(g_Player.cardBulletID);
					g_Player.canUseCard = false;
					PlaySound(SOUND_LABEL_SE_usecard);
					PlaySound(SOUND_LABEL_SE_aqua);
					break;
				}
			}
		}
		//other
		if (g_Player.isInBobStatus) {
			ChangeForceY(PLAYER_BOB_JUMP_SPEED);
			PlaySound(SOUND_LABEL_SE_jump);
		}
	}

	//===========debug
	//if (GetKeyboardPress(DIK_UP))   g_Player.rot.x += 0.01 * D3DX_PI;
	//if (GetKeyboardPress(DIK_DOWN)) g_Player.rot.x -= 0.01 * D3DX_PI;
	//if (GetKeyboardPress(DIK_LEFT)) g_Player.rot.y -= 0.01 * D3DX_PI;
	//if (GetKeyboardPress(DIK_RIGHT))g_Player.rot.y += 0.01 * D3DX_PI;
	//if (GetKeyboardPress(DIK_C))    g_Player.rot.z -= 0.01 * D3DX_PI;
	//if (GetKeyboardPress(DIK_V))    g_Player.rot.z += 0.01 * D3DX_PI;
	//if (g_Player.rot.x > D3DX_PI * 2) g_Player.rot.x -= D3DX_PI * 2;
	//if (g_Player.rot.x < -D3DX_PI * 2) g_Player.rot.x += D3DX_PI * 2;
	//if (g_Player.rot.y > D3DX_PI * 2) g_Player.rot.y -= D3DX_PI * 2;
	//if (g_Player.rot.y < -D3DX_PI * 2) g_Player.rot.y += D3DX_PI * 2;
	//if (g_Player.rot.z > D3DX_PI * 2) g_Player.rot.z -= D3DX_PI * 2;
	//if (g_Player.rot.z < -D3DX_PI * 2) g_Player.rot.z += D3DX_PI * 2;

	if(GetKeyboardTrigger(DIK_F1)) g_Player.pos = playerRebornPos[0];
	if(GetKeyboardTrigger(DIK_F2)) g_Player.pos = playerRebornPos[1];
	if(GetKeyboardTrigger(DIK_F3)) g_Player.pos = playerRebornPos[2];
	if(GetKeyboardTrigger(DIK_F4)) g_Player.pos = playerRebornPos[3];
	if(GetKeyboardTrigger(DIK_F5)) g_Player.pos = playerRebornPos[4];
	if(GetKeyboardTrigger(DIK_F6)) g_Player.pos = playerRebornPos[5];
	if(GetKeyboardTrigger(DIK_F7)) g_Player.pos = playerRebornPos[6];
	if(GetKeyboardTrigger(DIK_F8)) g_Player.pos = playerRebornPos[7];
	if(GetKeyboardTrigger(DIK_F9)) g_Player.pos = playerRebornPos[8];
	if(GetKeyboardTrigger(DIK_Y)) g_Player.pos = playerRebornPos[9];
	if(GetKeyboardTrigger(DIK_U)) g_Player.pos = playerRebornPos[10];
	if(GetKeyboardTrigger(DIK_I)) g_Player.pos = playerRebornPos[11];

	if (GetKeyboardTrigger(DIK_CAPSLOCK)) {
		g_canCreateSPParticle = !g_canCreateSPParticle;
	}
	if (GetKeyboardTrigger(DIK_2)) {
		g_Player.playerAbility = CardAbility_water;
	}

}

void PhysicsUpdate(PLAYER & player) {
	COLLIDER3D *Collider = GetCollider();
	PLATFORM *plat = GetPlatform();

	//==========Caculate Speed
	D3DXVECTOR3 speedXZ = D3DXVECTOR3(player.speed.x, 0.0f, player.speed.z);
	D3DXVECTOR3 dir = speedXZ;
	D3DXVec3Normalize(&dir, &dir);
	//if(D3DXVec3Length(&dir) != 0.0f)player.dirXZ = dir; //プレーヤの向き
	player.speedXZLengh = D3DXVec3Length(&speedXZ);
	if (player.speedXZLengh >= player.maxMoveSpeed) {
		player.speed.x = dir.x * player.maxMoveSpeed;
		player.speed.z = dir.z * player.maxMoveSpeed;
	}
	if (player.speedXZLengh > 0) {
		player.speed -= dir * player.decelerate;
	}
	if (player.speedXZLengh <= player.decelerate) {
		player.speed.x = 0.0f; 
		player.speed.z = 0.0f;
	}

	//Gravity
	if (player.gravityScale > 0.0f) {
		if (player.isGround == false)player.speed.y -= player.gravityScale;

	}
	if (player.speed.y < PLAYER_MAX_FALLEN_SPEED) {
		player.speed.y = PLAYER_MAX_FALLEN_SPEED;
	}

	//==========Caculate Rot
	player.rot.y = (atan2f(player.dirXZ.z, player.dirXZ.x) - D3DX_PI * 0.5f) * -1.0f;   //coordinate(counter clockwise), obj rotation(clockwise) : different way => -1

	//==========Dash => maxMove & gravity 加算の後 maxMoveを変更する必要がある
	if (player.isDash) {
		player.speed.x = player.dashDir.x * player.dashSpeed;
		player.speed.z = player.dashDir.z * player.dashSpeed;
		//player.speed.y = 0.0f;
	}

	//==========Ground Detect
	if (player.On_Moving_Plat) {
		player.pos.y = Collider[player.bodyColliderIdx].hy * 0.5f + Collider[player.On_Moving_PlatID].pos.y + Collider[player.On_Moving_PlatID].hy * 0.5f;
	}
	//==========ground Detect
	//==============groundTest Raycast
	D3DXVECTOR3 hit, normal;
	bool rayHit = false;
	if(player.InHeartWorldArea) rayHit += RayHitGround(player.pos, &hit, &normal, GroundType_HeartWorld);
	if(player.InHtoWArea)       rayHit += RayHitGround(player.pos, &hit, &normal, GroundType_HtoW);
	if(player.InWonderLandArea) rayHit += RayHitGround(player.pos, &hit, &normal, GroundType_Wonderland);
	if(player.InWonderLand2Area)rayHit += RayHitGround(player.pos, &hit, &normal, GroundType_Wonderland2);
	if(player.InWtoSArea)       rayHit += RayHitGround(player.pos, &hit, &normal, GroundType_WtoS);
	if(player.InSandWorldArea)  rayHit += RayHitGround(player.pos, &hit, &normal, GroundType_SandWorld);
	if(player.InStoSArea)       rayHit += RayHitGround(player.pos, &hit, &normal, GroundType_StoS);
	if(player.InSnowWorldArea)  rayHit += RayHitGround(player.pos, &hit, &normal, GroundType_SnowWorld);
	if (rayHit) {
		if (player.speed.y <= 0.0f) {
			bool slopeANS = false;
			if (player.isOnSlope && player.pos.y - hit.y <= PLAYER_GROUND_OFFSET_Y * 1.5f)slopeANS = true;
			if (player.pos.y - hit.y <= PLAYER_GROUND_OFFSET_Y * 1.2f || slopeANS) {
				player.isGround = true;
				player.pos = hit;
				player.pos.y += PLAYER_GROUND_OFFSET_Y;
				if(player.InSnowWorldArea || player.InStoSArea)player.isInIceStatus = true;
				else player.isInIceStatus = false;
				// SLOPE
				float slope = dotProduct(&player.dirXZ, &normal);
				if (slope >= VALUE_SLOPE_ADDSPD_DEGREE) {
					player.isOnSlope = true;
					g_Player.maxMoveSpeed = PLAYER_MAX_SLOPEUP_MOVE_SPEED;
				}
				else if (slope <= -VALUE_SLOPE_ADDSPD_DEGREE) {
					player.isOnSlope = true;
					if (g_Player.maxMoveSpeed > PLAYER_MAX_MOVE_SPEED - ((PLAYER_MAX_MOVE_SPEED -PLAYER_MAX_SLOPEDOWN_MOVE_SPEED) * (-slope/0.5f))) {
						g_Player.maxMoveSpeed -= backToMAXMoveSpeed;					
					}
					else {
						g_Player.maxMoveSpeed = PLAYER_MAX_MOVE_SPEED - ((PLAYER_MAX_MOVE_SPEED - PLAYER_MAX_SLOPEDOWN_MOVE_SPEED) * (-slope / 0.5f));
					}
				}
				else {
					player.isOnSlope = false;
				}
			}
			else {
				player.isGround = false;
			}
		}
	}
	else {
		player.isGround = false;
	}
	
	//==========OBJ in PlatCircleRange => OBJInRange = {inRangeOBJ1, inRangeOBJ2 .... OBJNum}
	int OBJNum = 0;
	for (int i = 0; i < MAX_COLLIDER3D; i++) {
		if (Collider[i].tag == collider3DTag_PlatCircleRange) {
			if (CheckHitByID(player.CircleRangeColliderIdx, i)) {
				OBJNum++;
			}
		}
	}
	int *OBJInRange = NULL; 
	if (OBJNum > 0) OBJInRange = new int[OBJNum];
	int OBJPutCnt = 0;
	for (int i = 0; i < MAX_COLLIDER3D; i++) {
		if (Collider[i].tag == collider3DTag_PlatCircleRange) {
			if (CheckHitByID(player.CircleRangeColliderIdx, i)) {
				OBJInRange[OBJPutCnt] = i;
				OBJPutCnt++;
			}
		}
	}
	PrintDebugProc("~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	PrintDebugProc("OBJPutCnt: %d \n", OBJPutCnt);
	for (int i = 0; i < OBJPutCnt; i++) {
		PrintDebugProc("Number :%d OBJInRangeID: %d  MasterID: %d\n", i, OBJInRange[i], Collider[OBJInRange[i]].masterID);
	}

	PrintDebugProc("~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	//==========Slope plat ground
	SetPositionCollider3D(player.bodyColliderIdx, player.pos, player.rot);
	SetPositionCollider3D(player.groundCheckColliderIdx, D3DXVECTOR3(player.pos.x, player.pos.y - PLAYER_GROUNDCHECKC_Y - PLAYER_BODYCHECKC_XZ * 0.5f, player.pos.z));
	SetPositionCollider3D(player.CircleRangeColliderIdx, player.pos);
	D3DXVECTOR3 cantactPVec;
	for (int i = 0; i < OBJNum; i++) {
		int targetCID = OBJInRange[i];
		if (CheckHitByID(player.CircleRangeColliderIdx, targetCID)) {
			if (!plat[Collider[targetCID].masterID].canMove) {
				if (player.speed.y <= 0.0f) {
					if (RayHitPlat(player.pos, targetCID, &hit, &normal)) {
						bool slopeANS = false;
						if (player.isOnSlope && player.pos.y - hit.y <= PLAYER_GROUND_OFFSET_Y * 1.5f)slopeANS = true;
						if (player.pos.y - hit.y <= PLAYER_GROUND_OFFSET_Y * 1.2f || slopeANS) {
							if (normal.y > WALL_SLOPE_DEGREE) {
								PrintDebugProc("///////////////////////////////\n");
								PrintDebugProc("hit Plat ID: %d\n", targetCID);
								player.isGround = true;
								player.pos = hit;
								player.pos.y += PLAYER_GROUND_OFFSET_Y;
								// SLOPE
								float slope = dotProduct(&player.dirXZ, &normal);
								if (slope >= VALUE_SLOPE_ADDSPD_DEGREE) {
									player.isOnSlope = true;
									g_Player.maxMoveSpeed = PLAYER_MAX_SLOPEUP_MOVE_SPEED;
								}
								else if (slope <= -VALUE_SLOPE_ADDSPD_DEGREE) {
									player.isOnSlope = true;
									if (g_Player.maxMoveSpeed > PLAYER_MAX_MOVE_SPEED - ((PLAYER_MAX_MOVE_SPEED - PLAYER_MAX_SLOPEDOWN_MOVE_SPEED) * (-slope / 0.5f))) {
										g_Player.maxMoveSpeed -= backToMAXMoveSpeed;
									}
									else {
										g_Player.maxMoveSpeed = PLAYER_MAX_MOVE_SPEED - ((PLAYER_MAX_MOVE_SPEED - PLAYER_MAX_SLOPEDOWN_MOVE_SPEED) * (-slope / 0.5f));
									}
								}
								else {
									player.isOnSlope = false;
								}
								//===================================ICE
								if (plat[Collider[targetCID].masterID].platformType == PlatformType_ice) {
									player.isInIceStatus = true;
								}
								else {
									player.isInIceStatus = false;
								}
							}
							else if (normal.y <= WALL_SLOPE_DEGREE && normal.y > 0.0f) { // slide
								PrintDebugProc("///////////////////////////////\n");
								PrintDebugProc("hit Plat ID: %d\n", targetCID);
								PrintDebugProc("///////Slide///////////\n");
								player.pos = hit;
								player.pos.y += PLAYER_GROUND_OFFSET_Y;
								D3DXVECTOR3 HITtoP = hit - player.pos; // push out
								float dis = fabsf(D3DXVec3Dot(&HITtoP, &normal));
								for (int j = 0; j <= 100; j++) {
									if (dis <= 50.0f) { // distance from wall : 100

										player.pos += normal * 0.1f;
										player.dashTimer = 0;
										player.isDash = false;
										player.isFloat = false;

										HITtoP = hit - player.pos;
										dis = fabsf(D3DXVec3Dot(&HITtoP, &normal));
										//player.speed = player.pos - player.prePos;
									}
									else {
										break;
									}
								}
							}
						}
					}
				}
			}
			if (player.speed.y > 0.0f) { // 天井
				D3DXVECTOR3 cellingY = player.pos;
				cellingY.y += PLAYER_GROUNDCHECKC_Y;
				if (RayHitPlat(cellingY, targetCID, &hit, &normal)) {
					if (normal.y < 0.0f) {
						if (player.speed.y > 0.0f && hit.y > player.pos.y) {
							player.speed.y = 0.0f;
						}
					}
				}
			}
		}
		

	}
	
	//========= MovingPlat
	MovingPlatPhysicsUpdate(player, OBJNum, OBJInRange);

	if (player.speed.y > 0.0f) player.isGround = false;

	player.isGroundPrev = player.isGround;

	if (player.isGround) {
		player.speed.y = 0.0f;
		player.Exit_Moving_Plat = false;
	}

	//==========Update Speed
	player.pos += player.speed;

	// Push out wall
	// 推出
	D3DXVECTOR3 dirXZ;
	if (player.speed.x == 0.0f && player.speed.z == 0.0f) dirXZ =  player.dirXZ;
	else  dirXZ = player.speed;
	rayHit = false;
	if(player.InHeartWorldArea) rayHit += RayHitGroundWall(player.pos , dirXZ, &hit, &normal, GroundType_HeartWorld);
	if(player.InHtoWArea)       rayHit += RayHitGroundWall(player.pos , dirXZ, &hit, &normal, GroundType_HtoW);
	if(player.InWonderLandArea) rayHit += RayHitGroundWall(player.pos , dirXZ, &hit, &normal, GroundType_Wonderland);
	if(player.InWonderLand2Area)rayHit += RayHitGroundWall(player.pos , dirXZ, &hit, &normal, GroundType_Wonderland2);
	if(player.InWtoSArea)       rayHit += RayHitGroundWall(player.pos , dirXZ, &hit, &normal, GroundType_WtoS);
	if(player.InSandWorldArea)  rayHit += RayHitGroundWall(player.pos , dirXZ, &hit, &normal, GroundType_SandWorld);
	if(player.InStoSArea)       rayHit += RayHitGroundWall(player.pos , dirXZ, &hit, &normal, GroundType_StoS);
	if(player.InSnowWorldArea)  rayHit += RayHitGroundWall(player.pos , dirXZ, &hit, &normal, GroundType_SnowWorld);

	if (rayHit) {
		if (fabsf(normal.y) <= WALL_SLOPE_DEGREE) {
			D3DXVECTOR3 HITtoP = hit - player.pos;
			float dis = fabsf(D3DXVec3Dot(&HITtoP, &normal));
			for (int i = 0; i <= 100; i++) {
				if (dis <= 50.0f) { // distance from wall : 100
					float dir = D3DXVec3Dot(&player.speed, &normal);

					if (dir > 0.0f) player.pos -= normal * 0.1f * i;
					else player.pos += normal * 0.1f * i;
					player.dashTimer = 0;
					player.isDash = false;
					player.isFloat = false;

					HITtoP = hit - player.pos;
					dis = fabsf(D3DXVec3Dot(&HITtoP, &normal));
				}
				else {
					break;
				}
			}
		}
	}
	// Push out plat
	// 推出
	for (int i = 0; i < OBJNum; i++) {
		int targetCID = OBJInRange[i];
		if (CheckHitByID(player.CircleRangeColliderIdx, targetCID)) {
			if (player.speed.x == 0.0f && player.speed.z == 0.0f) dirXZ = Collider[targetCID].pos - player.pos;
			else  dirXZ = player.speed;
			if (RayHitPlatWall(player.pos, targetCID, dirXZ, &hit, &normal)) {
				if (fabsf(normal.y) <= WALL_SLOPE_DEGREE) {
					D3DXVECTOR3 HITtoP = hit - player.pos;
					float dis = fabsf(D3DXVec3Dot(&HITtoP, &normal));
					for (int j = 0; j <= 100; j++) {
						if (dis <= 50.0f) { // distance from wall : 100

							player.pos += normal * 0.1f * j;
							if (player.On_Moving_Plat) {
								int targetBoxCID = plat[Collider[targetCID].masterID].colliderIdx;
								player.On_Moving_Plat_Pos += normal * 0.1f * j;
							}
							player.dashTimer = 0;
							player.isDash = false;
							player.isFloat = false;

							HITtoP = hit - player.pos;
							dis = fabsf(D3DXVec3Dot(&HITtoP, &normal));
						}
						else {
							break;
						}
					}
				}
			}
		}
	}
	player.prePos = player.pos;
	
	//==========Anim
	D3DXVECTOR3 AnimspeedXZ = D3DXVECTOR3(player.speed.x, 0.0f, player.speed.z);
	player.speedXZLengh = D3DXVec3Length(&AnimspeedXZ);
	////////////////////////////////////////////////////////////////////////////
	if(OBJInRange) { delete[] OBJInRange; OBJInRange = NULL; }
}

void CollisionUpdate(PLAYER & player)
{
	//========collider position
	SetPositionCollider3D(player.bodyColliderIdx, player.pos, player.rot);
	SetPositionCollider3D(player.groundCheckColliderIdx, D3DXVECTOR3(player.pos.x, player.pos.y - PLAYER_GROUNDCHECKC_Y - PLAYER_BODYCHECKC_XZ * 0.5f, player.pos.z));
	SetPositionCollider3D(player.CircleRangeColliderIdx, player.pos);

	//========collision
	COLLIDER3D *collider = GetCollider();
	//===enemy
	// 一次性
	showSign(-1);

	ENEMY *Enemy = GetEnemy();
	int targetID;
	if (CheckHitByTagReturnMasterID(player.bodyColliderIdx, collider3DTag_enemy, &targetID)) {
		if (Enemy[targetID].enemyType == EnemyType_gem) {
			g_Player.pos = playerRebornPos[CurPlayerRebornPos];
		}
		if(Enemy[targetID].enemyType == EnemyType_star) {
			
			ReleaseEnemy(targetID);
			g_Player.Anim_GetStar = true;
			g_Player.isInBobStatus = false;
			PlaySound(SOUND_LABEL_SE_getStar);
			AddScore(1);
			if (Enemy[targetID].CanChase) { // ending
				getFinalStar = true;
			}
		}
		if (Enemy[targetID].enemyType == EnemyType_coin) {
			ReleaseEnemy(targetID);
			PlaySound(SOUND_LABEL_SE_start);
			AddCoin(1);
		}
		if (Enemy[targetID].enemyType == EnemyType_quickSand) {
			g_Player.isInSandStatus = true;
			g_Player.isInBobStatus = false;
			PlaySound(SOUND_LABEL_SE_aqua);
			player.gravityScale = VALUE_GRAVITY;
			player.maxMoveSpeed = PLAYER_MAX_MOVE_SPEED;
		}
		if (Enemy[targetID].enemyType == EnemyType_mushroomSpring) {
			ChangeForceY(PLAYER_MUSH_JUMP_SPEED);
			g_Player.Anim_isStepOnMushroomSpring = true;
			player.canThrowCard = true;
			player.canDash = true;
			Enemy[targetID].Status = EnemyStatus_Hit;
			PlaySound(SOUND_LABEL_SE_hit);
			PlaySound(SOUND_LABEL_SE_jump);
			SetParticle(D3DXVECTOR3(player.pos.x, player.pos.y - PLAYER_GROUND_OFFSET_Y, player.pos.z), g_Player.dirXZ, Particletype_BumpSoftThings, 25);
		}
		if (Enemy[targetID].enemyType == EnemyType_mushroom) {
			g_Player.isInBobStatus = false;
			player.gravityScale = VALUE_GRAVITY;
			player.maxMoveSpeed = PLAYER_MAX_MOVE_SPEED;
			ChangeForceY(PLAYER_SAND_JUMP_SPEED);
			player.speed.x = -player.dirXZ.x * player.dashSpeed;
			player.speed.z = -player.dirXZ.z * player.dashSpeed;
			PlaySound(SOUND_LABEL_SE_hit2);
		}

		if (Enemy[targetID].enemyType == EnemyType_Sign) {
			int min = D3DXVec3LengthSq(&(player.pos - playerRebornPos[0]));
			int ans = 0;
			for (int i = 0; i < Max_Birth_Point; i++) {
				float c = D3DXVec3LengthSq(&(player.pos - playerRebornPos[i]));
				if (c <= min) {
					min = c;
					ans = i;
				}
			}
			CurPlayerRebornPos = ans;

			//overflow
			if (CurPlayerRebornPos == 0) {
				if (player.pos.x <= -45000.0) CurPlayerRebornPos = 9;
				if (player.pos.x <= -50000.0) CurPlayerRebornPos = 10;
				if (player.pos.y >= 6000.0f) CurPlayerRebornPos = 11;
			}

			//drawSign
			if (CurPlayerRebornPos == 0) { showSign(0); showSign(1);}
			else if (CurPlayerRebornPos == 1)showSign(2);
			else if (CurPlayerRebornPos == 2)showSign(3);
			else if (CurPlayerRebornPos == 6)showSign(4);
			else if (CurPlayerRebornPos == 11)showSign(5);
			else showSign(6); // SP
			
		}
		
	}
	else {
		if (player.isInSandStatus) {
			player.SandStateTimer = 3;
			player.isInSandStatus = false;
			player.gravityScale = VALUE_GRAVITY;
			player.maxMoveSpeed = PLAYER_MAX_MOVE_SPEED;
			player.canDash = true;
			player.canThrowCard = true;
			player.canUseCardTrigger = true;
		}
	}
	//持續偵測 ===> 會卡
	//int sandCnt = 0;
	//for (int i = 0; i < MAX_COLLIDER3D; i++) {
	//	if(collider[i].tag == collider3DTag_enemy)
	//	if (CheckHitByIDAABB(player.bodyColliderIdx,i)) {
	//		//sand
	//		if (Enemy[collider[i].masterID].enemyType == EnemyType_sandArea) {
	//			g_Player.isInSand = true;
	//			sandCnt = 1;
	//			PrintDebugProc("Enemy[collider[i].masterID] %d\n", Enemy[collider[i].masterID]);
	//
	//		}
	//		//
	//
	//	}
	//}
	////判定
	//if (sandCnt == 0)g_Player.isInSand = false;
	//PrintDebugProc("g_Player.isInSand: %d\n", g_Player.isInSand);

	//===card
	BULLET *Bullet = GetBullet();
	if (CheckHitByTagReturnMasterID(player.bodyColliderIdx, collider3DTag_card, &targetID)) {
		if (Bullet[targetID].cardIsForward == false) {
			if (!player.isGround && !player.isDash && !player.isFloat && !player.isInBobStatus && !g_Player.isInSandStatus) {
				ChangeForceY(PLAYER_STEPCARD_JUMP_SPEED);
				PlaySound(SOUND_LABEL_SE_sword);
			}
			else if (player.isDash || player.isFloat) {
				ChangeForceY(PLAYER_STEPCARD_DASHJUMP_SPEED);
				PlaySound(SOUND_LABEL_SE_sword);
			}
			g_Player.cardInHand = true;
			g_Player.Anim_isCatchedCard = true;
			g_Player.playerAbility = Bullet[targetID].cardAbility;
			ReleaseBullet(targetID);

		}
	}
	//===ability
	if (CheckHitByTagReturnMasterID(player.bodyColliderIdx, collider3DTag_bullet, &targetID)) {
		
		if (Bullet[targetID].bulletType == BulletType_windStorm) {
			D3DXVECTOR3 dir = player.pos - Bullet[targetID].pos;
			D3DXVec3Normalize(&dir, &dir);
			player.speed.x = dir.x * PLAYER_WIND_MOVE_SPEED;
			ChangeForceY(PLAYER_WIND_JUMP_SPEED);
			player.speed.z = dir.z * PLAYER_WIND_MOVE_SPEED;
			player.maxMoveSpeed = PLAYER_WIND_MOVE_SPEED;
			if(player.speedXZLengh > PLAYER_MAX_MOVE_SPEED * 0.5f) player.Anim_isWindBoost = true;
			g_Player.backToMAXMove = false;
		}
		else if (Bullet[targetID].bulletType == BulletType_waterBob) {
			//泡泡狀態
			if (!player.isInBobStatus) {
				ChangeForceY(PLAYER_MUSH_JUMP_SPEED);
				g_Player.Anim_isStepOnMushroomSpring = true;
				//player.canThrowCard = true;
				//player.canDash = true;
				Bullet[targetID].Status = BulletStatus_Hit;
				//SetParticle(D3DXVECTOR3(player.pos.x, player.pos.y - PLAYER_GROUND_OFFSET_Y, player.pos.z), g_Player.dirXZ, Particletype_BumpSoftThings, 25);

				//ReleaseBullet(targetID);
			}
		}
		
	}
	//===fall Down Area
	if (CheckHitByTag(player.bodyColliderIdx, collider3DTag_fallDownArea)) {
		g_Player.pos = playerRebornPos[CurPlayerRebornPos];
		if (!GetGameSceneEditMode()) {
			GetCamera()->rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			GetCamera()->rot.x -= D3DX_PI * 0.1f;
		}
		g_Player.speed = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		AddMiss(1);
	}
	//===ground Check Area
	if (CheckHitByTag(player.bodyColliderIdx, collider3DTag_GroundArea_HeartWorld)) g_Player.InHeartWorldArea = true;
	else g_Player.InHeartWorldArea = false;
	if (CheckHitByTag(player.bodyColliderIdx, collider3DTag_GroundArea_HtoW)) g_Player.InHtoWArea = true;
	else g_Player.InHtoWArea = false;
	if (CheckHitByTag(player.bodyColliderIdx, collider3DTag_GroundArea_WonderLand)) g_Player.InWonderLandArea = true;
	else g_Player.InWonderLandArea = false;
	if (CheckHitByTag(player.bodyColliderIdx, collider3DTag_GroundArea_WonderLand2)) g_Player.InWonderLand2Area = true;
	else g_Player.InWonderLand2Area = false;
	if (CheckHitByTag(player.bodyColliderIdx, collider3DTag_GroundArea_WtoS)) g_Player.InWtoSArea = true;
	else g_Player.InWtoSArea = false;
	if (CheckHitByTag(player.bodyColliderIdx, collider3DTag_GroundArea_SandWorld)) g_Player.InSandWorldArea = true;
	else g_Player.InSandWorldArea = false;
	if (CheckHitByTag(player.bodyColliderIdx, collider3DTag_GroundArea_StoS)) g_Player.InStoSArea = true;
	else g_Player.InStoSArea = false;
	if (CheckHitByTag(player.bodyColliderIdx, collider3DTag_GroundArea_SnowWorld)) g_Player.InSnowWorldArea = true;
	else g_Player.InSnowWorldArea = false;

	
	
	
}

void PlayerStatusUpdate(PLAYER & player) // & other
{
	// InAir
	if (player.isGround) {
		player.decelerate = VALUE_DECELERATE;
		if (player.isInIceStatus)player.decelerate = VALUE_ICE_DECELERATE;
	}
	else {
		player.decelerate = VALUE_AIR_DECELERATE;
	}
	if (player.Exit_Moving_Plat) {
		player.decelerate = 0.1f;
	}

	// Dash Sys
	if (player.isDash) {
		if(player.isGround)player.maxMoveSpeed = PLAYER_DASH_SPEED;
		else player.maxMoveSpeed = PLAYER_AIRDASH_SPEED;
		player.dashTimer += 1; player.isFloat = true;
		if (player.dashTimer >= PLAYER_DASHTIME) {
			player.dashTimer = 0;
			player.isDash = false;
			player.isFloat = true;
		}
	}

	if (player.isFloat) {
		if(!player.isGround)player.maxMoveSpeed = PLAYER_AIRDASH_SPEED;
		else { 
			g_Player.backToMAXMove = true;
			player.isFloat = false;
		}
	}

	//脫離平台後掉落地面重置
	if (player.isGround) {
		g_Player.backToMAXMove = true;
		if(!g_Player.Anim_isStepOnMushroomSpring) g_Player.Anim_isJumping = false;
	}
	else {
		player.isOnSlope = false;
	}
	player.On_Moving_PlatPre = player.On_Moving_Plat;
	if (g_Player.backToMAXMove && !player.isOnSlope && player.isGround) {
		player.maxMoveSpeed -= backToMAXMoveSpeed;
		if (player.maxMoveSpeed <= PLAYER_MAX_MOVE_SPEED || player.speedXZLengh <= PLAYER_MAX_MOVE_SPEED) {
			player.maxMoveSpeed = PLAYER_MAX_MOVE_SPEED;
			g_Player.backToMAXMove = false;
		}
	}

	// Dash CD
	if (!player.canDash) {
		player.dashCDTimer += 1;
		if (player.dashCDTimer >= PLAYER_DASHCDTIME) {
			if (player.isGround) {
				player.dashCDTimer = 0;
				player.canDash = true;
			}
		}
	}

	//Throwing
	if (player.isThrowing) {
		player.gravityScale = VALUE_THROWINGGRAVITY;
		player.throwingTimer += 1;
		if (player.throwingTimer >= PLAYER_THROWING_TIME) {
			player.throwingTimer = 0;
			player.isThrowing = false;
			player.gravityScale = VALUE_GRAVITY;
		}
	}
	if (player.isGround)player.canThrowCard = true; //掉落地面重置

	//UseCardCD
	if (player.canUseCard == false) {
		player.canUseCardTimer += 1;
		if (player.isGround) {
			if(!player.isInBobStatus)player.canUseCardTrigger = true;
		}
		if (player.canUseCardTimer >= PLAYER_USEABILITYCD) {
			if (player.canUseCardTrigger) {
				player.canUseCardTimer = 0;
				player.canUseCard = true;
				player.canUseCardTrigger = false;
			}
		}
	}

	//Card Ability Effect
	if (player.cardInHand && player.playerAbility == CardAbility_water) {
		waterAbilityTimer += 1;
		if (waterAbilityTimer >= PLAYER_WATERABILITYCD) {
			waterAbilityTimer = 0;
			SetParticle(player.pos, D3DXVECTOR3(0.0f, 1.0f, 0.0f), Particletype_WaterAbilityEffect, 3);
		}
	}
	else {
		waterAbilityTimer = 0;
	}

	// BobState
	if (player.isInBobStatus == true) {
		player.bobStateTimer += 1;
		player.gravityScale = VALUE_BOB_GRAVITY;
		player.maxMoveSpeed = PLAYER_MAX_BOB_MOVE_SPEED;
		if (player.bobStateTimer >= PLAYER_BOBSTATETIME) {
			player.bobStateTimer = 0;
			player.isInBobStatus = false;
			PlaySound(SOUND_LABEL_SE_aqua);
			player.gravityScale = VALUE_GRAVITY;
			player.maxMoveSpeed = PLAYER_MAX_MOVE_SPEED;
		}
	}

	//canCreateMoveSmoke
	if (player.speedXZLengh > 0.0f && player.isGround) {
		int movecdTime = 35.5f - (player.speedXZLengh / player.maxMoveSpeed) * 24.0f;
		player.moveSmokeTimer += 1;
		if (player.moveSmokeTimer >= movecdTime) {
			COLLIDER3D *Collider = GetCollider();
			player.moveSmokeTimer = 0;
			SetParticle(D3DXVECTOR3(g_Player.pos.x, g_Player.pos.y - Collider[g_Player.bodyColliderIdx].hy * 1.0f, g_Player.pos.z), D3DXVECTOR3(0.0f, 1.0f, 0.0f), Particletype_Smoke, 2);
			if (player.InWonderLandArea || player.InWonderLand2Area) {
				if(g_canCreateSPParticle)SetParticle(D3DXVECTOR3(g_Player.pos.x, g_Player.pos.y - Collider[g_Player.bodyColliderIdx].hy * 1.0f, g_Player.pos.z), D3DXVECTOR3(0.0f, 1.0f, 0.0f), Particletype_WalkOnGrass, 2);
			}
			if (player.InSandWorldArea) {
				if (g_canCreateSPParticle)SetParticle(D3DXVECTOR3(g_Player.pos.x, g_Player.pos.y - Collider[g_Player.bodyColliderIdx].hy * 1.0f, g_Player.pos.z), D3DXVECTOR3(0.0f, 1.0f, 0.0f), Particletype_WalkOnSand, 6);
			}
		}
		//else player.moveSmokeTimer = 0;
	}

	//InSand
	if (player.isInSandStatus == true) {
		if (g_Player.speed.y <= 0.0f) {
			g_Player.speed.y = 0.0f;
			player.gravityScale = VALUE_SNADGRAVITY;
		}
		else {
			player.gravityScale = VALUE_SNADJUMP_GRAVITY;
		}
		player.SandStateTimer += 1;
		player.maxMoveSpeed = PLAYER_MAX_SAND_MOVE_SPEED;
		if (player.SandStateTimer >= PLAYER_SANDSTATETIME && player.isGround == false) {
			player.SandStateTimer = 3;
			player.isInSandStatus = false;
			player.gravityScale = VALUE_GRAVITY;
			player.maxMoveSpeed = PLAYER_MAX_MOVE_SPEED;
		}
	}
	else {
		if (player.SandStateTimer > 0)player.SandStateTimer -= 1;
	}
}

void AnimStatusUpdate(PLAYER & player)
{
	if (GetAnimPlayType() != APT_CBR) { // normal
		if (player.speed.x != 0.0f || player.speed.z != 0.0f) {
			player.Anim_Status = AnimStatus_Move;
		}
		else {
			player.Anim_Status = AnimStatus_Idel;
			//player.Anim_Status = AnimStatus_Move;
		}
		if (player.backToMAXMove && player.speedXZLengh > PLAYER_MAX_MOVE_SPEED) {
			player.Anim_Status = AnimStatus_FastMove;
		}
		if (!player.isGround && player.Anim_isJumping) {
			player.Anim_Status = AnimStatus_Jump;
		}
		if (!player.isGround && player.speedXZLengh > PLAYER_MAX_MOVE_SPEED) {
			player.Anim_Status = AnimStatus_BigJump;
		}
		else if (!player.isGround && !player.Anim_isJumping) {
			player.Anim_Status = AnimStatus_Air;
		}
		//WaterBobStatus
		if (player.isInBobStatus) {
			player.Anim_Status = AnimStatus_BobStatus;
		}
		//sand
		if (player.SandStateTimer > 0) {
			if (player.speed.x != 0.0f || player.speed.z != 0.0f) {
				player.Anim_Status = AnimStatus_Move;
			}
			else {
				player.Anim_Status = AnimStatus_Idel;
			}
		}
	}
	// CBR
	if (player.isDash) {
		player.Anim_Status = AnimStatus_Dash;
	}
	if (player.Anim_isWindBoost) {
		player.Anim_Status = AnimStatus_Dash;
		player.Anim_isWindBoost = false;
	}
	if (player.isThrowing) {
		player.Anim_Status = AnimStatus_ThrowCard;
	}
	if (player.Anim_isCatchedCard) {
		if(!player.isGround)player.Anim_Status = AnimStatus_CatchCardJump;
		player.Anim_isCatchedCard = false;
	}
	if (player.Anim_isStepOnMushroomSpring) {
		player.Anim_Status = AnimStatus_CatchCardJump;
		player.Anim_isStepOnMushroomSpring = false;
	}
	if (player.Anim_GetStar) {
		player.Anim_Status = AnimStatus_GetStar;
		player.maxMoveSpeed = 0.0f;
		player.gravityScale = 0.0f;
		player.speed = D3DXVECTOR3(0.0f,0.0f,0.0f);

		//face to camera
		CAMERA *cam = GetCamera();
		D3DXVECTOR3 cdir = g_Player.pos - GetCamera()->pos;
		cdir = D3DXVECTOR3{ cdir.x, 0.0f, cdir.z }; // don't want to fly (move according to dir.y)
		D3DXVec3Normalize(&cdir, &cdir);
		g_Player.dirXZ = -cdir;

		g_getStarAnimTimer += 1;
		if (g_getStarAnimTimer <= 60) {
			SetParticle(player.pos, D3DXVECTOR3(1.0f, 0.0f, 1.0f) , Particletype_CardMove, 2);
		}
		else if (g_getStarAnimTimer == 80) {
			SetParticle(player.pos, g_Player.dirXZ, Particletype_BumpSoftThings, 45);
		}

		if (GetKeyboardTrigger(DIK_SPACE) || IsMouseLeftTriggered()) {
			if (!getFinalStar) {
				player.Anim_Status = AnimStatus_Air;
				player.Anim_GetStar = false;
				player.maxMoveSpeed = PLAYER_MAX_MOVE_SPEED;
				player.maxMoveSpeed = PLAYER_MAX_MOVE_SPEED;
				player.gravityScale = VALUE_GRAVITY;
				g_getStarAnimTimer = 0.0f;
			}
			else {
				SetFade(FADE_OUT, MODE_RESULT);
			}
		}
	}
}

void ChangeForceY(float force)
{
	g_Player.speed.y = force;
	if(g_Player.Moving_Plat_Speed.y > 0) {
		//g_Player.speed.y += g_Player.Moving_Plat_Speed.y;
	}
	g_Player.On_Moving_Plat = false;
	g_Player.Anim_isJumping = true;
}

void MovingPlatPhysicsUpdate(PLAYER &player, int OBJNum, int *OBJInRange)
{
	COLLIDER3D *Collider = GetCollider();
	PLATFORM *plat = GetPlatform();
	//==============plat
	SetPositionCollider3D(player.bodyColliderIdx, player.pos, player.rot);
	SetPositionCollider3D(player.groundCheckColliderIdx, D3DXVECTOR3(player.pos.x, player.pos.y - PLAYER_GROUNDCHECKC_Y - PLAYER_BODYCHECKC_XZ * 0.5f, player.pos.z));
	SetPositionCollider3D(player.CircleRangeColliderIdx, player.pos);
	int ansCnt = 0;
	for (int i = 0; i < OBJNum; i++) {
		int targetCID = plat[Collider[OBJInRange[i]].masterID].colliderIdx;
		if (CheckHitByID(player.groundCheckColliderIdx, targetCID)) {
			if (player.speed.y <= 0.0f && player.pos.y + Collider[player.bodyColliderIdx].hy*0.5f >= (Collider[targetCID].pos.y + Collider[targetCID].hy*0.5f)) { // player's head higher than target's head
				if (Collider[targetCID].rot.x == 0.0f && plat[Collider[targetCID].masterID].canMove) {
					player.pos.y = PLAYER_GROUND_OFFSET_Y + Collider[targetCID].pos.y + Collider[targetCID].hy * 0.5f;
					ansCnt++;
					//====== NOT On moving platform
					if (player.On_Moving_Plat == false) {
						if (plat[Collider[targetCID].masterID].canMove) {
							player.On_Moving_Plat_Pos.x = player.pos.x - Collider[targetCID].pos.x;
							player.On_Moving_Plat_Pos.z = player.pos.z - Collider[targetCID].pos.z;
							player.On_Moving_Plat = true;
							player.On_Moving_PlatID = targetCID;
							if (player.Exit_Moving_Plat) {
								player.speed.x = 0.0f;
								player.speed.z = 0.0f;
							}
						}
					}
					//====== On moving platform
					if (player.On_Moving_Plat == true) {
						player.Moving_Plat_Speed = Collider[targetCID].pos - player.Moving_Plat_PrePos;
					}
					//===================================
					if (plat[Collider[targetCID].masterID].canMove) {
						player.Moving_Plat_PrePos = Collider[targetCID].pos;
						player.pos.x = Collider[targetCID].pos.x + player.On_Moving_Plat_Pos.x;
						player.pos.z = Collider[targetCID].pos.z + player.On_Moving_Plat_Pos.z;
						player.On_Moving_Plat_Pos.x += player.speed.x;
						player.On_Moving_Plat_Pos.z += player.speed.z;
					}
					//===================================ICE
					if (plat[Collider[targetCID].masterID].platformType == PlatformType_ice) {
						player.isInIceStatus = true;
					}
					else {
						player.isInIceStatus = false;
					}
					player.isGround = true;
				}
			}

		}
	}
	if(ansCnt == 0) player.On_Moving_Plat = false;

	//プラットフォームから出る瞬間
	if (player.On_Moving_PlatPre == true && player.On_Moving_Plat == false) {
		player.speed.x += player.Moving_Plat_Speed.x;
		player.speed.y += player.Moving_Plat_Speed.y;
		player.speed.z += player.Moving_Plat_Speed.z;
		player.maxMoveSpeed += D3DXVec3Length(&player.Moving_Plat_Speed); // 暫時放大最高速度 => 在平台上跳躍使speed.xz正常變動
		if (player.maxMoveSpeed >= 100.0f) player.maxMoveSpeed = PLAYER_MAX_MOVE_SPEED; // prevent extream speed
		g_Player.backToMAXMove = false;
		player.Moving_Plat_PrePos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		if (player.Moving_Plat_Speed.x != 0.0f || player.Moving_Plat_Speed.z != 0.0f)player.Exit_Moving_Plat = true;
	}


	//=========Moving Wall Detect \\ checkBB only
	//bool onStair = false;
	//for (int i = 0; i < OBJNum; i++) {
	//	int targetCID = plat[Collider[OBJInRange[i]].masterID].colliderIdx;
	//	if (CheckHitByID(player.groundCheckColliderIdx, targetCID)) {
	//		COLLIDER3D targetC = Collider[targetCID];
	//		if (plat[targetC.masterID].canMove) {
	//			COLLIDER3D playerC = Collider[player.bodyColliderIdx];
	//			D3DXVECTOR3 speedDir;
	//			D3DXVECTOR3 speedXZ = D3DXVECTOR3(player.speed.x, 0.0f, player.speed.z);
	//			D3DXVec3Normalize(&speedDir, &speedXZ);

	//			if (targetC.rot.x == 0.0f && targetC.rot.y == 0.0f) { // no slope & no rot
	//				if ((player.pos.y - playerC.hy*0.4f < targetC.pos.y + targetC.hy*0.5f) && (player.pos.y + playerC.hy*0.4f > targetC.pos.y - targetC.hy*0.5f)) {//高さ=>地面になった 0.5 => 0.4 : prevent from wall detect

	//					//like stair
	//					if ((player.pos.y - (targetC.pos.y + targetC.hy*0.5f)) >= CAN_STAIR_HEIGHT) {
	//						player.pos.y += 2.5f;
	//						player.pos.x += player.dirXZ.x * 0.5f;
	//						player.pos.z += player.dirXZ.z * 0.5f;
	//						onStair = true;
	//					}
	//					if (!onStair) { // 移動
	//						if (playerC.pos.z + playerC.lz*0.5f > targetC.pos.z + targetC.lz*0.5f &&
	//							(playerC.pos.x - playerC.wx*0.5f < (targetC.pos.x + targetC.wx*0.5f) && (playerC.pos.x + playerC.wx*0.5f > (targetC.pos.x - targetC.wx*0.5f)))) {
	//							if (speedDir.z < 0)player.speed.z = 0.0f;
	//							player.pos.z = Collider[targetCID].pos.z + Collider[targetCID].lz*0.5f + Collider[player.bodyColliderIdx].lz*0.5f;
	//						}
	//						else if (playerC.pos.x + playerC.wx*0.5f > targetC.pos.x + targetC.wx*0.5f &&
	//							(playerC.pos.z - playerC.lz*0.5f < (targetC.pos.z + targetC.lz*0.5f) && (playerC.pos.z + playerC.lz*0.5f > (targetC.pos.z - targetC.lz*0.5f)))) {
	//							if (speedDir.x < 0)player.speed.x = 0.0f;
	//							player.pos.x = Collider[targetCID].pos.x + Collider[targetCID].wx*0.5f + Collider[player.bodyColliderIdx].wx*0.5f;
	//						}
	//						else if (playerC.pos.z - playerC.lz*0.5f < targetC.pos.z - targetC.lz*0.5f &&
	//							(playerC.pos.x - playerC.wx*0.5f < (targetC.pos.x + targetC.wx*0.5f) && (playerC.pos.x + playerC.wx*0.5f > (targetC.pos.x - targetC.wx*0.5f)))) {
	//							if (speedDir.z > 0)player.speed.z = 0.0f;
	//							player.pos.z = Collider[targetCID].pos.z - Collider[targetCID].lz*0.5f - Collider[player.bodyColliderIdx].lz*0.5f;
	//						}
	//						else if (playerC.pos.x - playerC.wx*0.5f < targetC.pos.x - targetC.wx*0.5f &&
	//							(playerC.pos.z - playerC.lz*0.5f < (targetC.pos.z + targetC.lz*0.5f) && (playerC.pos.z + playerC.lz*0.5f > (targetC.pos.z - targetC.lz*0.5f)))) {
	//							if (speedDir.x > 0)player.speed.x = 0.0f;
	//							player.pos.x = Collider[targetCID].pos.x - Collider[targetCID].wx*0.5f - Collider[player.bodyColliderIdx].wx*0.5f;
	//						}
	//					}
	//					if (player.On_Moving_Plat) {
	//						player.On_Moving_Plat_Pos.x = player.pos.x - Collider[player.On_Moving_PlatID].pos.x;
	//						player.On_Moving_Plat_Pos.z = player.pos.z - Collider[player.On_Moving_PlatID].pos.z;
	//					}

	//					//ダッシュとき壁にぶつかる => とめられ
	//					if (player.isDash || player.isFloat) {
	//						player.speed.y = 0.0f;
	//						player.isDash = false;
	//						player.isFloat = false;
	//					}
	//				}
	//			}
	//		}
	//	}
	//}
}
