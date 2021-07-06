//=============================================================================
//
// スプライトの処理 [sprite.cpp]
// Author : GP11B132 14 ゴショウケン
//
// DX11_SPRITE INSTANCE[num] => LOAD() => DrawSprite() : Use Matrix to move,scale,rotate,billboard  [don't forget UnLoad] 
//
//=============================================================================
#include "sprite.h"

void LoadSprite(char *TextureName, DX11_SPRITE *sprite, float width, float height)
{
	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &sprite->VertexBuffer);

	// 頂点バッファに値をセットする
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(sprite->VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float fWidth = width;
	float fHeight = height;

	// 頂点座標の設定
	vertex[0].Position = D3DXVECTOR3(-fWidth / 2.0f, fHeight, 0.0f);
	vertex[1].Position = D3DXVECTOR3(fWidth / 2.0f, fHeight, 0.0f);
	vertex[2].Position = D3DXVECTOR3(-fWidth / 2.0f, 0.0f, 0.0f);
	vertex[3].Position = D3DXVECTOR3(fWidth / 2.0f, 0.0f, 0.0f);

	// 拡散光の設定
	vertex[0].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[1].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[2].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	vertex[3].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// テクスチャ座標の設定
	vertex[0].TexCoord = D3DXVECTOR2(0.0f, 0.0f);
	vertex[1].TexCoord = D3DXVECTOR2(1.0f, 0.0f);
	vertex[2].TexCoord = D3DXVECTOR2(0.0f, 1.0f);
	vertex[3].TexCoord = D3DXVECTOR2(1.0f, 1.0f);

	GetDeviceContext()->Unmap(sprite->VertexBuffer, 0);

	ZeroMemory(&sprite->Material, sizeof(sprite->Material));
	sprite->Material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	//テクスチャ
	D3DX11CreateShaderResourceViewFromFile(GetDevice(),
		TextureName,
		NULL,
		NULL,
		&sprite->Texture,
		NULL);
}

//=============================================================================
// 終了処理
//=============================================================================
void UnloadSprite(DX11_SPRITE *sprite)
{
	if (sprite->Texture) { sprite->Texture->Release();  sprite->Texture = NULL; }
	if (sprite->VertexBuffer) { sprite->VertexBuffer->Release();  sprite->VertexBuffer = NULL; }
}


//=============================================================================
// 描画処理
//=============================================================================
void DrawSprite(DX11_SPRITE *sprite)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &sprite->VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	SetMaterial(sprite->Material);

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &sprite->Texture);

	// ポリゴンの描画
	GetDeviceContext()->Draw(4, 0);

}