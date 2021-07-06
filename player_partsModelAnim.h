//=============================================================================
//
// プレーヤパーツアニメ処理 [player_partsModelAnim.h]
// Author : GP11B132 14 ゴショウケン
//
//=============================================================================
#pragma once

#include "player.h"

enum ENUMAnimStatus {
	AnimStatus_Idel,
	AnimStatus_Move,
	AnimStatus_FastMove,
	AnimStatus_Jump,
	AnimStatus_BigJump,
	AnimStatus_Air,
	AnimStatus_Dash,
	AnimStatus_ThrowCard,
	AnimStatus_CatchCardJump,
	AnimStatus_BobStatus,
	AnimStatus_GetStar,
};

enum ANIM_PLAY_TYPE {
	APT_LOOP = -1,	 // loop
	APT_STOPF = -2,	 // stop in final
	APT_CBR = -3,	 // Can't be replace
};

struct PLAYER_PART_ANIM
{
	bool                Use;
	int                 TDN;                //Tabel Data MAX number
	float               move_time;

	D3DXVECTOR3			pos;				// モデルの位置
	D3DXVECTOR3			rot;				// モデルの向き(回転)
	D3DXVECTOR3			scl;				// モデルの大きさ(スケール)
	D3DXMATRIX			mtxWorld;			// ワールドマトリックス
	DX11_MODEL			model;				// モデル情報
	
	//階層アニメーション
	TRANSFORM *tbl_adr;
	
	//親は、NULL、子供は親のアドレスを入れる
	PLAYER_PART_ANIM    *parent;
	PLAYER    *root_parent; // PLAYER と 繋がる

};

void InitPlayerPartAnim(void);
void UninitPlayerPartAnim(void);
void UpdatePlayerPartAnim(void);
void DrawPlayerPartAnim(void);

ANIM_PLAY_TYPE GetAnimPlayType();
void setStartMoveTime(float i);