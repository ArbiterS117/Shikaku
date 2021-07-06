//=============================================================================
//
// バレット処理 [bullet.cpp]
// Author : GP11B132 14 ゴショウケン
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "shadow.h"
#include "bullet.h"
#include "Collider3D.h"
#include "enemy.h"
#include "score.h"
#include "debugproc.h"
#include "particle.h"
#include "player.h"
#include "sound.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	BULLET_WIDTH		(80.0f)			// 頂点サイズ
#define	BULLET_HEIGHT		(80.0f)			// 頂点サイズ

#define	MAX_BULLET			(256)			// 木最大数

#define BULLET_SPEED        (5.0f)          // バレットのスピード
#define BULLET_LIVETIME     (202.0f)        // LIVETIME

//===card
#define CARD_SPEED             (35.0f)
#define CARD_DECELERATE        (1.3f)
#define CARD_FORWARDTIME       (7)
#define CARD_SUMMONBACK_SPEED  (50.0f)
//ability
#define WINDSTORM_LIVETIMETIME (25)
#define WATERBOB_LIVETIMETIME  (900)

//Timer
#define THROW_CARD_STAR_CDTIME (2)          // カード投げるの星
#define BULET_WATERABILITYCD   (10)         // カード能力アピールCD時間
static int waterAbilityTimer = 0;

//===============Anim Time (Start to Finish => no matter how many states it has) (frame)
#define AnimTime_WATERBOB_SPIN     (300.0f)
#define AnimTime_WATERBOB_HIT      (7.0f)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
static int     SetBullet(D3DXVECTOR3 pos, D3DXVECTOR3 dir);
static void    PhysicsUpdate(BULLET &bullet);
static void    CollisionUpdate(BULLET &bullet);
static void    SystemUpdate(BULLET &bullet);
static void    AnimUpdate(BULLET &bullet);

//*****************************************************************************
// グローバル変数
//*****************************************************************************					  
static BULLET		   g_bullet[MAX_BULLET];	        // 木ワーク
static bool			   g_bAlpaTest;		                // アルファテストON/OFF
static int			   g_nAlpha;			            // アルファテストの閾値

////===timer		
static int             throwCardStarTimer;				

// Sprite 資料
enum enumBulletSprite
{
	BulletSprite_tree,
	BulletSprite_bullet,
	BulletSprite_card,
	BulletSprite_windStorm,
	BulletSprite_waterBob,
	MAX_SPRITE
};
#define	SPRITE_tree		 "data/TEXTURE/tree001.png"	        // 128*128
#define	SPRITE_bullet    "data/TEXTURE/bullet000.png"		// 80*80
#define	SPRITE_card      "data/TEXTURE/card.png"	        // 256*256
#define	SPRITE_windStorm "data/TEXTURE/wind.png"	        // 512*512
#define	SPRITE_waterBob  "data/TEXTURE/bob.png"	            // 36*36

// Model 資料
enum enumBulletModel
{
	BulletModel_miku_01,
	BulletModel_miku_02,
	BulletModel_player2,
	BulletModel_card,
	BulletModel_waterBob,
	BulletModel_sword,
	MAX_MODEL
};
#define	MODEL_ENEMY		"data/MODEL/miku_01.obj"		// 読み込むモデル名
#define	MODEL_ENEMY2    "data/MODEL/miku_02.obj"		// 読み込むモデル名
#define	MODEL_ENEMY3	"data/MODEL/player2.obj"		// 読み込むモデル名
#define	MODEL_CARD      "data/MODEL/card.obj"	        
#define	MODEL_waterBob  "data/MODEL/waterBob.obj"	        

static DX11_MODEL	   g_Model[MAX_MODEL];				// モデル情報
static DX11_SPRITE	   g_Sprite[MAX_SPRITE];			// スプライト情報

static float             g_AnimTimer[MAX_BULLET];  // AnimTimer
static D3DXVECTOR3       g_AnimOriPos[MAX_BULLET]; // AnimTimer
static D3DXVECTOR3       g_AnimOriScl[MAX_BULLET]; // AnimTimer
static D3DXVECTOR3       g_AnimOriRot[MAX_BULLET]; // AnimTimer

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitBullet(void)
{
	LoadModel(MODEL_ENEMY ,        &g_Model[BulletModel_miku_01]);
	LoadModel(MODEL_ENEMY2,        &g_Model[BulletModel_miku_02]);
	LoadModel(MODEL_ENEMY3,        &g_Model[BulletModel_player2]);
	LoadModel(MODEL_CARD,          &g_Model[BulletModel_card]);
	LoadModel(MODEL_waterBob,      &g_Model[BulletModel_waterBob]);
	LoadModel(MODEL_CARD,         &g_Model[BulletModel_sword]);
	LoadSprite(SPRITE_tree,        &g_Sprite[BulletSprite_tree], 50.0f, 50.0f);
	LoadSprite(SPRITE_bullet,      &g_Sprite[BulletSprite_bullet], 50.0f, 50.0f);
	LoadSprite(SPRITE_card,        &g_Sprite[BulletSprite_card], 128.0f, 128.0f);
	LoadSprite(SPRITE_windStorm,   &g_Sprite[BulletSprite_windStorm], 128.0f, 128.0f);
	LoadSprite(SPRITE_waterBob,    &g_Sprite[BulletSprite_waterBob], 128.0f, 128.0f);

	// 初期化
	for (int i = 0; i < MAX_BULLET; i++)
	{
		g_bullet[i].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_bullet[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_bullet[i].scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		g_bullet[i].Use = false;

		g_bullet[i].dirXZ = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

		g_bullet[i].bulletType = BulletType_tree;
		g_bullet[i].liveTimer = 0;
		g_bullet[i].Status = BulletStatus_Idle;
	}

	g_bAlpaTest = true;	// true;
	g_nAlpha = 255;// 0x0;

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitBullet(void)
{
	// モデルの解放処理
	for (int i = 0; i < MAX_MODEL; i++) {
		UnloadModel(&g_Model[i]);
	}

	// スプライトの解放処理
	for (int i = 0; i < MAX_SPRITE; i++) {
		UnloadSprite(&g_Sprite[i]);
	}

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateBullet(void)
{

	for (int i = 0; i < MAX_BULLET; i++)
	{
		if (g_bullet[i].Use)
		{
			//================physic
			PhysicsUpdate(g_bullet[i]);

			//================collider detect
			CollisionUpdate(g_bullet[i]);

			//================sys
			SystemUpdate(g_bullet[i]);

			//================Anim
			AnimUpdate(g_bullet[i]);
			
		}
	}

	// アルファテストON/OFF
	if (GetKeyboardTrigger(DIK_F1))
	{
		//g_bAlpaTest = g_bAlpaTest ? false : true;
	}

	// アルファテストの閾値変更
	if (GetKeyboardPress(DIK_I))
	{
		g_nAlpha--;
		if (g_nAlpha < 0)
		{
			g_nAlpha = 0;
		}
	}
	if (GetKeyboardPress(DIK_K))
	{
		g_nAlpha++;
		if (g_nAlpha > 255)
		{
			g_nAlpha = 255;
		}
	}

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawBullet(void)
{
	// αテスト設定
	if (g_bAlpaTest == true)
	{
		// αテストを有効に
		SetAlphaTestEnable(true);
	}

	// ライティングを無効
	SetLightEnable(false);

	D3DXMATRIX mtxView, mtxScale, mtxRot, mtxTranslate;

	// ビューマトリックスを取得
	CAMERA *cam = GetCamera();
	mtxView = cam->mtxView;

	for (int i = 0; i < MAX_BULLET; i++)
	{
		if (g_bullet[i].Use)
		{
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_bullet[i].mtxWorld);

			if (g_bullet[i].drawType == BulletDrawType_2D_Bilboard) {
				// =========ポリゴンを正面に向ける
#if 0

			// 逆行列を計算で求める
				D3DXMatrixInverse(&g_bullet[i].mtxWorld, NULL, &mtxView); //激遅い
				g_bullet[i].mtxWorld._41 = 0.0f; //移動Tx値をクリア
				g_bullet[i].mtxWorld._42 = 0.0f; //移動Ty値をクリア
				g_bullet[i].mtxWorld._43 = 0.0f; //移動Tz値をクリア
#else
			//逆行列の作り方 正方行列
			//DirectXのカメラ行列は正方行列になっています
			//正方行列には性質がある
			//正方行列の転置行列は逆行列になる
			//転置行列 → 左上から右下へ向かって斜め線を引く
			//数値交換 → その線を対象として上下左右入れ替えた行列

			//このやり方は超早い
				g_bullet[i].mtxWorld._11 = mtxView._11;
				g_bullet[i].mtxWorld._12 = mtxView._21;
				g_bullet[i].mtxWorld._13 = mtxView._31;
				g_bullet[i].mtxWorld._21 = mtxView._12;
				g_bullet[i].mtxWorld._22 = mtxView._22;
				g_bullet[i].mtxWorld._23 = mtxView._32;
				g_bullet[i].mtxWorld._31 = mtxView._13;
				g_bullet[i].mtxWorld._32 = mtxView._23;
				g_bullet[i].mtxWorld._33 = mtxView._33;
				//g_bullet[i].mtxWorld._41 = 0.0f; //移動Tx値をクリア 移動しない
				//g_bullet[i].mtxWorld._42 = 0.0f; //移動Ty値をクリア
				//g_bullet[i].mtxWorld._43 = 0.0f; //移動Tz値をクリア
#endif
			// =========
			}

			// スケールを反映
			D3DXMatrixScaling(&mtxScale, g_bullet[i].scl.x,
				g_bullet[i].scl.y,
				g_bullet[i].scl.z);
			D3DXMatrixMultiply(&g_bullet[i].mtxWorld, &g_bullet[i].mtxWorld, &mtxScale);

			// 回転を反映
			if (g_bullet[i].drawType != BulletDrawType_2D_Bilboard) {
				D3DXMatrixRotationYawPitchRoll(&mtxRot, g_bullet[i].rot.y + D3DX_PI, g_bullet[i].rot.x, g_bullet[i].rot.z);
				D3DXMatrixMultiply(&g_bullet[i].mtxWorld, &g_bullet[i].mtxWorld, &mtxRot);
			}

			// 移動を反映
			D3DXMatrixTranslation(&mtxTranslate, g_bullet[i].pos.x,
				g_bullet[i].pos.y,
				g_bullet[i].pos.z);
			D3DXMatrixMultiply(&g_bullet[i].mtxWorld, &g_bullet[i].mtxWorld, &mtxTranslate);


			// ワールドマトリックスの設定
			SetWorldMatrix(&g_bullet[i].mtxWorld);

			/////////////////////////////////////////////////
			if (g_bullet[i].drawType == BulletDrawType_2D || g_bullet[i].drawType == BulletDrawType_2D_Bilboard)DrawSprite(&g_bullet[i].sprite);
			if (g_bullet[i].drawType == BulletDrawType_3D || g_bullet[i].drawType == BulletDrawType_3D_Bilboard)DrawModel(&g_bullet[i].model);

		}
	}

	// ライティングを有効に
	SetLightEnable(true);

	// αテストを無効に
	SetAlphaTestEnable(false); // 処理速度遅い
}

//=============================================================================
// 木のパラメータをセット
//=============================================================================
int SetBullet(D3DXVECTOR3 pos, D3DXVECTOR3 dir) {
	int nIdx = -1;

	for (int i = 0; i < MAX_BULLET; i++)
	{
		if (!g_bullet[i].Use)
		{
			nIdx = i;
			g_bullet[i].bulletID = i;

			g_bullet[i].pos = pos;
			g_bullet[i].scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
			g_bullet[i].Use = true;

			g_bullet[i].dirXZ = dir;
			float BaseAngle = atan2f(dir.z , dir.x);    // 中心点から頂点に対する角度
			g_bullet[i].rot = D3DXVECTOR3(D3DX_PI * 0.5f, 0.0f, BaseAngle + D3DX_PI * 0.5f);  // D3DX_PI => 1 ; 0 => -1 

			g_bullet[i].liveTimer = 0;

			break;
		}
	}

	return nIdx;
}

int SetBullet(enumBulletType bulletType, D3DXVECTOR3 pos, D3DXVECTOR3 dir, int cardAbility)
{
	int id = SetBullet(pos, dir);

	if(id == -1)return id;

	g_bullet[id].bulletType = bulletType;
	g_bullet[id].Status = BulletStatus_Idle;

	switch (g_bullet[id].bulletType)
	{
	case BulletType_tree:
		g_bullet[id].sprite = g_Sprite[BulletSprite_bullet];
		g_bullet[id].drawType = BulletDrawType_2D;
		g_bullet[id].scl = D3DXVECTOR3(2.0f, 4.0f, 2.0f);
		//collider 設定
		g_bullet[id].colliderIdx = CreateCollider3DCircle(collider3DTag_bullet, pos, 40.0f, g_bullet[id].bulletID);
		// 影の設定
		g_bullet[id].shadowIdx = CreateShadow(g_bullet[id].pos, 3.5f, 3.5f);
		break;

	case BulletType_box:
		g_bullet[id].model = g_Model[BulletModel_miku_02];
		g_bullet[id].drawType = BulletDrawType_3D;
		g_bullet[id].scl = D3DXVECTOR3(20.0f, 20.0f, 40.0f);
		//collider 設定
		g_bullet[id].colliderIdx = CreateCollider3DBox(collider3DTag_bullet, pos, 200.0f, 200.0f, 400.0f, g_bullet[id].bulletID);
		// 影の設定
		g_bullet[id].shadowIdx = CreateShadow(g_bullet[id].pos, 12.0f, 12.0f);
		break;

	case BulletType_card:
		g_bullet[id].model = g_Model[BulletModel_card];
		g_bullet[id].drawType = BulletDrawType_3D;
		g_bullet[id].scl = D3DXVECTOR3(20.0f, 20.0f, 20.0f);
		g_bullet[id].rot = D3DXVECTOR3(0.0f, D3DX_PI * 0.5f, 0.0f);
		
		g_bullet[id].speed = g_bullet[id].dirXZ * CARD_SPEED;
		g_bullet[id].cardForwardTimer = 0;
		g_bullet[id].cardIsForward = true;
		g_bullet[id].canSummonBack = false;
		g_bullet[id].isSummonBack = false;
		g_bullet[id].cardAbility = cardAbility;
		//collider 設定
		g_bullet[id].colliderIdx = CreateCollider3DBox(collider3DTag_card, pos, 200.0f, 200.0f, 200.0f, g_bullet[id].bulletID);
		// 影の設定
		g_bullet[id].shadowIdx = CreateShadow(g_bullet[id].pos, 3.5f, 3.5f);
		break;

	case BulletType_windStorm:
		g_bullet[id].model = g_Model[BulletModel_sword];
		g_bullet[id].drawType = BulletDrawType_3D;
		g_bullet[id].scl = D3DXVECTOR3(5.0f, 5.0f, 5.0f);
		g_bullet[id].rot = D3DXVECTOR3(0.0f, D3DX_PI * 0.5f, 0.0f);
		//collider 設定
		g_bullet[id].colliderIdx = CreateCollider3DBox(collider3DTag_bullet, pos, 600.0f, 120.0f, 600.0f, g_bullet[id].bulletID);
		// 影の設定
		g_bullet[id].shadowIdx = CreateShadow(g_bullet[id].pos, 3.5f, 3.5f);
		break;

	case BulletType_waterBob:
		g_bullet[id].model = g_Model[BulletModel_waterBob];
		g_bullet[id].drawType = BulletDrawType_3D;
		g_bullet[id].scl = D3DXVECTOR3(10.0f, 10.0f, 10.0f);
		g_bullet[id].model.SubsetArray->Material.Material.Diffuse.a = 0.51f;
		//collider 設定
		g_bullet[id].colliderIdx = CreateCollider3DCircle(collider3DTag_bullet, pos, 100.0f, g_bullet[id].bulletID);
		// 影の設定
		g_bullet[id].shadowIdx = CreateShadow(g_bullet[id].pos, 3.5f, 3.5f);
		break; 
	case BulletType_waterBobFloat:
		g_bullet[id].model = g_Model[BulletModel_waterBob];
		g_bullet[id].drawType = BulletDrawType_3D;
		g_bullet[id].scl = D3DXVECTOR3(20.0f, 20.0f, 20.0f);
		g_bullet[id].model.SubsetArray->Material.Material.Diffuse.a = 0.51f;
		//collider 設定
		g_bullet[id].colliderIdx = CreateCollider3DCircle(collider3DTag_bullet, pos, 100.0f, g_bullet[id].bulletID);
		// 影の設定
		g_bullet[id].shadowIdx = CreateShadow(g_bullet[id].pos, 3.5f, 3.5f);
		break;
	}
	g_AnimOriPos[id] = g_bullet[id].pos;
	g_AnimOriScl[id] = g_bullet[id].scl;
	g_AnimOriRot[id] = g_bullet[id].rot;
	return id;
}

void ReleaseBullet(int id)
{
	g_bullet[id].Use = false;
	ReleaseCollider3D(g_bullet[id].colliderIdx);
	ReleaseShadow(g_bullet[id].shadowIdx);
}

//=============================================================================
// 情報を取得
//=============================================================================
BULLET *GetBullet(void)
{
	return &g_bullet[0];
}
//=============================================================================
// UPDATE
//=============================================================================
void PhysicsUpdate(BULLET & bullet)
{
	switch (bullet.bulletType)
	{
	case BulletType_tree:
		bullet.pos += bullet.dirXZ * BULLET_SPEED;
		break;
	case BulletType_box:
		bullet.pos += bullet.dirXZ * BULLET_SPEED;
		break;
	case BulletType_card:
		if (bullet.cardIsForward == false) {
			if (D3DXVec3Length(&bullet.speed) > CARD_DECELERATE) { // forward
				bullet.speed -= bullet.dirXZ * CARD_DECELERATE;
				//particle
				throwCardStarTimer += 1;
				if (throwCardStarTimer >= THROW_CARD_STAR_CDTIME) {
					throwCardStarTimer = 0;
					SetParticle(bullet.pos, D3DXVECTOR3(bullet.dirXZ.x, 0.0f, bullet.dirXZ.z), Particletype_CardMove, 1);
				}
			}
			if (D3DXVec3Length(&bullet.speed) <= CARD_DECELERATE) {
				bullet.speed = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			}
			//rot
			bullet.rot.y = 0.0f;
			if (bullet.rot.x >= D3DX_PI * 2.0f)bullet.rot.x -= D3DX_PI * 2.0f;
			bullet.rot.x += D3DX_PI * 0.008f;
			if (bullet.rot.z >= D3DX_PI * 2.0f)bullet.rot.z -= D3DX_PI * 2.0f;
			bullet.rot.z += D3DX_PI * 0.008f;
		}
		if (bullet.isSummonBack) { // summonBack
			bullet.playerPos = GetPlayer()->pos;
			D3DXVECTOR3 cardDir = bullet.playerPos - bullet.pos;
			D3DXVec3Normalize(&cardDir, &cardDir);
			bullet.speed = cardDir * CARD_SUMMONBACK_SPEED;
			bullet.rot = D3DXVECTOR3(0.0f, D3DX_PI * 0.5f, 0.0f);
			//particle
			throwCardStarTimer += 1;
			if (throwCardStarTimer >= THROW_CARD_STAR_CDTIME) {
				throwCardStarTimer = 0;
				SetParticle(bullet.pos, D3DXVECTOR3(bullet.dirXZ.x, 0.0f, bullet.dirXZ.z), Particletype_CardMove, 1);
			}
		}

		bullet.pos += bullet.speed;
		break;
	case BulletType_waterBobFloat:
		PLAYER *player = GetPlayer();
		bullet.pos = player->pos;
		break;
	}
}

void CollisionUpdate(BULLET & bullet)
{
	//collider position
	SetPositionCollider3D(bullet.colliderIdx, bullet.pos);

	// Hit Enemy
	if (bullet.bulletType == BulletType_card) {
		int targetID = 0;
		if (CheckHitByTagReturnMasterID(bullet.colliderIdx, collider3DTag_enemy, &targetID)) {
			//destroy enemy
			ENEMY *enemy = GetEnemy();
			if (enemy[targetID].Status != EnemyStatus_Dead) { // dead => 当たらない
				//ReleaseEnemy(targetID);
				HitEnemy(targetID, bullet.dirXZ);

				//Gain Ability
				if (enemy[targetID].enemyType == EnemyType_wind)  bullet.cardAbility = CardAbility_wind;
				if (enemy[targetID].enemyType == EnemyType_water) bullet.cardAbility = CardAbility_water;
				if (enemy[targetID].enemyType == EnemyType_gem);

				if (enemy[targetID].enemyType == EnemyType_mushroomSpring) {
					if (bullet.isSummonBack == false) SetParticle(bullet.pos, bullet.dirXZ, Particletype_BumpSoftThings, 25);
					bullet.isSummonBack = true;
					PlaySound(SOUND_LABEL_SE_hit);
				}
				//destroy bullet
				if (bullet.bulletType != BulletType_card) {
					bullet.Use = false;
					ReleaseCollider3D(bullet.colliderIdx);
					ReleaseShadow(bullet.shadowIdx);
				}

				//AddScore(1);
			}
		}
	}
}

void SystemUpdate(BULLET & bullet)
{
	switch (bullet.bulletType)
	{
	case BulletType_box:
		bullet.liveTimer += 1;
		if (bullet.liveTimer > BULLET_LIVETIME) {
			bullet.Use = false;
			ReleaseCollider3D(bullet.colliderIdx);
			ReleaseShadow(bullet.shadowIdx);
		}
		break;

	case BulletType_tree:
		bullet.liveTimer += 1;
		if (bullet.liveTimer > BULLET_LIVETIME) {
			bullet.Use = false;
			ReleaseCollider3D(bullet.colliderIdx);
			ReleaseShadow(bullet.shadowIdx);
		}
		break;

	case BulletType_card:
		if (bullet.cardIsForward) {
			bullet.cardForwardTimer += 1;
			if (bullet.cardForwardTimer >= CARD_FORWARDTIME) {
				bullet.cardForwardTimer = 0;
				bullet.cardIsForward = false;
				bullet.canSummonBack = true;
			}
		}
		if (bullet.Use == false) GetPlayer()->cardInHand = true; // if card disappear, player can throw 

		//Card Ability Effect
		if (bullet.cardAbility == CardAbility_water) {
			waterAbilityTimer += 1;
			if (waterAbilityTimer >= BULET_WATERABILITYCD) {
				waterAbilityTimer = 0;
				SetParticle(bullet.pos, D3DXVECTOR3(0.0f, 1.0f, 0.0f), Particletype_WaterAbilityEffect, 3);
			}
		}
		else {
			waterAbilityTimer = 0;
		}

		break;

	case BulletType_windStorm:
		bullet.liveTimer += 1;
		if (bullet.liveTimer > WINDSTORM_LIVETIMETIME) {
			bullet.Use = false;
			ReleaseCollider3D(bullet.colliderIdx);
			ReleaseShadow(bullet.shadowIdx);
		}
		break;

	case BulletType_waterBob:
		bullet.liveTimer += 1;
		if (bullet.liveTimer > WATERBOB_LIVETIMETIME) {
			bullet.Use = false;
			ReleaseCollider3D(bullet.colliderIdx);
			ReleaseShadow(bullet.shadowIdx);
		}
		break;
	case BulletType_waterBobFloat:
		bullet.liveTimer += 1;
		if (bullet.liveTimer > WATERBOB_LIVETIMETIME) {
			bullet.Use = false;
			ReleaseCollider3D(bullet.colliderIdx);
			ReleaseShadow(bullet.shadowIdx);
		}
		PLAYER *player = GetPlayer();
		if (!player->isInBobStatus) {
			bullet.Status = BulletStatus_Hit;
		}
		break;
	}


	// 影の位置設定
	SetPositionShadow(bullet.shadowIdx, D3DXVECTOR3(bullet.pos.x, 0.1f, bullet.pos.z));

	bullet.sprite.Material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, (float)g_nAlpha / 255.0f);
} 

void AnimUpdate(BULLET &bullet) {
	if (bullet.PreStatus != bullet.Status) g_AnimTimer[bullet.bulletID] = 0.0f;// 状態変換
	//====waterBob
	if (bullet.bulletType == BulletType_waterBob || bullet.bulletType == BulletType_waterBobFloat) {
		if (bullet.Status == BulletStatus_Idle) {
			g_AnimTimer[bullet.bulletID] ++;
			if (g_AnimTimer[bullet.bulletID] <= AnimTime_WATERBOB_SPIN) { // rot
				bullet.rot.x = D3DXToRadian(45);
				bullet.rot.y += D3DXToRadian(360 / AnimTime_WATERBOB_SPIN);
			}
			else {
				g_AnimTimer[bullet.bulletID] = 0.0f;
				bullet.rot = g_AnimOriRot[bullet.bulletID];
				bullet.pos = g_AnimOriPos[bullet.bulletID];
				bullet.rot.x = D3DXToRadian(45);
			}
			if (g_AnimTimer[bullet.bulletID] <= 1*AnimTime_WATERBOB_SPIN / 2.0f) {//pos part1
				bullet.pos.y += 0.7f ;
			}
			else if (g_AnimTimer[bullet.bulletID] <= 2 * AnimTime_WATERBOB_SPIN / 2.0f) { // part2
				bullet.pos.y -= 0.7f;
			}
			
		}
		if (bullet.Status == BulletStatus_Hit) {
			g_AnimTimer[bullet.bulletID] ++;
			if (g_AnimTimer[bullet.bulletID] <= AnimTime_WATERBOB_HIT ) { // part 1
				bullet.scl.x *= 1.15f;
				bullet.scl.z *= 1.15f;
				bullet.scl.y /= 1.15f;
			}
		
			else { // finish
				g_AnimTimer[bullet.bulletID] = 0.0f;
				bullet.scl = g_AnimOriScl[bullet.bulletID];
				SetParticle(bullet.pos, D3DXVECTOR3(0.0f, 1.0f, 0.0f), Particletype_WaterSplash, 55);
				ReleaseBullet(bullet.bulletID);
			}
		}
	}

	bullet.PreStatus = bullet.Status;
}
