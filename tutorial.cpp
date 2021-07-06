//=============================================================================
//
// チュトリアル画面処理 [tutorial.cpp]
// Author : GP11B132 14 ゴショウケン
//
//=============================================================================
#include "tutorial.h"
#include "input.h"
#include "fade.h"
#include "UISprite.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_UISPRITE                (3)

#define INTRO_TEXTURE_WIDTH         (960.0f)
#define INTRO_TEXTURE_HEIGHT        (546.0f)
#define UISPRITE_INTROA             "data/TEXTURE/result/goal.png"
#define UISPRITE_INTROB             "data/TEXTURE/result/goal.png"
#define UISPRITE_INTROC             "data/TEXTURE/result/goal.png"
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static DX11_UISPRITE		        g_UISprite[MAX_UISPRITE]; // UIスプライト情報

int lv = 0;
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTutorial(void)
{
	LoadUISprite(UISPRITE_INTROA, &g_UISprite[0], INTRO_TEXTURE_WIDTH, INTRO_TEXTURE_HEIGHT, SCREEN_CENTER_X, SCREEN_CENTER_Y);
	LoadUISprite(UISPRITE_INTROB, &g_UISprite[1], INTRO_TEXTURE_WIDTH, INTRO_TEXTURE_HEIGHT, SCREEN_CENTER_X, SCREEN_CENTER_Y);
	LoadUISprite(UISPRITE_INTROC, &g_UISprite[2], INTRO_TEXTURE_WIDTH, INTRO_TEXTURE_HEIGHT, SCREEN_CENTER_X, SCREEN_CENTER_Y);

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTutorial(void)
{
	// スプライトの解放処理
	for (int i = 0; i < MAX_UISPRITE; i++) {
		UnloadUISprite(&g_UISprite[i]);
	}

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateTutorial(void)
{
	// ボタン押したら、ステージを切り替える
	if (lv >= 0) {
		if (GetKeyboardTrigger(DIK_RETURN) || IsMouseLeftTriggered()) SetFade(FADE_OUT, MODE_GAME);
	}
	else {
		if (GetKeyboardTrigger(DIK_RETURN) || IsMouseLeftTriggered()) lv++;
	}
		
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTutorial(void)
{
	// 2D描画なので深度無効
	SetDepthEnable(false);
	SetLightEnable(false);

	if(lv == 0)DrawUISprite(&g_UISprite[0]);
	if (lv == 1)DrawUISprite(&g_UISprite[0]);
	if (lv >= 2)DrawUISprite(&g_UISprite[0]);

	SetDepthEnable(true);
	SetLightEnable(true);

}

