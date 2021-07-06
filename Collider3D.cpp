//=============================================================================
//
// collider処理 [Collider3D.cpp]
// Author : GP11B132 14 ゴショウケン
//
//=============================================================================
#include "main.h"
#include "Collider3D.h"
#include "renderer.h"
#include "input.h"
#include "debugproc.h"
#include "camera.h"
#include "platform.h"
#include "ground.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define TEXTURE_MAX			                (1)		// テクスチャの数
#define MAX_SPRITE			                (1)		// テクスチャの数
								    
#define	COLLIDER3D_RENDER_SIZE_X	        (0.5f)  // 頂点サイズX
#define	COLLIDER3D_RENDER_CIRCLE_POLYNUM	(26)    // 頂点数


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexCollider3D(void);
HRESULT MakeVertexCollider3DCircle(void);
HRESULT MakeVertexCollider3DLine(void);

bool CheckHitBB (int col1ID, int col2ID);
bool CheckHitBC (int col1ID, int col2ID);
bool CheckHitBBC(int col1ID, int col2ID);
bool CheckHitBBC(int col1ID, int col2ID, D3DXVECTOR3 &contactPoint);
bool CheckHitBCB(int col1ID, int col2ID);
bool CheckHitBCB(int col1ID, int col2ID, D3DXVECTOR3 &contactPoint); 

bool CheckHitOBB(int col1ID, int col2ID);
FLOAT LenSegOnSeparateAxis(D3DXVECTOR3 *Sep, D3DXVECTOR3 *e1, D3DXVECTOR3 *e2, D3DXVECTOR3 *e3 = 0);
bool CheckHitOBBReturnLen(int col1ID, int col2ID, D3DXVECTOR3 *Len);
float CheckHitOBBToPoint(int col1ID, int col2ID, D3DXVECTOR3 *vec);
																																												   //as switch 
bool RayCast(D3DXVECTOR3 p0, D3DXVECTOR3 p1, D3DXVECTOR3 p2, D3DXVECTOR3 pos0, D3DXVECTOR3 pos1, D3DXVECTOR3 *hit, D3DXVECTOR3 *normal, D3DXVECTOR3 caculatedNormal = D3DXVECTOR3(-100.0f,0.0f,0.0f));
//*****************************************************************************
// グローバル変数
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	             // 頂点情報
static ID3D11Buffer					*g_VertexBuffercircle = NULL;	     // 円頂点情報
static ID3D11Buffer					*g_VertexBufferLine = NULL;	         // 線頂点情報
static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	 // テクスチャ情報

static COLLIDER3D				     g_Collider3D[MAX_COLLIDER3D];	     // colliderワーク
static int						     g_TexNo;					         // テクスチャ番号
								     
static bool                          g_Draw3DCollider = false;
static bool                          g_isLINESTRIP = true;
static bool                          g_DrawChaceArea = false;

static char* g_TextureName[] = {
	"data/TEXTURE/white.jpg",
};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitCollider3D(void)
{
	// 頂点バッファの作成
	MakeVertexCollider3D();
	MakeVertexCollider3DCircle();
	MakeVertexCollider3DLine();

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

	// ワーク初期化
	for (int i = 0; i < MAX_COLLIDER3D; i++)
	{
		ZeroMemory(&g_Collider3D[i].material, sizeof(g_Collider3D[i].material));
		g_Collider3D[i].material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		g_Collider3D[i].pos = D3DXVECTOR3(0.0f, 0.1f, 0.0f);
		g_Collider3D[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Collider3D[i].scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		g_Collider3D[i].Use = false;
		g_Collider3D[i].enable = false;

		g_Collider3D[i].type = Collider3DType_box;
		g_Collider3D[i].tag = collider3DTag_none;
		g_Collider3D[i].wx = 0.0f;
		g_Collider3D[i].hy = 0.0f;
		g_Collider3D[i].lz = 0.0f;
		g_Collider3D[i].r = 0.0f;

		g_Collider3D[i].normalDirect[0] = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
		g_Collider3D[i].normalDirect[1] = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		g_Collider3D[i].normalDirect[2] = D3DXVECTOR3(0.0f, 0.0f, 1.0f);
		g_Collider3D[i].isCollision = false;

		g_Collider3D[i].canDraw = true;
	}

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitCollider3D(void)
{
	// 頂点バッファの解放
	if (g_VertexBuffer)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}
	if (g_VertexBuffercircle)
	{
		g_VertexBuffercircle->Release();
		g_VertexBuffercircle = NULL;
	}
	if (g_VertexBufferLine)
	{
		g_VertexBufferLine->Release();
		g_VertexBufferLine = NULL;
	}
	// テクスチャの解放
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		if (g_Texture[i]) {
			g_Texture[i]->Release();
			g_Texture[i] = NULL;
		}
	}

}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateCollider3D(void)
{
	if (GetKeyboardTrigger(DIK_1)) {
		g_Draw3DCollider = !g_Draw3DCollider;
	}
	if (GetKeyboardTrigger(DIK_6)) {
		g_isLINESTRIP = !g_isLINESTRIP;
	}
	if (GetKeyboardTrigger(DIK_5)) {
		g_DrawChaceArea = !g_DrawChaceArea;
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawCollider3D(void)
{
	if (!g_Draw3DCollider) return;

	// 減算合成
	//SetBlendState(BLEND_MODE_SUBTRACT);
	// Z比較なし
	//SetDepthEnable(false);
	// ライティングを無効
	SetLightEnable(false);

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;

	for (int i = 0; i < MAX_COLLIDER3D; i++)
	{
		if (g_Collider3D[i].Use && g_Collider3D[i].canDraw && g_Collider3D[i].enable)
		{
			if(g_Collider3D[i].type == Collider3DType_box)GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);
			if(g_Collider3D[i].type == Collider3DType_circle)GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffercircle, &stride, &offset);

			// プリミティブトポロジ設定
			GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
			if (g_isLINESTRIP) GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

			// テクスチャ設定
			GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);

			D3DXMATRIX mtxScl, mtxRot, mtxTranslate, mtxView;
			// ビューマトリックスを取得
			CAMERA *cam = GetCamera();
			mtxView = cam->mtxView;

			if (!g_DrawChaceArea && g_Collider3D[i].tag == collider3DTag_EnemyChaseCheck)continue;
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_Collider3D[i].mtxWorld);

			if (g_Collider3D[i].type == Collider3DType_circle) {
				g_Collider3D[i].mtxWorld._11 = mtxView._11;
				g_Collider3D[i].mtxWorld._12 = mtxView._21;
				g_Collider3D[i].mtxWorld._13 = mtxView._31;
				g_Collider3D[i].mtxWorld._21 = mtxView._12;
				g_Collider3D[i].mtxWorld._22 = mtxView._22;
				g_Collider3D[i].mtxWorld._23 = mtxView._32;
				g_Collider3D[i].mtxWorld._31 = mtxView._13;
				g_Collider3D[i].mtxWorld._32 = mtxView._23;
				g_Collider3D[i].mtxWorld._33 = mtxView._33;
			}

			// スケールを反映
			if(g_Collider3D[i].type == Collider3DType_box)D3DXMatrixScaling(&mtxScl, g_Collider3D[i].wx, g_Collider3D[i].hy, g_Collider3D[i].lz);
			if (g_Collider3D[i].type == Collider3DType_circle)D3DXMatrixScaling(&mtxScl, g_Collider3D[i].r * 2, g_Collider3D[i].r * 2, g_Collider3D[i].r * 2);
			D3DXMatrixMultiply(&g_Collider3D[i].mtxWorld, &g_Collider3D[i].mtxWorld, &mtxScl);

			// 回転を反映
			D3DXMatrixRotationYawPitchRoll(&mtxRot, g_Collider3D[i].rot.y, g_Collider3D[i].rot.x, g_Collider3D[i].rot.z);
			D3DXMatrixMultiply(&g_Collider3D[i].mtxWorld, &g_Collider3D[i].mtxWorld, &mtxRot);

			// 移動を反映
			D3DXMatrixTranslation(&mtxTranslate, g_Collider3D[i].pos.x, g_Collider3D[i].pos.y, g_Collider3D[i].pos.z);
			D3DXMatrixMultiply(&g_Collider3D[i].mtxWorld, &g_Collider3D[i].mtxWorld, &mtxTranslate);

			// ワールドマトリックスの設定
			SetWorldMatrix(&g_Collider3D[i].mtxWorld);

			// マテリアルの設定
			g_Collider3D[i].material.Diffuse = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.5f); // set color
			if(g_Collider3D[i].tag == collider3DTag_EnemyChaseCheck)g_Collider3D[i].material.Diffuse = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.5f); // set chase Area to blue
			if(g_Collider3D[i].tag == collider3DTag_groundCheck)g_Collider3D[i].material.Diffuse = D3DXCOLOR(1.0f, 0.0f, 1.0f, 1.5f); // set groundchecl collider to purple
			if(g_Collider3D[i].tag == collider3DTag_cellingCheck)g_Collider3D[i].material.Diffuse = D3DXCOLOR(1.0f, 0.5f, 1.0f, 1.5f); // set groundchecl collider to purple
			if (g_Collider3D[i].isCollision) {
				g_Collider3D[i].material.Diffuse = D3DXCOLOR(0.0f, 1.0f, 1.0f, 1.5f); // set groundchecl collider to purple
				g_Collider3D[i].isCollision = false;
			}
			SetMaterial(g_Collider3D[i].material);

			// ポリゴン描画
			
			if (g_Collider3D[i].type == Collider3DType_box) {
				if (g_isLINESTRIP) { //line strip
					for (int i = 0; i < 7; i++) {
						GetDeviceContext()->Draw(2, i);
					}
					for (int i = 8; i < 23; i++) {
						GetDeviceContext()->Draw(2, i);
					}
					GetDeviceContext()->Draw(2, 24);
				}
				else {//Tri strip
					for (int i = 0; i < 6; i++) {
						GetDeviceContext()->Draw(4, 4 * i);
					}
				}
			}
			if (g_Collider3D[i].type == Collider3DType_circle) {
				for (int i = 0; i < COLLIDER3D_RENDER_CIRCLE_POLYNUM; i++) {
					GetDeviceContext()->Draw(2, i);
				}
			}

			if(g_Collider3D[i].type == Collider3DType_circle) g_Collider3D[i].canDraw = false; 
			if (g_Collider3D[i].tag == collider3DTag_groundCheck)g_Collider3D[i].canDraw = false;

			////===========================================DrawLine
			//// 頂点バッファ設定
			//stride = sizeof(VERTEX_3D);
			//offset = 0;
			//GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBufferLine, &stride, &offset);
			//
			//// プリミティブトポロジ設定
			//GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
			//
			//// テクスチャ設定
			//GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo]);
			//
			//// ワールドマトリックスの初期化
			//D3DXMatrixIdentity(&g_Collider3D[i].mtxWorld);
			//
			//// スケールを反映
			//if (g_Collider3D[i].type == Collider3DType_box)D3DXMatrixScaling(&mtxScl, g_Collider3D[i].wx, g_Collider3D[i].hy, g_Collider3D[i].lz);
			//if (g_Collider3D[i].type == Collider3DType_circle)D3DXMatrixScaling(&mtxScl, g_Collider3D[i].r, g_Collider3D[i].r, g_Collider3D[i].r);
			//D3DXMatrixMultiply(&g_Collider3D[i].mtxWorld, &g_Collider3D[i].mtxWorld, &mtxScl);
			//
			//// 回転を反映
			//D3DXMatrixRotationYawPitchRoll(&mtxRot, g_Collider3D[i].rot.y, g_Collider3D[i].rot.x, g_Collider3D[i].rot.z);
			//D3DXMatrixMultiply(&g_Collider3D[i].mtxWorld, &g_Collider3D[i].mtxWorld, &mtxRot);
			//
			//// 移動を反映
			//D3DXMatrixTranslation(&mtxTranslate, g_Collider3D[i].pos.x, g_Collider3D[i].pos.y, g_Collider3D[i].pos.z);
			//D3DXMatrixMultiply(&g_Collider3D[i].mtxWorld, &g_Collider3D[i].mtxWorld, &mtxTranslate);
			//
			//// ワールドマトリックスの設定
			//SetWorldMatrix(&g_Collider3D[i].mtxWorld);
			//
			//for (int i = 0; i < 3; i++) {
			//	// マテリアルの設定
			//	if(i == 0)g_Collider3D[i].material.Diffuse = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.5f); // set color to red
			//	if(i == 1)g_Collider3D[i].material.Diffuse = D3DXCOLOR(0.0f, 1.0f, 1.0f, 1.5f); // set color to green
			//	if(i == 2)g_Collider3D[i].material.Diffuse = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.5f); // set color to blue
			//	SetMaterial(g_Collider3D[i].material);
			//
			//	// ポリゴン描画
			//	GetDeviceContext()->Draw(2, 2 * i);
			//}
		}
	}

	// 通常ブレンド
	//SetBlendState(BLEND_MODE_ALPHABLEND);
	// Z比較あり
	//SetDepthEnable(true);
	// ライティングを有効
	SetLightEnable(true);
}

//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexCollider3D()
{
	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 26;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	{//頂点バッファの中身を埋める
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		//地面のポリゴン
		vertex[0]  = {D3DXVECTOR3( COLLIDER3D_RENDER_SIZE_X, -COLLIDER3D_RENDER_SIZE_X,  COLLIDER3D_RENDER_SIZE_X), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f)};////底
		vertex[1]  = {D3DXVECTOR3(-COLLIDER3D_RENDER_SIZE_X, -COLLIDER3D_RENDER_SIZE_X,  COLLIDER3D_RENDER_SIZE_X), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f)};
		vertex[2]  = {D3DXVECTOR3( COLLIDER3D_RENDER_SIZE_X, -COLLIDER3D_RENDER_SIZE_X, -COLLIDER3D_RENDER_SIZE_X), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f)};
		vertex[3]  = {D3DXVECTOR3(-COLLIDER3D_RENDER_SIZE_X, -COLLIDER3D_RENDER_SIZE_X, -COLLIDER3D_RENDER_SIZE_X), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f)};
		vertex[4]  = {D3DXVECTOR3(-COLLIDER3D_RENDER_SIZE_X,  COLLIDER3D_RENDER_SIZE_X, -COLLIDER3D_RENDER_SIZE_X), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f)};////前
		vertex[5]  = {D3DXVECTOR3( COLLIDER3D_RENDER_SIZE_X,  COLLIDER3D_RENDER_SIZE_X, -COLLIDER3D_RENDER_SIZE_X), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f)};
		vertex[6]  = {D3DXVECTOR3(-COLLIDER3D_RENDER_SIZE_X, -COLLIDER3D_RENDER_SIZE_X, -COLLIDER3D_RENDER_SIZE_X), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f)};
		vertex[7]  = {D3DXVECTOR3( COLLIDER3D_RENDER_SIZE_X, -COLLIDER3D_RENDER_SIZE_X, -COLLIDER3D_RENDER_SIZE_X), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f)};
		vertex[8]  = {D3DXVECTOR3(-COLLIDER3D_RENDER_SIZE_X,  COLLIDER3D_RENDER_SIZE_X,  COLLIDER3D_RENDER_SIZE_X), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f)};////左
		vertex[9]  = {D3DXVECTOR3(-COLLIDER3D_RENDER_SIZE_X,  COLLIDER3D_RENDER_SIZE_X, -COLLIDER3D_RENDER_SIZE_X), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f)};
		vertex[10] = {D3DXVECTOR3(-COLLIDER3D_RENDER_SIZE_X, -COLLIDER3D_RENDER_SIZE_X,  COLLIDER3D_RENDER_SIZE_X), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f)};
		vertex[11] = {D3DXVECTOR3(-COLLIDER3D_RENDER_SIZE_X, -COLLIDER3D_RENDER_SIZE_X, -COLLIDER3D_RENDER_SIZE_X), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f)};
		vertex[12] = {D3DXVECTOR3( COLLIDER3D_RENDER_SIZE_X,  COLLIDER3D_RENDER_SIZE_X, -COLLIDER3D_RENDER_SIZE_X), D3DXVECTOR3(1.0f, 0.0f, 0.0f),  D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f)};////右
		vertex[13] = {D3DXVECTOR3( COLLIDER3D_RENDER_SIZE_X,  COLLIDER3D_RENDER_SIZE_X,  COLLIDER3D_RENDER_SIZE_X), D3DXVECTOR3(1.0f, 0.0f, 0.0f),  D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f)};
		vertex[14] = {D3DXVECTOR3( COLLIDER3D_RENDER_SIZE_X, -COLLIDER3D_RENDER_SIZE_X, -COLLIDER3D_RENDER_SIZE_X), D3DXVECTOR3(1.0f, 0.0f, 0.0f),  D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f)};
		vertex[15] = {D3DXVECTOR3( COLLIDER3D_RENDER_SIZE_X, -COLLIDER3D_RENDER_SIZE_X,  COLLIDER3D_RENDER_SIZE_X), D3DXVECTOR3(1.0f, 0.0f, 0.0f),  D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f)};
		vertex[16] = {D3DXVECTOR3( COLLIDER3D_RENDER_SIZE_X,  COLLIDER3D_RENDER_SIZE_X,  COLLIDER3D_RENDER_SIZE_X), D3DXVECTOR3(0.0f, 0.0f, 1.0f),  D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f)};////後
		vertex[17] = {D3DXVECTOR3(-COLLIDER3D_RENDER_SIZE_X,  COLLIDER3D_RENDER_SIZE_X,  COLLIDER3D_RENDER_SIZE_X), D3DXVECTOR3(0.0f, 0.0f, 1.0f),  D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f)};
		vertex[18] = {D3DXVECTOR3( COLLIDER3D_RENDER_SIZE_X, -COLLIDER3D_RENDER_SIZE_X,  COLLIDER3D_RENDER_SIZE_X), D3DXVECTOR3(0.0f, 0.0f, 1.0f),  D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f)};
		vertex[19] = {D3DXVECTOR3(-COLLIDER3D_RENDER_SIZE_X, -COLLIDER3D_RENDER_SIZE_X,  COLLIDER3D_RENDER_SIZE_X), D3DXVECTOR3(0.0f, 0.0f, 1.0f),  D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f)};
		vertex[20] = {D3DXVECTOR3(-COLLIDER3D_RENDER_SIZE_X,  COLLIDER3D_RENDER_SIZE_X,  COLLIDER3D_RENDER_SIZE_X), D3DXVECTOR3(0.0f, 1.0f, 0.0f),  D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f)};////頂
		vertex[21] = {D3DXVECTOR3( COLLIDER3D_RENDER_SIZE_X,  COLLIDER3D_RENDER_SIZE_X,  COLLIDER3D_RENDER_SIZE_X), D3DXVECTOR3(0.0f, 1.0f, 0.0f),  D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f)};
		vertex[22] = {D3DXVECTOR3(-COLLIDER3D_RENDER_SIZE_X,  COLLIDER3D_RENDER_SIZE_X, -COLLIDER3D_RENDER_SIZE_X), D3DXVECTOR3(0.0f, 1.0f, 0.0f),  D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f)};
		vertex[23] = {D3DXVECTOR3( COLLIDER3D_RENDER_SIZE_X,  COLLIDER3D_RENDER_SIZE_X, -COLLIDER3D_RENDER_SIZE_X), D3DXVECTOR3(0.0f, 1.0f, 0.0f),  D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f)};
		vertex[24] = {D3DXVECTOR3( COLLIDER3D_RENDER_SIZE_X,  COLLIDER3D_RENDER_SIZE_X, -COLLIDER3D_RENDER_SIZE_X), D3DXVECTOR3(0.0f, 1.0f, 0.0f),  D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f)};
		vertex[25] = {D3DXVECTOR3( COLLIDER3D_RENDER_SIZE_X, -COLLIDER3D_RENDER_SIZE_X, -COLLIDER3D_RENDER_SIZE_X), D3DXVECTOR3(0.0f, 1.0f, 0.0f),  D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f)};

		GetDeviceContext()->Unmap(g_VertexBuffer, 0);
	}

	return S_OK;
}
HRESULT MakeVertexCollider3DCircle()
{
	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * (COLLIDER3D_RENDER_CIRCLE_POLYNUM + 1);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffercircle);

	{//頂点バッファの中身を埋める
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_VertexBuffercircle, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		//ポリゴン
		for (int i = 0; i < COLLIDER3D_RENDER_CIRCLE_POLYNUM; i++) {
			float ang = ((float)i / (float)COLLIDER3D_RENDER_CIRCLE_POLYNUM) * D3DX_PI * 2;
			vertex[i] = { D3DXVECTOR3(COLLIDER3D_RENDER_SIZE_X * cosf(ang), COLLIDER3D_RENDER_SIZE_X * sinf(ang), 0.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f) };
		}
		vertex[COLLIDER3D_RENDER_CIRCLE_POLYNUM] = { D3DXVECTOR3(COLLIDER3D_RENDER_SIZE_X * cosf(0.0f), COLLIDER3D_RENDER_SIZE_X * sinf(0.0f), 0.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f) };
		GetDeviceContext()->Unmap(g_VertexBuffercircle, 0);
	}

	return S_OK;
}
HRESULT MakeVertexCollider3DLine()
{
	// 頂点バッファ生成
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 6;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBufferLine);

	{//頂点バッファの中身を埋める
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_VertexBufferLine, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		//地面のポリゴン
		vertex[0] = { D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f) };////底
		vertex[1] = { D3DXVECTOR3(COLLIDER3D_RENDER_SIZE_X, 0.0f, 0.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f) };
		vertex[2] = { D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f) };////底
		vertex[3] = { D3DXVECTOR3(0.0f, COLLIDER3D_RENDER_SIZE_X, 0.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f) };
		vertex[4] = { D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f) };////底
		vertex[5] = { D3DXVECTOR3(0.0f, 0.0f, COLLIDER3D_RENDER_SIZE_X), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f) };

		GetDeviceContext()->Unmap(g_VertexBufferLine, 0);
	}

	return S_OK;
}

//=============================================================================
// 内積
//=============================================================================
float dotProduct(D3DXVECTOR3 *v1, D3DXVECTOR3 *v2)
{
#if 0
	float ans = v1->x * v2->x + v1->y * v2->y + v1->z * v2->z;
#else
	// ダイ?クトＸでは、、、
	float ans = D3DXVec3Dot(v1, v2);
#endif

	return(ans);


}

//=============================================================================
// 外積
//=============================================================================
void crossProduct(D3DXVECTOR3 *ret, D3DXVECTOR3 *v1, D3DXVECTOR3 *v2)
{
#if 0
	ret->x = v1->y * v2->z - v1->z * v2->y;
	ret->y = v1->z * v2->x - v1->x * v2->z;
	ret->z = v1->x * v2->y - v1->y * v2->x;
#else
	// ダイ?クトＸでは、、、
	D3DXVec3Cross(ret, v1, v2);
#endif

}

//=============================================================================
// colliderの作成
//=============================================================================
int CreateCollider3DBox(enumCollider3DTag tag, D3DXVECTOR3 pos, float wx, float hy, float lz, int masterID)
{
	int nIdxCollider3D = -1;

	for (int i = 0; i < MAX_COLLIDER3D; i++)
	{
		if (!g_Collider3D[i].Use)
		{
			g_Collider3D[i].Use = true;
			g_Collider3D[i].canDraw = true;
			g_Collider3D[i].enable = true;
			g_Collider3D[i].pos = pos;
			g_Collider3D[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			g_Collider3D[i].scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

			g_Collider3D[i].type = Collider3DType_box;
			g_Collider3D[i].tag = tag;
			g_Collider3D[i].wx = wx;
			g_Collider3D[i].hy = hy;
			g_Collider3D[i].lz = lz;

			g_Collider3D[i].masterID = masterID;

			nIdxCollider3D = i;
			break;
		}
	}

	return nIdxCollider3D;
}

int CreateCollider3DCircle(enumCollider3DTag tag, D3DXVECTOR3 pos, float r, int masterID)
{
	int nIdxCollider3D = -1;

	for (int i = 0; i < MAX_COLLIDER3D; i++)
	{
		if (!g_Collider3D[i].Use)
		{
			g_Collider3D[i].Use = true;
			g_Collider3D[i].canDraw = true;
			g_Collider3D[i].enable = true;
			g_Collider3D[i].pos = pos;
			g_Collider3D[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			g_Collider3D[i].scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

			g_Collider3D[i].type = Collider3DType_circle;
			g_Collider3D[i].tag = tag;
			g_Collider3D[i].r = r;

			g_Collider3D[i].masterID = masterID;

			nIdxCollider3D = i;
			break;
		}
	}

	return nIdxCollider3D;
}

//=============================================================================
// マテリアルカラーの設定
//=============================================================================
void SetColorCollider3D(int nIdxCollider3D, D3DXCOLOR col)
{
	g_Collider3D[nIdxCollider3D].material.Diffuse = col;
}

//=============================================================================
// colliderの破棄
//=============================================================================
void ReleaseCollider3D(int nIdxCollider3D)
{
	if (nIdxCollider3D >= 0 && nIdxCollider3D < MAX_COLLIDER3D)
	{
		g_Collider3D[nIdxCollider3D].Use = false;
	}
}

//=============================================================================
// 位置の設定
//=============================================================================
void SetPositionCollider3D(int nIdxCollider3D, D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	g_Collider3D[nIdxCollider3D].pos = pos;
	g_Collider3D[nIdxCollider3D].rot = rot;

}

void SetScaleCollider3D(int nIdxCollider3D, D3DXVECTOR3 scl)
{
	g_Collider3D[nIdxCollider3D].wx = scl.x;
	g_Collider3D[nIdxCollider3D].hy = scl.y;
	g_Collider3D[nIdxCollider3D].lz = scl.z;

}
void SetScaleCollider3D(int nIdxCollider3D, float r) 
{
	g_Collider3D[nIdxCollider3D].r = r;
}

//=============================================================================
// BBによる当たり判定処理
// 回転は考慮しない
// 戻り値：当たってたらtrue
//=============================================================================
bool CheckHitBB(int col1ID, int col2ID)
{
	COLLIDER3D col1, col2;
	col1 = g_Collider3D[col1ID];
	col2 = g_Collider3D[col2ID];
	//当たり判定AABB        　
	if ((col1.pos.x + col1.wx * 0.5f >col2.pos.x - col2.wx * 0.5f) &&
		(col1.pos.x - col1.wx * 0.5f <col2.pos.x + col2.wx * 0.5f) &&
		(col1.pos.y + col1.hy * 0.5f >col2.pos.y - col2.hy * 0.5f) &&
		(col1.pos.y - col1.hy * 0.5f <col2.pos.y + col2.hy * 0.5f) &&
		(col1.pos.z + col1.lz * 0.5f >col2.pos.z - col2.lz * 0.5f) &&
		(col1.pos.z - col1.lz * 0.5f <col2.pos.z + col2.lz * 0.5f))
	{
		return true;
	}

	return false;
}

//=============================================================================
// BCによる当たり判定処理
// サイズは半径
// 戻り値：当たってたらtrue
//=============================================================================
bool CheckHitBC(int col1ID, int col2ID) // 半径
{
	COLLIDER3D col1, col2;
	col1 = g_Collider3D[col1ID];
	col2 = g_Collider3D[col2ID];
	float len = (col1.r + col2.r) * (col1.r + col2.r);	// 半径を2乗した物
	D3DXVECTOR3 temp = col1.pos - col2.pos;
	float fLengthSq = D3DXVec3LengthSq(&temp);		// 2点間の距離（2乗した物） 三平方の定理含むは D3DXVec3Length() -> bad bigO

	if (len > fLengthSq)
	{
		
		g_Collider3D[col1ID].canDraw = true;
		g_Collider3D[col2ID].canDraw = true;
		return true;
	}

	return false;
}

//=============================================================================
// BBCによる当たり判定処理
// サイズは半径 回転は考慮しない
// 戻り値：当たってたらtrue
//=============================================================================
bool CheckHitBBC(int col1ID, int col2ID) {
	COLLIDER3D col1, col2;
	col1 = g_Collider3D[col1ID];
	col2 = g_Collider3D[col2ID];
	// get box closest point to sphere center by clamping
	float x = max(col1.pos.x - col1.wx * 0.5f, min(col2.pos.x, col1.pos.x + col1.wx * 0.5f)); // Math.max(box.minX, Math.min(sphere.x, box.maxX));
	float y = max(col1.pos.y - col1.hy * 0.5f, min(col2.pos.y, col1.pos.y + col1.hy * 0.5f)); // Math.max(box.minY, Math.min(sphere.y, box.maxY));
	float z = max(col1.pos.z - col1.lz * 0.5f, min(col2.pos.z, col1.pos.z + col1.lz * 0.5f)); // Math.max(box.minZ, Math.min(sphere.z, box.maxZ));

	// this is the same as isPointInsideSphere
	float distance = (x - col2.pos.x) * (x - col2.pos.x) +
		             (y - col2.pos.y) * (y - col2.pos.y) +
		             (z - col2.pos.z) * (z - col2.pos.z);

	if (distance < col2.r * col2.r)
	{
		return true;
	}
	return false;
}
bool CheckHitBBC(int col1ID, int col2ID, D3DXVECTOR3 &contactPoint) {
	COLLIDER3D col1, col2;
	col1 = g_Collider3D[col1ID];
	col2 = g_Collider3D[col2ID];
	// get box closest point to sphere center by clamping
	float x = max(col1.pos.x - col1.wx * 0.5f, min(col2.pos.x, col1.pos.x + col1.wx * 0.5f)); // Math.max(box.minX, Math.min(sphere.x, box.maxX));
	float y = max(col1.pos.y - col1.hy * 0.5f, min(col2.pos.y, col1.pos.y + col1.hy * 0.5f)); // Math.max(box.minY, Math.min(sphere.y, box.maxY));
	float z = max(col1.pos.z - col1.lz * 0.5f, min(col2.pos.z, col1.pos.z + col1.lz * 0.5f)); // Math.max(box.minZ, Math.min(sphere.z, box.maxZ));
	contactPoint.x = x;
	contactPoint.y = y;
	contactPoint.z = z;
	// this is the same as isPointInsideSphere
	float distance = (x - col2.pos.x) * (x - col2.pos.x) +
		(y - col2.pos.y) * (y - col2.pos.y) +
		(z - col2.pos.z) * (z - col2.pos.z);

	if (distance < col2.r * col2.r)
	{
		return true;
	}
	return false;
}

bool CheckHitBCB(int col1ID, int col2ID) {
	return CheckHitBBC(col2ID, col1ID);
}
bool CheckHitBCB(int col1ID, int col2ID, D3DXVECTOR3 &contactPoint) {
	return CheckHitBBC(col2ID, col1ID, contactPoint);
}
//============================================================================
// OBBによる当たり判定処理
// サイズは半径 回転は考慮
// 戻り値：当たってたらtrue
//=============================================================================

bool CheckHitOBB(int col1ID, int col2ID)
{
	COLLIDER3D &col1 = g_Collider3D[col1ID], &col2 = g_Collider3D[col2ID];
	
	D3DXMATRIX RotateMtx1;
	D3DXMATRIX RotateMtx2;
	
	D3DXMatrixRotationYawPitchRoll(&RotateMtx1, col1.rot.y, col1.rot.x, col1.rot.z);
	D3DXMatrixRotationYawPitchRoll(&RotateMtx2, col2.rot.y, col2.rot.x, col2.rot.z);

	col1.normalDirect[0] = D3DXVECTOR3(RotateMtx1._11, RotateMtx1._12, RotateMtx1._13);
	col1.normalDirect[1] = D3DXVECTOR3(RotateMtx1._21, RotateMtx1._22, RotateMtx1._23);
	col1.normalDirect[2] = D3DXVECTOR3(RotateMtx1._31, RotateMtx1._32, RotateMtx1._33);
	col2.normalDirect[0] = D3DXVECTOR3(RotateMtx2._11, RotateMtx2._12, RotateMtx2._13);
	col2.normalDirect[1] = D3DXVECTOR3(RotateMtx2._21, RotateMtx2._22, RotateMtx2._23);
	col2.normalDirect[2] = D3DXVECTOR3(RotateMtx2._31, RotateMtx2._32, RotateMtx2._33);
	// 各方向ベクトルの確保
	// （N***:標準化方向ベクトル）

	D3DXVECTOR3 NAe1 = col1.normalDirect[0], Ae1 = NAe1 * col1.wx * 0.5f;
	D3DXVECTOR3 NAe2 = col1.normalDirect[1], Ae2 = NAe2 * col1.hy * 0.5f;
	D3DXVECTOR3 NAe3 = col1.normalDirect[2], Ae3 = NAe3 * col1.lz * 0.5f;
	D3DXVECTOR3 NBe1 = col2.normalDirect[0], Be1 = NBe1 * col2.wx * 0.5f;
	D3DXVECTOR3 NBe2 = col2.normalDirect[1], Be2 = NBe2 * col2.hy * 0.5f;
	D3DXVECTOR3 NBe3 = col2.normalDirect[2], Be3 = NBe3 * col2.lz * 0.5f;
	D3DXVECTOR3 Interval = col1.pos - col2.pos;

	// 分離軸 : Ae1
	FLOAT rA = D3DXVec3Length(&Ae1);
	FLOAT rB = LenSegOnSeparateAxis(&NAe1, &Be1, &Be2, &Be3);
	FLOAT L = fabs(D3DXVec3Dot(&Interval, &NAe1));
	if (L > rA + rB)
		return false; // 衝突していない

	 // 分離軸 : Ae2
	rA = D3DXVec3Length(&Ae2);
	rB = LenSegOnSeparateAxis(&NAe2, &Be1, &Be2, &Be3);
	L = fabs(D3DXVec3Dot(&Interval, &NAe2));
	if (L > rA + rB)
		return false;

	// 分離軸 : Ae3
	rA = D3DXVec3Length(&Ae3);
	rB = LenSegOnSeparateAxis(&NAe3, &Be1, &Be2, &Be3);
	L = fabs(D3DXVec3Dot(&Interval, &NAe3));
	if (L > rA + rB)
		return false;

	// 分離軸 : Be1
	rA = LenSegOnSeparateAxis(&NBe1, &Ae1, &Ae2, &Ae3);
	rB = D3DXVec3Length(&Be1);
	L = fabs(D3DXVec3Dot(&Interval, &NBe1));
	if (L > rA + rB)
		return false;

	// 分離軸 : Be2
	rA = LenSegOnSeparateAxis(&NBe2, &Ae1, &Ae2, &Ae3);
	rB = D3DXVec3Length(&Be2);
	L = fabs(D3DXVec3Dot(&Interval, &NBe2));
	if (L > rA + rB)
		return false;

	// 分離軸 : Be3
	rA = LenSegOnSeparateAxis(&NBe3, &Ae1, &Ae2, &Ae3);
	rB = D3DXVec3Length(&Be3);
	L = fabs(D3DXVec3Dot(&Interval, &NBe3));
	if (L > rA + rB)
		return false;

	// 分離軸 : C11
	D3DXVECTOR3 Cross;
	D3DXVec3Cross(&Cross, &NAe1, &NBe1);
	rA = LenSegOnSeparateAxis(&Cross, &Ae2, &Ae3);
	rB = LenSegOnSeparateAxis(&Cross, &Be2, &Be3);
	L = fabs(D3DXVec3Dot(&Interval, &Cross));
	if (L > rA + rB)
		return false;

	// 分離軸 : C12
	D3DXVec3Cross(&Cross, &NAe1, &NBe2);
	rA = LenSegOnSeparateAxis(&Cross, &Ae2, &Ae3);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be3);
	L = fabs(D3DXVec3Dot(&Interval, &Cross));
	if (L > rA + rB)
		return false;

	// 分離軸 : C13
	D3DXVec3Cross(&Cross, &NAe1, &NBe3);
	rA = LenSegOnSeparateAxis(&Cross, &Ae2, &Ae3);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be2);
	L = fabs(D3DXVec3Dot(&Interval, &Cross));
	if (L > rA + rB)
		return false;

	// 分離軸 : C21
	D3DXVec3Cross(&Cross, &NAe2, &NBe1);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae3);
	rB = LenSegOnSeparateAxis(&Cross, &Be2, &Be3);
	L = fabs(D3DXVec3Dot(&Interval, &Cross));
	if (L > rA + rB)
		return false;

	// 分離軸 : C22
	D3DXVec3Cross(&Cross, &NAe2, &NBe2);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae3);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be3);
	L = fabs(D3DXVec3Dot(&Interval, &Cross));
	if (L > rA + rB)
		return false;

	// 分離軸 : C23
	D3DXVec3Cross(&Cross, &NAe2, &NBe3);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae3);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be2);
	L = fabs(D3DXVec3Dot(&Interval, &Cross));
	if (L > rA + rB)
		return false;

	// 分離軸 : C31
	D3DXVec3Cross(&Cross, &NAe3, &NBe1);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae2);
	rB = LenSegOnSeparateAxis(&Cross, &Be2, &Be3);
	L = fabs(D3DXVec3Dot(&Interval, &Cross));
	if (L > rA + rB)
		return false;

	// 分離軸 : C32
	D3DXVec3Cross(&Cross, &NAe3, &NBe2);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae2);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be3);
	L = fabs(D3DXVec3Dot(&Interval, &Cross));
	if (L > rA + rB)
		return false;

	// 分離軸 : C33
	D3DXVec3Cross(&Cross, &NAe3, &NBe3);
	rA = LenSegOnSeparateAxis(&Cross, &Ae1, &Ae2);
	rB = LenSegOnSeparateAxis(&Cross, &Be1, &Be2);
	L = fabs(D3DXVec3Dot(&Interval, &Cross));
	if (L > rA + rB)
		return false;

	// 分離平面が存在しないので「衝突している」
	return true;
}

// 分離軸に投影された軸成分から投影線分長を算出
FLOAT LenSegOnSeparateAxis(D3DXVECTOR3 *Sep, D3DXVECTOR3 *e1, D3DXVECTOR3 *e2, D3DXVECTOR3 *e3)
{
	// 3つの内積の絶対値の和で投影線分長を計算
	// 分離軸Sepは標準化されていること
	FLOAT r1 = fabs(D3DXVec3Dot(Sep, e1));
	FLOAT r2 = fabs(D3DXVec3Dot(Sep, e2));
	FLOAT r3 = e3 ? (fabs(D3DXVec3Dot(Sep, e3))) : 0;
	return r1 + r2 + r3;
}
//                      rect         point
float CheckHitOBBToPoint(int col1ID, int col2ID, D3DXVECTOR3 *minPonit)
{
	COLLIDER3D &col1 = g_Collider3D[col1ID], &col2 = g_Collider3D[col2ID];
	D3DXVECTOR3 Vec(0, 0, 0);   // 最終的に長さを求めるベクトル

	D3DXMATRIX RotateMtx1;
	D3DXMatrixRotationYawPitchRoll(&RotateMtx1, col1.rot.y, col1.rot.x, col1.rot.z);
	col1.normalDirect[0] = D3DXVECTOR3(RotateMtx1._11, RotateMtx1._12, RotateMtx1._13);
	col1.normalDirect[1] = D3DXVECTOR3(RotateMtx1._21, RotateMtx1._22, RotateMtx1._23);
	col1.normalDirect[2] = D3DXVECTOR3(RotateMtx1._31, RotateMtx1._32, RotateMtx1._33);

	// 各軸についてはみ出た部分のベクトルを算出
	for (int i = 0; i < 3; i++)
	{
		FLOAT L;
		if(i == 0) L = col1.wx * 0.5f;
		if(i == 1) L = col1.hy * 0.5f;
		if(i == 2) L = col1.lz * 0.5f;
		
		if (L <= 0) continue;  // L=0は計算できない
		FLOAT s ;
		if(i == 0) s = D3DXVec3Dot(&(col2.pos - col1.pos), &col1.normalDirect[0]) / L;
		if(i == 1) s = D3DXVec3Dot(&(col2.pos - col1.pos), &col1.normalDirect[1]) / L;
		if(i == 2) s = D3DXVec3Dot(&(col2.pos - col1.pos), &col1.normalDirect[2]) / L;
		// sの値から、はみ出した部分があればそのベクトルを加算
		s = fabs(s);
		if (s > 1) {
			if(i == 0) Vec += (1 - s)*L*col1.normalDirect[0];   // はみ出した部分のベクトル算出
			if(i == 1) Vec += (1 - s)*L*col1.normalDirect[1];   // はみ出した部分のベクトル算出
			if(i == 2) Vec += (1 - s)*L*col1.normalDirect[2];   // はみ出した部分のベクトル算出
		}
	}
	*minPonit = Vec; // col1.pos + Vec
	//PrintDebugProc("D3DXVec3Length(&Vec):%f player R: %f\n", D3DXVec3Length(&Vec), col2.r);
	//PrintDebugProc("VecX:%f VecY:%f VecZ:%f\n", Vec.x, Vec.y, Vec.z);
	return D3DXVec3Length(&Vec);   // 長さ*長さを出力
}

bool CheckHitOBBReturnLen(int col1ID, int col2ID, D3DXVECTOR3 *Len)
{
	COLLIDER3D &col1 = g_Collider3D[col1ID], &col2 = g_Collider3D[col2ID];

	D3DXMATRIX RotateMtx1;
	D3DXMATRIX RotateMtx2;

	D3DXMatrixRotationYawPitchRoll(&RotateMtx1, col1.rot.y, col1.rot.x, col1.rot.z);
	D3DXMatrixRotationYawPitchRoll(&RotateMtx2, col2.rot.y, col2.rot.x, col2.rot.z);

	col1.normalDirect[0] = D3DXVECTOR3(RotateMtx1._11, RotateMtx1._12, RotateMtx1._13);
	col1.normalDirect[1] = D3DXVECTOR3(RotateMtx1._21, RotateMtx1._22, RotateMtx1._23);
	col1.normalDirect[2] = D3DXVECTOR3(RotateMtx1._31, RotateMtx1._32, RotateMtx1._33);
	col2.normalDirect[0] = D3DXVECTOR3(RotateMtx2._11, RotateMtx2._12, RotateMtx2._13);
	col2.normalDirect[1] = D3DXVECTOR3(RotateMtx2._21, RotateMtx2._22, RotateMtx2._23);
	col2.normalDirect[2] = D3DXVECTOR3(RotateMtx2._31, RotateMtx2._32, RotateMtx2._33);

	// 平面の法線に対するOBBの射影線の長さを算出
	FLOAT r = 0.0f;          // 近接距離
	D3DXVECTOR3 PlaneNormal; // 平面の法線ベクトル

	//================x
	//plane.GetNormal_W(&PlaneNormal);
	PlaneNormal = col2.normalDirect[0];
	int i;
	for (i = 0; i < 3; i++) {
		D3DXVECTOR3 Direct = col1.normalDirect[i]; // OBBの1つの軸ベクトル
		if(i == 0)r += fabs(D3DXVec3Dot(&(Direct * col1.wx*0.5f), &PlaneNormal));
		if(i == 1)r += fabs(D3DXVec3Dot(&(Direct * col1.hy*0.5f), &PlaneNormal));
		if(i == 2)r += fabs(D3DXVec3Dot(&(Direct * col1.lz*0.5f), &PlaneNormal));
	}

	// 平面とOBBの距離を算出
	D3DXVECTOR3 ObbPos = col1.pos;
	D3DXVECTOR3 PlanePos = col2.pos + (col2.wx * 0.5f * col2.normalDirect[0]);
	FLOAT s = D3DXVec3Dot(&(ObbPos - PlanePos), &PlaneNormal);

	// 戻し距離を算出
	if (Len != NULL) {
		if (s > 0)
			Len->x = r - fabs(s);
		else
			Len->x = r + fabs(s);
	}
	//================-x
	r = 0.0f;
	//plane.GetNormal_W(&PlaneNormal);
	PlaneNormal = -col2.normalDirect[0];
	for (i = 0; i < 3; i++) {
		D3DXVECTOR3 Direct = col1.normalDirect[i]; // OBBの1つの軸ベクトル
		if (i == 0)r += fabs(D3DXVec3Dot(&(Direct * col1.wx*0.5f), &PlaneNormal));
		if (i == 1)r += fabs(D3DXVec3Dot(&(Direct * col1.hy*0.5f), &PlaneNormal));
		if (i == 2)r += fabs(D3DXVec3Dot(&(Direct * col1.lz*0.5f), &PlaneNormal));
	}

	// 平面とOBBの距離を算出
	ObbPos = col1.pos;
	PlanePos = col2.pos + (col2.wx * -0.5f * col2.normalDirect[0]);
	s = D3DXVec3Dot(&(ObbPos - PlanePos), &PlaneNormal);

	// 戻し距離を算出
	if (Len != NULL) {
		if (s > 0)
			Len->x += -(r - fabs(s));
		else
			Len->x += -(r + fabs(s));
	}
	if(Len->x > 0)Len->x -= col2.wx + col1.wx;
	else if(Len->x < 0)Len->x += col2.wx + col1.wx;
	//================y
	r = 0.0f;
	//plane.GetNormal_W(&PlaneNormal);
	PlaneNormal = col2.normalDirect[1];
	for (i = 0; i < 3; i++) {
		D3DXVECTOR3 Direct = col1.normalDirect[i]; // OBBの1つの軸ベクトル
		if (i == 0)r += fabs(D3DXVec3Dot(&(Direct * col1.wx*0.5f), &PlaneNormal));
		if (i == 1)r += fabs(D3DXVec3Dot(&(Direct * col1.hy*0.5f), &PlaneNormal));
		if (i == 2)r += fabs(D3DXVec3Dot(&(Direct * col1.lz*0.5f), &PlaneNormal));
	}

	// 平面とOBBの距離を算出
	ObbPos = col1.pos;
	PlanePos = col2.pos + (col2.hy * 0.5f * col2.normalDirect[1]);
	s = D3DXVec3Dot(&(ObbPos - PlanePos), &PlaneNormal);

	// 戻し距離を算出
	if (Len != NULL) {
		if (s > 0)
			Len->y = r - fabs(s);
		else
			Len->y = r + fabs(s);
	}
	//================-y
	r = 0.0f;
	//plane.GetNormal_W(&PlaneNormal);
	PlaneNormal = -col2.normalDirect[1];
	for (i = 0; i < 3; i++) {
		D3DXVECTOR3 Direct = col1.normalDirect[i]; // OBBの1つの軸ベクトル
		if (i == 0)r += fabs(D3DXVec3Dot(&(Direct * col1.wx*0.5f), &PlaneNormal));
		if (i == 1)r += fabs(D3DXVec3Dot(&(Direct * col1.hy*0.5f), &PlaneNormal));
		if (i == 2)r += fabs(D3DXVec3Dot(&(Direct * col1.lz*0.5f), &PlaneNormal));
	}

	// 平面とOBBの距離を算出
	ObbPos = col1.pos;
	PlanePos = col2.pos + (col2.hy * -0.5f * col2.normalDirect[1]);
	s = D3DXVec3Dot(&(ObbPos - PlanePos), &PlaneNormal);

	// 戻し距離を算出
	if (Len != NULL) {
		if (s > 0)
			Len->y += -(r - fabs(s));
		else
			Len->y += -(r + fabs(s));
	}
	if(Len->y > 0)Len->y -= col2.hy + col1.hy;
	else if(Len->y < 0)Len->y += col2.hy + col1.hy;
	//================z
	r = 0.0f;
	//plane.GetNormal_W(&PlaneNormal);
	PlaneNormal = col2.normalDirect[2];
	for (i = 0; i < 3; i++) {
		D3DXVECTOR3 Direct = col1.normalDirect[i]; // OBBの1つの軸ベクトル
		if (i == 0)r += fabs(D3DXVec3Dot(&(Direct * col1.wx*0.5f), &PlaneNormal));
		if (i == 1)r += fabs(D3DXVec3Dot(&(Direct * col1.hy*0.5f), &PlaneNormal));
		if (i == 2)r += fabs(D3DXVec3Dot(&(Direct * col1.lz*0.5f), &PlaneNormal));
	}

	// 平面とOBBの距離を算出
	ObbPos = col1.pos;
	PlanePos = col2.pos + (col2.lz * 0.5f * col2.normalDirect[2]);
	s = D3DXVec3Dot(&(ObbPos - PlanePos), &PlaneNormal);

	// 戻し距離を算出
	if (Len != NULL) {
		if (s > 0)
			Len->z = r - fabs(s);
		else
			Len->z = r + fabs(s);
	}
	//================-z
	r = 0.0f;
	//plane.GetNormal_W(&PlaneNormal);
	PlaneNormal = -col2.normalDirect[2];
	for (i = 0; i < 3; i++) {
		D3DXVECTOR3 Direct = col1.normalDirect[i]; // OBBの1つの軸ベクトル
		if (i == 0)r += fabs(D3DXVec3Dot(&(Direct * col1.wx*0.5f), &PlaneNormal));
		if (i == 1)r += fabs(D3DXVec3Dot(&(Direct * col1.hy*0.5f), &PlaneNormal));
		if (i == 2)r += fabs(D3DXVec3Dot(&(Direct * col1.lz*0.5f), &PlaneNormal));
	}

	// 平面とOBBの距離を算出
	ObbPos = col1.pos;
	PlanePos = col2.pos + (col2.lz * -0.5f * col2.normalDirect[2]);
	s = D3DXVec3Dot(&(ObbPos - PlanePos), &PlaneNormal);

	// 戻し距離を算出
	if (Len != NULL) {
		if (s > 0)
			Len->z += -(r - fabs(s));
		else
			Len->z += -(r + fabs(s));
	}
	if (Len->z > 0)Len->z -= col2.lz + col1.lz;
	else if (Len->z < 0)Len->z += col2.lz + col1.lz;
	//==================
	// 衝突判定
	if (fabs(s) - r < 0.0f)
		return true; // 衝突している

	return false; // 衝突していない
}
//=============================================================================
// 当たり判定処理
//=============================================================================
// check by tag  
bool CheckHitByTag(int selfID, int tag)
{
	for (int i = 0; i < MAX_COLLIDER3D; i++) {
		if (g_Collider3D[i].Use && g_Collider3D[selfID].enable && selfID != i) {
			if (g_Collider3D[i].tag == tag) {
				if (g_Collider3D[selfID].type == Collider3DType_box && g_Collider3D[i].type == Collider3DType_box) {
					if (CheckHitBB(selfID, i)) {
						return true;
					}
				}
				else if (g_Collider3D[selfID].type == Collider3DType_circle && g_Collider3D[i].type == Collider3DType_circle) {
					if (CheckHitBC(selfID, i)) {
						return true;
					}
				}
				else if (g_Collider3D[selfID].type == Collider3DType_box && g_Collider3D[i].type == Collider3DType_circle) {
					if (CheckHitBBC(selfID, i)) {
						return true;
					}
				}
				else {
					if (CheckHitBCB(selfID, i)) {
						return true;
					}
				}

			}
		}
	}
	return false;
}

bool CheckHitByTagReturnMasterID(int selfID, int tag, int *TargetMasterID) {
	for (int i = 0; i < MAX_COLLIDER3D; i++) {
		if (g_Collider3D[i].Use && g_Collider3D[selfID].enable) {
			if (g_Collider3D[i].tag == tag) {
				if (g_Collider3D[selfID].type == Collider3DType_box && g_Collider3D[i].type == Collider3DType_box) {
					if (CheckHitBB(selfID, i)) {
						*TargetMasterID = g_Collider3D[i].masterID;
						return true;
					}
				}
				else if (g_Collider3D[selfID].type == Collider3DType_circle && g_Collider3D[i].type == Collider3DType_circle) {
					if (CheckHitBC(selfID, i)) {
						*TargetMasterID = g_Collider3D[i].masterID;
						return true;
					}
				}
				else if (g_Collider3D[selfID].type == Collider3DType_box && g_Collider3D[i].type == Collider3DType_circle) {
					if (CheckHitBBC(selfID, i)) {
						*TargetMasterID = g_Collider3D[i].masterID;
						return true;
					}
				}
				else {
					if (CheckHitBCB(selfID, i)) {
						*TargetMasterID = g_Collider3D[i].masterID;
						return true;
					}
				}

			}
		}
	}
	return false;
}

bool CheckHitByTagReturnCID(int selfID, int tag, int *TargetColliderID) {
	for (int i = 0; i < MAX_COLLIDER3D; i++) {
		if (g_Collider3D[i].Use && g_Collider3D[selfID].enable) {
			if (g_Collider3D[i].tag == tag) {
				if (g_Collider3D[selfID].type == Collider3DType_box && g_Collider3D[i].type == Collider3DType_box) {
					if (CheckHitBB(selfID, i)) {
						*TargetColliderID = i;
						return true;
					}
				}
				else if (g_Collider3D[selfID].type == Collider3DType_circle && g_Collider3D[i].type == Collider3DType_circle) {
					if (CheckHitBC(selfID, i)) {
						*TargetColliderID = i;
						return true;
					}
				}
				else if (g_Collider3D[selfID].type == Collider3DType_box && g_Collider3D[i].type == Collider3DType_circle) {
					if (CheckHitBBC(selfID, i)) {
						*TargetColliderID = i;
						return true;
					}
				}
				else {
					if (CheckHitBCB(selfID, i)) {
						*TargetColliderID = i;
						return true;
					}
				}

			}
		}
	}
	return false;
}

// check by ID
bool CheckHitByID(int selfID, int targetID)
{
	
	if (g_Collider3D[targetID].Use && g_Collider3D[selfID].enable && selfID != targetID) {
		if (g_Collider3D[selfID].type == Collider3DType_box && g_Collider3D[targetID].type == Collider3DType_box) {
			if (CheckHitBB(selfID, targetID)) {
				return true;
			}
		}
		else if (g_Collider3D[selfID].type == Collider3DType_circle && g_Collider3D[targetID].type == Collider3DType_circle) {
			if (CheckHitBC(selfID, targetID)) {
				return true;
			}
		}
		else if (g_Collider3D[selfID].type == Collider3DType_box && g_Collider3D[targetID].type == Collider3DType_circle) {
			if (CheckHitBBC(selfID, targetID)) {
				return true;
			}
		}
		else {
			if (CheckHitBCB(selfID, targetID)) {
				return true;
			}
		}

	}
	return false;
}

bool CheckHitByIDOBB(int selfID, int targetID)
{
	if (g_Collider3D[targetID].Use && g_Collider3D[selfID].enable) {
		if (CheckHitOBB(selfID, targetID)) {
			g_Collider3D[targetID].isCollision = true;
			return true;
		}
	}
	return false;
}

bool CheckHitByIDOBBToPoint(int selfID, int targetID, D3DXVECTOR3 *minPonit)
{
	if (g_Collider3D[targetID].Use && g_Collider3D[selfID].enable) {
		if (g_Collider3D[targetID].type == Collider3DType_box) {
			if(g_Collider3D[selfID].type == Collider3DType_box){
				if ((CheckHitOBBToPoint(targetID, selfID, minPonit) < g_Collider3D[selfID].wx * 0.6f)) { // collide
					return true;
				}
			}
			else if (g_Collider3D[selfID].type == Collider3DType_circle){
				if ((CheckHitOBBToPoint(targetID, selfID, minPonit) < g_Collider3D[selfID].r * 0.6f)) { // collide
					return true;
				}
			}
		}
			
	}
	return false;
}

bool CheckHitByTagOBBTEST(int selfID, int tag, int * TargetColliderID)
{
	for (int i = 0; i < MAX_COLLIDER3D; i++) {
		if (g_Collider3D[i].Use && g_Collider3D[selfID].enable) {
			if (g_Collider3D[i].tag == tag) {
				if (g_Collider3D[selfID].type == Collider3DType_box && g_Collider3D[i].type == Collider3DType_box) {
					if (CheckHitOBB(selfID, i)) {
						*TargetColliderID = i;
						g_Collider3D[i].isCollision = true;
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool CheckHitByTagOBBTESTReturnLen(int selfID, int tag, int * TargetColliderID, D3DXVECTOR3 *Len)
{
	for (int i = 0; i < MAX_COLLIDER3D; i++) {
		if (g_Collider3D[i].Use && g_Collider3D[selfID].enable) {
			if (g_Collider3D[i].tag == tag) {
				if (g_Collider3D[selfID].type == Collider3DType_box && g_Collider3D[i].type == Collider3DType_box) {
					if (CheckHitOBB(selfID, i)) {
						*TargetColliderID = i;
						g_Collider3D[i].isCollision = true;
						if (CheckHitOBBReturnLen(selfID, i, Len)) {
							// 彈出來距離
						}
						return true;
					}
				}
			}
		}
	}
	return false;
}

bool CheckHitByTagOBBPointReturnCID(int selfID, int tag, D3DXVECTOR3 *minPonit, int *TargetColliderID)
{
	for (int i = 0; i < MAX_COLLIDER3D; i++) {
		if (g_Collider3D[i].Use && g_Collider3D[selfID].enable) {
			if (g_Collider3D[i].tag == tag) {
				if (g_Collider3D[i].type == Collider3DType_box) {
					if(g_Collider3D[selfID].type == Collider3DType_box){
						if ((CheckHitOBBToPoint(i, selfID, minPonit) < g_Collider3D[selfID].wx * 0.6f)) { // collide
							*TargetColliderID = i;
							return true;
						}
					}
					else if (g_Collider3D[selfID].type == Collider3DType_circle){
						if ((CheckHitOBBToPoint(i, selfID, minPonit) < g_Collider3D[selfID].r * 0.6f)) { // collide
							*TargetColliderID = i;
							return true;
						}
					}
				}
			}
		}
	}
	return false;
}

bool RayHitPlat(D3DXVECTOR3 pos, int TargetCID, D3DXVECTOR3 * HitPosition, D3DXVECTOR3 * Normal)
{
	PLATFORM *plat = GetPlatform();
	int targetMID = g_Collider3D[TargetCID].masterID;
	D3DXVECTOR3 start = pos;
	D3DXVECTOR3 end = pos;
	D3DXVECTOR3 HitPositionTemp;
	D3DXVECTOR3 NormalTemp;

	D3DXMATRIX  mtxScl, mtxRot, mtxTranslate, mtxWorld;
	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&mtxWorld);
	// スケールを反映
	D3DXMatrixScaling(&mtxScl, plat[targetMID].scl.x, plat[targetMID].scl.y, plat[targetMID].scl.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxScl);

	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, plat[targetMID].rot.y, plat[targetMID].rot.x, plat[targetMID].rot.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

	// 移動を反映
	if (plat[targetMID].canMove) {
		D3DXMatrixTranslation(&mtxTranslate, plat[targetMID].CaculatedPos.x, plat[targetMID].CaculatedPos.y, plat[targetMID].CaculatedPos.z);
	}
	else {
		D3DXMatrixTranslation(&mtxTranslate, plat[targetMID].pos.x, plat[targetMID].pos.y, plat[targetMID].pos.z);
	}
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTranslate);

	// 少し上から、ズドーンと下へレイを飛ばす
	//start.y += 50.0f;
	end.y -= 1000.0f;
	int ansI = -1;
	float NDis = 0.0f;
	bool firstFindNDis = false;
	//PrintDebugProc("TargetNum: %d IndexNum: %d VertexNum: %d\n", targetMID, plat[targetMID].model.IndexNum, plat[targetMID].model.VertexNum);
	for (int i = 0; i < plat[targetMID].model.IndexNum; i += 3) {
		D3DXVECTOR3 p0, p1, p2;
		p0 = plat[targetMID].model.Vertexlist[plat[targetMID].model.Indexlist[i]];
		p1 = plat[targetMID].model.Vertexlist[plat[targetMID].model.Indexlist[i + 1]];
		p2 = plat[targetMID].model.Vertexlist[plat[targetMID].model.Indexlist[i + 2]];
		
		D3DXVec3TransformCoord(&p0,&p0,&mtxWorld);
		D3DXVec3TransformCoord(&p1,&p1,&mtxWorld);
		D3DXVec3TransformCoord(&p2,&p2,&mtxWorld);
		bool ans = RayCast(p0, p2, p1, start, end, &HitPositionTemp, &NormalTemp);
		if (ans) {
			// Find nearest
			float dis = D3DXVec3LengthSq(&(HitPositionTemp - pos));
			if (!firstFindNDis) {
				firstFindNDis = true;
				NDis = dis;
				ansI = i;
				*HitPosition = HitPositionTemp;
				*Normal = NormalTemp;
			}
			if (NDis > dis) {
				NDis = dis;
				ansI = i;
				*HitPosition = HitPositionTemp;
				*Normal = NormalTemp;
			}
		}
	}

	// return nearest
	if (ansI != -1) {
		if (Normal->y > 0.5f) {
			//PrintDebugProc("PlatGround: hitposX: %f hitposY: %f hitposZ: %f\n", HitPosition->x, HitPosition->y, HitPosition->z);
			//PrintDebugProc("PlatGround: normalX: %f normalY: %f normalZ: %f\n", Normal->x, Normal->y, Normal->z);
			//PrintDebugProc("i: %d\n", ansI);
		}
		return true;
	}

	return false;
}

bool RayHitPlatWall(D3DXVECTOR3 pos, int TargetCID, D3DXVECTOR3 forwardXY, D3DXVECTOR3 * HitPosition, D3DXVECTOR3 * Normal)
{
	PLATFORM *plat = GetPlatform();
	int targetMID = g_Collider3D[TargetCID].masterID;
	forwardXY.y = 0.0f;
	D3DXVec3Normalize(&forwardXY, &forwardXY);
	D3DXVECTOR3 start = pos;
	D3DXVECTOR3 end = pos;
	D3DXVECTOR3 HitPositionTemp;
	D3DXVECTOR3 NormalTemp;

		D3DXMATRIX  mtxScl, mtxRot, mtxTranslate, mtxWorld;
	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&mtxWorld);
	// スケールを反映
	D3DXMatrixScaling(&mtxScl, plat[targetMID].scl.x, plat[targetMID].scl.y, plat[targetMID].scl.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxScl);

	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, plat[targetMID].rot.y, plat[targetMID].rot.x, plat[targetMID].rot.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

	// 移動を反映
	if (plat[targetMID].canMove) {
		D3DXMatrixTranslation(&mtxTranslate, plat[targetMID].CaculatedPos.x, plat[targetMID].CaculatedPos.y, plat[targetMID].CaculatedPos.z);
	}
	else {
		D3DXMatrixTranslation(&mtxTranslate, plat[targetMID].pos.x, plat[targetMID].pos.y, plat[targetMID].pos.z);
	}
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTranslate);

	// 少し上から、ズドーンと下へレイを飛ばす
	//start -= forwardXY * 50.0f;
	end += forwardXY * 1000.0f;
	int ansI = -1;
	float NDis = 0.0f;
	bool firstFindNDis = false;
	for (int i = 0; i < plat[targetMID].model.IndexNum; i += 3) {
		D3DXVECTOR3 p0, p1, p2;
		p0 = plat[targetMID].model.Vertexlist[plat[targetMID].model.Indexlist[i]];
		p1 = plat[targetMID].model.Vertexlist[plat[targetMID].model.Indexlist[i + 1]];
		p2 = plat[targetMID].model.Vertexlist[plat[targetMID].model.Indexlist[i + 2]];

		D3DXVec3TransformCoord(&p0, &p0, &mtxWorld);
		D3DXVec3TransformCoord(&p1, &p1, &mtxWorld);
		D3DXVec3TransformCoord(&p2, &p2, &mtxWorld);
		bool ans = RayCast(p0, p2, p1, start, end, &HitPositionTemp, &NormalTemp);
		if (ans) {
			// Find nearest
			float dis = D3DXVec3LengthSq(&(HitPositionTemp - pos));
			if (!firstFindNDis) {
				firstFindNDis = true;
				NDis = dis;
				ansI = i;
				*HitPosition = HitPositionTemp;
				*Normal = NormalTemp;
			}
			if (NDis > dis) {
				NDis = dis;
				ansI = i;
				*HitPosition = HitPositionTemp;
				*Normal = NormalTemp;
			}
		}
	}

	// return nearest
	if (ansI != -1) {
		if (fabsf(Normal->y) <= 0.5f) {
			//PrintDebugProc("PlatWall: hitposX: %f hitposY: %f hitposZ: %f\n", HitPosition->x, HitPosition->y, HitPosition->z);
			//PrintDebugProc("PlatWall: normalX: %f normalY: %f normalZ: %f\n", Normal->x, Normal->y, Normal->z);
			//PrintDebugProc("i: %d\n", ansI);
		}
		return true;
	}

	return false;
}

bool RayHitGround(D3DXVECTOR3 pos, D3DXVECTOR3 * HitPosition, D3DXVECTOR3 * Normal ,int id)
{
	D3DXVECTOR3 start = pos;
	D3DXVECTOR3 end = pos;
	D3DXVECTOR3 HitPositionTemp;
	D3DXVECTOR3 NormalTemp;

	D3DXVECTOR3 *caculatedNormal = NULL;
	caculatedNormal = getGroundNormal(id);
	DX11_MODEL model = getGroundModel(id);

	D3DXMATRIX  mtxScl, mtxRot, mtxTranslate, mtxWorld;
	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&mtxWorld);
	// スケールを反映
	D3DXMatrixScaling(&mtxScl, 20.0f, 20.0f, -20.0f);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxScl);

	// 回転を反映
	//D3DXMatrixRotationYawPitchRoll(&mtxRot, 0.0f, 0.0f, 0.0f);
	//D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

	// 移動を反映
	//D3DXMatrixTranslation(&mtxTranslate, 0.0f, 0.0f, 0.0f);
	//D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTranslate);

	// 少し上から、ズドーンと下へレイを飛ばす
	//start.y += 100.0f;
	end.y -= 1000.0f;
	int ansI = -1;
	float NDis = 0.0f;
	bool firstFindNDis = false;
	PrintDebugProc("Ground: IndexNum: %d VertexNum: %d\n", model.IndexNum, model.VertexNum);
	for (int i = 0; i < model.IndexNum; i += 3) {
		D3DXVECTOR3 p0, p1, p2;
		p0 = model.Vertexlist[model.Indexlist[i]];
		p1 = model.Vertexlist[model.Indexlist[i + 1]];
		p2 = model.Vertexlist[model.Indexlist[i + 2]];

		D3DXVec3TransformCoord(&p0, &p0, &mtxWorld);
		D3DXVec3TransformCoord(&p1, &p1, &mtxWorld);
		D3DXVec3TransformCoord(&p2, &p2, &mtxWorld);
		
		bool ans = RayCast(p0, p1, p2, start, end, &HitPositionTemp, &NormalTemp,caculatedNormal[i]);
		if (ans) {
			// Find nearest
			float dis = D3DXVec3LengthSq(&(HitPositionTemp - pos));
			if (!firstFindNDis) {
				firstFindNDis = true;
				NDis = dis;
				ansI = i;
				*HitPosition = HitPositionTemp;
				*Normal = NormalTemp;
			}
			if (NDis > dis) {
				NDis = dis;
				ansI = i;
				*HitPosition = HitPositionTemp;
				*Normal = NormalTemp;
			}
			return true;
		}
	}

	// return nearest
	if (ansI != -1) {
		if (fabsf(Normal->y) > 0.5f) {
			//PrintDebugProc("Ground: hitposX: %f hitposY: %f hitposZ: %f\n", HitPosition->x, HitPosition->y, HitPosition->z);
			//PrintDebugProc("Ground: normalX: %f normalY: %f normalZ: %f\n", Normal->x, Normal->y, Normal->z);
			//PrintDebugProc("i: %d\n", ansI);
		}
		return true;
	}

	return false;
}

bool RayHitGroundWall(D3DXVECTOR3 pos, D3DXVECTOR3 forwardXY, D3DXVECTOR3 * HitPosition, D3DXVECTOR3 * Normal, int id)
{
	forwardXY.y = 0.0f;
	D3DXVec3Normalize(&forwardXY, &forwardXY);
	D3DXVECTOR3 start = pos;
	D3DXVECTOR3 end = pos;
	D3DXVECTOR3 HitPositionTemp;
	D3DXVECTOR3 NormalTemp;

	D3DXVECTOR3 *caculatedNormal = NULL;
	caculatedNormal = getGroundNormal(id);
	DX11_MODEL model = getGroundModel(id);

	D3DXMATRIX  mtxScl, mtxRot, mtxTranslate, mtxWorld;
	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&mtxWorld);
	// スケールを反映
	D3DXMatrixScaling(&mtxScl, 20.0f, 20.0f, -20.0f);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxScl);

	// 回転を反映
	//D3DXMatrixRotationYawPitchRoll(&mtxRot, 0.0f, 0.0f, 0.0f);
	//D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

	// 移動を反映
	//D3DXMatrixTranslation(&mtxTranslate, 0.0f, 0.0f, 0.0f);
	//D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTranslate);

	// 少し上から、ズドーンと下へレイを飛ばす
	//start -= forwardXY * 50.0f;
	end += forwardXY * 1000.0f;
	int ansI = -1;
	float NDis = 0.0f;
	bool firstFindNDis = false;
	for (int i = 0; i < model.IndexNum; i += 3) {
		D3DXVECTOR3 p0, p1, p2;
		p0 = model.Vertexlist[model.Indexlist[i]];
		p1 = model.Vertexlist[model.Indexlist[i + 1]];
		p2 = model.Vertexlist[model.Indexlist[i + 2]];


		D3DXVec3TransformCoord(&p0, &p0, &mtxWorld);
		D3DXVec3TransformCoord(&p1, &p1, &mtxWorld);
		D3DXVec3TransformCoord(&p2, &p2, &mtxWorld);

		bool ans = RayCast(p0, p1, p2, start, end, &HitPositionTemp, &NormalTemp, caculatedNormal[i]);
		if (ans) {
			// Find nearest
			float dis = D3DXVec3LengthSq(&(HitPositionTemp - pos));
			if (!firstFindNDis) {
				firstFindNDis = true;
				NDis = dis;
				ansI = i;
				*HitPosition = HitPositionTemp;
				*Normal = NormalTemp;
			}
			if (NDis > dis) {
				NDis = dis;
				ansI = i;
				*HitPosition = HitPositionTemp;
				*Normal = NormalTemp;
			}
			//return true;
		}
	}

	// return nearest
	if (ansI != -1 ) {
		if (fabsf(Normal->y) <= 0.5f) {
			//PrintDebugProc("Wall: hitposX: %f hitposY: %f hitposZ: %f\n", HitPosition->x, HitPosition->y, HitPosition->z);
			//PrintDebugProc("Wall: normalX: %f normalY: %f normalZ: %f\n", Normal->x, Normal->y, Normal->z);
			//PrintDebugProc("i: %d\n", ansI);
		}
		return true;
	}

	return false;
}

bool RayCast(D3DXVECTOR3 p0, D3DXVECTOR3 p1, D3DXVECTOR3 p2, D3DXVECTOR3 pos0, D3DXVECTOR3 pos1, D3DXVECTOR3 * hit, D3DXVECTOR3 *normal ,D3DXVECTOR3 caculatedNormal)
{
	D3DXVECTOR3		nor;		// ポリゴンの法線
	D3DXVECTOR3		vec1;
	D3DXVECTOR3		vec2;
	float			d1, d2;

	if(caculatedNormal.x == -100.0f)
	{	// ポリゴンの外積をとって法線を求める(固定物なら予めInit()で行っておくと良い)
		vec1 = p1 - p0;
		vec2 = p2 - p0;
		crossProduct(&nor, &vec2, &vec1);
		D3DXVec3Normalize(&nor, &nor);		// 計算しやすいように法線をノーマルイズしておく
		*normal = nor;						// 求めた法線を入れておく
		if (nor.x == 0.0f && nor.y == 0.0f &&nor.z == 0.0f) return false; // 平行
	}
	else 
	{
		vec1 = p1 - p0;
		vec2 = p2 - p0;
		nor = caculatedNormal;
		*normal = nor;						// 求めた法線を入れておく
		if (nor.x == 0.0f && nor.y == 0.0f &&nor.z == 0.0f) return false; // 平行
	}

	// ポリゴン平面と線分の内積とって衝突している可能性を調べる（鋭角なら＋、鈍角ならー、直角なら０）
	vec1 = pos0 - p0;
	vec2 = pos1 - p0;
	{	// 求めたポリゴンの法線と２つのベクト?（線分の両端とポリゴン上の任意の点）の内積とって衝突している可能性を調べる
		d1 = dotProduct(&vec1, &nor);
		d2 = dotProduct(&vec2, &nor);
		if ((d1 * d2) > 0.0f)
		{
			// 当たっている可能性は無い
			return(false);
		}
	}


	{	// ポリゴンと線分の交点を求める
		d1 = (float)fabs(d1);
		d2 = (float)fabs(d2);
		float a = d1 / (d1 + d2);							// 内分比

		D3DXVECTOR3		vec3 = (1 - a) * vec1 + a * vec2;	// p0から交点へのベクト?
		D3DXVECTOR3		p3 = p0 + vec3;						// 交点
		*hit = p3;											// 求めた交点を入れておく
		{	// 求めた交点がポリゴンの中にあるか調べる

			// ポリゴンの各辺のベクト?
			D3DXVECTOR3		v1 = p1 - p0;
			D3DXVECTOR3		v2 = p2 - p1;
			D3DXVECTOR3		v3 = p0 - p2;

			// 各頂点と交点とのベクト?
			D3DXVECTOR3		v4 = p3 - p1;
			D3DXVECTOR3		v5 = p3 - p2;
			D3DXVECTOR3		v6 = p3 - p0;

			// 外積で各辺の法線を求めて、ポリゴンの法線との内積をとって??をチェックする
			D3DXVECTOR3		n1, n2, n3;

			crossProduct(&n1, &v4, &v1);
			if (dotProduct(&n1, &nor) < 0.0f) return(false); // 当たっていない

			crossProduct(&n2, &v5, &v2);
			if (dotProduct(&n2, &nor) < 0.0f) return(false); // 当たっていない

			crossProduct(&n3, &v6, &v3);
			if (dotProduct(&n3, &nor) < 0.0f) return(false); // 当たっていない
		}
	}


	return(true);	// 当たっている！(hitには当たっている交点が入っている。normalには法線が入っている)
}
//=============================================================================
// 情報を取得
//=============================================================================
COLLIDER3D *GetCollider(void)
{
	return &g_Collider3D[0];
}

void SetCollider3DEnable(int id, bool b)
{
	g_Collider3D[id].enable = b;
}
