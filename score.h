//=============================================================================
//
// スコア処理 [score.h]
// Author : GP11B132 14 ゴショウケン
//
//=============================================================================
#pragma once

#include "main.h"
#include "renderer.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define SCORE_MAX			(99)		// スコアの最大値
#define SCORE_DIGIT			(2)			// 桁数


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitScore(void);
void UninitScore(void);
void UpdateScore(void);
void DrawScore(void);

void AddScore(int add);
void AddCoin(int add);
void AddAllScore(int add);
void AddAllCoin(int add);
void AddMiss(int add);
void AddScoreTime(int sec);
int GetScore();
int GetCoin();
int GetAllScore();
int GetAllCoin();
int GetMiss();
int GetGameTimeSec();
int GetGameTimeMin();

void showSign(int i);


