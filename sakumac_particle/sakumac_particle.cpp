//=============================================================================
//
// パーティクル処理 [particle.cpp]
// Author : GP11A132_16_佐久間和也
//
//=============================================================================
#include <time.h>

#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "model.h"
#include "shadow.h"
#include "particle.h"
#include "player.h"
#include "enemy.h"
#include "bezier.h"
#include "bullet.h"
#include "debugproc.h"
#include "bezier_point.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX			(4)			// テクスチャの数

#define	PARTICLE_SIZE_X		(20.0f)		// 頂点サイズ
#define	PARTICLE_SIZE_Y		(20.0f)		// 頂点サイズ
#define	VALUE_MOVE_PARTICLE	(50.0f)		// 移動速度


#define	DISP_SHADOW						// 影の表示
//#undef DISP_SHADOW

// パーティクルの色
#define COLOR_R				(1.0f)
#define COLOR_G				(1.0f)
#define COLOR_B				(0.0f)
#define COLOR_A				(0.5f)
#define DIFFUSE				(0.05f)

// パーティクルの広がり抑制
#define SUPPRESSION_X				(0.055f)
#define SUPPRESSION_Y				(0.25f)
#define SUPPRESSION_Z				(0.015f)


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexParticle(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// 頂点バッファ

static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報
static int							g_TexNo[EFFECT_MAX];					// テクスチャ番号

static D3DXMATRIX				g_mtxWorldParticle;				// ワールドマトリックス

static PARTICLE					g_aParticle[EFFECT_MAX][MAX_PARTICLE];		// パーティクルワーク
static BEZ						bez[MAX_BEZ];

static D3DXVECTOR3				g_posBase;						// ビルボード発生位置
static float					g_roty = 0.0f;					// 移動方向
static float					g_spd = 0.0f;					// 移動スピード
static float					count = 0.0f;					// カウント
static bool						bSwitch = false;				// パーティクルスイッチ


static int effectNo = EFFECT_DEFAULT;


static char *g_TextureName[] =
{
	"data/TEXTURE/hosi.png",
	"data/TEXTURE/1.png",
	"data/TEXTURE/effect000.jpg",
	"data/TEXTURE/fire.png",


};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitParticle(void)
{
	// 頂点情報の作成
	MakeVertexParticle();

	// テクスチャ生成
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// パーティクルワークの初期化
	for (int p = 0; p < EFFECT_MAX; p++)
	{
		g_TexNo[p] = 0;

		for (int i = 0; i < MAX_PARTICLE; i++)
		{
			g_aParticle[p][i].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			g_aParticle[p][i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			g_aParticle[p][i].scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
			g_aParticle[p][i].move = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

			ZeroMemory(&g_aParticle[p][i].material, sizeof(g_aParticle[p][i].material));
			g_aParticle[p][i].material.Diffuse = D3DXCOLOR(0.8f, 0.7f, 0.2f, 0.85f);

			g_aParticle[p][i].fSizeX = PARTICLE_SIZE_X;
			g_aParticle[p][i].fSizeY = PARTICLE_SIZE_Y;
			g_aParticle[p][i].nIdxShadow = -1;
			g_aParticle[p][i].nLife = 0;
			g_aParticle[p][i].bUse = false;
			g_aParticle[p][i].hitNo = 0;
		}
	}
	
	for (int b = 0; b < MAX_BEZ; b++)
	{
		bez[b].bez = 0;
		bez[b].finish = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		bez[b].start = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		bez[b].P1 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		bez[b].P2 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		bez[b].color = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);

		bez[b].x = 0.0f;
		bez[b].y = 0.0f;
		bez[b].z = 0.0f;
		bez[b].x2 = 0.0f;
		bez[b].y2 = 0.0f;
		bez[b].z2 = 0.0f;
		bez[b].x3 = 0.0f;
		bez[b].y3 = 0.0f;
		bez[b].z3 = 0.0f;

	}


	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitParticle(void)
{
	//テクスチャの解放
	for (int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if (g_Texture[nCntTex] != NULL)
		{
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	// 頂点バッファの解放
	if (g_VertexBuffer != NULL)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateParticle(void)
{
	PLAYER *player = GetPlayer();
	ENEMY *enemy = GetEnemy();


#ifdef _DEBUG
	if (GetKeyboardTrigger(DIK_E))
	{
		effectNo = (effectNo + 1) % EFFECT_MAX;
		InitParticle();
	}
#endif

	switch (effectNo)
	{
	case EFFECT_DEFAULT:
		// 初期状態
		break;

	case EFFECT_LISSAJOUS:
		// リサージュ
		effectLissajous(g_posBase,8, 9, 7, 50, 35, 50, 500);
		break;

	case EFFECT_SMOKE:
		// 煙
		effectSmoke(D3DXVECTOR3(player->pos), D3DXCOLOR(1.16f, 0.8f, 0.48f, 1.0f), 1.7f, 1.7f, 1);
		break;

	case EFFECT_FIRE:
		//炎
		effectFire(player->pos, D3DXCOLOR(0.7f, 0.2f, 0.0f, 1.0f), 1.7f, 1.7f, 1);
		break;

	case EFFECT_STAR:
		// キラキラ
		effectStar(g_posBase);
		break;

	case EFFECT_CIRCLE:
		// サークル
		effectCircle(g_posBase);
		break;

	case EFFECT_BEZIER:
		SetBezier(player->pos, D3DXVECTOR3(0.0f,0.0f,0.0f));
		break;

	case EFFECT_ALL:
		effectLissajous(g_posBase,8, 9, 7, 50, 35, 50, 500);
		effectStar(g_posBase);
		effectCircle(g_posBase);

		break;
	}


	//if (GetKeyboardPress(DIK_SPACE) && !GetKeyboardPress(DIK_LSHIFT))
	//{
	//	effectBezier(player->pos);
	//}

	for (int p = 0; p < EFFECT_MAX; p++)
	{
		for (int i = 0; i < MAX_PARTICLE; i++)
		{

			if (g_aParticle[p][i].bUse)
			{// 使用中



				g_aParticle[p][i].nLife--;
				if (g_aParticle[p][i].nLife <= 0)
				{
					g_aParticle[p][i].bUse = false;
					ReleaseShadow(g_aParticle[p][i].nIdxShadow);
					g_aParticle[p][i].nIdxShadow = -1;
				}
			}
		}
	}

#ifdef _DEBUG
	//PrintDebugProc("Bez:%d \n", bez[0].bez);
	//PrintDebugProc("Bezier1 X:%f Y:%f Z:%f \n", bez[0].x, bez[0].y, bez[0].z);
	//PrintDebugProc("Bezier2 X:%f Y:%f Z:%f \n", bez[0].x2, bez[0].y2, bez[0].z2);
	//PrintDebugProc("Bezier3 X:%f Y:%f Z:%f \n", bez[0].x3, bez[0].y3, bez[0].z3);
	PrintDebugProc("count:%f \n", count);
	PrintDebugProc("count:%f \n", g_aParticle[EFFECT_CIRCLE][1].count);
	PrintDebugProc("color.a:%f \n", g_aParticle[EFFECT_CIRCLE][1].material.Diffuse.a);

#endif
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawParticle(void)
{
	D3DXMATRIX mtxView,mtxScale,mtxTranslate;
	CAMERA *cam = GetCamera();

	// ライティングを無効に
	SetLightEnable(false);

	// 加算合成に設定
	SetBlendState(BLEND_MODE_ADD);

	// Z比較無し
	SetDepthEnable(false);

	// フォグ無効
	SetFogEnable(false);

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);



	for (int p = 0; p < EFFECT_MAX; p++)
	{
		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo[p]]);

		for (int i = 0; i < MAX_PARTICLE; i++)
		{
			if (g_aParticle[p][i].bUse)
			{
				// ワールドマトリックスの初期化
				D3DXMatrixIdentity(&g_mtxWorldParticle);

				// ビューマトリックスを取得
				mtxView = cam->mtxView;

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
				D3DXMatrixScaling(&mtxScale, g_aParticle[p][i].scale.x, g_aParticle[p][i].scale.y, g_aParticle[p][i].scale.z);
				D3DXMatrixMultiply(&g_mtxWorldParticle, &g_mtxWorldParticle, &mtxScale);

				// 移動を反映
				D3DXMatrixTranslation(&mtxTranslate, g_aParticle[p][i].pos.x, g_aParticle[p][i].pos.y, g_aParticle[p][i].pos.z);
				D3DXMatrixMultiply(&g_mtxWorldParticle, &g_mtxWorldParticle, &mtxTranslate);

				// ワールドマトリックスの設定
				SetWorldMatrix(&g_mtxWorldParticle);

				// マテリアル設定
				SetMaterial(g_aParticle[p][i].material);

				// ポリゴンの描画
				GetDeviceContext()->Draw(4, 0);
			}
		}
	}

	// ライティングを有効に
	SetLightEnable(true);

	// 通常ブレンドに戻す
	SetBlendState(BLEND_MODE_ALPHABLEND);

	// Z比較有効
	SetDepthEnable(true);

	// フォグ有効
	SetFogEnable(true);

}

//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexParticle(void)
{
	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	{//頂点バッファの中身を埋める
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		// 頂点座標の設定
		vertex[0].Position = D3DXVECTOR3(-PARTICLE_SIZE_X / 2, PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[1].Position = D3DXVECTOR3(PARTICLE_SIZE_X / 2, PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[2].Position = D3DXVECTOR3(-PARTICLE_SIZE_X / 2, -PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[3].Position = D3DXVECTOR3(PARTICLE_SIZE_X / 2, -PARTICLE_SIZE_Y / 2, 0.0f);

		// 法線の設定
		vertex[0].Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		vertex[1].Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		vertex[2].Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		vertex[3].Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

		// 反射光の設定
		vertex[0].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[1].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[2].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[3].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		// テクスチャ座標の設定
		vertex[0].TexCoord = D3DXVECTOR2(0.0f, 0.0f);
		vertex[1].TexCoord = D3DXVECTOR2(1.0f, 0.0f);
		vertex[2].TexCoord = D3DXVECTOR2(0.0f, 1.0f);
		vertex[3].TexCoord = D3DXVECTOR2(1.0f, 1.0f);

		GetDeviceContext()->Unmap(g_VertexBuffer, 0);
	}

	return S_OK;
}

//=============================================================================
// マテリアルカラーの設定
//=============================================================================
void SetColorParticle(int pNo, int nIdxParticle, D3DXCOLOR col)
{

	g_aParticle[pNo][nIdxParticle].material.Diffuse = col;
}

//=============================================================================
// 頂点情報の作成
//=============================================================================
int SetParticle(D3DXVECTOR3 pos, D3DXCOLOR col, int nLife)
{
	int nIdxParticle = -1;

	for (int p = 0; p < EFFECT_MAX; p++)
	{
		for (int i = 0; i < MAX_PARTICLE; i++)
		{
			if (!g_aParticle[p][i].bUse)
			{
				g_aParticle[p][i].pos = pos;
				g_aParticle[p][i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
				g_aParticle[p][i].material.Diffuse = col;
				g_aParticle[p][i].nLife = nLife;
				g_aParticle[p][i].bUse = true;
				nIdxParticle = i;
			}
		}
	}
	return nIdxParticle;
}

PARTICLE *GetParticle()
{
	return &(g_aParticle[0][0]);
}

BEZ *GetBez(void)
{
	return &bez[0];
}



//=============================================================================
// パーティクルパターン
//=============================================================================

// リサージュ曲線
void effectLissajous(D3DXVECTOR3 pos, int sinX, int sinY, int sinZ ,float sizeX, float sizeY, float sizeZ, int Quantity)		// リサジェー曲線
{
	D3DXVECTOR3 rot;

	pos.y = 10.0f;
	rot = GetCamera()->rot;
	g_TexNo[EFFECT_LISSAJOUS] = 0;
	float size = 0.5;

	for (int i = 0; i < Quantity; i++)
	{
		if (!g_aParticle[EFFECT_LISSAJOUS][i].bUse)
		{
			g_aParticle[EFFECT_LISSAJOUS][i].nLife = 100;
			g_aParticle[EFFECT_LISSAJOUS][i].bUse = true;
			g_aParticle[EFFECT_LISSAJOUS][i].material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f);
		}

		g_aParticle[EFFECT_LISSAJOUS][i].count += 0.01f;

		float angle = ((1.0f / D3DX_PI) + (i / D3DX_PI)) + g_aParticle[EFFECT_LISSAJOUS][i].count;

		g_aParticle[EFFECT_LISSAJOUS][i].scale = D3DXVECTOR3(size, size, size);
		g_aParticle[EFFECT_LISSAJOUS][i].pos.x = pos.x - sinf(angle * sinX) * sizeX;
		g_aParticle[EFFECT_LISSAJOUS][i].pos.y = (pos.y - 5.0f) - sinf(angle * sinY) * sizeY;
		g_aParticle[EFFECT_LISSAJOUS][i].pos.z = pos.z - cosf(angle * sinZ) * sizeZ;


		g_aParticle[EFFECT_LISSAJOUS][i].material.Diffuse.a -= 0.02f;

	}
	
}

// 炎
int effectFire(D3DXVECTOR3 pos, D3DXCOLOR col, float fSizeX, float fSizeY, int count)
{
	int part = -1;

	int nLife;
  	g_TexNo[EFFECT_FIRE] = 3;
	int c = count;
	float rad;
	D3DXVECTOR3 move;


	for (int i = 30 * c; i < (30 * c) + 30; i++)
	{
		if (!g_aParticle[EFFECT_FIRE][i].bUse)
		{
			nLife = rand() % 60 + 5;

			g_aParticle[EFFECT_FIRE][i].pos = pos;
			g_aParticle[EFFECT_FIRE][i].scale = D3DXVECTOR3(2.0f, 2.0f, 2.0f);
			g_aParticle[EFFECT_FIRE][i].material.Diffuse = col;
			g_aParticle[EFFECT_FIRE][i].nLife = nLife;
			g_aParticle[EFFECT_FIRE][i].bUse = true;

		}
		g_aParticle[EFFECT_FIRE][i].pos.y += 1.0f;
		rad = (FLOAT)(rand() % 628 - 314) / 100.0f;
		move.x = sinf(rad) * fSizeX;
		move.z = cosf(rad) * fSizeY;
		g_aParticle[EFFECT_FIRE][i].pos.x += move.x;
		g_aParticle[EFFECT_FIRE][i].pos.z += move.z;

	}
	return part;
}

// 煙
int effectSmoke(D3DXVECTOR3 pos, D3DXCOLOR col, float fSizeX, float fSizeY, int count)
{
	//ランダムの初期化
	//srand((unsigned)time(NULL));

	int part = -1;

	int nLife;
	g_TexNo[EFFECT_SMOKE] = 3;
	int c = count;
	float rad;
	D3DXVECTOR3 move;

	for (int i = 40 * c; i < (40 * c) + 40; i++)
	{
		if (!g_aParticle[EFFECT_SMOKE][i].bUse)
		{
			nLife = rand() % 60 + 20;

			g_aParticle[EFFECT_SMOKE][i].pos = pos;
			g_aParticle[EFFECT_SMOKE][i].scale = D3DXVECTOR3(3.0f, 3.0f, 3.0f);
			g_aParticle[EFFECT_SMOKE][i].material.Diffuse = col;
			g_aParticle[EFFECT_SMOKE][i].nLife = nLife;
			g_aParticle[EFFECT_SMOKE][i].bUse = true;

			g_aParticle[EFFECT_SMOKE][i].pos.x = (pos.x - 24) + rand() % 48;
			g_aParticle[EFFECT_SMOKE][i].pos.z = (pos.z - 24) + rand() % 48;

		}

		g_aParticle[EFFECT_SMOKE][i].pos.y += 1.0f;

		rad = (FLOAT)(rand() % 628 - 314) / 100.0f;
		move.x = sinf(rad) * fSizeX;
		move.z = cosf(rad) * fSizeY;
		g_aParticle[EFFECT_SMOKE][i].pos.x += move.x;
		g_aParticle[EFFECT_SMOKE][i].pos.z += move.z;

	}
	return part;
}

// キラキラ
void effectStar(D3DXVECTOR3 pos)
{
	g_TexNo[EFFECT_STAR] = 0;
	float s = (FLOAT)(rand() % 10)*0.1f;

	for (int i = 0; i < 100; i++)
	{
		if (!g_aParticle[EFFECT_STAR][i].bUse)
		{
			g_aParticle[EFFECT_STAR][i].nLife = rand() % 10;
			g_aParticle[EFFECT_STAR][i].bUse = true;
			g_aParticle[EFFECT_STAR][i].material.Diffuse = D3DXCOLOR(0.5f, 0.3f, 0.0f, 1.0f);
		}

		g_aParticle[EFFECT_STAR][i].material.Diffuse.a -= 0.015f;
		g_aParticle[EFFECT_STAR][i].count += 0.01f;
		float angle = (D3DX_PI / 200) * g_aParticle[EFFECT_STAR][i].count;
		g_aParticle[EFFECT_STAR][i].scale = D3DXVECTOR3(s, s, s);
		g_aParticle[EFFECT_STAR][i].pos.x = pos.x - sinf(angle + 1 * i) * (rand() % 50);
		g_aParticle[EFFECT_STAR][i].pos.z = pos.z - cosf(angle + 1 * i) * (rand() % 50);
		g_aParticle[EFFECT_STAR][i].pos.y = (FLOAT)(rand() % 50);

	}
	
}

// サークル
int effectCircle(D3DXVECTOR3 pos)
{
	int a = -1;
	float c = 150.0f;
	g_TexNo[EFFECT_CIRCLE] = 2;
	float angle = (D3DX_PI / 200);

	for (int i = 0; i < 200; i++)
	{

		if (!g_aParticle[EFFECT_CIRCLE][i].bUse)
		{
			g_aParticle[EFFECT_CIRCLE][i].nLife = 100;
			g_aParticle[EFFECT_CIRCLE][i].bUse = true;
			g_aParticle[EFFECT_CIRCLE][i].material.Diffuse = D3DXCOLOR(0.5f, 0.3f, 0.0f, 1.0f);

		}

		g_aParticle[EFFECT_CIRCLE][i].count += 2.0f;
		g_aParticle[EFFECT_CIRCLE][i].pos.x = pos.x - sinf(angle + 1 * i) * g_aParticle[EFFECT_CIRCLE][i].count;
		g_aParticle[EFFECT_CIRCLE][i].pos.z = pos.z - cosf(angle + 1 * i) * g_aParticle[EFFECT_CIRCLE][i].count;
		g_aParticle[EFFECT_CIRCLE][i].pos.y = 5.0f;

		g_aParticle[EFFECT_CIRCLE][i].material.Diffuse.a -= 0.015f;

		if (g_aParticle[EFFECT_CIRCLE][i].count >= c)
		{
			g_aParticle[EFFECT_CIRCLE][i].count = 0.0f;
			g_aParticle[EFFECT_CIRCLE][i].material.Diffuse.a = 1.0f;
			g_aParticle[EFFECT_CIRCLE][i].bUse = false;
			effectNo = EFFECT_DEFAULT;
		}

	}

	return a;
}

// ベジェ曲線
void effectBezier(D3DXVECTOR3 pos)
{
	PLAYER *player = GetPlayer();

	D3DXVECTOR3 finish[4];
	D3DXVECTOR3 start;
	D3DXVECTOR3 P1;
	D3DXVECTOR3 P2;
	D3DXCOLOR color;
	int max = 200;

	start  =  player->pos;
	start.y = 0.1f;

	P1     = D3DXVECTOR3(-30.0f, 20.0f,  30.0f) + pos;
	P2     = D3DXVECTOR3(-30.0f, 30.0f, -30.0f) + pos;
	finish[0] = D3DXVECTOR3( 30.0f, 70.0f, -30.0f) + pos;
	color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	effect(start, P1, P2, finish[0], max, color);

	P1 = D3DXVECTOR3( 30.0f, 20.0f, 30.0f) + pos;
	P2 = D3DXVECTOR3(-30.0f, 30.0f, 30.0f) + pos;
	finish[1] = D3DXVECTOR3(-30.0f, 70.0f, -30.0f) + pos;
	color = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
	effect(start, P1, P2, finish[1], max, color);

	P1 = D3DXVECTOR3(30.0f, 20.0f, -30.0f) + pos;
	P2 = D3DXVECTOR3(30.0f, 30.0f, 30.0f) + pos;
	finish[2] = D3DXVECTOR3(-30.0f, 70.0f, 30.0f) + pos;
	color = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	effect(start, P1, P2, finish[2], max, color);

	P1 = D3DXVECTOR3(-30.0f, 20.0f, -30.0f) + pos;
	P2 = D3DXVECTOR3( 30.0f, 30.0f, -30.0f) + pos;
	finish[3] = D3DXVECTOR3(30.0f, 70.0f, 30.0f) + pos;
	color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	effect(start, P1, P2, finish[3], max, color);

}

void ParticleSwitch(D3DXVECTOR3 pos,int No)
{
	switch (No)
	{
	case EFFECT_LISSAJOUS:
		effectNo = EFFECT_LISSAJOUS;
		break;

	case EFFECT_FIRE:
		effectNo = EFFECT_FIRE;
		break;

	case EFFECT_SMOKE:
		effectNo = EFFECT_SMOKE;
		break;

	case EFFECT_STAR:
		effectNo = EFFECT_STAR;

		break;

	case EFFECT_CIRCLE:
		g_posBase = pos;
		effectNo = EFFECT_CIRCLE;
		break;

	case EFFECT_BEZIER:
		effectBezier(pos);

		break;

	case EFFECT_ALL:
		g_posBase = pos;
		effectNo = EFFECT_ALL;

		break;
	}
	
}
