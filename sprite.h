//=============================================================================
//
// スプライトの処理 [sprite.h]
// Author : GP11B132 14 ゴショウケン
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

//*********************************************************
// 構造体
//*********************************************************

struct DX11_SPRITE
{
	ID3D11Buffer*	VertexBuffer;

	MATERIAL					Material;
	ID3D11ShaderResourceView	*Texture;
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void LoadSprite(char *FileName, DX11_SPRITE *sprite, float width, float height);
void UnloadSprite(DX11_SPRITE *sprite);
void DrawSprite(DX11_SPRITE *sprite);
