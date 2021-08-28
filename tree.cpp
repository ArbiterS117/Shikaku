//=============================================================================
//
// 木処理 [tree.cpp]
// Author : GP11B132 14 ゴショウケン
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "shadow.h"
#include "tree.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX			(5)				// テクスチャの数

#define	TREE_WIDTH			(50.0f)			// 頂点サイズ
#define	TREE_HEIGHT			(80.0f)			// 頂点サイズ

#define	MAX_TREE			(256)			// 木最大数

//*****************************************************************************
// 構造体定義
//*****************************************************************************
typedef struct
{
	D3DXMATRIX	mtxWorld;		// ワールドマトリックス
	D3DXVECTOR3 pos;			// 位置
	D3DXVECTOR3 scl;			// スケール
	MATERIAL	material;		// マテリアル
	float		fWidth;			// 幅
	float		fHeight;		// 高さ
	int			nIdxShadow;		// 影ID
	bool		bUse;			// 使用しているかどうか

} TREE;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexTree(void);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// 頂点バッファ
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// テクスチャ情報

static TREE					g_aTree[MAX_TREE];	// 木ワーク
static int					g_TexNo;			// テクスチャ番号
static bool					g_bAlpaTest;		// アルファテストON/OFF
static int					g_nAlpha;			// アルファテストの閾値

static char *g_TextureName[] =
{
	"data/TEXTURE/tree001.png",
	"data/TEXTURE/tree002.png",
	"data/TEXTURE/tree003.png",
	"data/TEXTURE/tree004.png",
	"data/TEXTURE/tree005.png",
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitTree(void)
{
	MakeVertexTree();

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

	g_TexNo = 0;

	// 木ワークの初期化
	for (int i = 0; i < MAX_TREE; i++)
	{
		ZeroMemory(&g_aTree[i].material, sizeof(g_aTree[i].material));
		g_aTree[i].material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		g_aTree[i].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aTree[i].scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		g_aTree[i].fWidth = TREE_WIDTH;
		g_aTree[i].fHeight = TREE_HEIGHT;
		g_aTree[i].bUse = false;
	}

	g_bAlpaTest = true;	// true;
	g_nAlpha = 255;// 0x0;

	// 木の設定
	SetTree(D3DXVECTOR3(0.0f, 0.0f, 0.0f), 60.0f, 90.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	SetTree(D3DXVECTOR3(2000.0f, 0.0f, 0.0f), 60.0f, 90.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	SetTree(D3DXVECTOR3(-2000.0f, 0.0f, 0.0f), 60.0f, 90.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	SetTree(D3DXVECTOR3(0.0f, 0.0f, 2000.0f), 60.0f, 90.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	SetTree(D3DXVECTOR3(0.0f, 0.0f, -2000.0f), 60.0f, 90.0f, D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitTree(void)
{
	for (int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if (g_Texture[nCntTex] != NULL)
		{// テクスチャの解放
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	if (g_VertexBuffer != NULL)
	{// 頂点バッファの解放
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateTree(void)
{

	for (int i = 0; i < MAX_TREE; i++)
	{
		if (g_aTree[i].bUse)
		{
			// 影の位置設定
			SetPositionShadow(g_aTree[i].nIdxShadow, D3DXVECTOR3(g_aTree[i].pos.x, 0.1f, g_aTree[i].pos.z));

			g_aTree[i].material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, (float)g_nAlpha / 255.0f);
		}
	}

	// アルファテストON/OFF
	if (GetKeyboardTrigger(DIK_F1))
	{
		g_bAlpaTest = g_bAlpaTest ? false : true;
	}

	// アルファテストの閾値変更
	if (GetKeyboardPress(DIK_I))
	{
		g_nAlpha--;
		if (g_nAlpha < 0)
		{
			g_nAlpha = 0;
		}
	}
	if (GetKeyboardPress(DIK_K))
	{
		g_nAlpha++;
		if (g_nAlpha > 255)
		{
			g_nAlpha = 255;
		}
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawTree(void)
{
	// αテスト設定
	if (g_bAlpaTest == true)
	{
		// αテストを有効に
		SetAlphaTestEnable(true);
	}

	// ライティングを無効
	SetLightEnable(false);

	D3DXMATRIX mtxView, mtxScale, mtxTranslate;

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	for (int i = 0; i < MAX_TREE; i++)
	{
		if (g_aTree[i].bUse)
		{
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_aTree[i].mtxWorld);

			// ビューマトリックスを取得
			CAMERA *cam = GetCamera();
			mtxView = cam->mtxView;

			// =========ポリゴンを正面に向ける
#if 0
			// 逆行列を計算で求める
			D3DXMatrixInverse(&g_aTree[i].mtxWorld, NULL, &mtxView); //激遅い
			g_aTree[i].mtxWorld._41 = 0.0f; //移動Tx値をクリア
			g_aTree[i].mtxWorld._42 = 0.0f; //移動Ty値をクリア
			g_aTree[i].mtxWorld._43 = 0.0f; //移動Tz値をクリア
#else
			//逆行列の作り方 正方行列
			//DirectXのカメラ行列は正方行列になっています
			//正方行列には性質がある
			//正方行列の転置行列は逆行列になる
			//転置行列 → 左上から右下へ向かって斜め線を引く
			//数値交換 → その線を対象として上下左右入れ替えた行列

			//このやり方は超早い
			g_aTree[i].mtxWorld._11 = mtxView._11;
			g_aTree[i].mtxWorld._12 = mtxView._21;
			g_aTree[i].mtxWorld._13 = mtxView._31;
			g_aTree[i].mtxWorld._21 = mtxView._12;
			g_aTree[i].mtxWorld._22 = mtxView._22;
			g_aTree[i].mtxWorld._23 = mtxView._32;
			g_aTree[i].mtxWorld._31 = mtxView._13;
			g_aTree[i].mtxWorld._32 = mtxView._23;
			g_aTree[i].mtxWorld._33 = mtxView._33;
			//g_aTree[i].mtxWorld._41 = 0.0f; //移動Tx値をクリア 移動しない
			//g_aTree[i].mtxWorld._42 = 0.0f; //移動Ty値をクリア
			//g_aTree[i].mtxWorld._43 = 0.0f; //移動Tz値をクリア
#endif
			// =========

			// スケールを反映
			D3DXMatrixScaling(&mtxScale, g_aTree[i].scl.x,
				g_aTree[i].scl.y,
				g_aTree[i].scl.z);
			D3DXMatrixMultiply(&g_aTree[i].mtxWorld, &g_aTree[i].mtxWorld, &mtxScale);

			// 移動を反映
			D3DXMatrixTranslation(&mtxTranslate, g_aTree[i].pos.x,
				g_aTree[i].pos.y,
				g_aTree[i].pos.z);
			D3DXMatrixMultiply(&g_aTree[i].mtxWorld, &g_aTree[i].mtxWorld, &mtxTranslate);


			// ワールドマトリックスの設定
			SetWorldMatrix(&g_aTree[i].mtxWorld);

			// マテリアル設定
			SetMaterial(g_aTree[i].material);

			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[i % TEXTURE_MAX]);

			// ポリゴンの描画
			GetDeviceContext()->Draw(4, 0);
		}
	}

	// ライティングを有効に
	SetLightEnable(true);

	// αテストを無効に
	SetAlphaTestEnable(false); // 処理速度遅い
}

//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexTree(void)
{
	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	// 頂点バッファに値をセットする
	D3D11_MAPPED_SUBRESOURCE msr;
	GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

	VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

	float fWidth = 60.0f;
	float fHeight = 90.0f;

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

	GetDeviceContext()->Unmap(g_VertexBuffer, 0);

	return S_OK;
}

//=============================================================================
// 木のパラメータをセット
//=============================================================================
int SetTree(D3DXVECTOR3 pos, float fWidth, float fHeight, D3DXCOLOR col)
{
	int nIdxTree = -1;

	for (int i = 0; i < MAX_TREE; i++)
	{
		if (!g_aTree[i].bUse)
		{
			g_aTree[i].pos = pos;
			g_aTree[i].scl = D3DXVECTOR3(10.0f, 10.0f, 10.0f);
			g_aTree[i].fWidth = fWidth;
			g_aTree[i].fHeight = fHeight;
			g_aTree[i].bUse = true;

			// 影の設定
			g_aTree[i].nIdxShadow = CreateShadow(g_aTree[i].pos, 3.5f, 3.5f);

			nIdxTree = i;

			break;
		}
	}

	return nIdxTree;
}
