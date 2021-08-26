//=============================================================================
//
// ground処理 [ground.cpp]
// Author : GP11B132 14 ゴショウケン
//
//=============================================================================
#include "ground.h"
#include "Collider3D.h"
#include "player.h"
#include "input.h"
#include "debugproc.h"
#include "camera.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_MODEL		(11)					            // モデルの数
#define	MODEL_GROUND_TEST       	"data/MODEL/Test.obj"		       
//#define	MODEL_GROUND_TEST       	"data/MODEL/map.obj"		       
//#define	MODEL_GROUND_TEST       	"data/MODEL/stage.obj"	

#define	MODEL_GROUND_HEARTWORLD  	"data/MODEL/HeartWorld.obj"		       
#define	MODEL_GROUND_HEARTWORLD_D  	"data/MODEL/HeartWorld_D.obj"		       
#define	MODEL_GROUND_HtoW        	"data/MODEL/HtoW.obj"		       
#define	MODEL_GROUND_HtoW_D        	"data/MODEL/HtoW_D.obj"		       
#define	MODEL_GROUND_WONDERLAND     "data/MODEL/WonderLand.obj"		
#define	MODEL_GROUND_WONDERLAND2    "data/MODEL/WonderLand2.obj"		
#define	MODEL_GROUND_WONDERLAND_D	"data/MODEL/WonderLand_D.obj"		
#define	MODEL_GROUND_WtoS           "data/MODEL/WtoS.obj"		
#define	MODEL_GROUND_SANDWORLD	    "data/MODEL/SandWorld.obj"		       
#define	MODEL_GROUND_SANDWORLD_D    "data/MODEL/SandWorld_D.obj"		       
#define	MODEL_GROUND_StoS        	"data/MODEL/StoS.obj"		       
#define	MODEL_GROUND_SNOWWORLD      "data/MODEL/SnowWorld.obj"		       
#define	MODEL_SKY    	"data/MODEL/sky.obj"		                       
#define	MODEL_SKY2    	"data/MODEL/sky2.obj"		                       
#define	MODEL_SKY3    	"data/MODEL/sky3.obj"		                       

#define SCALE_X         (10)
#define SCALE_Y         (10)
#define SCALE_Z         (-10)
#define Move_X          (0)
#define Move_Y          (0)
#define Move_Z          (0)

#define VIEW_ROTX_MAX   (D3DX_PI * 0.45f)
#define VIEW_ROTX_MIN   (-D3DX_PI * 0.45f)

#define WORLDROTATEMAG		(0.0003f)
//*****************************************************************************
// グローバル変数
//*****************************************************************************
static DX11_MODEL			g_Model[MAX_MODEL];						// モデル情報

D3DXVECTOR3 worldRot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
D3DXVECTOR3 deltaRot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

float skyRot = 0.0f;
float a = 0.0f;
int lvvvv = 0;

//GroundMesh Normal 
D3DXVECTOR3 *groundNormal_HeartWorld;
D3DXVECTOR3 *groundNormal_HtoW;
D3DXVECTOR3 *groundNormal_WonderLand;
D3DXVECTOR3 *groundNormal_WonderLand2;
D3DXVECTOR3 *groundNormal_WtoS;
D3DXVECTOR3 *groundNormal_SandWorld;
D3DXVECTOR3 *groundNormal_SandWorld_D;
D3DXVECTOR3 *groundNormal_StoS;
D3DXVECTOR3 *groundNormal_SnowWorld;

void InitNormals();

HRESULT InitGround(void)
{
	LoadModel(MODEL_GROUND_TEST, &g_Model[GroundType_Test]);
	//LoadModel(MODEL_GROUND_HEARTWORLD, &g_Model[GroundType_HeartWorld]);
	//LoadModel(MODEL_GROUND_HEARTWORLD_D, &g_Model[GroundType_HeartWorld_D]);
	//LoadModel(MODEL_GROUND_HtoW, &g_Model[GroundType_HtoW]);
	//LoadModel(MODEL_GROUND_HtoW_D, &g_Model[GroundType_HtoW_D]);
	//LoadModel(MODEL_GROUND_WONDERLAND, &g_Model[GroundType_Wonderland]);
	//LoadModel(MODEL_GROUND_WONDERLAND2, &g_Model[GroundType_Wonderland2]);
	//LoadModel(MODEL_GROUND_WONDERLAND_D, &g_Model[GroundType_Wonderland_D]);
	//LoadModel(MODEL_GROUND_WtoS, &g_Model[GroundType_WtoS]);
	//LoadModel(MODEL_GROUND_SANDWORLD, &g_Model[GroundType_SandWorld]);
	//LoadModel(MODEL_GROUND_SANDWORLD_D, &g_Model[GroundType_SandWorld_D]);
	//LoadModel(MODEL_GROUND_StoS, &g_Model[GroundType_StoS]);
	//LoadModel(MODEL_GROUND_SNOWWORLD, &g_Model[GroundType_SnowWorld]);
	LoadModel(MODEL_SKY, &g_Model[GroundType_Sky1]);
	LoadModel(MODEL_SKY2, &g_Model[GroundType_Sky2]);
	LoadModel(MODEL_SKY3, &g_Model[GroundType_Sky3]);

	InitNormals();
	
	return S_OK;
}

void UninitGround(void)
{
	// モデルの解放処理
	for (int i = 0; i < MAX_MODEL; i++) {
		UnloadModel(&g_Model[i]);
	}

	if (groundNormal_HeartWorld)  { delete[] groundNormal_HeartWorld;  groundNormal_HeartWorld  = NULL; }
	if (groundNormal_HtoW)        { delete[] groundNormal_HtoW;        groundNormal_HtoW        = NULL; }
	if (groundNormal_WonderLand)  { delete[] groundNormal_WonderLand;  groundNormal_WonderLand  = NULL; }
	if (groundNormal_WonderLand2) { delete[] groundNormal_WonderLand2; groundNormal_WonderLand2 = NULL; }
	if (groundNormal_WtoS)        { delete[] groundNormal_WtoS;        groundNormal_WtoS        = NULL; }
	if (groundNormal_SandWorld)   { delete[] groundNormal_SandWorld;   groundNormal_SandWorld   = NULL; }
	if (groundNormal_SandWorld_D) { delete[] groundNormal_SandWorld_D; groundNormal_SandWorld_D = NULL; }
	if (groundNormal_StoS)        { delete[] groundNormal_StoS;        groundNormal_StoS        = NULL; }
	if (groundNormal_SnowWorld)   { delete[] groundNormal_SnowWorld;   groundNormal_SnowWorld   = NULL; }

}

void UpdateGround(void)
{
	PrintDebugProc("\n\n worldRotX:%f, worldRotX:%f, worldRotX:%f\n\n", worldRot.x/ D3DX_PI, worldRot.y/ D3DX_PI, worldRot.z/ D3DX_PI);

	bool wrm = GetWorldRotateMode();

	bool isRoted = false;

	//TEST rot
	/*if (GetKeyboardTrigger(DIK_UP)) {
		deltaRot.y += 0.5f * D3DX_PI;
		isRoted = true;
	}
	else if (GetKeyboardTrigger(DIK_DOWN)) {
		deltaRot.y -= 0.5f * D3DX_PI;
		isRoted = true;
	}
	else if (GetKeyboardTrigger(DIK_LEFT)) {
		deltaRot.x -= 0.5f * D3DX_PI;
		isRoted = true;
	}
	else if (GetKeyboardTrigger(DIK_RIGHT)) {
		deltaRot.x += 0.5f * D3DX_PI;
		isRoted = true;
	}
	else if (GetKeyboardTrigger(DIK_C)) {
		deltaRot.z += 0.5f * D3DX_PI;
		isRoted = true;
	}
	else if (GetKeyboardTrigger(DIK_V)) {
		deltaRot.z -= 0.5f * D3DX_PI;
		isRoted = true;
	}*/

	if (wrm)
	{
		if (IsMouseCenterPressed())
		{
			//ShowCursor(true);
			float dx = GetMousedX();
			float dy = GetMousedY();
			deltaRot.y -= dy * WORLDROTATEMAG;
			deltaRot.x -= dx * WORLDROTATEMAG;
			
			isRoted = true;
			if (deltaRot.y < -D3DX_PI) deltaRot.y += D3DX_PI * 2.0f;
			if (deltaRot.y > D3DX_PI)  deltaRot.y -= D3DX_PI * 2.0f;
			if (deltaRot.x < -D3DX_PI) deltaRot.x += D3DX_PI * 2.0f;
			if (deltaRot.x > D3DX_PI)  deltaRot.x -= D3DX_PI * 2.0f;
			PrintDebugProc("\n\n deltaRot.x:%f, deltaRot.y:%f ,deltaRot.z:%f\n\n", deltaRot.x, deltaRot.y, deltaRot.z);
		}
	}

	if (isRoted) {

		worldRot += deltaRot;
		if (worldRot.y < -D3DX_PI) worldRot.y += D3DX_PI * 2.0f;
		if (worldRot.y > D3DX_PI)  worldRot.y -= D3DX_PI * 2.0f;
		if (worldRot.x < -D3DX_PI) worldRot.x += D3DX_PI * 2.0f;
		if (worldRot.x > D3DX_PI)  worldRot.x -= D3DX_PI * 2.0f;

		//auto temp = worldRot + deltaRot;
		D3DXMATRIX mtxRot, mtxTranslate;
		D3DXMATRIX mtxWorld;
		
		D3DXVECTOR3 playerOriPos = GetPlayer()->pos;

		// 移動を反映
		D3DXMatrixTranslation(&mtxTranslate, playerOriPos.x, playerOriPos.y, playerOriPos.z); // have to put the model at (0,0,0)
		D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTranslate);

		// 回転を反映
		D3DXMatrixRotationYawPitchRoll(&mtxRot, deltaRot.x, deltaRot.y, deltaRot.z);
		D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

		
		if (!GetWorldRotateMode())
		{
			D3DXVECTOR3 playerNewPos;
			D3DXVec3TransformCoord(&playerNewPos, &playerNewPos, &mtxWorld);
			setPlayerPosition(playerNewPos);
		}
		
	}
}

void DrawGround(void)
{
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;
	D3DXMATRIX mtxWorld;
	int debugCtrlCount = 0; // reset SelectShineTimer

	// カリング無効
	SetCullingMode(CULL_MODE_NONE);

	//=================Ground
	SetLightEnable(false);
	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&mtxWorld);

	// スケールを反映
	D3DXMatrixScaling(&mtxScl, SCALE_X, SCALE_Y, SCALE_Z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxScl);

	// 回転を反映
	//auto temp = worldRot + deltaRot;
	D3DXMatrixRotationYawPitchRoll(&mtxRot, worldRot.x, worldRot.y, worldRot.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

	// 移動を反映
	//D3DXMatrixTranslation(&mtxTranslate, Move_X, Move_Y, Move_Z);
	//D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTranslate);

	// ワールドマトリックスの設定
	SetWorldMatrix(&mtxWorld);

	// モデル描画
	DrawModel(&g_Model[GroundType_Test]);
	//DrawModel(&g_Model[GroundType_HeartWorld]);
	//DrawModel(&g_Model[GroundType_HeartWorld_D]);
	//DrawModel(&g_Model[GroundType_HtoW]);
	//DrawModel(&g_Model[GroundType_HtoW_D]);
	//DrawModel(&g_Model[GroundType_Wonderland]);
	//DrawModel(&g_Model[GroundType_Wonderland2]);
	//DrawModel(&g_Model[GroundType_Wonderland_D]);
	//DrawModel(&g_Model[GroundType_WtoS]);
	//DrawModel(&g_Model[GroundType_SandWorld]);
	//DrawModel(&g_Model[GroundType_SandWorld_D]);
	//DrawModel(&g_Model[GroundType_StoS]);
	//DrawModel(&g_Model[GroundType_SnowWorld]);

	SetLightEnable(true);

	//=================SKY SPHERE
	SetLightEnable(false);
	SetFogEnable(false);
	skyRot += 0.0005f * D3DX_PI;
	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&mtxWorld);

	// スケールを反映
	D3DXMatrixScaling(&mtxScl, 4000, 4000, 4000);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxScl);

	// 回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, skyRot, 0.0f, 0.0f);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

	// 移動を反映
	PLAYER *player = GetPlayer();
	D3DXMatrixTranslation(&mtxTranslate, player->pos.x, 0.0f, player->pos.z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTranslate);

	// ワールドマトリックスの設定
	SetWorldMatrix(&mtxWorld);

	float p = 0.0f;
	float np = 0.0f;
	a += 1;
	if (a < 2000.0f) {
		if (lvvvv == 0) {
			p = 0.5f - a / 2000.0f;
			np = 0.0f + a / 2000.0f * 0.8f;
			g_Model[GroundType_Sky1].SubsetArray->Material.Material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, p);
			g_Model[GroundType_Sky2].SubsetArray->Material.Material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, np);
			g_Model[GroundType_Sky3].SubsetArray->Material.Material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
		}
		if (lvvvv == 1) {
			p = 0.8f - a / 2000.0f;
			np = 0.0f + a / 2000.0f * 0.7f;
			g_Model[GroundType_Sky1].SubsetArray->Material.Material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
			g_Model[GroundType_Sky2].SubsetArray->Material.Material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, p);
			g_Model[GroundType_Sky3].SubsetArray->Material.Material.Diffuse = D3DXCOLOR(np, np, np, np);
		}
		if (lvvvv == 2) {
			p = 0.7f - a / 2000.0f;
			np = 0.0f + a / 2000.0f * 0.5f;
			g_Model[GroundType_Sky1].SubsetArray->Material.Material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, np);
			g_Model[GroundType_Sky2].SubsetArray->Material.Material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.0f);
			g_Model[GroundType_Sky3].SubsetArray->Material.Material.Diffuse = D3DXCOLOR(p, p, p, p);
		}
	}
	else {
		a = 0.0f;
		lvvvv += 1;
		if (lvvvv == 3)lvvvv = 0;
	}
	//g_Model[2].SubsetArray->Material.Material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.5f);
	//g_Model[3].SubsetArray->Material.Material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 0.8f);
	//g_Model[4].SubsetArray->Material.Material.Diffuse = D3DXCOLOR(0.7f, 0.7f, 0.7f, 0.7f);
	// モデル描画
	DrawModel(&g_Model[GroundType_Sky1]);
	DrawModel(&g_Model[GroundType_Sky2]);
	DrawModel(&g_Model[GroundType_Sky3]);

	SetFogEnable(true);
	SetLightEnable(true);
	//=================
	// カリング設定を戻す
	SetCullingMode(CULL_MODE_BACK);
}

DX11_MODEL getGroundModel(int id)
{
	return g_Model[id];
}

D3DXVECTOR3 *getGroundNormal(int id)
{
	if (id == GroundType_HeartWorld)    return &groundNormal_HeartWorld[0];
	if (id == GroundType_HtoW)	        return &groundNormal_HtoW[0];
	if(id == GroundType_Wonderland)	    return &groundNormal_WonderLand[0];
	if(id == GroundType_Wonderland2)	return &groundNormal_WonderLand2[0];
	if(id == GroundType_WtoS)	        return &groundNormal_WtoS[0];
	if(id == GroundType_SandWorld)	    return &groundNormal_SandWorld[0];
	if(id == GroundType_SandWorld_D)	return &groundNormal_SandWorld_D[0];
	if(id == GroundType_StoS)	        return &groundNormal_StoS[0];
	if(id == GroundType_SnowWorld)	    return &groundNormal_SnowWorld[0];
	
}

float returnWorldRot(int i)
{
	if (i == 1)	return worldRot.x;
	if (i == 2)	return worldRot.y;
	if (i == 3)	return worldRot.z;
}

D3DXVECTOR3 GetGroungModelSCL()
{
	return D3DXVECTOR3(SCALE_X, SCALE_Y, SCALE_Z);
}

void InitNormals()
{
	//Record GroundMesh Normal
	D3DXMATRIX  mtxScl, mtxRot, mtxTranslate, mtxWorld;
	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&mtxWorld);
	// スケールを反映
	D3DXMatrixScaling(&mtxScl, SCALE_X, SCALE_Y, SCALE_Z);
	D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxScl);

	// 回転を反映
	//D3DXMatrixRotationYawPitchRoll(&mtxRot, 0.0f, 0.0f, 0.0f);
	//D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxRot);

	// 移動を反映
	//D3DXMatrixTranslation(&mtxTranslate, Move_X, Move_Y, Move_Z);
	//D3DXMatrixMultiply(&mtxWorld, &mtxWorld, &mtxTranslate);

	//====================GetNormal
	// HeartWorld
	groundNormal_HeartWorld = new D3DXVECTOR3[g_Model[GroundType_HeartWorld].IndexNum];
	for (int i = 0; i < g_Model[GroundType_HeartWorld].IndexNum; i += 3) {
		D3DXVECTOR3 p0, p1, p2;
		p0 = g_Model[GroundType_HeartWorld].Vertexlist[g_Model[GroundType_HeartWorld].Indexlist[i]];
		p1 = g_Model[GroundType_HeartWorld].Vertexlist[g_Model[GroundType_HeartWorld].Indexlist[i + 1]];
		p2 = g_Model[GroundType_HeartWorld].Vertexlist[g_Model[GroundType_HeartWorld].Indexlist[i + 2]];

		D3DXVec3TransformCoord(&p0, &p0, &mtxWorld);
		D3DXVec3TransformCoord(&p1, &p1, &mtxWorld);
		D3DXVec3TransformCoord(&p2, &p2, &mtxWorld);
		D3DXVECTOR3		nor;		// ポリゴンの法線
		D3DXVECTOR3		vec1;
		D3DXVECTOR3		vec2;

		{	// ポリゴンの外積をとって法線を求める(固定物なら予めInit()で行っておくと良い)
			vec1 = p1 - p0;
			vec2 = p2 - p0;
			crossProduct(&nor, &vec2, &vec1);
			D3DXVec3Normalize(&nor, &nor);		// 計算しやすいように法線をノーマルイズしておく
			groundNormal_HeartWorld[i] = nor;				// 求めた法線を入れておく
			groundNormal_HeartWorld[i + 1] = nor;			// 求めた法線を入れておく
			groundNormal_HeartWorld[i + 2] = nor;			// 求めた法線を入れておく
		}
	}

	// HtoW
	groundNormal_HtoW = new D3DXVECTOR3[g_Model[GroundType_HtoW].IndexNum];
	for (int i = 0; i < g_Model[GroundType_HtoW].IndexNum; i += 3) {
		D3DXVECTOR3 p0, p1, p2;
		p0 = g_Model[GroundType_HtoW].Vertexlist[g_Model[GroundType_HtoW].Indexlist[i]];
		p1 = g_Model[GroundType_HtoW].Vertexlist[g_Model[GroundType_HtoW].Indexlist[i + 1]];
		p2 = g_Model[GroundType_HtoW].Vertexlist[g_Model[GroundType_HtoW].Indexlist[i + 2]];

		D3DXVec3TransformCoord(&p0, &p0, &mtxWorld);
		D3DXVec3TransformCoord(&p1, &p1, &mtxWorld);
		D3DXVec3TransformCoord(&p2, &p2, &mtxWorld);
		D3DXVECTOR3		nor;		// ポリゴンの法線
		D3DXVECTOR3		vec1;
		D3DXVECTOR3		vec2;

		{	// ポリゴンの外積をとって法線を求める(固定物なら予めInit()で行っておくと良い)
			vec1 = p1 - p0;
			vec2 = p2 - p0;
			crossProduct(&nor, &vec2, &vec1);
			D3DXVec3Normalize(&nor, &nor);		// 計算しやすいように法線をノーマルイズしておく
			groundNormal_HtoW[i] = nor;				// 求めた法線を入れておく
			groundNormal_HtoW[i + 1] = nor;			// 求めた法線を入れておく
			groundNormal_HtoW[i + 2] = nor;			// 求めた法線を入れておく
		}
	}

	// WonderLand
	groundNormal_WonderLand = new D3DXVECTOR3[g_Model[GroundType_Wonderland].IndexNum];
	for (int i = 0; i < g_Model[GroundType_Wonderland].IndexNum; i += 3) {
		D3DXVECTOR3 p0, p1, p2;
		p0 = g_Model[GroundType_Wonderland].Vertexlist[g_Model[GroundType_Wonderland].Indexlist[i]];
		p1 = g_Model[GroundType_Wonderland].Vertexlist[g_Model[GroundType_Wonderland].Indexlist[i + 1]];
		p2 = g_Model[GroundType_Wonderland].Vertexlist[g_Model[GroundType_Wonderland].Indexlist[i + 2]];

		D3DXVec3TransformCoord(&p0, &p0, &mtxWorld);
		D3DXVec3TransformCoord(&p1, &p1, &mtxWorld);
		D3DXVec3TransformCoord(&p2, &p2, &mtxWorld);
		D3DXVECTOR3		nor;		// ポリゴンの法線
		D3DXVECTOR3		vec1;
		D3DXVECTOR3		vec2;

		{	// ポリゴンの外積をとって法線を求める(固定物なら予めInit()で行っておくと良い)
			vec1 = p1 - p0;
			vec2 = p2 - p0;
			crossProduct(&nor, &vec2, &vec1);
			D3DXVec3Normalize(&nor, &nor);		// 計算しやすいように法線をノーマルイズしておく
			groundNormal_WonderLand[i] = nor;				// 求めた法線を入れておく
			groundNormal_WonderLand[i + 1] = nor;			// 求めた法線を入れておく
			groundNormal_WonderLand[i + 2] = nor;			// 求めた法線を入れておく
		}
	}

	// WonderLand2
	groundNormal_WonderLand2 = new D3DXVECTOR3[g_Model[GroundType_Wonderland2].IndexNum];
	for (int i = 0; i < g_Model[GroundType_Wonderland2].IndexNum; i += 3) {
		D3DXVECTOR3 p0, p1, p2;
		p0 = g_Model[GroundType_Wonderland2].Vertexlist[g_Model[GroundType_Wonderland2].Indexlist[i]];
		p1 = g_Model[GroundType_Wonderland2].Vertexlist[g_Model[GroundType_Wonderland2].Indexlist[i + 1]];
		p2 = g_Model[GroundType_Wonderland2].Vertexlist[g_Model[GroundType_Wonderland2].Indexlist[i + 2]];

		D3DXVec3TransformCoord(&p0, &p0, &mtxWorld);
		D3DXVec3TransformCoord(&p1, &p1, &mtxWorld);
		D3DXVec3TransformCoord(&p2, &p2, &mtxWorld);
		D3DXVECTOR3		nor;		// ポリゴンの法線
		D3DXVECTOR3		vec1;
		D3DXVECTOR3		vec2;

		{	// ポリゴンの外積をとって法線を求める(固定物なら予めInit()で行っておくと良い)
			vec1 = p1 - p0;
			vec2 = p2 - p0;
			crossProduct(&nor, &vec2, &vec1);
			D3DXVec3Normalize(&nor, &nor);		// 計算しやすいように法線をノーマルイズしておく
			groundNormal_WonderLand2[i] = nor;				// 求めた法線を入れておく
			groundNormal_WonderLand2[i + 1] = nor;			// 求めた法線を入れておく
			groundNormal_WonderLand2[i + 2] = nor;			// 求めた法線を入れておく
		}
	}

	// WtoS
	groundNormal_WtoS = new D3DXVECTOR3[g_Model[GroundType_WtoS].IndexNum];
	for (int i = 0; i < g_Model[GroundType_WtoS].IndexNum; i += 3) {
		D3DXVECTOR3 p0, p1, p2;
		p0 = g_Model[GroundType_WtoS].Vertexlist[g_Model[GroundType_WtoS].Indexlist[i]];
		p1 = g_Model[GroundType_WtoS].Vertexlist[g_Model[GroundType_WtoS].Indexlist[i + 1]];
		p2 = g_Model[GroundType_WtoS].Vertexlist[g_Model[GroundType_WtoS].Indexlist[i + 2]];

		D3DXVec3TransformCoord(&p0, &p0, &mtxWorld);
		D3DXVec3TransformCoord(&p1, &p1, &mtxWorld);
		D3DXVec3TransformCoord(&p2, &p2, &mtxWorld);
		D3DXVECTOR3		nor;		// ポリゴンの法線
		D3DXVECTOR3		vec1;
		D3DXVECTOR3		vec2;

		{	// ポリゴンの外積をとって法線を求める(固定物なら予めInit()で行っておくと良い)
			vec1 = p1 - p0;
			vec2 = p2 - p0;
			crossProduct(&nor, &vec2, &vec1);
			D3DXVec3Normalize(&nor, &nor);		// 計算しやすいように法線をノーマルイズしておく
			groundNormal_WtoS[i] = nor;				// 求めた法線を入れておく
			groundNormal_WtoS[i + 1] = nor;			// 求めた法線を入れておく
			groundNormal_WtoS[i + 2] = nor;			// 求めた法線を入れておく
		}
	}

	// SandWorld
	groundNormal_SandWorld = new D3DXVECTOR3[g_Model[GroundType_SandWorld].IndexNum];
	for (int i = 0; i < g_Model[GroundType_SandWorld].IndexNum; i += 3) {
		D3DXVECTOR3 p0, p1, p2;
		p0 = g_Model[GroundType_SandWorld].Vertexlist[g_Model[GroundType_SandWorld].Indexlist[i]];
		p1 = g_Model[GroundType_SandWorld].Vertexlist[g_Model[GroundType_SandWorld].Indexlist[i + 1]];
		p2 = g_Model[GroundType_SandWorld].Vertexlist[g_Model[GroundType_SandWorld].Indexlist[i + 2]];

		D3DXVec3TransformCoord(&p0, &p0, &mtxWorld);
		D3DXVec3TransformCoord(&p1, &p1, &mtxWorld);
		D3DXVec3TransformCoord(&p2, &p2, &mtxWorld);
		D3DXVECTOR3		nor;		// ポリゴンの法線
		D3DXVECTOR3		vec1;
		D3DXVECTOR3		vec2;

		{	// ポリゴンの外積をとって法線を求める(固定物なら予めInit()で行っておくと良い)
			vec1 = p1 - p0;
			vec2 = p2 - p0;
			crossProduct(&nor, &vec2, &vec1);
			D3DXVec3Normalize(&nor, &nor);		// 計算しやすいように法線をノーマルイズしておく
			groundNormal_SandWorld[i] = nor;				// 求めた法線を入れておく
			groundNormal_SandWorld[i + 1] = nor;			// 求めた法線を入れておく
			groundNormal_SandWorld[i + 2] = nor;			// 求めた法線を入れておく
		}
	}

	// StoS
	groundNormal_StoS = new D3DXVECTOR3[g_Model[GroundType_StoS].IndexNum];
	for (int i = 0; i < g_Model[GroundType_StoS].IndexNum; i += 3) {
		D3DXVECTOR3 p0, p1, p2;
		p0 = g_Model[GroundType_StoS].Vertexlist[g_Model[GroundType_StoS].Indexlist[i]];
		p1 = g_Model[GroundType_StoS].Vertexlist[g_Model[GroundType_StoS].Indexlist[i + 1]];
		p2 = g_Model[GroundType_StoS].Vertexlist[g_Model[GroundType_StoS].Indexlist[i + 2]];

		D3DXVec3TransformCoord(&p0, &p0, &mtxWorld);
		D3DXVec3TransformCoord(&p1, &p1, &mtxWorld);
		D3DXVec3TransformCoord(&p2, &p2, &mtxWorld);
		D3DXVECTOR3		nor;		// ポリゴンの法線
		D3DXVECTOR3		vec1;
		D3DXVECTOR3		vec2;

		{	// ポリゴンの外積をとって法線を求める(固定物なら予めInit()で行っておくと良い)
			vec1 = p1 - p0;
			vec2 = p2 - p0;
			crossProduct(&nor, &vec2, &vec1);
			D3DXVec3Normalize(&nor, &nor);		// 計算しやすいように法線をノーマルイズしておく
			groundNormal_StoS[i] = nor;				// 求めた法線を入れておく
			groundNormal_StoS[i + 1] = nor;			// 求めた法線を入れておく
			groundNormal_StoS[i + 2] = nor;			// 求めた法線を入れておく
		}
	}

	// SnowWorld
	//groundNormal_SnowWorld = new D3DXVECTOR3[g_Model[GroundType_SnowWorld].IndexNum];
	//for (int i = 0; i < g_Model[GroundType_SnowWorld].IndexNum; i += 3) {
	//	D3DXVECTOR3 p0, p1, p2;
	//	p0 = g_Model[GroundType_SnowWorld].Vertexlist[g_Model[GroundType_SnowWorld].Indexlist[i]];
	//	p1 = g_Model[GroundType_SnowWorld].Vertexlist[g_Model[GroundType_SnowWorld].Indexlist[i + 1]];
	//	p2 = g_Model[GroundType_SnowWorld].Vertexlist[g_Model[GroundType_SnowWorld].Indexlist[i + 2]];
	//
	//	D3DXVec3TransformCoord(&p0, &p0, &mtxWorld);
	//	D3DXVec3TransformCoord(&p1, &p1, &mtxWorld);
	//	D3DXVec3TransformCoord(&p2, &p2, &mtxWorld);
	//	D3DXVECTOR3		nor;		// ポリゴンの法線
	//	D3DXVECTOR3		vec1;
	//	D3DXVECTOR3		vec2;
	//
	//	{	// ポリゴンの外積をとって法線を求める(固定物なら予めInit()で行っておくと良い)
	//		vec1 = p1 - p0;
	//		vec2 = p2 - p0;
	//		crossProduct(&nor, &vec2, &vec1);
	//		D3DXVec3Normalize(&nor, &nor);		// 計算しやすいように法線をノーマルイズしておく
	//		groundNormal_SnowWorld[i] = nor;				// 求めた法線を入れておく
	//		groundNormal_SnowWorld[i + 1] = nor;			// 求めた法線を入れておく
	//		groundNormal_SnowWorld[i + 2] = nor;			// 求めた法線を入れておく
	//	}
	//}

}

void AdjustDeltaRot() {

	if (-0.25 * D3DX_PI < worldRot.x && worldRot.x <= 0.25 * D3DX_PI)
	{
		worldRot.x = 0.0f;
	} 
	else if (-0.75 * D3DX_PI < worldRot.x && worldRot.x <= -0.25 * D3DX_PI)
	{
		worldRot.x = -0.5 * D3DX_PI;
	}
	else if (0.25 * D3DX_PI < worldRot.x && worldRot.x <= 0.75 * D3DX_PI)
	{
		worldRot.x = 0.5 * D3DX_PI;
	}
	else if ((0.75 * D3DX_PI < worldRot.x && worldRot.x <= D3DX_PI) || (-0.75 * D3DX_PI > worldRot.x && worldRot.x >= -D3DX_PI))
	{
		worldRot.x = D3DX_PI;
	}


	if ((-0.25 * D3DX_PI < worldRot.y && worldRot.y <= 0.25 * D3DX_PI))
	{
		worldRot.y = 0.0f;
	}
	else if (-0.75 * D3DX_PI < worldRot.y && worldRot.y <= -0.25 * D3DX_PI)
	{
		worldRot.y = -0.5 * D3DX_PI;
	}
	else if (0.25 * D3DX_PI < worldRot.y && worldRot.y <= 0.75 * D3DX_PI)
	{
		worldRot.y = 0.5 * D3DX_PI;
	}
	else if ((0.75 * D3DX_PI < worldRot.y && worldRot.y <= D3DX_PI) || (-0.75 * D3DX_PI > worldRot.y && worldRot.y >= -D3DX_PI))
	{
		worldRot.y = D3DX_PI;
	}
}