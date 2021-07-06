//=============================================================================
//
// モデルの処理 [model.h]
// Author : GP11B132 14 ゴショウケン
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

//*********************************************************
// 構造体
//*********************************************************

// マテリアル構造体
struct DX11_MODEL_MATERIAL
{
	MATERIAL					Material;
	ID3D11ShaderResourceView	*Texture;
};

// 描画サブセット構造体
struct DX11_SUBSET
{
	unsigned short	StartIndex;
	unsigned short	IndexNum;
	DX11_MODEL_MATERIAL	Material;
};

struct DX11_MODEL
{
	ID3D11Buffer*	VertexBuffer;
	ID3D11Buffer*	IndexBuffer;

	DX11_SUBSET		*SubsetArray;
	unsigned short	SubsetNum;

	// Mesh collider
	D3DXVECTOR3     *Vertexlist; // store vertex position
	unsigned short  *Indexlist;  // store index data
	unsigned short	VertexNum;
	unsigned short	IndexNum;
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void LoadModel(char *FileName, DX11_MODEL *Model, bool oriClr = false);
void UnloadModel(DX11_MODEL *Model);
void DrawModel(DX11_MODEL *Model);
