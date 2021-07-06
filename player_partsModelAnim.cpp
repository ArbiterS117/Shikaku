//=============================================================================
//
// プレーヤパーツアニメ処理 [player_partsModelAnim.h] //階層アニメーション & モデル
// Author : GP11B132 14 ゴショウケン
//
//=============================================================================

#include "player_partsModelAnim.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define MAX_MODEL		  (5)					            // モデルの数
#define	MODEL_ALICE_BODY  "data/MODEL/ALICE_Body.obj"		// 読み込むモデル名
#define MODEL_ALICE_LHAND "data/MODEL/ALICE_LHand.obj"      // 読み込むモデル名
#define MODEL_ALICE_RHAND "data/MODEL/ALICE_RHand.obj"      // 読み込むモデル名
#define MODEL_ALICE_LFEET "data/MODEL/ALICE_LFeet.obj"      // 読み込むモデル名
#define MODEL_ALICE_RFEET "data/MODEL/ALICE_RFeet.obj"      // 読み込むモデル名
#define PLAYER_PARTS_MAX  (5)                               // プレイヤーのパーツの数
#define PLAYER_SCALE      (3.5f)                            // プレイヤーのパーツの数
enum PartType
{
	B  = 0,
	RH = 1,
	LH = 2,
	RF = 3,
	LF = 4,
};
//========================================================================================================================================
//========================================================================================================================================
//========================================================================================================================================
//========================================================================================================================================
//========================================================================================================================================
//========================================================================================================================================
//========================================================================================================================================
//========================================================================================================================================
//========== Idle ========================================================================================================================
//========================================================================================================================================
static TRANSFORM Anim_tblB_Idle[] = { // 身体
	{D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(PLAYER_SCALE, PLAYER_SCALE, PLAYER_SCALE), 60},
	{D3DXVECTOR3(0.0f, 0.5f, 0.0f), D3DXVECTOR3(D3DX_PI * -0.025f, 0.0f, 0.0f), D3DXVECTOR3(PLAYER_SCALE, PLAYER_SCALE, PLAYER_SCALE), 60},
	{D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(D3DX_PI *  0.0f, 0.0f, 0.0f), D3DXVECTOR3(PLAYER_SCALE, PLAYER_SCALE, PLAYER_SCALE), 60},
};
static TRANSFORM Anim_tblRH_Idle[] = { //RHAND
	{D3DXVECTOR3( 2.0f, 0.5f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * -0.4f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 60},
	{D3DXVECTOR3( 2.0f, 0.5f, 0.0f), D3DXVECTOR3(D3DX_PI * 0.2f, 0.0f, D3DX_PI * -0.4f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 60},
	{D3DXVECTOR3( 2.0f, 0.5f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * -0.4f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 20},
};
static TRANSFORM Anim_tblLH_Idle[] = { //LHAND
	{D3DXVECTOR3(-2.0f, 0.5f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI *  0.4f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 60},
	{D3DXVECTOR3(-2.0f, 0.5f, 0.0f), D3DXVECTOR3(D3DX_PI * 0.2f, 0.0f, D3DX_PI *  0.4f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 60},
	{D3DXVECTOR3(-2.0f, 0.5f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI *  0.4f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 20},
};
static TRANSFORM Anim_tblRF_Idle[] = { //RFEET
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 20},
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 20},
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 20},
};
static TRANSFORM Anim_tblLF_Idle[] = { //LFEET
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 20},
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 20},
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 20},
};

//========================================================================================================================================
//========== Move ========================================================================================================================
//========================================================================================================================================
float AnimMovePar = 8.0f; //8 ~ 36 (fast ~ slow)
static TRANSFORM Anim_tblB_Move[] = { // 身体
	{D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(D3DX_PI * -0.05f, 0.0f, 0.0f), D3DXVECTOR3(PLAYER_SCALE, PLAYER_SCALE, PLAYER_SCALE), 10},
	{D3DXVECTOR3(0.0f, 2.0f, 0.0f), D3DXVECTOR3(D3DX_PI * -0.05f, 0.0f, 0.0f), D3DXVECTOR3(PLAYER_SCALE, PLAYER_SCALE, PLAYER_SCALE), 10},
};
static TRANSFORM Anim_tblRH_Move[] = { //RHAND
	{D3DXVECTOR3(2.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f,  D3DX_PI *  0.4f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 8},
	{D3DXVECTOR3(2.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f,  D3DX_PI * -0.3f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 8},
	{D3DXVECTOR3(2.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f,  D3DX_PI *  0.4f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 8},
};
static TRANSFORM Anim_tblLH_Move[] = { //LHAND
	{D3DXVECTOR3(-2.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f,  D3DX_PI *  0.4f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 8},
	{D3DXVECTOR3(-2.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f,  D3DX_PI * -0.3f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 8},
	{D3DXVECTOR3(-2.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f,  D3DX_PI *  0.4f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 8},
};
static TRANSFORM Anim_tblRF_Move[] = { //RFEET
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(D3DX_PI *  0.3f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 8},
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(D3DX_PI * -0.2f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 8},
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(D3DX_PI *  0.3f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 8},
};
static TRANSFORM Anim_tblLF_Move[] = { //LFEET
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(D3DX_PI * -0.2f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 8},
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(D3DX_PI *  0.3f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 8},
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(D3DX_PI * -0.2f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 8},
};
//========================================================================================================================================
//========== FastMove ====================================================================================================================
//========================================================================================================================================
static TRANSFORM Anim_tblB_FastMove[] = { // 身体
	{D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(D3DX_PI * -0.1f, 0.0f, 0.0f), D3DXVECTOR3(PLAYER_SCALE, PLAYER_SCALE, PLAYER_SCALE), 3},
	{D3DXVECTOR3(0.0f, 3.0f, 0.0f), D3DXVECTOR3(D3DX_PI * -0.1f, 0.0f, 0.0f), D3DXVECTOR3(PLAYER_SCALE, PLAYER_SCALE, PLAYER_SCALE), 3},
};
static TRANSFORM Anim_tblRH_FastMove[] = { //RHAND
	{D3DXVECTOR3(2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f,  D3DX_PI *  0.3f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 8},
	{D3DXVECTOR3(2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f,  D3DX_PI *  0.3f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 8},
	{D3DXVECTOR3(2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f,  D3DX_PI *  0.3f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 8},
};
static TRANSFORM Anim_tblLH_FastMove[] = { //LHAND
	{D3DXVECTOR3(-2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f,  D3DX_PI * -0.3f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 8},
	{D3DXVECTOR3(-2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f,  D3DX_PI * -0.3f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 8},
	{D3DXVECTOR3(-2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f,  D3DX_PI * -0.3f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 8},
};
static TRANSFORM Anim_tblRF_FastMove[] = { //RFEET
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(D3DX_PI *  0.3f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 7},
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(D3DX_PI * -0.2f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 7},
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(D3DX_PI *  0.3f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 7},
};
static TRANSFORM Anim_tblLF_FastMove[] = { //LFEET
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(D3DX_PI * -0.2f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 7},
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(D3DX_PI *  0.3f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 7},
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(D3DX_PI * -0.2f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 7},
};
//========================================================================================================================================
//========== Jump ========================================================================================================================
//========================================================================================================================================
static TRANSFORM Anim_tblB_Jump[] = { // 身体
	{D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(PLAYER_SCALE, PLAYER_SCALE, PLAYER_SCALE), 5},
	{D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(D3DX_PI * -0.1f, 0.0f, 0.0f), D3DXVECTOR3(PLAYER_SCALE, PLAYER_SCALE, PLAYER_SCALE), 8},
	{D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(D3DX_PI * -0.1f, 0.0f, 0.0f), D3DXVECTOR3(PLAYER_SCALE, PLAYER_SCALE, PLAYER_SCALE), 8},
};
static TRANSFORM Anim_tblRH_Jump[] = { //RHAND
	{D3DXVECTOR3(2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 5},
	{D3DXVECTOR3(2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * 0.3f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 5},
	{D3DXVECTOR3(2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * 0.3f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 5},
};
static TRANSFORM Anim_tblLH_Jump[] = { //LHAND
	{D3DXVECTOR3(-2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 5},
	{D3DXVECTOR3(-2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * 0.3f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 5},
	{D3DXVECTOR3(-2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * 0.3f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 5},
};
static TRANSFORM Anim_tblRF_Jump[] = { //RFEET
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(           0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 7},
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(D3DX_PI *  0.3f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 7},
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(D3DX_PI *  0.3f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 7},
};
static TRANSFORM Anim_tblLF_Jump[] = { //LFEET
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(           0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 7},
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(D3DX_PI * -0.3f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 7},
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(D3DX_PI * -0.3f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 7},
};
//========================================================================================================================================
//========== BigJump ========================================================================================================================
//========================================================================================================================================
static TRANSFORM Anim_tblB_BigJump[] = { // 身体
	{D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(PLAYER_SCALE, PLAYER_SCALE, PLAYER_SCALE), 5},
	{D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(D3DX_PI * -0.13f, 0.0f, 0.0f), D3DXVECTOR3(PLAYER_SCALE, PLAYER_SCALE, PLAYER_SCALE), 8},
	{D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(D3DX_PI * -0.13f, 0.0f, 0.0f), D3DXVECTOR3(PLAYER_SCALE, PLAYER_SCALE, PLAYER_SCALE), 8},
};
static TRANSFORM Anim_tblRH_BigJump[] = { //RHAND
	{D3DXVECTOR3(2.75f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 5},
	{D3DXVECTOR3(2.75f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * 0.2f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 5},
	{D3DXVECTOR3(2.75f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * 0.2f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 5},
};
static TRANSFORM Anim_tblLH_BigJump[] = { //LHAND
	{D3DXVECTOR3(-2.75f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 5},
	{D3DXVECTOR3(-2.75f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * -0.2f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 5},
	{D3DXVECTOR3(-2.75f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * -0.2f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 5},
};
static TRANSFORM Anim_tblRF_BigJump[] = { //RFEET
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 7},
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(D3DX_PI *  0.3f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 7},
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(D3DX_PI *  0.3f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 7},
};
static TRANSFORM Anim_tblLF_BigJump[] = { //LFEET
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 7},
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(D3DX_PI * -0.3f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 7},
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(D3DX_PI * -0.3f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 7},
};
//========================================================================================================================================
//========== Air =========================================================================================================================
//========================================================================================================================================
static TRANSFORM Anim_tblB_Air[] = { // 身体 Fall Down
	{D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(PLAYER_SCALE, PLAYER_SCALE, PLAYER_SCALE), 15},
	{D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(D3DX_PI * 0.1f, 0.0f, 0.0f), D3DXVECTOR3(PLAYER_SCALE, PLAYER_SCALE, PLAYER_SCALE), 10},
	{D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(D3DX_PI * 0.1f, 0.0f, 0.0f), D3DXVECTOR3(PLAYER_SCALE, PLAYER_SCALE, PLAYER_SCALE), 8},
};
static TRANSFORM Anim_tblRH_Air[] = { //RHAND
	{D3DXVECTOR3(2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 15},
	{D3DXVECTOR3(2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * 0.3f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 15},
	{D3DXVECTOR3(2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * 0.3f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 15},
};
static TRANSFORM Anim_tblLH_Air[] = { //LHAND
	{D3DXVECTOR3(-2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI *  0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 15},
	{D3DXVECTOR3(-2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * -0.3f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 15},
	{D3DXVECTOR3(-2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * -0.3f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 15},
};
static TRANSFORM Anim_tblRF_Air[] = { //RFEET
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 15},
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(D3DX_PI * -0.3f, 0.0f, D3DX_PI * 0.1f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 15},
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(D3DX_PI * -0.3f, 0.0f, D3DX_PI * 0.1f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 15},
};
static TRANSFORM Anim_tblLF_Air[] = { //LFEET
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 15},
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(D3DX_PI * -0.3f, 0.0f, D3DX_PI * -0.1f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 15},
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(D3DX_PI * -0.3f, 0.0f, D3DX_PI * -0.1f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 15},
};
//========================================================================================================================================
//========== Dash ========================================================================================================================
//========================================================================================================================================
static TRANSFORM Anim_tblB_Dash[] = { // 身体
	{D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(D3DX_PI * 0.0f, D3DX_PI * 0.5f, D3DX_PI * 0.5f), D3DXVECTOR3(PLAYER_SCALE, PLAYER_SCALE, PLAYER_SCALE), 14},
	{D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(D3DX_PI * 2.0f, D3DX_PI * 0.5f, D3DX_PI * 0.5f), D3DXVECTOR3(PLAYER_SCALE, PLAYER_SCALE, PLAYER_SCALE),1},
	{D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(D3DX_PI * 0.0f, D3DX_PI * 0.5f, D3DX_PI * 0.5f), D3DXVECTOR3(PLAYER_SCALE, PLAYER_SCALE, PLAYER_SCALE), 1},
};
static TRANSFORM Anim_tblRH_Dash[] = { //RHAND
	{D3DXVECTOR3(2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * -0.3f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 14},
	{D3DXVECTOR3(2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * -0.3f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 1},
	{D3DXVECTOR3(2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * -0.3f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 1},
};
static TRANSFORM Anim_tblLH_Dash[] = { //LHAND
	{D3DXVECTOR3(-2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI *  0.3f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 14},
	{D3DXVECTOR3(-2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI *  0.3f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 1},
	{D3DXVECTOR3(-2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI *  0.3f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 1},
};
static TRANSFORM Anim_tblRF_Dash[] = { //RFEET
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 14},
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 1},
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 1},
};
static TRANSFORM Anim_tblLF_Dash[] = { //LFEET
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 14},
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 1},
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 1},
};
//========================================================================================================================================
//========== ThrowCard ===================================================================================================================
//========================================================================================================================================
static TRANSFORM Anim_tblB_ThrowCard[] = { // 身体
	{D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(D3DX_PI * 0.1f, D3DX_PI * 0.0f, 0.0f), D3DXVECTOR3(PLAYER_SCALE, PLAYER_SCALE, PLAYER_SCALE), 15},
	{D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(D3DX_PI * 0.1f, D3DX_PI * 2.0f, 0.0f), D3DXVECTOR3(PLAYER_SCALE, PLAYER_SCALE, PLAYER_SCALE), 10},
};
static TRANSFORM Anim_tblRH_ThrowCard[] = { //RHAND
	{D3DXVECTOR3(2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, D3DX_PI * -0.4f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 15},
	{D3DXVECTOR3(2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, D3DX_PI * 0.4f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 3},
	{D3DXVECTOR3(2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, D3DX_PI * 0.4f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 15},
};
static TRANSFORM Anim_tblLH_ThrowCard[] = { //LHAND
	{D3DXVECTOR3(-2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, D3DX_PI * 0.8f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 15},
	{D3DXVECTOR3(-2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, D3DX_PI * 0.35f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 3},
	{D3DXVECTOR3(-2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, D3DX_PI * 0.35f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 15},
};
static TRANSFORM Anim_tblRF_ThrowCard[] = { //RFEET
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 15},
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(D3DX_PI *  0.3f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 3},
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(D3DX_PI *  0.3f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 3},
};
static TRANSFORM Anim_tblLF_ThrowCard[] = { //LFEET
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 15},
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 3},
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 15},
};
//========================================================================================================================================
//========== CatchCardJump ===============================================================================================================
//========================================================================================================================================
static TRANSFORM Anim_tblB_CatchCardJump[] = { // 身体
	{D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(D3DX_PI * -0.1f, D3DX_PI * 0.0f, 0.0f), D3DXVECTOR3(PLAYER_SCALE, PLAYER_SCALE, PLAYER_SCALE), 15},
	{D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(D3DX_PI * -0.1f, D3DX_PI * -2.0f, 0.0f), D3DXVECTOR3(PLAYER_SCALE, PLAYER_SCALE, PLAYER_SCALE), 10},
};
static TRANSFORM Anim_tblRH_CatchCardJump[] = { //RHAND
	{D3DXVECTOR3(2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 15},
	{D3DXVECTOR3(2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * 0.3f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 3},
	{D3DXVECTOR3(2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * 0.3f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 3},
};
static TRANSFORM Anim_tblLH_CatchCardJump[] = { //LHAND
	{D3DXVECTOR3(-2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 15},
	{D3DXVECTOR3(-2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * 0.3f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 3},
	{D3DXVECTOR3(-2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * 0.3f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 3},
};
static TRANSFORM Anim_tblRF_CatchCardJump[] = { //RFEET
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 15},
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(D3DX_PI *  0.3f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 3},
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(D3DX_PI *  0.3f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 3},
};
static TRANSFORM Anim_tblLF_CatchCardJump[] = { //LFEET
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 15},
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(D3DX_PI * -0.3f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 3},
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(D3DX_PI * -0.3f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 3},
};
//========================================================================================================================================
//========== BobStatus ===============================================================================================================
//========================================================================================================================================
static TRANSFORM Anim_tblB_BobStatus[] = { // 身体
	{D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(D3DX_PI *  0.0f, D3DX_PI *  0.0f, 0.0f), D3DXVECTOR3(PLAYER_SCALE, PLAYER_SCALE, PLAYER_SCALE), 240},
	{D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(D3DX_PI *  2.0f, D3DX_PI * -2.0f, 0.0f), D3DXVECTOR3(PLAYER_SCALE, PLAYER_SCALE, PLAYER_SCALE), 240},
};
static TRANSFORM Anim_tblRH_BobStatus[] = { //RHAND
	//{D3DXVECTOR3(2.75f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 5},
	{D3DXVECTOR3(2.75f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * 0.2f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 5},
	{D3DXVECTOR3(2.75f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * 0.2f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 5},
};
static TRANSFORM Anim_tblLH_BobStatus[] = { //LHAND
	//{D3DXVECTOR3(-2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI *  0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 15},
	{D3DXVECTOR3(-2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * -0.3f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 15},
	{D3DXVECTOR3(-2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * -0.3f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 15},
};
static TRANSFORM Anim_tblRF_BobStatus[] = { //RFEET
	//{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 15},
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(D3DX_PI * -0.3f, 0.0f, D3DX_PI * 0.1f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 15},
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(D3DX_PI * -0.3f, 0.0f, D3DX_PI * 0.1f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 15},
};
static TRANSFORM Anim_tblLF_BobStatus[] = { //LFEET
	//{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 15},
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(D3DX_PI * -0.3f, 0.0f, D3DX_PI * -0.1f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 15},
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(D3DX_PI * -0.3f, 0.0f, D3DX_PI * -0.1f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 15},
};
//========================================================================================================================================
//========== GetStar ===================================================================================================================
//========================================================================================================================================
static TRANSFORM Anim_tblB_GetStar[] = { // 身体
	{D3DXVECTOR3(0.0f, 20.0f, 0.0f), D3DXVECTOR3(D3DX_PI * 0.1f, D3DX_PI * 0.0f, 0.0f), D3DXVECTOR3(PLAYER_SCALE, PLAYER_SCALE, PLAYER_SCALE), 10},
	{D3DXVECTOR3(0.0f, 15.0f, 0.0f), D3DXVECTOR3(D3DX_PI * 0.1f, D3DX_PI * 2.0f, 0.0f), D3DXVECTOR3(PLAYER_SCALE, PLAYER_SCALE, PLAYER_SCALE), 10},
	{D3DXVECTOR3(0.0f, 10.0f, 0.0f), D3DXVECTOR3(D3DX_PI * 0.1f, D3DX_PI * 4.0f, 0.0f), D3DXVECTOR3(PLAYER_SCALE, PLAYER_SCALE, PLAYER_SCALE), 10},
	{D3DXVECTOR3(0.0f, 5.0f, 0.0f), D3DXVECTOR3(D3DX_PI * 0.1f, D3DX_PI * 6.0f, 0.0f), D3DXVECTOR3(PLAYER_SCALE, PLAYER_SCALE, PLAYER_SCALE), 10},
	{D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(D3DX_PI * 0.1f, D3DX_PI * 8.0f, 0.0f), D3DXVECTOR3(PLAYER_SCALE, PLAYER_SCALE, PLAYER_SCALE), 10},
	{D3DXVECTOR3(0.0f, -5.0f, 0.0f), D3DXVECTOR3(D3DX_PI * 0.1f, D3DX_PI * 10.0f, 0.0f), D3DXVECTOR3(PLAYER_SCALE, PLAYER_SCALE, PLAYER_SCALE), 10},
    {D3DXVECTOR3(0.0f, -10.0f, 0.0f), D3DXVECTOR3(D3DX_PI * 0.1f, D3DX_PI * 10.0f, 0.0f), D3DXVECTOR3(PLAYER_SCALE, PLAYER_SCALE, PLAYER_SCALE), 20 },
    {D3DXVECTOR3(0.0f, -10.0f, 0.0f), D3DXVECTOR3(D3DX_PI * 0.1f, D3DX_PI * 10.0f, 0.0f), D3DXVECTOR3(PLAYER_SCALE, PLAYER_SCALE, PLAYER_SCALE), 3 },
    {D3DXVECTOR3(0.0f, 10.0f, 0.0f), D3DXVECTOR3(-D3DX_PI * 0.1f, D3DX_PI * 10.0f, 0.0f), D3DXVECTOR3(PLAYER_SCALE * 1.2f, PLAYER_SCALE* 1.2f, PLAYER_SCALE* 1.2f), 3 },
    {D3DXVECTOR3(0.0f, 10.0f, 0.0f), D3DXVECTOR3(-D3DX_PI * 0.1f, D3DX_PI * 10.0f, 0.0f), D3DXVECTOR3(PLAYER_SCALE * 1.2f, PLAYER_SCALE* 1.2f, PLAYER_SCALE* 1.2f), 10 },
};
static TRANSFORM Anim_tblRH_GetStar[] = { //RHAND
	{D3DXVECTOR3(2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * 0.1f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 60},
	{D3DXVECTOR3(2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, D3DX_PI * 0.4f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 20},
	{D3DXVECTOR3(2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, D3DX_PI * 0.4f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 3},
	{D3DXVECTOR3(2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * -0.3f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 5},
	{D3DXVECTOR3(2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * -0.3f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 5},
};
static TRANSFORM Anim_tblLH_GetStar[] = { //LHAND
	{D3DXVECTOR3(-2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * -0.1f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 60},
	{D3DXVECTOR3(-2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, D3DX_PI * 0.35f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 20},
	{D3DXVECTOR3(-2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, D3DX_PI * 0.35f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 3},
	{D3DXVECTOR3(-2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * -0.3f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 5},
	{D3DXVECTOR3(-2.5f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, D3DX_PI * -0.3f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 5},
};
static TRANSFORM Anim_tblRF_GetStar[] = { //RFEET
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 60},
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(D3DX_PI *  0.3f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 20},
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(D3DX_PI *  0.3f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 3},
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(D3DX_PI *  -0.3f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 7},
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(D3DX_PI *  -0.3f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 7},
};
static TRANSFORM Anim_tblLF_GetStar[] = { //LFEET
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 60},
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 20},
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 3},
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(D3DX_PI * 0.3f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 7},
	{D3DXVECTOR3(0.0f, -0.25f, 0.0f), D3DXVECTOR3(D3DX_PI * 0.3f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), 7},
};
//========================================================================================================================================
//========================================================================================================================================
//========================================================================================================================================
//========================================================================================================================================
//========================================================================================================================================
//========================================================================================================================================
//========================================================================================================================================
//========================================================================================================================================
//========================================================================================================================================
//========================================================================================================================================

ANIM_PLAY_TYPE   playType;     // playing style
float  Anim_StartFrom_MoveTime = 0;

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static PLAYER_PART_ANIM     g_Parts[PLAYER_PARTS_MAX];            // プレイヤーのパーツ用
static DX11_MODEL			g_Model[MAX_MODEL];					  // モデル情報
static PLAYER              *g_player = GetPlayer();
static int                  AnimStatus = AnimStatus_Idel;
static int                  AnimStatusPre = AnimStatus;

//=============================================================================
// 初期化処理
//=============================================================================
void InitPlayerPartAnim(void)
{
	//Load model
	LoadModel(MODEL_ALICE_BODY, &g_Model[0]);
	LoadModel(MODEL_ALICE_LHAND, &g_Model[1]);
	LoadModel(MODEL_ALICE_RHAND, &g_Model[2]);
	LoadModel(MODEL_ALICE_LFEET, &g_Model[3]);
	LoadModel(MODEL_ALICE_RFEET, &g_Model[4]);

	////====階層アニメーション parts init
	for (int i = 0; i < PLAYER_PARTS_MAX; i++) {
		g_Parts[i].Use = true;

		// 位置・回転・スケールの初期設定
		g_Parts[i].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Parts[i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_Parts[i].scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

		// 階層アニメーション用のメンバー変数の初期化
		//g_Parts[i].tbl_adr = move_tbl2;
		g_Parts[i].TDN = 0;
		g_Parts[i].move_time = 0.0f;


	}
	g_Parts[B].scl  = D3DXVECTOR3(PLAYER_SCALE, PLAYER_SCALE, PLAYER_SCALE);
	g_Parts[RH].scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	g_Parts[LH].scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	g_Parts[RF].scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	g_Parts[LF].scl = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	// パーツの読み込み
	g_Parts[B].model  = g_Model[B];
	g_Parts[RH].model = g_Model[RH];
	g_Parts[LH].model = g_Model[LH];
	g_Parts[RF].model = g_Model[RF];
	g_Parts[LF].model = g_Model[LF];

	// 親子関係  // ← ここに親のアドレスを入れる
	g_Parts[B].root_parent = g_player;
	g_Parts[RH].parent = &g_Parts[B];
	g_Parts[LH].parent = &g_Parts[B];
	g_Parts[RF].parent = &g_Parts[B];
	g_Parts[LF].parent = &g_Parts[B];
	// 階層アニメーション用のメンバー変数の初期化
	g_Parts[B].tbl_adr  = Anim_tblB_Idle;
	g_Parts[RH].tbl_adr = Anim_tblRH_Idle;
	g_Parts[LH].tbl_adr = Anim_tblLH_Idle;
	g_Parts[RF].tbl_adr = Anim_tblRF_Idle;
	g_Parts[LF].tbl_adr = Anim_tblLF_Idle;

	g_Parts[B].TDN  = sizeof(Anim_tblB_Idle)  / sizeof(TRANSFORM);
	g_Parts[RH].TDN = sizeof(Anim_tblRH_Idle) / sizeof(TRANSFORM);
	g_Parts[LH].TDN = sizeof(Anim_tblLH_Idle) / sizeof(TRANSFORM);
	g_Parts[RF].TDN = sizeof(Anim_tblRF_Idle) / sizeof(TRANSFORM);
	g_Parts[LF].TDN = sizeof(Anim_tblRF_Idle) / sizeof(TRANSFORM);
}

void UninitPlayerPartAnim(void)
{
	// モデルの解放処理
	for (int i = 0; i < MAX_MODEL; i++)
	{
		UnloadModel(&g_Model[i]);
	}
}

void UpdatePlayerPartAnim(void)
{
	////================AnimStatus更新
	AnimStatus = g_player->Anim_Status;
	if (AnimStatusPre != AnimStatus) {              // アニメ変わる
		g_Parts[B].move_time  = 0.0f;
		g_Parts[RH].move_time = 0.0f;
		g_Parts[LH].move_time = 0.0f;
		g_Parts[RF].move_time = 0.0f;
		g_Parts[LF].move_time = 0.0f;
		if (Anim_StartFrom_MoveTime != 0.0f) {
			g_Parts[B].move_time  = Anim_StartFrom_MoveTime;
			g_Parts[RH].move_time =	Anim_StartFrom_MoveTime;
			g_Parts[LH].move_time =	Anim_StartFrom_MoveTime;
			g_Parts[RF].move_time =	Anim_StartFrom_MoveTime;
			g_Parts[LF].move_time =	Anim_StartFrom_MoveTime;
			Anim_StartFrom_MoveTime = 0.0f;
		}

		switch (AnimStatus)
		{
		case AnimStatus_Idel:
			g_Parts[B].tbl_adr  = Anim_tblB_Idle;
			g_Parts[RH].tbl_adr = Anim_tblRH_Idle;
			g_Parts[LH].tbl_adr = Anim_tblLH_Idle;
			g_Parts[RF].tbl_adr = Anim_tblRF_Idle;
			g_Parts[LF].tbl_adr = Anim_tblLF_Idle;

			g_Parts[B].TDN  = sizeof(Anim_tblB_Idle) / sizeof(TRANSFORM);
			g_Parts[RH].TDN = sizeof(Anim_tblRH_Idle) / sizeof(TRANSFORM);
			g_Parts[LH].TDN = sizeof(Anim_tblLH_Idle) / sizeof(TRANSFORM);
			g_Parts[RF].TDN = sizeof(Anim_tblRF_Idle) / sizeof(TRANSFORM);
			g_Parts[LF].TDN = sizeof(Anim_tblLF_Idle) / sizeof(TRANSFORM);
			playType = APT_LOOP;
			break;
		case AnimStatus_Move:
			g_Parts[B].tbl_adr  = Anim_tblB_Move;
			g_Parts[RH].tbl_adr = Anim_tblRH_Move;
			g_Parts[LH].tbl_adr = Anim_tblLH_Move;
			g_Parts[RF].tbl_adr = Anim_tblRF_Move;
			g_Parts[LF].tbl_adr = Anim_tblLF_Move;

			g_Parts[B].TDN  = sizeof(Anim_tblB_Move) / sizeof(TRANSFORM);
			g_Parts[RH].TDN = sizeof(Anim_tblRH_Move) / sizeof(TRANSFORM);
			g_Parts[LH].TDN = sizeof(Anim_tblLH_Move) / sizeof(TRANSFORM);
			g_Parts[RF].TDN = sizeof(Anim_tblRF_Move) / sizeof(TRANSFORM);
			g_Parts[LF].TDN = sizeof(Anim_tblLF_Move) / sizeof(TRANSFORM);
			playType = APT_LOOP;
			break;
		case AnimStatus_FastMove:
			g_Parts[B].tbl_adr  = Anim_tblB_FastMove;
			g_Parts[RH].tbl_adr = Anim_tblRH_FastMove;
			g_Parts[LH].tbl_adr = Anim_tblLH_FastMove;
			g_Parts[RF].tbl_adr = Anim_tblRF_FastMove;
			g_Parts[LF].tbl_adr = Anim_tblLF_FastMove;

			g_Parts[B].TDN  = sizeof(Anim_tblB_FastMove) / sizeof(TRANSFORM);
			g_Parts[RH].TDN = sizeof(Anim_tblRH_FastMove) / sizeof(TRANSFORM);
			g_Parts[LH].TDN = sizeof(Anim_tblLH_FastMove) / sizeof(TRANSFORM);
			g_Parts[RF].TDN = sizeof(Anim_tblRF_FastMove) / sizeof(TRANSFORM);
			g_Parts[LF].TDN = sizeof(Anim_tblLF_FastMove) / sizeof(TRANSFORM);
			playType = APT_LOOP;
			break;
		case AnimStatus_Jump:
			g_Parts[B].tbl_adr  = Anim_tblB_Jump;
			g_Parts[RH].tbl_adr = Anim_tblRH_Jump;
			g_Parts[LH].tbl_adr = Anim_tblLH_Jump;
			g_Parts[RF].tbl_adr = Anim_tblRF_Jump;
			g_Parts[LF].tbl_adr = Anim_tblLF_Jump;

			g_Parts[B].TDN  = sizeof(Anim_tblB_Jump) / sizeof(TRANSFORM);
			g_Parts[RH].TDN = sizeof(Anim_tblRH_Jump) / sizeof(TRANSFORM);
			g_Parts[LH].TDN = sizeof(Anim_tblLH_Jump) / sizeof(TRANSFORM);
			g_Parts[RF].TDN = sizeof(Anim_tblRF_Jump) / sizeof(TRANSFORM);
			g_Parts[LF].TDN = sizeof(Anim_tblLF_Jump) / sizeof(TRANSFORM);
			playType = APT_STOPF;
			break;
		case AnimStatus_BigJump:
			g_Parts[B].tbl_adr = Anim_tblB_BigJump;
			g_Parts[RH].tbl_adr = Anim_tblRH_BigJump;
			g_Parts[LH].tbl_adr = Anim_tblLH_BigJump;
			g_Parts[RF].tbl_adr = Anim_tblRF_BigJump;
			g_Parts[LF].tbl_adr = Anim_tblLF_BigJump;

			g_Parts[B].TDN = sizeof(Anim_tblB_BigJump) / sizeof(TRANSFORM);
			g_Parts[RH].TDN = sizeof(Anim_tblRH_BigJump) / sizeof(TRANSFORM);
			g_Parts[LH].TDN = sizeof(Anim_tblLH_BigJump) / sizeof(TRANSFORM);
			g_Parts[RF].TDN = sizeof(Anim_tblRF_BigJump) / sizeof(TRANSFORM);
			g_Parts[LF].TDN = sizeof(Anim_tblLF_BigJump) / sizeof(TRANSFORM);
			playType = APT_STOPF;
			break;
		case AnimStatus_Air:
			g_Parts[B].tbl_adr  = Anim_tblB_Air;
			g_Parts[RH].tbl_adr = Anim_tblRH_Air;
			g_Parts[LH].tbl_adr = Anim_tblLH_Air;
			g_Parts[RF].tbl_adr = Anim_tblRF_Air;
			g_Parts[LF].tbl_adr = Anim_tblLF_Air;

			g_Parts[B].TDN  = sizeof(Anim_tblB_Air) / sizeof(TRANSFORM);
			g_Parts[RH].TDN = sizeof(Anim_tblRH_Air) / sizeof(TRANSFORM);
			g_Parts[LH].TDN = sizeof(Anim_tblLH_Air) / sizeof(TRANSFORM);
			g_Parts[RF].TDN = sizeof(Anim_tblRF_Air) / sizeof(TRANSFORM);
			g_Parts[LF].TDN = sizeof(Anim_tblLF_Air) / sizeof(TRANSFORM);
			playType = APT_STOPF;
			break;
		case AnimStatus_Dash:
			g_Parts[B].tbl_adr  = Anim_tblB_Dash;
			g_Parts[RH].tbl_adr = Anim_tblRH_Dash;
			g_Parts[LH].tbl_adr = Anim_tblLH_Dash;
			g_Parts[RF].tbl_adr = Anim_tblRF_Dash;
			g_Parts[LF].tbl_adr = Anim_tblLF_Dash;

			g_Parts[B].TDN  = sizeof(Anim_tblB_Dash) / sizeof(TRANSFORM);
			g_Parts[RH].TDN = sizeof(Anim_tblRH_Dash) / sizeof(TRANSFORM);
			g_Parts[LH].TDN = sizeof(Anim_tblLH_Dash) / sizeof(TRANSFORM);
			g_Parts[RF].TDN = sizeof(Anim_tblRF_Dash) / sizeof(TRANSFORM);
			g_Parts[LF].TDN = sizeof(Anim_tblLF_Dash) / sizeof(TRANSFORM);
			playType = APT_CBR;
			break;
		case AnimStatus_ThrowCard:
			g_Parts[B].tbl_adr = Anim_tblB_ThrowCard;
			g_Parts[RH].tbl_adr = Anim_tblRH_ThrowCard;
			g_Parts[LH].tbl_adr = Anim_tblLH_ThrowCard;
			g_Parts[RF].tbl_adr = Anim_tblRF_ThrowCard;
			g_Parts[LF].tbl_adr = Anim_tblLF_ThrowCard;

			g_Parts[B].TDN = sizeof(Anim_tblB_ThrowCard) / sizeof(TRANSFORM);
			g_Parts[RH].TDN = sizeof(Anim_tblRH_ThrowCard) / sizeof(TRANSFORM);
			g_Parts[LH].TDN = sizeof(Anim_tblLH_ThrowCard) / sizeof(TRANSFORM);
			g_Parts[RF].TDN = sizeof(Anim_tblRF_ThrowCard) / sizeof(TRANSFORM);
			g_Parts[LF].TDN = sizeof(Anim_tblLF_ThrowCard) / sizeof(TRANSFORM);
			playType = APT_CBR;
			break;
		case AnimStatus_CatchCardJump:
			g_Parts[B].tbl_adr = Anim_tblB_CatchCardJump;
			g_Parts[RH].tbl_adr = Anim_tblRH_CatchCardJump;
			g_Parts[LH].tbl_adr = Anim_tblLH_CatchCardJump;
			g_Parts[RF].tbl_adr = Anim_tblRF_CatchCardJump;
			g_Parts[LF].tbl_adr = Anim_tblLF_CatchCardJump;

			g_Parts[B].TDN = sizeof(Anim_tblB_CatchCardJump) / sizeof(TRANSFORM);
			g_Parts[RH].TDN = sizeof(Anim_tblRH_CatchCardJump) / sizeof(TRANSFORM);
			g_Parts[LH].TDN = sizeof(Anim_tblLH_CatchCardJump) / sizeof(TRANSFORM);
			g_Parts[RF].TDN = sizeof(Anim_tblRF_CatchCardJump) / sizeof(TRANSFORM);
			g_Parts[LF].TDN = sizeof(Anim_tblLF_CatchCardJump) / sizeof(TRANSFORM);
			playType = APT_CBR;
			break;
		case AnimStatus_BobStatus:
			g_Parts[B].tbl_adr  =  Anim_tblB_BobStatus;
			g_Parts[RH].tbl_adr = Anim_tblRH_BobStatus;
			g_Parts[LH].tbl_adr = Anim_tblLH_BobStatus;
			g_Parts[RF].tbl_adr = Anim_tblRF_BobStatus;
			g_Parts[LF].tbl_adr = Anim_tblLF_BobStatus;

			g_Parts[B].TDN  = sizeof( Anim_tblB_BobStatus) / sizeof(TRANSFORM);
			g_Parts[RH].TDN = sizeof(Anim_tblRH_BobStatus) / sizeof(TRANSFORM);
			g_Parts[LH].TDN = sizeof(Anim_tblLH_BobStatus) / sizeof(TRANSFORM);
			g_Parts[RF].TDN = sizeof(Anim_tblRF_BobStatus) / sizeof(TRANSFORM);
			g_Parts[LF].TDN = sizeof(Anim_tblLF_BobStatus) / sizeof(TRANSFORM);
			playType = APT_LOOP;
			break;
		case AnimStatus_GetStar:
			g_Parts[B].tbl_adr  = Anim_tblB_GetStar;
			g_Parts[RH].tbl_adr = Anim_tblRH_GetStar;
			g_Parts[LH].tbl_adr = Anim_tblLH_GetStar;
			g_Parts[RF].tbl_adr = Anim_tblRF_GetStar;
			g_Parts[LF].tbl_adr = Anim_tblLF_GetStar;

			g_Parts[B].TDN  = sizeof(Anim_tblB_GetStar) / sizeof(TRANSFORM);
			g_Parts[RH].TDN = sizeof(Anim_tblRH_GetStar) / sizeof(TRANSFORM);
			g_Parts[LH].TDN = sizeof(Anim_tblLH_GetStar) / sizeof(TRANSFORM);
			g_Parts[RF].TDN = sizeof(Anim_tblRF_GetStar) / sizeof(TRANSFORM);
			g_Parts[LF].TDN = sizeof(Anim_tblLF_GetStar) / sizeof(TRANSFORM);
			playType = APT_CBR;
			break;
		}
	}
	AnimStatusPre = AnimStatus;
	//==================アニメーション変数更新
	//Anim Speed according to player speed
	AnimMovePar = 30.5f - (g_player->speedXZLengh / g_player->maxMoveSpeed) * 24.0f; // 8~ 36
	
#ifdef _DEBUG	
	char* str = GetDebugStr();
	//sprintf(&str[strlen(str)], " AnimMovePar: %f ", AnimMovePar);
#endif

	////================階層アニメーション更新
	int CBRcount = 0;
	for (int i = 0; i < PLAYER_PARTS_MAX; i++) {
		if (g_Parts[i].Use == true) {
			// 移動処理
			int		index = (int)g_Parts[i].move_time;
			float	time = g_Parts[i].move_time - index; // 0 ~ 1
			int		size = g_Parts[i].TDN; // sizeof(g_Parts[i].tbl_adr) / sizeof(TRANSFORM)

			float dt = 1.0f / g_Parts[i].tbl_adr[index].frame;	// 1フレームで進める時間
			if (AnimStatus == AnimStatus_Move) dt = 1.0f / AnimMovePar; //Anim Speed according to player speed
			
			g_Parts[i].move_time += dt;					// アニメーションの合計時間に足す

			if (index > (size - 2))	// ゴールをオーバーしていたら、ゴールへ戻す
			{
				if (playType == APT_LOOP) {
					g_Parts[i].move_time = 0.0f;
					index = 0;
				}
				else if (playType == APT_STOPF) {
					index = (size - 2);
					if (index < 0)index = 0;
					g_Parts[i].move_time -= dt;
				}
				else if (playType == APT_CBR) {
					index = (size - 2);
					if (index < 0)index = 0;
					g_Parts[i].move_time -= dt;
					CBRcount++;
				}
			}

			if (size != 1) {
				// 座標を求める	X = StartX + (EndX - StartX) * 今の時間
				D3DXVECTOR3 vec = g_Parts[i].tbl_adr[index + 1].pos - g_Parts[i].tbl_adr[index].pos;
				g_Parts[i].pos = g_Parts[i].tbl_adr[index].pos + vec * time;

				// 回転を求める	R = StartX + (EndX - StartX) * 今の時間
				D3DXVECTOR3 rot = g_Parts[i].tbl_adr[index + 1].rot - g_Parts[i].tbl_adr[index].rot;
				g_Parts[i].rot = g_Parts[i].tbl_adr[index].rot + rot * time;

				// scaleを求める S = StartX + (EndX - StartX) * 今の時間
				D3DXVECTOR3 scl = g_Parts[i].tbl_adr[index + 1].scl - g_Parts[i].tbl_adr[index].scl;
				g_Parts[i].scl = g_Parts[i].tbl_adr[index].scl + scl * time;
			}

		}
	}
	if (CBRcount >= PLAYER_PARTS_MAX) { // every parts end
		playType = APT_STOPF; // stop CBR status
		if (AnimStatus == AnimStatus_CatchCardJump) {
			setStartMoveTime(1.0f); // Start from no.2 Anim => for jump
		}
	}
}

void DrawPlayerPartAnim(void)
{
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;

	//階層アニメーション 親を描画してから、PARTS(子)の描画
	for (int i = 0; i < PLAYER_PARTS_MAX; i++) {

		// 使われているなら処理する
		if (g_Parts[i].Use == false) continue;

		// ワールドマトリックスの初期化
		D3DXMatrixIdentity(&g_Parts[i].mtxWorld);

		// スケールを反映
		D3DXMatrixScaling(&mtxScl, g_Parts[i].scl.x, g_Parts[i].scl.y, g_Parts[i].scl.z);
		D3DXMatrixMultiply(&g_Parts[i].mtxWorld, &g_Parts[i].mtxWorld, &mtxScl);

		// 回転を反映
		D3DXMatrixRotationYawPitchRoll(&mtxRot, g_Parts[i].rot.y, g_Parts[i].rot.x, g_Parts[i].rot.z);
		D3DXMatrixMultiply(&g_Parts[i].mtxWorld, &g_Parts[i].mtxWorld, &mtxRot);

		// 移動を反映
		D3DXMatrixTranslation(&mtxTranslate, g_Parts[i].pos.x, g_Parts[i].pos.y, g_Parts[i].pos.z);
		D3DXMatrixMultiply(&g_Parts[i].mtxWorld, &g_Parts[i].mtxWorld, &mtxTranslate);

		if (g_Parts[i].parent != NULL)	// 子供だったら親と結合する
		{
			D3DXMatrixMultiply(&g_Parts[i].mtxWorld, &g_Parts[i].mtxWorld, &g_Parts[i].parent->mtxWorld);
		}

		if (g_Parts[i].root_parent != NULL)	// 子供だったら親と結合する
		{
			D3DXMatrixMultiply(&g_Parts[i].mtxWorld, &g_Parts[i].mtxWorld, &g_Parts[i].root_parent->mtxWorld);
		}


		// ワールドマトリックスの設定
		SetWorldMatrix(&g_Parts[i].mtxWorld);

		// モデル描画
		DrawModel(&g_Parts[i].model);
	}
}

ANIM_PLAY_TYPE GetAnimPlayType()
{
	return playType;
}

void setStartMoveTime(float i)
{
	Anim_StartFrom_MoveTime = i;
}
