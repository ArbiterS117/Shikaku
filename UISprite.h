//=============================================================================
//
// スプライト設定処理 [UISprite.h]
// Author : GP11B132 14 ゴショウケン
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*********************************************************
// 構造体
//*********************************************************

struct DX11_UISPRITE
{
	ID3D11Buffer*	VertexBuffer;

	MATERIAL					Material;
	ID3D11ShaderResourceView	*Texture;

	//===================テクスチャデータ==============
	float PosX = 0.0f;
	float PosY = 0.0f;
	float Rot  = 0.0f;
	float Texture_W = 100.0f;
	float Texture_H = 100.0f;
	
	int   Texture_Divide_X = 1; // 横
	int   Texture_Divide_Y = 1; // 縦
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void LoadUISprite(char *TextureName, DX11_UISPRITE *UIsprite, float width = 50.0f, float height = 50.0f, 
				  float posX = SCREEN_CENTER_X, float posY = SCREEN_CENTER_Y, int t_dx = 1, int t_dy = 1, float rot = 0.0f,
				  D3DXCOLOR Color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
void UnloadUISprite(DX11_UISPRITE *UIsprite);
void DrawUISprite(DX11_UISPRITE *UIsprite, int num = 0, bool anti = false);
void DrawUISprite(DX11_UISPRITE *UIsprite, float posX, float posY, int num = 0);
void DrawUISprite(DX11_UISPRITE *UIsprite, float posX, float posY, float Width, float Height,int num = 0); // Texture_W,Texture_H を無視