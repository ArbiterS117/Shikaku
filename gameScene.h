//=============================================================================
//
// ゲーム画面処理 [gameScene.h]
// Author : GP11B132 14 ゴショウケン
//
//=============================================================================
#pragma once

#include "main.h"

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitGameScene(void);
void UninitGameScene(void);
void UpdateGameScene(void);
void DrawGameScene(void);

void EffectStopFrame(int frame); // number of stop frames
bool IsStoppingFrame();

//bool IsGameSceneEditMode();
//void SetGameSceneEditMenuSelectedID(int num);