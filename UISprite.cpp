//=============================================================================
//
// スプライト設定処理 [UISprite.cpp]
// Author : GP11B132 14 ゴショウケン
//
// DX11_UISPRITE INSTANCE[num] => LOAD(overload) => DrawUISprite(overload)  [don't forget UnLoad] 
//
//=============================================================================
#include "UISprite.h"

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void SetUISprite(DX11_UISPRITE *sprite, float X, float Y, float Width, float Height,
	float U, float V, float UW, float VH,D3DXCOLOR Color, float Rot);
//= ============================================================================
// INIT処理
//=============================================================================
void LoadUISprite(char *TextureName, DX11_UISPRITE *sprite, float width, float height, float posX, float posY, int t_dx, int t_dy, float rot, D3DXCOLOR Color)
{
	//データ設定
	sprite->Texture_W        = width;
	sprite->Texture_H        = height;
	sprite->PosX             = posX;
	sprite->PosY             = posY;
	sprite->Texture_Divide_X = t_dx;
	sprite->Texture_Divide_Y = t_dy;
	sprite->Rot              = rot;

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

	float fhWidth  = width / 2.0f;
	float fhHeight = height / 2.0f;

	// 頂点座標の設定
	vertex[0].Position = D3DXVECTOR3(sprite->PosX - fhWidth, sprite->PosY - fhHeight, 0.0f);
	vertex[1].Position = D3DXVECTOR3(sprite->PosX + fhWidth, sprite->PosY - fhHeight, 0.0f);
	vertex[2].Position = D3DXVECTOR3(sprite->PosX - fhWidth, sprite->PosY + fhHeight, 0.0f);
	vertex[3].Position = D3DXVECTOR3(sprite->PosX + fhWidth, sprite->PosY + fhHeight, 0.0f);

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
	sprite->Material.Diffuse = Color;

	//テクスチャ
	D3DX11CreateShaderResourceViewFromFile(GetDevice(),
		TextureName,
		NULL,
		NULL,
		&sprite->Texture,
		NULL);

	sprite->Uianimation.hasanimation = false;
}

//=============================================================================
// 終了処理
//=============================================================================
void UnloadUISprite(DX11_UISPRITE *sprite)
{
	if (sprite->Texture) { sprite->Texture->Release();  sprite->Texture = NULL; }
	if (sprite->VertexBuffer) { sprite->VertexBuffer->Release();  sprite->VertexBuffer = NULL; }
}


//=============================================================================
// 描画処理
//=============================================================================
void DrawUISprite(DX11_UISPRITE *sprite, int num, bool anti)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &sprite->VertexBuffer, &stride, &offset);

	// マトリクス設定
	SetWorldViewProjection2D();

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	SetMaterial(sprite->Material);

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &sprite->Texture);

	float tw = 1.0f / (float)sprite->Texture_Divide_X;		// テクスチャの幅
	float th = 1.0f / (float)sprite->Texture_Divide_Y;		// テクスチャの高さ
	float tx = num  % sprite->Texture_Divide_X * tw;	    // テクスチャの左上X座標
	float ty = num  / sprite->Texture_Divide_X * th;		// テクスチャの左上Y座標

	if (anti) SetUISprite(sprite, sprite->PosX, sprite->PosY, sprite->Texture_W, sprite->Texture_H, tx + tw, ty, -tw, th, sprite->Material.Diffuse, sprite->Rot);
	else      SetUISprite(sprite, sprite->PosX, sprite->PosY, sprite->Texture_W, sprite->Texture_H, tx, ty, tw, th, sprite->Material.Diffuse, sprite->Rot);

	// ポリゴンの描画
	GetDeviceContext()->Draw(4, 0);

}

void DrawUISprite(DX11_UISPRITE *sprite, float posX, float posY, int num)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &sprite->VertexBuffer, &stride, &offset);

	// マトリクス設定
	SetWorldViewProjection2D();

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	SetMaterial(sprite->Material);

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &sprite->Texture);

	float tw = 1.0f / (float)sprite->Texture_Divide_X;		// テクスチャの幅
	float th = 1.0f / (float)sprite->Texture_Divide_Y;		// テクスチャの高さ
	float tx = num % sprite->Texture_Divide_X * tw;	    // テクスチャの左上X座標
	float ty = num / sprite->Texture_Divide_X * th;		// テクスチャの左上Y座標

	SetUISprite(sprite, posX, posY, sprite->Texture_W, sprite->Texture_H, tx, ty, tw, th, sprite->Material.Diffuse, sprite->Rot);

	// ポリゴンの描画
	GetDeviceContext()->Draw(4, 0);

}
void DrawUISprite(DX11_UISPRITE * sprite, float posX, float posY, float Width, float Height, int num)
{
	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &sprite->VertexBuffer, &stride, &offset);

	// マトリクス設定
	SetWorldViewProjection2D();

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	// マテリアル設定
	SetMaterial(sprite->Material);

	// テクスチャ設定
	GetDeviceContext()->PSSetShaderResources(0, 1, &sprite->Texture);

	float tw = 1.0f / (float)sprite->Texture_Divide_X;		// テクスチャの幅
	float th = 1.0f / (float)sprite->Texture_Divide_Y;		// テクスチャの高さ
	float tx = num % sprite->Texture_Divide_X * tw;	    // テクスチャの左上X座標
	float ty = num / sprite->Texture_Divide_X * th;		// テクスチャの左上Y座標

	SetUISprite(sprite, posX, posY, Width, Height, tx, ty, tw, th, sprite->Material.Diffuse, sprite->Rot);

	// ポリゴンの描画
	GetDeviceContext()->Draw(4, 0);
}
//=============================================================================
// SetSprites
//=============================================================================
void SetUISprite(DX11_UISPRITE *sprite, float X, float Y, float Width, float Height,
	float U, float V, float UW, float VH,
	D3DXCOLOR Color, float Rot)
{
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(sprite->VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D *vertex = (VERTEX_3D*)msr.pData;

	float hw, hh;
	hw = Width  * 0.5f;		// コンピューターは割り算が苦手
	hh = Height * 0.5f;		// 掛け算の方が処理が速い

	// 指定された座標を中心に回転するプログラム
	float BaseAngle = atan2f(hh, hw);			// 中心点から頂点に対する角度
	D3DXVECTOR2 temp = D3DXVECTOR2(hw, hh);
	float Radius = D3DXVec2Length(&temp);	// 中心点から頂点に対する距離

	float uaniblendX = X;
	float uaniblendY = Y;
	if (sprite->Uianimation.amt == UIAnimationMoveType::Curve)
	{
		// エネミー０番だけテーブルに従って座標移動（線形補間）
		int nowNo = (int)sprite->Uianimation.worktime;		// 整数分であるテーブル番号を取り出している
		int maxNo = ANIMATION_CURVE_POINT_COUNT + 1;			// 登録テーブル数を数えている
		int nextNo = (nowNo + 1) % maxNo;			// 移動先テーブルの番号を求めている
		D3DXVECTOR3	pos = sprite->Uianimation.curve[nextNo] - sprite->Uianimation.curve[nowNo];// XYZ移動量を計算している
		float nowTime = sprite->Uianimation.worktime - nowNo;	// 時間部分である少数を取り出している
		pos *= nowTime;								// 現在の移動量を計算している
		sprite->Uianimation.worktime += sprite->Uianimation.timestep;	// 時間を進めている
		// 計算して求めた移動量を現在の移動テーブルXYZに足している＝表示座標を求めている
		D3DXVECTOR3 temp = sprite->Uianimation.curve[nextNo] + pos;

		if ((int)sprite->Uianimation.worktime >= maxNo - 1)		// 登録テーブル最後まで移動したか？
		{
			if (sprite->Uianimation.loop == true)
			{
				sprite->Uianimation.worktime = 0.0f;
			}
		}
		uaniblendX = temp.x;
		uaniblendY = temp.y;
	}
	// ここでアフィン変換（sincosのやつ）を使って4頂点を回転させる
	float x = uaniblendX - cosf(BaseAngle + Rot) * Radius;
	float y = uaniblendY - sinf(BaseAngle + Rot) * Radius;
	vertex[0].Position = D3DXVECTOR3(x, y, 0.0f);

	x = uaniblendX + cosf(BaseAngle - Rot) * Radius;
	y = uaniblendY - sinf(BaseAngle - Rot) * Radius;
	vertex[1].Position = D3DXVECTOR3(x, y, 0.0f);

	x = uaniblendX - cosf(BaseAngle - Rot) * Radius;
	y = uaniblendY + sinf(BaseAngle - Rot) * Radius;
	vertex[2].Position = D3DXVECTOR3(x, y, 0.0f);

	x = uaniblendX + cosf(BaseAngle + Rot) * Radius;
	y = uaniblendX + sinf(BaseAngle + Rot) * Radius;
	vertex[3].Position = D3DXVECTOR3(x, y, 0.0f);

	vertex[0].Diffuse = Color;
	vertex[1].Diffuse = Color;
	vertex[2].Diffuse = Color;
	vertex[3].Diffuse = Color;

	vertex[0].TexCoord = D3DXVECTOR2(U, V);
	vertex[1].TexCoord = D3DXVECTOR2(U + UW, V);
	vertex[2].TexCoord = D3DXVECTOR2(U, V + VH);
	vertex[3].TexCoord = D3DXVECTOR2(U + UW, V + VH);

	GetDeviceContext()->Unmap(sprite->VertexBuffer, 0);

}

void SetUIAnimation(DX11_UISPRITE *UIsprite, UIAnimation uani) 
{
	UIsprite->Uianimation = uani;
	if (uani.at == UIAnimationMoveType::Curve)
	{
		auto curve = GetThreePowerBeizerList(uani.stpos, uani.edpos, uani.cp1pos, uani.cp2pos,
			ANIMATION_CURVE_POINT_COUNT);
		for (unsigned int i = 0; i < ANIMATION_CURVE_POINT_COUNT; i++)
		{
			UIsprite->Uianimation.curve[i] = curve[i];
		}
		delete[]curve;
	}
	
}