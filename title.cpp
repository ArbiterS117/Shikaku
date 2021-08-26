//=============================================================================
//
// タイトル画面処理 [title.cpp]
// Author : GP11B132 14 ゴショウケン
//
//=============================================================================
#include "title.h"
#include "input.h"
#include "fade.h"
#include "sound.h"
#include "UISprite.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX					(9)				// テクスチャの数
#define TEXTURE_WIDTH				(SCREEN_WIDTH)	// 背景サイズ
#define TEXTURE_HEIGHT				(SCREEN_HEIGHT)	// 

#define TEXTURE_WIDTH_BLOCK			(64)	        // ブロック背景サイズ
#define TEXTURE_HEIGHT_BLOCK		(64)	        // 
#define TEXTURE_BLOCK_SCALE		    (96)	        // 

#define TEXTURE_WIDTH_LOGO			(400)			// ロゴサイズ
#define TEXTURE_HEIGHT_LOGO			(154)			// 

#define TEXTURE_WIDTH_BTN			(400)			// ボタンサイズ
#define TEXTURE_HEIGHT_BTN			(90)			// 

#define BLOCK_MOVE_TIME             (TEXTURE_HEIGHT_BLOCK * 2)

#define MAX_UISPRITE                (9)

#define UISPRITE_TITLE             "data/TEXTURE/LOGO1.png"
#define UISPRITE_B1                "data/TEXTURE/Blue.png"  
#define UISPRITE_B2                "data/TEXTURE/Gray.png"  
#define UISPRITE_B3                "data/TEXTURE/Green.png" 
#define UISPRITE_B4                "data/TEXTURE/Brown.png" 
#define UISPRITE_B5                "data/TEXTURE/Pink.png"  
#define UISPRITE_B6                "data/TEXTURE/Purple.png"
#define UISPRITE_B7                "data/TEXTURE/Yellow.png"
#define UISPRITE_PRESS_BTN         "data/TEXTURE/pressbutton.png"


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************


//*****************************************************************************
// グローバル変数
//*****************************************************************************
static DX11_UISPRITE  g_UISprite[MAX_UISPRITE]; // UIスプライト情報

// other
int blockMoveTimer = 0;
int blocktype = 4;  // 1~7 map to g_UISprite[1]~[7]
int scalesize = TEXTURE_HEIGHT_BLOCK;

bool ti_canInput = true;
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTitle(void)
{
	LoadUISprite(UISPRITE_TITLE, &g_UISprite[0], TEXTURE_WIDTH_LOGO, TEXTURE_HEIGHT_LOGO, SCREEN_CENTER_X, SCREEN_CENTER_Y);
	LoadUISprite(UISPRITE_B1, &g_UISprite[1], TEXTURE_WIDTH_BLOCK, TEXTURE_HEIGHT_BLOCK, 0.0f, 0.0f);
	LoadUISprite(UISPRITE_B2, &g_UISprite[2], TEXTURE_WIDTH_BLOCK, TEXTURE_HEIGHT_BLOCK, 0.0f, 0.0f);
	LoadUISprite(UISPRITE_B3, &g_UISprite[3], TEXTURE_WIDTH_BLOCK, TEXTURE_HEIGHT_BLOCK, 0.0f, 0.0f);
	LoadUISprite(UISPRITE_B4, &g_UISprite[4], TEXTURE_WIDTH_BLOCK, TEXTURE_HEIGHT_BLOCK, 0.0f, 0.0f);
	LoadUISprite(UISPRITE_B5, &g_UISprite[5], TEXTURE_WIDTH_BLOCK, TEXTURE_HEIGHT_BLOCK, 0.0f, 0.0f);
	LoadUISprite(UISPRITE_B6, &g_UISprite[6], TEXTURE_WIDTH_BLOCK, TEXTURE_HEIGHT_BLOCK, 0.0f, 0.0f);
	LoadUISprite(UISPRITE_B7, &g_UISprite[7], TEXTURE_WIDTH_BLOCK, TEXTURE_HEIGHT_BLOCK, 0.0f, 0.0f);
	LoadUISprite(UISPRITE_PRESS_BTN, &g_UISprite[8], TEXTURE_WIDTH_BTN, TEXTURE_HEIGHT_BTN, SCREEN_WIDTH / 2, 380.0f);

	// ゲーム音楽の再生
	PlaySound(SOUND_LABEL_BGM_5);

	///////////////////////////////////////////////////////////
	ID3D11Device *pDevice = GetDevice();

	ti_canInput = true;

	// BGM再生
	//PlaySound(SOUND_LABEL_BGM_sample000);

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTitle(void)
{
	// スプライトの解放処理
	for (int i = 0; i < MAX_UISPRITE; i++) {
		UnloadUISprite(&g_UISprite[i]);
	}
	
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateTitle(void)
{
	blockMoveTimer += 1;
	if (blockMoveTimer >= BLOCK_MOVE_TIME) {
		blockMoveTimer = 0;
		blocktype += 1;
		if (blocktype > 7)blocktype = 1;
		// scale
		scalesize = TEXTURE_BLOCK_SCALE;
	}
	if (scalesize > TEXTURE_HEIGHT_BLOCK) {
		scalesize -= 4;
	}


	if (GetKeyboardTrigger(DIK_RETURN) || IsButtonTriggered(0, BUTTON_START))
	{// Enter押したら、ステージを切り替える
		if (ti_canInput == true) {
			ti_canInput = false;
			SetFade(FADE_OUT, MODE_TUTORIAL);
			PlaySound(SOUND_LABEL_SE_start);
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
void DrawTitle(void)
{
	// 2D描画なので深度無効
	SetDepthEnable(false);
	SetLightEnable(false);

	// ブロックの背景を描画
	{
		for (int j = -1; j < SCREEN_HEIGHT / TEXTURE_HEIGHT_BLOCK + 2; j++) {
			for (int i = 0; i < SCREEN_WIDTH / TEXTURE_WIDTH_BLOCK + 1; i++) {
				DrawUISprite(&g_UISprite[blocktype], TEXTURE_WIDTH_BLOCK * i, TEXTURE_HEIGHT_BLOCK*j + blockMoveTimer / 2, scalesize, scalesize);
			}
		}
	}

	// タイトルの背景を描画
	DrawUISprite(&g_UISprite[0]);
	

	// エンターキー/スタートボタンを描画
	{
		// １枚のポリゴンの頂点とテクスチャ座標を設定
		if (blockMoveTimer <= BLOCK_MOVE_TIME / 2) {
			float a = (float)blockMoveTimer / (float)(BLOCK_MOVE_TIME / 2.0f);
			g_UISprite[8].Material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, a);
		}
		else {
			float a = (float)blockMoveTimer / (float)(BLOCK_MOVE_TIME / 2.0f) - 1.0f;
			g_UISprite[8].Material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1-a);
		}
		DrawUISprite(&g_UISprite[8]);
	}

	SetLightEnable(true);
	SetDepthEnable(true);
}

