//=============================================================================
//
// ゲームシーンUI処理 [score.cpp]
// Author : GP11B132 14 ゴショウケン
//
//=============================================================================
#include "gameScene.h"
#include "score.h"
#include "setSprite.h"
#include "UISprite.h"
#include "UIAnimation.h"

#include "platform.h"
#include "EditTool.h"
#include "input.h"
#include "enemy.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_UISPRITE                (13)
#define SCORE_DIGITAL_POS_X         (40.0f)
#define SCORE_DIGITAL_POS_Y         (32.0f)
#define MISS_DIGITAL_POS_X          (900.0f)
#define MISS_DIGITAL_POS_Y          (32.0f)
#define TEXTURE_WIDTH				(24)	// キャラサイズ
#define TEXTURE_HEIGHT				(48)	// 
#define TEXTURE_MAX					(1)		// テクスチャの数
//*****************************************************************************
// グローバル変数
//*****************************************************************************
static DX11_UISPRITE		        g_UISprite[MAX_UISPRITE];				// モデル情報
#define UISPRITE_TITLE             "data/TEXTURE/Title.png"
#define UISPRITE_NUM               "data/TEXTURE/number16x32.png"
#define UISPRITE_SIGHTPOINT        "data/TEXTURE/sightpoint.png"
#define UISPRITE_DEATH             "data/TEXTURE/result/deathCount.png"
#define UISPRITE_RANK              "data/TEXTURE/result/StarNum.png"
#define UISPRITE_COIN              "data/TEXTURE/result/coin.png"

#define UISPRITE_MOVE              "data/TEXTURE/result/move.png"
#define UISPRITE_JUMP              "data/TEXTURE/result/jump.png"
#define UISPRITE_DASH              "data/TEXTURE/result/dash.png"
#define UISPRITE_THROW             "data/TEXTURE/result/throw.png"
#define UISPRITE_USE               "data/TEXTURE/result/use.png"
#define UISPRITE_THROWUSE          "data/TEXTURE/result/throwUse.png"
#define UISPRITE_SP                "data/TEXTURE/result/SP.png"

int		g_GameTimeSec;			    
int		g_GameTimeMin;			    

int		g_Score;					// Star
int		g_Coin;					   
int		g_AllScore;
int		g_AllCoin;					   
int		g_Miss;					    
bool	g_DrawSightPoint = false;
bool	g_DrawMove     = false;
bool	g_DrawJump     = false;
bool	g_DrawDash     = false;
bool	g_DrawThrow    = false;
bool	g_DrawUse      = false;
bool	g_DrawThrowUse = false;
bool	g_DrawSP       = false;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitScore(void)
{
	LoadUISprite(UISPRITE_NUM, &g_UISprite[1], TEXTURE_WIDTH, TEXTURE_HEIGHT, SCREEN_CENTER_X, 100.0f, 10, 1,0.0f,D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	LoadUISprite(UISPRITE_TITLE, &g_UISprite[0], 500, 100, 200.0f, 200.0f, 1, 1,0.0f,D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	LoadUISprite(UISPRITE_SIGHTPOINT, &g_UISprite[2], 64, 64, SCREEN_CENTER_X, SCREEN_CENTER_Y, 1, 1, 0.0f,D3DXCOLOR(0.7f, 1.5f, 0.7f, 1.0f));
	LoadUISprite(UISPRITE_DEATH, &g_UISprite[3], 400, 80, SCREEN_WIDTH + 120.0f, 30.0f, 1, 1, 0.0f,D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	LoadUISprite(UISPRITE_RANK, &g_UISprite[4], 400, 80, 170.0f, 40.00, 1, 1, 0.0f,D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	LoadUISprite(UISPRITE_COIN, &g_UISprite[5], 400, 80, 170.0f, 40.00, 1, 1, 0.0f,D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

	LoadUISprite(UISPRITE_MOVE    , &g_UISprite[6],  250, 250, 150.0f, 170.00, 1, 1, 0.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	LoadUISprite(UISPRITE_JUMP    , &g_UISprite[7],  250, 250, 150.0f, 300.00, 1, 1, 0.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	LoadUISprite(UISPRITE_DASH    , &g_UISprite[8],  250, 250, 150.0f, 170.00, 1, 1, 0.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	LoadUISprite(UISPRITE_THROW   , &g_UISprite[9],  300, 300, 150.0f, 170.00, 1, 1, 0.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	LoadUISprite(UISPRITE_USE     , &g_UISprite[10], 300, 300, 150.0f, 170.00, 1, 1, 0.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	LoadUISprite(UISPRITE_THROWUSE, &g_UISprite[11], 300, 300, 150.0f, 170.00, 1, 1, 0.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	LoadUISprite(UISPRITE_SP      , &g_UISprite[12], 350, 350, 150.0f, 170.00, 1, 1, 0.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

	//test animation code
	//UIAnimation uani;
	auto sprite = &g_UISprite[6];
	D3DXVECTOR3 stpos = D3DXVECTOR3(sprite->PosX, sprite->PosY, 0.0f);
	D3DXVECTOR3 edpos = D3DXVECTOR3(sprite->PosX + 100.0f, sprite->PosY, 0.0f);
	D3DXVECTOR3 cp2pos = D3DXVECTOR3(sprite->PosX + 500.0f, sprite->PosY + 500.0f, 0.0f);
	D3DXVECTOR3 cp1pos = D3DXVECTOR3(sprite->PosX - 1250.0f, sprite->PosY + 500.0f, 0.0f);
	SetSpriteAnimation(sprite, UIAnimationStatusType::Default, UIAnimationMoveType::Curve, UIAnimationType::TransForm,
						true,2.0f,0.2f, stpos, edpos, cp1pos, cp2pos);

	//Tutorio

	g_Score = 0;	// スコアの初期化
	g_Coin  = 0;  	
	g_Miss  = 0;
	g_GameTimeSec = 0;
	g_GameTimeMin = 0;

	//count all coin and star
	ENEMY *g_Enemy = GetEnemy();
	g_AllScore = 0;
	g_AllCoin = 0;
	for (int i = 0; i < MAX_ENEMY; i++) {
		if (g_Enemy[i].enemyType == EnemyType_star) AddAllScore(1);
		if (g_Enemy[i].enemyType == EnemyType_coin) AddAllCoin(1);
	}

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitScore(void)
{
	// スプライトの解放処理
	for (int i = 0; i < MAX_UISPRITE; i++) {
		UnloadUISprite(&g_UISprite[i]);
	}

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateScore(void)
{

	if (GetKeyboardTrigger(DIK_NUMPAD0))g_DrawSightPoint = !g_DrawSightPoint;
	if (GetKeyboardTrigger(DIK_TAB))g_DrawSightPoint = !g_DrawSightPoint;

#ifdef _DEBUG	// デバッグ情報を表示する
	/*char *str = GetDebugStr();
	sprintf(&str[strlen(str)], " score: %d",g_Score);*/

#endif

}

//=============================================================================
// 描画処理
//=============================================================================
void DrawScore(void)
{
	SetLightEnable(false); 
	SetDepthEnable(false);  // faster

	//================Draw Title
	
	//DrawUISprite(&g_UISprite[0]); // Title
	if (GetGameSceneEditMode()) {
		if(g_DrawSightPoint)	DrawUISprite(&g_UISprite[2]); // SightPoint
	}
	else {
		//DrawUISprite(&g_UISprite[3]);
		DrawUISprite(&g_UISprite[4]);
		// sign
		if(g_DrawMove)DrawUISprite(&g_UISprite[6]);
		if(g_DrawJump)DrawUISprite(&g_UISprite[7]);
		if(g_DrawDash)DrawUISprite(&g_UISprite[8]);
		if(g_DrawThrow)DrawUISprite(&g_UISprite[9]);
		if(g_DrawUse)DrawUISprite(&g_UISprite[10]);
		if(g_DrawThrowUse)DrawUISprite(&g_UISprite[11]);
		if(g_DrawSP)DrawUISprite(&g_UISprite[12]);
	}									            

	if (!GetGameSceneEditMode()) {
		//================Draw Score
		// 桁数分処理する
		int number = g_Score;

		if (number > SCORE_MAX) number = SCORE_MAX;

		for (int i = 0; i < SCORE_DIGIT; i++)
		{
			// 今回表示する桁の数字
			float x = float(number % 10);

			// 次の桁へ
			number /= 10;

			// スコアの位置やテクスチャー座標を反映
			float px = SCORE_DIGITAL_POS_X - TEXTURE_WIDTH * i;	// 数字の表示位置X
			float py = SCORE_DIGITAL_POS_Y;			            // 数字の表示位置Y

			DrawUISprite(&g_UISprite[1], px, py, x);

		}

		//================Draw MISS
		// 桁数分処理する
		number = g_Miss;

		if (number > SCORE_MAX) number = SCORE_MAX;

		for (int i = 0; i < SCORE_DIGIT; i++)
		{
			// 今回表示する桁の数字
			float x = float(number % 10);

			// 次の桁へ
			number /= 10;

			// スコアの位置やテクスチャー座標を反映
			float px = MISS_DIGITAL_POS_X - TEXTURE_WIDTH * i;	// 数字の表示位置X
			float py = MISS_DIGITAL_POS_Y;			            // 数字の表示位置Y

			//DrawUISprite(&g_UISprite[1], px, py, x);

		}

	}
	

	SetLightEnable(true);
	SetDepthEnable(true);

}

//=============================================================================
// スコアを加算する
// 引数:add :追加する点数。マイナスも可能
//=============================================================================
void AddScore(int add)
{
	g_Score += add;
	if (g_Score > SCORE_MAX)
	{
		g_Score = SCORE_MAX;
	}

	if (g_Score < 0)g_Score = 0;

}

void AddCoin(int add)
{
	g_Coin += add;
	if (g_Coin > SCORE_MAX)
	{
		g_Coin = SCORE_MAX;
	}

	if (g_Coin < 0)g_Coin = 0;

}

void AddAllScore(int add)
{
	g_AllScore += add;
	if (g_AllScore > SCORE_MAX)
	{
		g_AllScore = SCORE_MAX;
	}

	if (g_AllScore < 0)g_AllScore = 0;
}

void AddAllCoin(int add)
{
	g_AllCoin += add;
	if (g_AllCoin > SCORE_MAX)
	{
		g_AllCoin = SCORE_MAX;
	}

	if (g_AllCoin < 0)g_AllCoin = 0;
}

void AddMiss(int add)
{
	g_Miss += add;
	if (g_Miss > SCORE_MAX)
	{
		g_Miss = SCORE_MAX;
	}

	if (g_Miss < 0)g_Miss = 0;

}

void AddScoreTime(int sec)
{
	g_GameTimeSec += sec;
	if (g_GameTimeSec >= 60) {
		g_GameTimeMin += 1;
		g_GameTimeSec -= 60;
	}
}

int GetScore()
{
	return g_Score;
}

int GetCoin()
{
	return g_Coin;
}

int GetAllScore()
{
	return g_AllScore;
}

int GetAllCoin()
{
	return g_AllCoin;
}

int GetMiss()
{
	return g_Miss;
}

int GetGameTimeSec()
{
	return g_GameTimeSec;
}

int GetGameTimeMin()
{
	return g_GameTimeMin;
}

void showSign(int i)
{
	switch (i)
	{
	case 0:
		g_DrawMove = true;
		break;
	case 1:
		g_DrawJump = true;
		break;
	case 2:
		g_DrawDash = true;
		break;
	case 3:
		g_DrawThrow = true;
		break;
	case 4:
		g_DrawUse = true;
		break;
	case 5:
		g_DrawThrowUse = true;
		break;
	case 6:
		g_DrawSP = true;
		break;

	case -1:
		g_DrawMove = false;
		g_DrawJump = false;
		g_DrawDash = false;
		g_DrawThrow = false;
		g_DrawUse = false;
		g_DrawThrowUse = false;
		g_DrawSP = false;
		break;
	default:
		break;
	}
}


