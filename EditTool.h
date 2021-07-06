//=============================================================================
//
// ゲームエディターモードの処理 [EditTool.h]
// Author : GP11B132 14 ゴショウケン
//
//=============================================================================
#pragma once

#include "main.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	VALUE_EDIT_SCALE_SPEED        (0.5)
#define	VALUE_RAYCASTBULLET_SPEED     (200)
#define	VALUE_RAYCASTBULLET_SHOOTTIME (60)
#define	VALUE_RAYCASTOBJDIS_DEFULT    (1700)
#define	VALUE_MOVE			          (25.0f)	// 移動量
//debug
#define SLProcAppearTime (180)

enum EditMenuID // MIN, MAX => for loot
{
	//MainEditMode
	EditMenu_MIN,
	EditMenu_Platform,
	EditMenu_Enemy,
	EditMenu_PlatformType,
	EditMenu_EnemyType,
	EditMenu_MAX,
	//PlatformMode
	PlatformEditMenu_MIN,
	PlatformEditMenu_Type,
	PlatformEditMenu_CanmoveAxis,
	PlatformEditMenu_CanspinAxis,
	PlatformEditMenu_Scale,
	PlatformEditMenu_Color,
	PlatformEditMenu_Rot,
	PlatformEditMenu_MAX,
	//EnemyMode
	EnemyEditMenu_MIN,
	EnemyEditMenu_Type,
	EnemyEditMenu_CanmoveAxis,
	EnemyEditMenu_Scale,
	EnemyEditMenu_CanChase,
	EnemyEditMenu_MAX,

};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitEditTool(void);
void UninitEditTool(void);
void UpdateEditTool(void);
void DrawEditTool(void);

bool GetGameSceneEditMode();

int  GetCurrentSelectMenu();
void SetCurrentSelectMenu(int num, bool setPre = true);
int  GetPreSelectMenu();
void SetPreSelectMenu(int num);
void AddCurrentSelectMenu();
void MinusCurrentSelectMenu();

//rayCast
int  GetRayCastObjDis();
void SetRayCastObjDis(float dis);
void SetRayCastBulletDir(D3DXVECTOR3 dir);
void SetRayCastBulletisShooting(bool b);

//Timer 
int  GetSLProcAppearTimer();
void SetSLProcAppearTimer(int num);


//platMode
int GetPlatFormDebugCtrlID(void);
void SetPlatFormDebugCtrlID(int num);

//EnemyMode
int GetEnemyDebugCtrlID(void);
void SetEnemyDebugCtrlID(int num);
