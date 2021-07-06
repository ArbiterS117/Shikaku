//=============================================================================
//
// ゲームエディターモードの入力処理 [EditToolInput.h]
// Author : GP11B132 14 ゴショウケン
//
//=============================================================================
#pragma once

#include "main.h"

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
int GetNewPlatformType();
int GetNewEnemyType();

//MainEditMenu
void MainEditInputUpdate();
void LoadAndRebuildData();
//PlatFormEditMenu
void PlatFormModeInputUpdate();
//EnemyEditMenu
void EnemyModeInputUpdate();
