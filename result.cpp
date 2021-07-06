//=============================================================================
//
// リザルト画面処理 [result.cpp]
// Author : GP11B132 14 ゴショウケン
//
//=============================================================================
#include "result.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "setSprite.h"
#include "gameScene.h"
#include "score.h"
#include "debugproc.h"
#include "UISprite.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 
#define TEXTURE_MAX					(12)		    // テクスチャの数

#define TEXTURE_WIDTH_LOGO			(400)			// ロゴサイズ
#define TEXTURE_HEIGHT_LOGO			(90)			// 

#define TEXTURE_WIDTH_RANKMARKS     (400)			// ランクマークサイズ
#define TEXTURE_HEIGHT_RANKMARKS    (400)			// 

#define TEXTURE_WIDTH_BLOCK			(64)	        // ブロック背景サイズ
#define TEXTURE_HEIGHT_BLOCK		(64)	        // 
#define BLOCK_MOVE_TIME             (TEXTURE_HEIGHT_BLOCK * 2)

#define TEXTURE_WIDTH_BTN			(400)			// ボタンサイズ
#define TEXTURE_HEIGHT_BTN			(90)			// 

#define TEXTURE_WIDTH_TEXT			(24)	        // 数字のテクスチャサイズ
#define TEXTURE_HEIGHT_TEXT			(48)	        // 
#define NUN_MAX			            (99)		    // 数字の最大値
#define NUN_DIGIT			         (2)			// 桁数

#define CONFIRM_TIME                (75)

#define MAX_UISPRITE                (15)

#define UISPRITE_BLOCK             "data/TEXTURE/Brown.png"
#define UISPRITE_CLEARTIME         "data/TEXTURE/result/clearTime.png"
#define UISPRITE_DEATHCNT          "data/TEXTURE/result/deathCount.png"
#define UISPRITE_RANK              "data/TEXTURE/result/rank.png"
#define UISPRITE_RS                "data/TEXTURE/result/rankS.png"
#define UISPRITE_RA                "data/TEXTURE/result/rankA.png"
#define UISPRITE_RB                "data/TEXTURE/result/rankB.png"
#define UISPRITE_RC                "data/TEXTURE/result/rankC.png"
#define UISPRITE_RD                "data/TEXTURE/result/rankD.png"
#define UISPRITE_NUM               "data/TEXTURE/number16x32.png"
#define UISPRITE_PRESS_BTN         "data/TEXTURE/pressbutton.png"
#define UISPRITE_SMOKE             "data/TEXTURE/Dust Particle.png"
#define UISPRITE_STAR              "data/TEXTURE/result/StarNum.png"
#define UISPRITE_COIN              "data/TEXTURE/result/coin.png"
#define UISPRITE_TFP               "data/TEXTURE/result/TFP.png"

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static DX11_UISPRITE  g_UISprite[MAX_UISPRITE]; // UIスプライト情報

//Anim
int re_blockMoveTimer = 0;

//system
int confirmStep = 0; // 1 => clearTime ; 2 => deaths count ; 3 => star; 4 => coin ;5 => rank ; 6 => Please Press button (SetMode Enable)
int confirmTimer = 0;
bool re_canInput = true;
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitResult(void)
{
	LoadUISprite(UISPRITE_BLOCK,     &g_UISprite[0], TEXTURE_WIDTH_LOGO, TEXTURE_HEIGHT_LOGO, SCREEN_CENTER_X, SCREEN_CENTER_Y);
	LoadUISprite(UISPRITE_CLEARTIME, &g_UISprite[1], TEXTURE_WIDTH_LOGO, TEXTURE_HEIGHT_LOGO, SCREEN_WIDTH / 2 - 100, 50);
	LoadUISprite(UISPRITE_DEATHCNT,  &g_UISprite[2], 1259 * 0.5, 186 * 0.5, SCREEN_CENTER_X - 100, 150);
	LoadUISprite(UISPRITE_RANK,      &g_UISprite[3], TEXTURE_WIDTH_LOGO, TEXTURE_HEIGHT_LOGO, SCREEN_CENTER_X - 100, 250);
	LoadUISprite(UISPRITE_RS,        &g_UISprite[4], TEXTURE_WIDTH_RANKMARKS, TEXTURE_HEIGHT_RANKMARKS,  SCREEN_CENTER_X + 350, SCREEN_CENTER_Y - 50.0f);
	LoadUISprite(UISPRITE_RA,        &g_UISprite[5], TEXTURE_WIDTH_RANKMARKS/1.5f, TEXTURE_HEIGHT_RANKMARKS/1.5f,  SCREEN_CENTER_X + 350, SCREEN_CENTER_Y - 50.0f);
	LoadUISprite(UISPRITE_RB,        &g_UISprite[6], TEXTURE_WIDTH_RANKMARKS/2.5f, TEXTURE_HEIGHT_RANKMARKS/2.5f,  SCREEN_CENTER_X + 350, SCREEN_CENTER_Y - 50.0f);
	LoadUISprite(UISPRITE_RC,        &g_UISprite[7], TEXTURE_WIDTH_RANKMARKS/3.5f, TEXTURE_HEIGHT_RANKMARKS/3.5f,  SCREEN_CENTER_X + 350, SCREEN_CENTER_Y - 50.0f);
	LoadUISprite(UISPRITE_RD,        &g_UISprite[8], TEXTURE_WIDTH_RANKMARKS/3.5f, TEXTURE_HEIGHT_RANKMARKS/3.5f,  SCREEN_CENTER_X + 350, SCREEN_CENTER_Y - 50.0f);
	LoadUISprite(UISPRITE_NUM,       &g_UISprite[9], TEXTURE_WIDTH_TEXT, TEXTURE_HEIGHT_TEXT, 0.0f, 0.0f, 10, 1);
	LoadUISprite(UISPRITE_PRESS_BTN, &g_UISprite[10], TEXTURE_WIDTH_BTN, TEXTURE_HEIGHT_BTN, SCREEN_CENTER_X + 200, SCREEN_CENTER_Y + 200.0f);
	LoadUISprite(UISPRITE_SMOKE,     &g_UISprite[11], 24, 24, 0.0f, 0.0f);
	LoadUISprite(UISPRITE_STAR,      &g_UISprite[12], 400, 98, 0.0f, 0.0f);
	LoadUISprite(UISPRITE_COIN,      &g_UISprite[13], 256, 256, 0.0f, 0.0f);
	LoadUISprite(UISPRITE_TFP,       &g_UISprite[14], 450, 65, 225.0f, 500.0f);
	
	// BGM再生
	//PlaySound(SOUND_LABEL_BGM_sample002);

	//system
	re_blockMoveTimer = 0;
	confirmStep = 0;
	confirmTimer = 0;
	re_canInput = true;


	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitResult(void)
{
	// スプライトの解放処理
	for (int i = 0; i < MAX_UISPRITE; i++) {
		UnloadUISprite(&g_UISprite[i]);
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateResult(void)
{
	//ブラック移動
	re_blockMoveTimer += 1;
	if (re_blockMoveTimer >= BLOCK_MOVE_TIME) {
		re_blockMoveTimer = 0;
	}

	//自動コンフォーム
	confirmTimer += 1;
	if (confirmTimer >= CONFIRM_TIME) {
		confirmTimer = 0;
		confirmStep += 1;
		if(confirmStep <= 4) PlaySound(SOUND_LABEL_SE_start);
		if(confirmStep == 5) PlaySound(SOUND_LABEL_SE_hit2);
		
	}

	//input
	if (re_canInput == true) {
		if (GetKeyboardTrigger(DIK_SPACE) || IsButtonTriggered(0, BUTTON_A) || IsButtonTriggered(0, BUTTON_B) || IsButtonTriggered(0, BUTTON_X) || IsMouseLeftTriggered() || IsMouseRightTriggered())
		{
			confirmTimer = 0;
			confirmStep += 1;
			if (confirmStep <= 4) PlaySound(SOUND_LABEL_SE_start);
			if (confirmStep == 5) PlaySound(SOUND_LABEL_SE_hit2);
		}
	}
	// Enter押したら、ステージを切り替える
	
	if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_START))
	{
			
		if (confirmStep > 4) {
			SetFade(FADE_OUT, MODE_TITLE);
			PlaySound(SOUND_LABEL_SE_start);
		}
		else {
			confirmStep += 1;
			if (confirmStep <= 4) PlaySound(SOUND_LABEL_SE_start);
			if (confirmStep == 5) PlaySound(SOUND_LABEL_SE_hit2);
		}
			
	}
	

#ifdef _DEBUG	// デバッグ情報を表示する
	//char *str = GetDebugStr();
	//sprintf(&str[strlen(str)], " PX:%.2f PY:%.2f", g_Pos.x, g_Pos.y);

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawResult(void)
{
	//数字描画用
	int                 number;
	float				g_w;
	static D3DXVECTOR3	g_Pos;

	// 2D描画なので深度無効
	SetDepthEnable(false);
	SetLightEnable(false);

	// ブロックの背景を描画
	{
		for (int j = -1; j < SCREEN_HEIGHT / TEXTURE_HEIGHT_BLOCK + 2; j++) {
			for (int i = 0; i < SCREEN_WIDTH / TEXTURE_WIDTH_BLOCK + 1; i++) {
				DrawUISprite(&g_UISprite[0], TEXTURE_WIDTH_BLOCK * i, TEXTURE_HEIGHT_BLOCK*j + re_blockMoveTimer / 2, TEXTURE_WIDTH_BLOCK, TEXTURE_HEIGHT_BLOCK);
			}
		}
	}

	// クリアタイムを描画
	{
		DrawUISprite(&g_UISprite[1]);
	}
	// クリアタイム数字を描画
	if (confirmStep > 0)
	{
		// 秒処理する
		g_Pos.x = SCREEN_WIDTH / 2 + 200;
		g_Pos.y = 50;
		g_w = TEXTURE_WIDTH_TEXT;

		// 桁数分処理する
		number = GetGameTimeSec();

		if (number > NUN_MAX) number = NUN_MAX;

		for (int i = 0; i < NUN_DIGIT; i++)
		{
			// 今回表示する桁の数字
			float x = float(number % 10);

			// 次の桁へ
			number /= 10;

			DrawUISprite(&g_UISprite[9], g_Pos.x - g_w * i, g_Pos.y, x);

		}

		// 分処理する
		g_Pos.x = SCREEN_WIDTH / 2 + 100;
		g_Pos.y = 50;
		g_w = TEXTURE_WIDTH_TEXT;

		// 桁数分処理する
		number = GetGameTimeMin();

		if (number > NUN_MAX) number = NUN_MAX;

		for (int i = 0; i < NUN_DIGIT; i++)
		{
			// 今回表示する桁の数字
			float x = float(number % 10);

			// 次の桁へ
			number /= 10;

			DrawUISprite(&g_UISprite[9], g_Pos.x - g_w * i, g_Pos.y, x);

		}

		// コロン(:)を描画 
		{
			DrawUISprite(&g_UISprite[11], SCREEN_WIDTH / 2 + 140, 40,0);
			DrawUISprite(&g_UISprite[11], SCREEN_WIDTH / 2 + 140, 60,0);
		}

	}


	// デス数を描画
	{
		DrawUISprite(&g_UISprite[2]);
	}

	// デス数数字を描画
	if (confirmStep > 1)
	{
		g_Pos.x = SCREEN_WIDTH / 2 + 150;
		g_Pos.y = 150;
		g_w = TEXTURE_WIDTH_TEXT;

		// 桁数分処理する

		number = GetMiss();

		for (int i = 0; i < NUN_DIGIT; i++)
		{
			// 今回表示する桁の数字
			float x = float(number % 10);

			// 次の桁へ
			number /= 10;

			DrawUISprite(&g_UISprite[9], g_Pos.x - g_w * i, g_Pos.y, x);
		}

	}

	// スターを描画
	{
		DrawUISprite(&g_UISprite[12],450.0f, 250.0f, 400.0f, 100.0f);
	}
	// スター数字を描画
	if (confirmStep > 2)
	{
		// 持っているスター数字
		g_Pos.x = SCREEN_WIDTH / 2 + 100;
		g_Pos.y = 250;
		g_w = TEXTURE_WIDTH_TEXT;

		// 桁数分処理する

		number = GetScore();

		for (int i = 0; i < NUN_DIGIT; i++)
		{
			// 今回表示する桁の数字
			float x = float(number % 10);

			// 次の桁へ
			number /= 10;

			DrawUISprite(&g_UISprite[9], g_Pos.x - g_w * i, g_Pos.y, x);
		}

		// すべてスター数字を描画
		g_Pos.x = SCREEN_WIDTH / 2 + 200;
		g_Pos.y = 250;
		g_w = TEXTURE_WIDTH_TEXT;

		// 桁数分処理する

		number = GetAllScore();

		for (int i = 0; i < NUN_DIGIT; i++)
		{
			// 今回表示する桁の数字
			float x = float(number % 10);

			// 次の桁へ
			number /= 10;

			DrawUISprite(&g_UISprite[9], g_Pos.x - g_w * i, g_Pos.y, x);
		}

		// コロン(:)を描画 
		{
			g_UISprite[11].Rot = D3DX_PI * 0.2f;
			DrawUISprite(&g_UISprite[11], SCREEN_WIDTH / 2 + 140, 250, 12, 120);
			g_UISprite[11].Rot = 0.0f;
		}
	}

	// コインを描画
	{
		DrawUISprite(&g_UISprite[13], 350.0f, 350.0f, 64.0f, 64.0f);
	}
	// コイン数字を描画
	if (confirmStep > 3)
	{
		// 持っているスター数字
		g_Pos.x = SCREEN_WIDTH / 2 + 100;
		g_Pos.y = 350;
		g_w = TEXTURE_WIDTH_TEXT;

		// 桁数分処理する

		number = GetCoin();

		for (int i = 0; i < NUN_DIGIT; i++)
		{
			// 今回表示する桁の数字
			float x = float(number % 10);

			// 次の桁へ
			number /= 10;

			DrawUISprite(&g_UISprite[9], g_Pos.x - g_w * i, g_Pos.y, x);
		}

		// すべてスター数字を描画
		g_Pos.x = SCREEN_WIDTH / 2 + 200;
		g_Pos.y = 350;
		g_w = TEXTURE_WIDTH_TEXT;

		// 桁数分処理する

		number = GetAllCoin();

		for (int i = 0; i < NUN_DIGIT; i++)
		{
			// 今回表示する桁の数字
			float x = float(number % 10);

			// 次の桁へ
			number /= 10;

			DrawUISprite(&g_UISprite[9], g_Pos.x - g_w * i, g_Pos.y, x);
		}

		// コロン(:)を描画 
		{
			g_UISprite[11].Rot = D3DX_PI * 0.2f;
			DrawUISprite(&g_UISprite[11], SCREEN_WIDTH / 2 + 140, 350, 12, 120);
			g_UISprite[11].Rot = 0.0f;
		}
	}

	// ランクマークを描画
	if (confirmStep > 4)
	{
		int time = GetGameTimeMin() * 60 + GetGameTimeSec();
		int deaths = GetMiss();
		int star = GetScore();
		int coin = GetCoin();

		int rank = 0;
		if      (time <= 300 && deaths < 4  && star >= 7 )  DrawUISprite(&g_UISprite[4]); //S
		else if (time <= 400 && deaths < 7  && star >= 6 )  DrawUISprite(&g_UISprite[5]); //A
		else if (time <= 500 && deaths < 10 && star >= 5 )  DrawUISprite(&g_UISprite[6]); //B
		else if (time <= 600 && deaths < 15 && star >= 4 )  DrawUISprite(&g_UISprite[7]); //C
		else if (time > 600  || deaths > 15 || star <  4 )  DrawUISprite(&g_UISprite[8]); //D

	}

	// エンターキー/スタートボタンを描画
	if (confirmStep > 5)
	{
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		if (re_blockMoveTimer <= BLOCK_MOVE_TIME / 2) {
			float a = (float)re_blockMoveTimer / (float)(BLOCK_MOVE_TIME / 2.0f);
			g_UISprite[10].Material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, a);
		}
		else {
			float a = (float)re_blockMoveTimer / (float)(BLOCK_MOVE_TIME / 2.0f) - 1.0f;
			g_UISprite[10].Material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1-a);
		}

		DrawUISprite(&g_UISprite[10]);
	}

	// TFP
	DrawUISprite(&g_UISprite[14]);

	SetLightEnable(true);
	SetDepthEnable(true);
}
