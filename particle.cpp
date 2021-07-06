//=============================================================================
//
// パーティクル処理 [particle.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "shadow.h"
#include "particle.h"
#include "player.h"
#include "sprite.h"
#include "debugproc.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_SPRITE          (7)			  
										  										  
#define SET_CONTINUE_PARTICLE_CDTIME     (1)  // 続ている粒子生成CD
#define SMOKE_PARTICLE_LIVETIME          (30) // スモーク粒子
#define CARD_MOVE_STAR_PARTICLE_LIVETIME (90) // スター粒子
#define DEAD_SS_PARTICLE_LIVETIME        (30) // スモーク粒子
#define BUMPSOFT_PARTICLE_LIVETIME       (30) // スモーク粒子
#define WATERSPLASH_PARTICLE_LIVETIME    (180) // スモーク粒子

#define CARD_MOVE_STAR_GRAVITY           (0.45)
#define WATERSPLASH_GRAVITY              (1.0)
										  
#define	MAX_PARTICLE		(512)             // パーティクル最大数
										  
#define	DISP_SHADOW						      // 影の表示
//#undef DISP_SHADOW

//*****************************************************************************
// 構造体定義
//*****************************************************************************


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
static int setContinueParticleCDTimer = 0; // 続ている粒子生成CD

void ignitelightBall();
void lightBallUpdate(PARTICLE &particle);
void straightUpdate(PARTICLE &particle);
void fallDownUpdate(PARTICLE &particle);
//*****************************************************************************
// グローバル変数
//*****************************************************************************
static D3DXMATRIX				g_mtxWorldParticle;				// ワールドマトリックス

static DX11_SPRITE		        g_Sprite[MAX_SPRITE];			// スプライト情報

static PARTICLE					g_Particle[MAX_PARTICLE];		// パーティクルワーク
static D3DXVECTOR3				g_posBase;						// ビルボード発生位置
static float					g_fWidthBase = 5.0f;			// 基準の幅
static float					g_fHeightBase = 10.0f;			// 基準の高さ
static bool						g_bPause = false;				// ポーズON/OFF
static float					g_roty = 0.0f;					// 移動方向
static float					g_spd = 0.0f;					// 移動スピード

static D3DXCOLOR                g_cardMoveStarColor = D3DXCOLOR(1.2f, 1.2f, 0.25f, 1.0f);

//========debug
bool LightBallSwitch = false;

// Sprite 資料
enum enumParticleSprite
{
	ParticleSprite_lightBall,
	ParticleSprite_smoke,
	ParticleSprite_star,
	ParticleSprite_sun,
	ParticleSprite_wC,
	ParticleSprite_rect,
	ParticleSprite_Bsmoke,
};
#define	SPRITE_lightBall	"data/TEXTURE/shadow000.jpg"	        // 読み込むスプライト名
#define	SPRITE_dust         "data/TEXTURE/Dust Particle.png"
#define	SPRITE_star         "data/TEXTURE/star.png"
#define	SPRITE_sun          "data/TEXTURE/sun.png"
#define	SPRITE_wC           "data/TEXTURE/wCircle_P.png"
#define	SPRITE_rect         "data/TEXTURE/White.png"
#define	SPRITE_Bdust        "data/TEXTURE/BDust Particle.png"
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitParticle(void)
{
	LoadSprite(SPRITE_lightBall, &g_Sprite[ParticleSprite_lightBall], 50.0f, 50.0f);
	LoadSprite(SPRITE_dust, &g_Sprite[ParticleSprite_smoke], 50.0f, 50.0f);
	LoadSprite(SPRITE_star, &g_Sprite[ParticleSprite_star], 16.0f, 16.0f);
	LoadSprite(SPRITE_sun, &g_Sprite[ParticleSprite_sun], 16.0f, 16.0f);
	LoadSprite(SPRITE_wC, &g_Sprite[ParticleSprite_wC], 32.0f, 32.0f);
	LoadSprite(SPRITE_rect, &g_Sprite[ParticleSprite_rect], 50.0f, 50.0f);
	LoadSprite(SPRITE_Bdust, &g_Sprite[ParticleSprite_Bsmoke], 50.0f, 50.0f);

	// パーティクルワークの初期化
	for (int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{
		g_Particle[nCntParticle].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Particle[nCntParticle].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Particle[nCntParticle].scl = D3DXVECTOR3(0.2f, 0.2f, 0.2f);
		g_Particle[nCntParticle].speed = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

		g_Particle[nCntParticle].sprite_w = 0.0f;
		g_Particle[nCntParticle].sprite_h = 0.0f;
		g_Particle[nCntParticle].nIdxShadow = -1;
		g_Particle[nCntParticle].liveTime = 0;
		g_Particle[nCntParticle].Use = false;
	}

	g_posBase = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_roty = 0.0f;
	g_spd = 0.0f;

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitParticle(void)
{
	// スプライトの解放処理
	for (int i = 0; i < MAX_SPRITE; i++) {
		UnloadSprite(&g_Sprite[i]);
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateParticle(void)
{
	PLAYER *pPlayer = GetPlayer();
	g_posBase = pPlayer->pos;

	if (setContinueParticleCDTimer > 0) {
		setContinueParticleCDTimer -= 1;
	}

	if (GetKeyboardTrigger(DIK_P))
	{
		g_bPause = g_bPause ? false : true;
	}
	if (GetKeyboardTrigger(DIK_O))
	{
		LightBallSwitch = LightBallSwitch ? false : true;
	}

	if (g_bPause == false)
	{
		for (int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
		{
			if (g_Particle[nCntParticle].Use) {
				switch (g_Particle[nCntParticle].particletype)
				{
				case Particletype_LightBall:
					lightBallUpdate(g_Particle[nCntParticle]);

					break;
				case Particletype_Smoke:
					straightUpdate(g_Particle[nCntParticle]);

					break;
				case Particletype_DashSmoke:
					straightUpdate(g_Particle[nCntParticle]);
					break;

				case Particletype_CardMove:
					fallDownUpdate(g_Particle[nCntParticle]);
					break;

				case Particletype_EnemyDead:
					if(g_Particle[nCntParticle].SpriteID == ParticleSprite_smoke)straightUpdate(g_Particle[nCntParticle]);
					if(g_Particle[nCntParticle].SpriteID == ParticleSprite_star)fallDownUpdate(g_Particle[nCntParticle]);
					break;
				case Particletype_BumpSoftThings:
					if (g_Particle[nCntParticle].SpriteID == ParticleSprite_wC)straightUpdate(g_Particle[nCntParticle]);
					if (g_Particle[nCntParticle].SpriteID == ParticleSprite_star)straightUpdate(g_Particle[nCntParticle]);
					break;
				case 	Particletype_WaterSplash:
					if (g_Particle[nCntParticle].SpriteID == ParticleSprite_smoke)fallDownUpdate(g_Particle[nCntParticle]);
					if (g_Particle[nCntParticle].SpriteID == ParticleSprite_wC)fallDownUpdate(g_Particle[nCntParticle]);
					break; 
				case 	Particletype_WaterAbilityEffect:
					if (g_Particle[nCntParticle].SpriteID == ParticleSprite_smoke)straightUpdate(g_Particle[nCntParticle]);
					if (g_Particle[nCntParticle].SpriteID == ParticleSprite_wC)straightUpdate(g_Particle[nCntParticle]);
					break;
				case 	Particletype_WalkOnGrass:
					if (g_Particle[nCntParticle].SpriteID == ParticleSprite_rect)fallDownUpdate(g_Particle[nCntParticle]);
					break;
				case 	Particletype_WalkOnSand:
					if (g_Particle[nCntParticle].SpriteID == ParticleSprite_Bsmoke)straightUpdate(g_Particle[nCntParticle]);
					break;
				default:
					break;
				}
			}

		}
	}

	// パーティクル発生
//		if((rand() % 2) == 0)
	{
		if(LightBallSwitch)ignitelightBall();
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawParticle(void)
{
	D3DXMATRIX mtxView, mtxScale, mtxTranslate , mtxRot;
	CAMERA *cam = GetCamera();
	// ビューマトリックスを取得
	mtxView = cam->mtxView;

	// ライティングを無効に
	SetLightEnable(false);

	// 加算合成に設定
	SetBlendState(BLEND_MODE_ADD);

	// Z比較無し
	SetDepthEnable(false);
	//SetAlphaTestEnable(true);

	for (int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{
		if (g_Particle[nCntParticle].Use)
		{
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_mtxWorldParticle);

			g_mtxWorldParticle._11 = mtxView._11;
			g_mtxWorldParticle._12 = mtxView._21;
			g_mtxWorldParticle._13 = mtxView._31;
			g_mtxWorldParticle._21 = mtxView._12;
			g_mtxWorldParticle._22 = mtxView._22;
			g_mtxWorldParticle._23 = mtxView._32;
			g_mtxWorldParticle._31 = mtxView._13;
			g_mtxWorldParticle._32 = mtxView._23;
			g_mtxWorldParticle._33 = mtxView._33;

			// スケールを反映
			D3DXMatrixScaling(&mtxScale, g_Particle[nCntParticle].scl.x, g_Particle[nCntParticle].scl.y, g_Particle[nCntParticle].scl.z);
			D3DXMatrixMultiply(&g_mtxWorldParticle, &g_mtxWorldParticle, &mtxScale);

			D3DXMatrixRotationYawPitchRoll(&mtxRot, g_Particle[nCntParticle].rot.y , g_Particle[nCntParticle].rot.x, g_Particle[nCntParticle].rot.z);
			D3DXMatrixMultiply(&g_mtxWorldParticle, &g_mtxWorldParticle, &mtxRot);

			// 移動を反映
			D3DXMatrixTranslation(&mtxTranslate, g_Particle[nCntParticle].pos.x, g_Particle[nCntParticle].pos.y, g_Particle[nCntParticle].pos.z);
			D3DXMatrixMultiply(&g_mtxWorldParticle, &g_mtxWorldParticle, &mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&g_mtxWorldParticle);

			// マテリアル設定
			//SetMaterial(g_Particle[nCntParticle].sprite.Material); ///////////////////////////// 需要把此材質設至到SPRITE
			//g_Particle[nCntParticle].sprite.Material.Diffuse = D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.0f);

			// ポリゴンの描画
			DrawSprite(&g_Particle[nCntParticle].sprite);
		}
	}

	// ライティングを有効に
	SetLightEnable(true);

	// 通常ブレンドに戻す
	SetBlendState(BLEND_MODE_ALPHABLEND);

	// Z比較有効
	SetDepthEnable(true);
	//SetAlphaTestEnable(false); // 処理速度遅い
}

//=============================================================================
// マテリアルカラーの設定
//=============================================================================
void SetColorParticle(int nIdxParticle, D3DXCOLOR col)
{
	g_Particle[nIdxParticle].sprite.Material.Diffuse = col;
}

//=============================================================================
// パーティクルの作成
//=============================================================================
//続いて
void SetParticleContinuous(D3DXVECTOR3 pos, D3DXVECTOR3 speed, D3DXCOLOR col, particleType type, float sprite_w, float sprite_h, int liveTime)
{

	//continueous particle CD Time
	bool isreturn = false;
	if (setContinueParticleCDTimer != 0 && type == Particletype_LightBall) isreturn = true;
	if (setContinueParticleCDTimer == 0 && type == Particletype_LightBall) setContinueParticleCDTimer = SET_CONTINUE_PARTICLE_CDTIME;
	if (isreturn) return;

	for (int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{
		if (!g_Particle[nCntParticle].Use)
		{
			/////=========== detect type => choose sprite ===============////////////////////////////////////////
			g_Particle[nCntParticle].particletype = type;
			g_Particle[nCntParticle].sprite = g_Sprite[0];

			g_Particle[nCntParticle].Use = true;
			g_Particle[nCntParticle].pos = pos;
			g_Particle[nCntParticle].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			g_Particle[nCntParticle].scl = D3DXVECTOR3(0.3f, 0.3f, 0.3f);
			g_Particle[nCntParticle].speed = speed;
			g_Particle[nCntParticle].sprite_w = sprite_w;
			g_Particle[nCntParticle].sprite_h = sprite_h;
			g_Particle[nCntParticle].liveTime = liveTime;

#ifdef DISP_SHADOW
			// 影の設定
			//g_Particle[nCntParticle].nIdxShadow = CreateShadow(D3DXVECTOR3(pos.x, 0.1f, pos.z), 0.8f, 0.8f);		// 影の設定
			if (g_Particle[nCntParticle].nIdxShadow != -1)
			{
				SetColorShadow(g_Particle[nCntParticle].nIdxShadow, D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f));
			}
#endif

			switch (g_Particle[nCntParticle].particletype)
			{
			case Particletype_LightBall:
				g_Particle[nCntParticle].sprite = g_Sprite[ParticleSprite_lightBall];
				g_Particle[nCntParticle].sprite.Material.Diffuse = col;

				break;
			
			default:
				break;
			}


			break;
		}
	}

}

//==========瞬発
void SetParticle(D3DXVECTOR3 pos, D3DXVECTOR3 dir, particleType type, int particlenum)
{
	for (int n = 0; n < particlenum; n++) {         // 生成したい粒子の数
		// もし未使用の粒子が無かったら生成しない( =これ以上生成てないって事 )
		for (int i = 0; i < MAX_PARTICLE; i++)
		{
			if (g_Particle[i].Use == false)		// 未使用状態の粒子を見つける
			{
				g_Particle[i].particletype = type;
				g_Particle[i].sprite = g_Sprite[0];

				g_Particle[i].Use = true;			// 使用状態へ変更する
				g_Particle[i].pos = pos;			// 座標をセット
				g_Particle[i].particletype = type;
				g_Particle[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
				g_Particle[i].scl = D3DXVECTOR3(2.0f, 2.0f, 2.0f);
				
#ifdef DISP_SHADOW
				// 影の設定
				//g_Particle[i].nIdxShadow = CreateShadow(D3DXVECTOR3(pos.x, 0.1f, pos.z), 0.8f, 0.8f);		// 影の設定
				if (g_Particle[i].nIdxShadow != -1)
				{
					SetColorShadow(g_Particle[i].nIdxShadow, D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f));
				}
#endif	
				int randDirx = 0;
				int randDiry = 0;
				int randDirz = 0;
				//==========================TYPE======================================
				switch (g_Particle[i].particletype)
				{
				case Particletype_Smoke:
					g_Particle[i].sprite = g_Sprite[ParticleSprite_smoke];
					g_Particle[i].scl = D3DXVECTOR3(1.5f, 1.5f, 1.5f);
					randDirx = rand() % 15 - 7;
					randDirz = rand() % 15 - 7;
					g_Particle[i].liveTime = SMOKE_PARTICLE_LIVETIME;
					g_Particle[i].speed = D3DXVECTOR3( (float)randDirx * 1.25f , 0.0f, (float)randDirz * 1.25f);

					break;
				case Particletype_DashSmoke:
					g_Particle[i].sprite = g_Sprite[ParticleSprite_smoke];
					g_Particle[i].scl = D3DXVECTOR3(1.5f, 1.5f, 1.5f);
					randDirx = rand() % 30 -15;
					randDiry = rand() % 30 - 5;
					randDirz = rand() % 30 -15;
					g_Particle[i].liveTime = SMOKE_PARTICLE_LIVETIME;
					g_Particle[i].speed = D3DXVECTOR3( (-dir.x + dir.z) *(float)randDirx, (float)randDiry, (-dir.z - dir.x) * (float)randDirz);
					break;

				case Particletype_CardMove:
					g_Particle[i].sprite = g_Sprite[ParticleSprite_star];
					g_Particle[i].scl = D3DXVECTOR3(1.75f, 1.75f, 1.75f);
					randDirx = rand() % 6 - 3;
					randDiry = rand() % 10 + 10;
					randDirz = rand() % 6 - 3;
					g_Particle[i].liveTime = CARD_MOVE_STAR_PARTICLE_LIVETIME;
					g_Particle[i].speed = D3DXVECTOR3((-dir.x + dir.z) *(float)randDirx, (float)randDiry, (-dir.z - dir.x) * (float)randDirz);
					g_Particle[i].gravityScale = CARD_MOVE_STAR_GRAVITY;
					g_Particle[i].sprite.Material.Diffuse = g_cardMoveStarColor;
					break;

				case Particletype_EnemyDead:
					//smoke
					if (n < particlenum / 2) {
						g_Particle[i].sprite = g_Sprite[ParticleSprite_smoke];
						g_Particle[i].SpriteID = ParticleSprite_smoke;
						g_Particle[i].scl = D3DXVECTOR3(2.5f, 2.5f, 2.5f);
						randDirx = rand() % 30 - 15;
						randDiry = rand() % 30 - 5;
						randDirz = rand() % 30 - 15;
						g_Particle[i].liveTime = DEAD_SS_PARTICLE_LIVETIME;
						g_Particle[i].speed = D3DXVECTOR3((float)randDirx, (float)randDiry, (float)randDirz);
					}
					//star
					else {
						g_Particle[i].sprite = g_Sprite[ParticleSprite_star];
						g_Particle[i].SpriteID = ParticleSprite_star;
						g_Particle[i].scl = D3DXVECTOR3(4.75f, 4.75f, 4.75f);
						randDirx = rand() % 40 - 20;
						randDiry = rand() % 10 + 10;
						randDirz = rand() % 40 - 20;
						g_Particle[i].liveTime = DEAD_SS_PARTICLE_LIVETIME + 10;
						g_Particle[i].speed = D3DXVECTOR3((float)randDirx, (float)randDiry, (float)randDirz);
						g_Particle[i].gravityScale = CARD_MOVE_STAR_GRAVITY;
						g_Particle[i].sprite.Material.Diffuse = g_cardMoveStarColor;
					}
					break;

				case Particletype_BumpSoftThings:
					//wC
					if (n < particlenum / 2) {
						g_Particle[i].sprite = g_Sprite[ParticleSprite_wC];
						g_Particle[i].SpriteID = ParticleSprite_wC;
						g_Particle[i].scl = D3DXVECTOR3(2.0f, 2.0f, 2.0f);
						randDirx = rand() % 30 - 15;
						randDiry = rand() % 30 - 5;
						randDirz = rand() % 30 - 15;												
						g_Particle[i].liveTime = BUMPSOFT_PARTICLE_LIVETIME + 10;
						g_Particle[i].speed = D3DXVECTOR3((float)randDirx, (float)randDiry, (float)randDirz);
						float colorR, colorG, colorB;
						colorR = (float)(rand() % 5 + 6)* 0.1f;
						colorG = (float)(rand() % 4 + 2)* 0.1f;
						colorB = (float)(rand() % 4 + 2)* 0.1f;
						g_Particle[i].sprite.Material.Diffuse = D3DXCOLOR(colorR, colorG, colorB, 1.0f);
					}
					//star
					else {
						g_Particle[i].sprite = g_Sprite[ParticleSprite_star];
						g_Particle[i].SpriteID = ParticleSprite_star;
						g_Particle[i].scl = D3DXVECTOR3(3.0f, 3.0f, 3.0f);
						randDirx = rand() % 40 - 20;
						randDiry = rand() % 10 + 10;
						randDirz = rand() % 40 - 20;
						g_Particle[i].liveTime = BUMPSOFT_PARTICLE_LIVETIME ;
						g_Particle[i].speed = D3DXVECTOR3((float)randDirx, (float)randDiry, (float)randDirz);
						g_Particle[i].sprite.Material.Diffuse = g_cardMoveStarColor;
					}
					break;
				case Particletype_WaterSplash:
					//wC
					if (n < particlenum / 3) {
						g_Particle[i].sprite = g_Sprite[ParticleSprite_wC];
						g_Particle[i].SpriteID = ParticleSprite_wC;
						g_Particle[i].scl = D3DXVECTOR3(2.0f, 2.0f, 2.0f);
						randDirx = rand() % 30 - 15;
						randDiry = rand() % 30 + 10;
						randDirz = rand() % 30 - 15;
						g_Particle[i].liveTime = WATERSPLASH_PARTICLE_LIVETIME;
						g_Particle[i].speed = D3DXVECTOR3((float)randDirx, (float)randDiry, (float)randDirz);
						g_Particle[i].gravityScale = WATERSPLASH_GRAVITY;
						float colorR, colorG, colorB;
						colorR = (float)(rand() % 3 + 2) *0.1f;
						colorG = (float)(rand() % 3 + 2) *0.1f;
						colorB = (float)(rand() % 5 + 6) *0.1f;
						g_Particle[i].sprite.Material.Diffuse = D3DXCOLOR(colorR, colorG, colorB, 1.0f);
					}
					//smoke
					else {
						g_Particle[i].sprite = g_Sprite[ParticleSprite_smoke];
						g_Particle[i].SpriteID = ParticleSprite_smoke;
						g_Particle[i].scl = D3DXVECTOR3(1.75f, 1.75f, 1.75f);
						randDirx = rand() % 20 - 10;
						randDiry = rand() % 10 + 30;
						randDirz = rand() % 20 - 10;
						g_Particle[i].liveTime = WATERSPLASH_PARTICLE_LIVETIME + (float)(rand() % 15);
						g_Particle[i].speed = D3DXVECTOR3((float)randDirx, (float)randDiry, (float)randDirz);
						g_Particle[i].gravityScale = WATERSPLASH_GRAVITY;
						g_Particle[i].sprite.Material.Diffuse = D3DXCOLOR(0.15f, 0.15f, 0.85f, 1.0f);
					}
					break;
				case Particletype_WaterAbilityEffect:
					//wC
					if (n < particlenum / 3) {
						g_Particle[i].sprite = g_Sprite[ParticleSprite_wC];
						g_Particle[i].SpriteID = ParticleSprite_wC;
						g_Particle[i].scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
						randDirx = rand() % 15 - 7;
						randDiry = rand() % 10 + 7;
						randDirz = rand() % 15 - 7;
						g_Particle[i].liveTime = WATERSPLASH_PARTICLE_LIVETIME;
						g_Particle[i].speed = D3DXVECTOR3((float)randDirx, (float)randDiry, (float)randDirz);
						g_Particle[i].gravityScale = WATERSPLASH_GRAVITY;
						float colorR, colorG, colorB;
						colorR = (float)(rand() % 3 + 2) *0.1f;
						colorG = (float)(rand() % 3 + 2) *0.1f;
						colorB = (float)(rand() % 5 + 6) *0.1f;
						g_Particle[i].sprite.Material.Diffuse = D3DXCOLOR(colorR, colorG, colorB, 1.0f);
					}
					//smoke
					else {
						g_Particle[i].sprite = g_Sprite[ParticleSprite_smoke];
						g_Particle[i].SpriteID = ParticleSprite_smoke;
						g_Particle[i].scl = D3DXVECTOR3(0.75f, 0.75f, 0.75f);
						randDirx = rand() % 10 - 5;
						randDiry = rand() % 10 + 7;
						randDirz = rand() % 10 - 5;
						g_Particle[i].liveTime = WATERSPLASH_PARTICLE_LIVETIME + (float)(rand() % 15);
						g_Particle[i].speed = D3DXVECTOR3((float)randDirx, (float)randDiry, (float)randDirz);
						g_Particle[i].gravityScale = WATERSPLASH_GRAVITY;
						g_Particle[i].sprite.Material.Diffuse = D3DXCOLOR(0.15f, 0.15f, 0.85f, 1.0f);
					}
					break;
				case Particletype_WalkOnGrass:
					g_Particle[i].sprite = g_Sprite[ParticleSprite_rect];
					g_Particle[i].scl = D3DXVECTOR3(0.2f, 0.7f, 0.2f);
					g_Particle[i].SpriteID = ParticleSprite_rect;
					randDirx = rand() % 15 - 7;
					randDiry = rand() % 10 + 17;
					randDirz = rand() % 15 - 7;
					g_Particle[i].liveTime = SMOKE_PARTICLE_LIVETIME + (float)(rand() % 15);
					g_Particle[i].gravityScale = WATERSPLASH_GRAVITY;
					g_Particle[i].speed = D3DXVECTOR3((float)randDirx * 1.25f, randDiry, (float)randDirz * 1.25f);
					g_Particle[i].sprite.Material.Diffuse = D3DXCOLOR(0.25f, 0.85f, 0.25f, 0.7f);

					break;
				case Particletype_WalkOnSand:
					g_Particle[i].sprite = g_Sprite[ParticleSprite_Bsmoke];
					g_Particle[i].scl = D3DXVECTOR3(0.3f, 0.3f, 0.3f);
					g_Particle[i].SpriteID = ParticleSprite_Bsmoke;
					randDirx = rand() % 15 - 7;
					randDiry = rand() % 10 + 7;
					randDirz = rand() % 15 - 7;
					g_Particle[i].liveTime = SMOKE_PARTICLE_LIVETIME + (float)(rand() % 3 - 6);
					g_Particle[i].speed = D3DXVECTOR3((float)randDirx * 1.25f, randDiry, (float)randDirz * 1.25f);
					g_Particle[i].sprite.Material.Diffuse = D3DXCOLOR(0.2f, 0.2f, 0.0f, 1.0f);

					break;
				default:
					break;
				}
				break;
			}
		}
	}
	return;							// 1セットしたので終了する
}
//============Updates==========================
void lightBallUpdate(PARTICLE &particle) {
	
	if (particle.Use)
	{// 使用中
		particle.pos.x += particle.speed.x;
		particle.pos.z += particle.speed.z;

		particle.pos.y += particle.speed.y;
		if (particle.pos.y <= particle.sprite_h / 2)
		{// 着地した
			particle.pos.y = particle.sprite_h / 2;
			particle.speed.y = -particle.speed.y * 0.75f;
		}

		particle.speed.x += (0.0f - particle.speed.x) * 0.015f;
		particle.speed.y -= 0.25f;
		particle.speed.z += (0.0f - particle.speed.z) * 0.015f;

#ifdef DISP_SHADOW
		if (particle.nIdxShadow != -1)
		{// 影使用中
			float colA;

			// 影の位置設定
			SetPositionShadow(particle.nIdxShadow, D3DXVECTOR3(particle.pos.x, 0.1f, particle.pos.z));

			// 影の色の設定
			colA = particle.sprite.Material.Diffuse.a;
			SetColorShadow(particle.nIdxShadow, D3DXCOLOR(0.5f, 0.5f, 0.5f, colA));
		}
#endif

		//liveTime
		particle.liveTime--;
		if (particle.liveTime <= 0)
		{
			particle.Use = false;
			ReleaseShadow(particle.nIdxShadow);
			particle.nIdxShadow = -1;
		}
		else
		{
			if (particle.liveTime <= 80)
			{
				particle.sprite.Material.Diffuse.r = 0.4f + (float)(80 + particle.liveTime) / 80 * 0.2f;
				particle.sprite.Material.Diffuse.g = 0.6f + (float)(80 - particle.liveTime) / 80 * 0.2f;
				particle.sprite.Material.Diffuse.b = 0.2f + (float)(80 - particle.liveTime) / 80 * 0.0f;
			}

			if (particle.liveTime <= 20)
			{
				// α値設定
				particle.sprite.Material.Diffuse.a -= 0.05f;
				if (particle.sprite.Material.Diffuse.a < 0.0f)
				{
					particle.sprite.Material.Diffuse.a = 0.0f;
				}
			}
		}

	}
}

void straightUpdate(PARTICLE &particle) {
	
	particle.pos += particle.speed;

	if (particle.SpriteID != ParticleSprite_Bsmoke) {
		particle.sprite.Material.Diffuse.a = particle.liveTime / SMOKE_PARTICLE_LIVETIME - 0.2f;
	}
	//liveTime
	particle.liveTime--;
	if (particle.liveTime <= 0)
	{
		particle.Use = false;
		ReleaseShadow(particle.nIdxShadow);
		particle.nIdxShadow = -1;
	}
	
}

void fallDownUpdate(PARTICLE & particle)
{
	//decelerate
	D3DXVECTOR3 starDir = particle.speed;
	D3DXVec3Normalize(&starDir, &starDir);
	particle.speed.y -= particle.gravityScale;

	particle.pos += particle.speed;
	if (particle.SpriteID != ParticleSprite_rect) {
		particle.sprite.Material.Diffuse.a = particle.liveTime / CARD_MOVE_STAR_PARTICLE_LIVETIME - 0.2f;
	}
	//if (particle.particletype == Particletype_EnemyDead) particle.sprite.Material.Diffuse.a = particle.liveTime / CARD_MOVE_STAR_PARTICLE_LIVETIME;

	//liveTime
	particle.liveTime--;
	if (particle.liveTime <= 0)
	{
		particle.Use = false;
		ReleaseShadow(particle.nIdxShadow);
		particle.nIdxShadow = -1;
	}

	if (particle.SpriteID == ParticleSprite_rect) {
		particle.rot.z += 0.05 * D3DX_PI;
	}
	
}

//========================================= other
void ignitelightBall() {
	if (g_bPause) return;
	D3DXVECTOR3 pos;
	D3DXVECTOR3 speed;
	float fAngle, fLength;
	int liveTime;
	float fSize;

	pos = g_posBase;

	fAngle = (float)(rand() % 628 - 314) / 100.0f;
	fLength = rand() % (int)(g_fWidthBase * 200) / 100.0f - g_fWidthBase;
	speed.x = sinf(fAngle) * fLength;
	speed.y = rand() % 300 / 100.0f + g_fHeightBase;
	speed.z = cosf(fAngle) * fLength;

	liveTime = rand() % 100 + 150;

	fSize = (float)(rand() % 1 + 5);

	pos.y += fSize / 2;  ////////////////////////////////////////////////////////////

	// ビルボードの設定
	SetParticleContinuous(pos, speed, D3DXCOLOR(0.4f, 0.6f, 0.2f, 0.85f), Particletype_LightBall, fSize, fSize, liveTime);
}
