//=============================================================================
//
// ゲームエディターモードのディスプレイ出力 [EditToolDebugProc.cpp]
// Author : GP11B132 14 ゴショウケン
//
//=============================================================================
#include "EditToolDebugProc.h"
#include "EditTool.h"
#include "platform.h"
#include "enemy.h"
#include "debugproc.h"
#include "EditToolInput.h"
//=============================================================================
//=============================================================================
// MainEditMenuDebugProc
//=============================================================================
//=============================================================================
void MainEditDebugProc()
{
	switch (GetCurrentSelectMenu())
	{
	case EditMenu_Platform:
		PrintDebugProc("======= <- CurrentMode: ");
		PrintDebugProc(" Platform -> =======\n");
		PrintDebugProc("LeftMouse: Select Object\nRightMouse: Create Platform\n9:Save\n8:Load\n");
		PrintDebugProc("Platform Type: ");
		if(GetNewPlatformType() == PlatformType_normal)PrintDebugProc("Normal\n");
		if(GetNewPlatformType() == PlatformType_ice)PrintDebugProc("Ice\n");
		if(GetNewPlatformType() == PlatformType_t1)PrintDebugProc("t1\n");
		if(GetNewPlatformType() == PlatformType_t2)PrintDebugProc("t2\n");
		if(GetNewPlatformType() == PlatformType_t3)PrintDebugProc("t3\n");
		if(GetNewPlatformType() == PlatformType_block)PrintDebugProc("block\n");
		if(GetNewPlatformType() == PlatformType_pyramid)PrintDebugProc("pyramid\n");
		if(GetNewPlatformType() == PlatformType_bigTree)PrintDebugProc("BigTree\n");
		if(GetNewPlatformType() == PlatformType_bigSnowMan)PrintDebugProc("BigSnowMan\n");
		if(GetNewPlatformType() == PlatformType_mountain)PrintDebugProc("Mountain\n");
		if(GetNewPlatformType() == PlatformType_t4)PrintDebugProc("t4\n");
		if(GetNewPlatformType() == PlatformType_CurveG)PrintDebugProc("CurveG\n");
		if(GetNewPlatformType() == PlatformType_grassBlock)PrintDebugProc("grassBlock\n");
		if(GetNewPlatformType() == PlatformType_hill)PrintDebugProc("hill\n");
		if(GetNewPlatformType() == PlatformType_pokerBlock)PrintDebugProc("pokerBlock\n");
		if(GetNewPlatformType() == PlatformType_slider)PrintDebugProc("slider\n");
		if(GetNewPlatformType() == PlatformType_snowMountain)PrintDebugProc("snowMountain\n");
		break;
	case EditMenu_PlatformType:
		PrintDebugProc("======= CurrentMode: ");
		PrintDebugProc(" Platform =======\n");
		PrintDebugProc("LeftMouse: Select Object\nRightMouse: Create Platform\n9:Save\n8:Load\n");
		PrintDebugProc("### <- Platform Type: ");
		if (GetNewPlatformType() == PlatformType_normal)PrintDebugProc("Normal ->\n");
		if (GetNewPlatformType() == PlatformType_ice)  PrintDebugProc("Ice ->\n");
		if (GetNewPlatformType() == PlatformType_t1)  PrintDebugProc("t1 ->\n");
		if (GetNewPlatformType() == PlatformType_t2)  PrintDebugProc("t2 ->\n");
		if (GetNewPlatformType() == PlatformType_t3)  PrintDebugProc("t3 ->\n");
		if (GetNewPlatformType() == PlatformType_block)  PrintDebugProc("block ->\n");
		if (GetNewPlatformType() == PlatformType_pyramid)  PrintDebugProc("pyramid ->\n");
		if (GetNewPlatformType() == PlatformType_bigTree)  PrintDebugProc("BigTree ->\n");
		if (GetNewPlatformType() == PlatformType_bigSnowMan)  PrintDebugProc("BigSnowMan ->\n");
		if (GetNewPlatformType() == PlatformType_mountain)  PrintDebugProc("Mountain ->\n");
		if (GetNewPlatformType() == PlatformType_t4)  PrintDebugProc("t4 ->\n");
		if (GetNewPlatformType() == PlatformType_CurveG)PrintDebugProc("CurveG ->\n");
		if (GetNewPlatformType() == PlatformType_grassBlock)PrintDebugProc("grassBlock ->\n");
		if (GetNewPlatformType() == PlatformType_hill)PrintDebugProc("hill ->\n");
		if (GetNewPlatformType() == PlatformType_pokerBlock)PrintDebugProc("pokerBlock ->\n");
		if (GetNewPlatformType() == PlatformType_slider)PrintDebugProc("slider ->\n");
		if (GetNewPlatformType() == PlatformType_snowMountain)PrintDebugProc("snowMountain ->\n");
		break;
	case EditMenu_Enemy:
		PrintDebugProc("======= <- CurrentMode: ");
		PrintDebugProc(" Enemy -> =======\n");
		PrintDebugProc("LeftMouse: Select Object\nRightMouse: Create Enemy\n9:Save\n8:Load\n");
		PrintDebugProc("Enemy Type: ");
		if (GetNewEnemyType() == EnemyType_mushroom)PrintDebugProc("MUSHROOM\n");
		if (GetNewEnemyType() == EnemyType_wind)    PrintDebugProc("WIND\n");
		if (GetNewEnemyType() == EnemyType_water)   PrintDebugProc("WATER\n");
		if (GetNewEnemyType() == EnemyType_gem)     PrintDebugProc("GEM\n");
		if (GetNewEnemyType() == EnemyType_star)   PrintDebugProc("STAR\n");
		if (GetNewEnemyType() == EnemyType_coin)   PrintDebugProc("COIN\n");
		if (GetNewEnemyType() == EnemyType_quickSand)   PrintDebugProc("QUICKSAND\n");
		if (GetNewEnemyType() == EnemyType_mushroomSpring)   PrintDebugProc("MUSHROOM SPRING\n");
		if (GetNewEnemyType() == EnemyType_Sign)   PrintDebugProc("SIGN\n");
		break;
	case EditMenu_EnemyType:
		PrintDebugProc("======= CurrentMode: ");
		PrintDebugProc(" Enemy =======\n");
		PrintDebugProc("LeftMouse: Select Object\nRightMouse: Create Enemy\n9:Save\n8:Load\n");
		PrintDebugProc("### <- Enemy Type: ");
		if (GetNewEnemyType() == EnemyType_mushroom)PrintDebugProc("MUSHROOM ->\n");
		if (GetNewEnemyType() == EnemyType_wind)    PrintDebugProc("WIND ->\n");
		if (GetNewEnemyType() == EnemyType_water)   PrintDebugProc("WATER ->\n");
		if (GetNewEnemyType() == EnemyType_gem)     PrintDebugProc("GEM ->\n");
		if (GetNewEnemyType() == EnemyType_star)   PrintDebugProc("STAR ->\n");
		if (GetNewEnemyType() == EnemyType_coin)   PrintDebugProc("COIN ->\n");
		if (GetNewEnemyType() == EnemyType_quickSand)   PrintDebugProc("QUICKSAND ->\n");
		if (GetNewEnemyType() == EnemyType_mushroomSpring)   PrintDebugProc("MUSHROOM SPRING ->\n");
		if (GetNewEnemyType() == EnemyType_Sign)   PrintDebugProc("SIGN ->\n");
		break;
	default:
		break;
	}
}
//=============================================================================
//=============================================================================
// PlatFormModeDebugProc
//=============================================================================
//=============================================================================
void PlatFormModeDebugProc() {
	PLATFORM *platform = GetPlatform();
	if (GetGameSceneEditMode() && GetPlatFormDebugCtrlID() != -1) {
		PrintDebugProc("LeftMouse: Place Object\nRightMouse: Delete Platform\n");
		PrintDebugProc("======= Now Edit: %d =======\n", platform[GetPlatFormDebugCtrlID()].platformID);
		PrintDebugProc("Position X: %f Y: %f Z: %f\n", platform[GetPlatFormDebugCtrlID()].pos.x, platform[GetPlatFormDebugCtrlID()].pos.y, platform[GetPlatFormDebugCtrlID()].pos.z);
		PrintDebugProc("SCALE X: %f Y: %f Z: %f\n", platform[GetPlatFormDebugCtrlID()].scl.x, platform[GetPlatFormDebugCtrlID()].scl.y, platform[GetPlatFormDebugCtrlID()].scl.z);
		if (GetCurrentSelectMenu() == PlatformEditMenu_Type) {
			PrintDebugProc("### <- Platform Type: ");
			if (platform[GetPlatFormDebugCtrlID()].platformType == PlatformType_normal)      PrintDebugProc("Normal ->\n");
			if (platform[GetPlatFormDebugCtrlID()].platformType == PlatformType_ice)         PrintDebugProc("Ice ->\n");
			if (platform[GetPlatFormDebugCtrlID()].platformType == PlatformType_t1)          PrintDebugProc("t1 ->\n");
			if (platform[GetPlatFormDebugCtrlID()].platformType == PlatformType_t2)          PrintDebugProc("t2 ->\n");
			if (platform[GetPlatFormDebugCtrlID()].platformType == PlatformType_t3)          PrintDebugProc("t3 ->\n");
			if (platform[GetPlatFormDebugCtrlID()].platformType == PlatformType_block)       PrintDebugProc("block ->\n");
			if (platform[GetPlatFormDebugCtrlID()].platformType == PlatformType_pyramid)     PrintDebugProc("pyramid ->\n");
			if (platform[GetPlatFormDebugCtrlID()].platformType == PlatformType_bigTree)     PrintDebugProc("BigTree ->\n");
			if (platform[GetPlatFormDebugCtrlID()].platformType == PlatformType_bigSnowMan)  PrintDebugProc("BigSnowMan ->\n");
			if (platform[GetPlatFormDebugCtrlID()].platformType == PlatformType_mountain)    PrintDebugProc("Mountain ->\n");
			if (platform[GetPlatFormDebugCtrlID()].platformType == PlatformType_t4)          PrintDebugProc("t4 ->\n");
			if (platform[GetPlatFormDebugCtrlID()].platformType == PlatformType_CurveG)      PrintDebugProc("CurveG ->\n");
			if (platform[GetPlatFormDebugCtrlID()].platformType == PlatformType_grassBlock)  PrintDebugProc("grassBlock ->\n");
			if (platform[GetPlatFormDebugCtrlID()].platformType == PlatformType_hill)        PrintDebugProc("hill ->\n");
			if (platform[GetPlatFormDebugCtrlID()].platformType == PlatformType_pokerBlock)  PrintDebugProc("pokerBlock ->\n");
			if (platform[GetPlatFormDebugCtrlID()].platformType == PlatformType_slider)      PrintDebugProc("slider ->\n");
			if (platform[GetPlatFormDebugCtrlID()].platformType == PlatformType_snowMountain)PrintDebugProc("snowMountain ->\n");
		}
		else {
			PrintDebugProc("Platform Type: ");
			if (platform[GetPlatFormDebugCtrlID()].platformType == PlatformType_normal)      PrintDebugProc("Normal \n");
			if (platform[GetPlatFormDebugCtrlID()].platformType == PlatformType_ice)         PrintDebugProc("Ice \n");
			if (platform[GetPlatFormDebugCtrlID()].platformType == PlatformType_t1)          PrintDebugProc("t1 \n");
			if (platform[GetPlatFormDebugCtrlID()].platformType == PlatformType_t2)          PrintDebugProc("t2 \n");
			if (platform[GetPlatFormDebugCtrlID()].platformType == PlatformType_t3)          PrintDebugProc("t3 \n");
			if (platform[GetPlatFormDebugCtrlID()].platformType == PlatformType_block)       PrintDebugProc("block \n");
			if (platform[GetPlatFormDebugCtrlID()].platformType == PlatformType_pyramid)     PrintDebugProc("pyramid \n");
			if (platform[GetPlatFormDebugCtrlID()].platformType == PlatformType_bigTree)     PrintDebugProc("BigTree \n");
			if (platform[GetPlatFormDebugCtrlID()].platformType == PlatformType_bigSnowMan)  PrintDebugProc("BigSnowMan \n");
			if (platform[GetPlatFormDebugCtrlID()].platformType == PlatformType_t4)          PrintDebugProc("t4 \n");
			if (platform[GetPlatFormDebugCtrlID()].platformType == PlatformType_CurveG)      PrintDebugProc("CurveG\n");
			if (platform[GetPlatFormDebugCtrlID()].platformType == PlatformType_grassBlock)  PrintDebugProc("grassBlock\n");
			if (platform[GetPlatFormDebugCtrlID()].platformType == PlatformType_hill)        PrintDebugProc("hill\n");
			if (platform[GetPlatFormDebugCtrlID()].platformType == PlatformType_pokerBlock)  PrintDebugProc("pokerBlock\n");
			if (platform[GetPlatFormDebugCtrlID()].platformType == PlatformType_slider)      PrintDebugProc("slider\n");
			if (platform[GetPlatFormDebugCtrlID()].platformType == PlatformType_snowMountain)PrintDebugProc("snowMountain\n");
		}

		if (GetCurrentSelectMenu() == PlatformEditMenu_CanmoveAxis) PrintDebugProc("### <- ");
		switch (platform[GetPlatFormDebugCtrlID()].canmoveAxis)
		{
		case 0:
			if (GetCurrentSelectMenu() == PlatformEditMenu_CanmoveAxis) PrintDebugProc("CanMoveAxis: X ->\n");
			else PrintDebugProc("CanMoveAxis: X\n");
			break;
		case 1:
			if (GetCurrentSelectMenu() == PlatformEditMenu_CanmoveAxis) PrintDebugProc("CanMoveAxis: Y ->\n");
			else PrintDebugProc("CanMoveAxis: Y\n");
			break;
		case 2:
			if (GetCurrentSelectMenu() == PlatformEditMenu_CanmoveAxis) PrintDebugProc("CanMoveAxis: Z ->\n");
			else PrintDebugProc("CanMoveAxis: Z\n");
			break;
		case -1:
			if (GetCurrentSelectMenu() == PlatformEditMenu_CanmoveAxis) PrintDebugProc("CanMoveAxis: NONE ->\n");
			else PrintDebugProc("CanMoveAxis: NONE\n");
			break;
		default:
			break;
		}

		if (GetCurrentSelectMenu() == PlatformEditMenu_CanspinAxis) PrintDebugProc("### <- ");
		switch (platform[GetPlatFormDebugCtrlID()].canspinAxis)
		{
		case 0:
			if (GetCurrentSelectMenu() == PlatformEditMenu_CanspinAxis) PrintDebugProc("CanSpinAxis: X ->\n");
			else PrintDebugProc("CanSpinAxis: X\n");
			break;
		case 1:
			if (GetCurrentSelectMenu() == PlatformEditMenu_CanspinAxis) PrintDebugProc("CanSpinAxis: Y ->\n");
			else PrintDebugProc("CanSpinAxis: Y\n");
			break;
		case 2:
			if (GetCurrentSelectMenu() == PlatformEditMenu_CanspinAxis) PrintDebugProc("CanSpinAxis: Z ->\n");
			else PrintDebugProc("CanSpinAxis: Z\n");
			break;
		case -1:
			if (GetCurrentSelectMenu() == PlatformEditMenu_CanspinAxis) PrintDebugProc("CanSpinAxis: NONE ->\n");
			else PrintDebugProc("CanSpinAxis: NONE\n");
			break;
		default:
			break;
		}

		if (GetCurrentSelectMenu() == PlatformEditMenu_Scale)  PrintDebugProc("### <- ");
		switch (platform[GetPlatFormDebugCtrlID()].scaleAxis)
		{
		case 0:
			if (GetCurrentSelectMenu() == PlatformEditMenu_Scale)  PrintDebugProc("ScaleAxis: X -> 2: Smaller  3: Bigger\n");
			else PrintDebugProc("ScaleAxis: X\n");
			break;
		case 1:
			if (GetCurrentSelectMenu() == PlatformEditMenu_Scale)  PrintDebugProc("ScaleAxis: Y -> 2: Smaller  3: Bigger\n");
			else PrintDebugProc("ScaleAxis: Y\n");
			break;
		case 2:
			if (GetCurrentSelectMenu() == PlatformEditMenu_Scale)  PrintDebugProc("ScaleAxis: Z -> 2: Smaller  3: Bigger\n");
			else PrintDebugProc("ScaleAxis: Z\n");
			break;
		case -1:
			if (GetCurrentSelectMenu() == PlatformEditMenu_Scale)  PrintDebugProc("ScaleAxis: ALL -> 2: Smaller  3: Bigger\n");
			else PrintDebugProc("ScaleAxis: ALL\n");
			break;
		default:
			break;
		}

		if (GetCurrentSelectMenu() == PlatformEditMenu_Color) PrintDebugProc("### <- ");
		switch (platform[GetPlatFormDebugCtrlID()].colorType)
		{
		case 0:
			if (GetCurrentSelectMenu() == PlatformEditMenu_Color) PrintDebugProc("Color: PEN ->\n");
			else PrintDebugProc("Color: PEN\n");
			break;
		case 1:
			if (GetCurrentSelectMenu() == PlatformEditMenu_Color) PrintDebugProc("Color: WHITE ->\n");
			else PrintDebugProc("Color: WHITE\n");
			break;
		case 2:
			if (GetCurrentSelectMenu() == PlatformEditMenu_Color) PrintDebugProc("Color: BLACK ->\n");
			else PrintDebugProc("Color: BLACK\n");
			break;
		case 3:
			if (GetCurrentSelectMenu() == PlatformEditMenu_Color) PrintDebugProc("Color: RED ->\n");
			else PrintDebugProc("Color: RED\n");
			break;
		default:
			break;
		}

		if (GetCurrentSelectMenu() == PlatformEditMenu_Rot) PrintDebugProc("### RotMode UJHK to Rot\n");
		else PrintDebugProc("RotMode\n");


	}
}
//=============================================================================
//=============================================================================
// EnemyModeDebugProc
//=============================================================================
//=============================================================================
void EnemyModeDebugProc() {
	ENEMY *enemy = GetEnemy();
	if (GetGameSceneEditMode() && GetEnemyDebugCtrlID() != -1) {
		PrintDebugProc("LeftMouse: Place Object\nRightMouse: Delete Enemy\n");
		PrintDebugProc("======= Now Edit: %d =======\n", enemy[GetEnemyDebugCtrlID()].enemyID);
		PrintDebugProc("Position X: %f Y: %f Z: %f\n", enemy[GetEnemyDebugCtrlID()].pos.x, enemy[GetEnemyDebugCtrlID()].pos.y, enemy[GetEnemyDebugCtrlID()].pos.z);
		PrintDebugProc("SCALE X: %f Y: %f Z: %f\n", enemy[GetEnemyDebugCtrlID()].scl.x, enemy[GetEnemyDebugCtrlID()].scl.y, enemy[GetEnemyDebugCtrlID()].scl.z);

		if (GetCurrentSelectMenu() == EnemyEditMenu_Type) PrintDebugProc("### <- ");
		switch (enemy[GetEnemyDebugCtrlID()].enemyType)
		{
		case EnemyType_mushroom:
			if (GetCurrentSelectMenu() == EnemyEditMenu_Type) PrintDebugProc("Type: MUSHROOM ->\n");
			else PrintDebugProc("Type: MUSHROOM\n");
			break;
		case EnemyType_wind:
			if (GetCurrentSelectMenu() == EnemyEditMenu_Type) PrintDebugProc("Type: WIND ->\n");
			else PrintDebugProc("Type: WIND\n");
			break;
		case EnemyType_water:
			if (GetCurrentSelectMenu() == EnemyEditMenu_Type) PrintDebugProc("Type: WATER ->\n");
			else PrintDebugProc("Type: WATER\n");
			break;
		case EnemyType_gem:
			if (GetCurrentSelectMenu() == EnemyEditMenu_Type) PrintDebugProc("Type: GEM ->\n");
			else PrintDebugProc("Type: GEM\n");
			break;
		case EnemyType_star:
			if (GetCurrentSelectMenu() == EnemyEditMenu_Type) PrintDebugProc("Type: STAR ->\n");
			else PrintDebugProc("Type: STAR\n");
			break;
		case EnemyType_coin:
			if (GetCurrentSelectMenu() == EnemyEditMenu_Type) PrintDebugProc("Type: COIN ->\n");
			else PrintDebugProc("Type: COIN\n");
			break; 
		case EnemyType_quickSand:
			if (GetCurrentSelectMenu() == EnemyEditMenu_Type) PrintDebugProc("Type: QUICKSAND ->\n");
			else PrintDebugProc("Type: QUICKSAND\n");
			break; 
		case EnemyType_mushroomSpring:
			if (GetCurrentSelectMenu() == EnemyEditMenu_Type) PrintDebugProc("Type: MUSHROOM SPRING ->\n");
			else PrintDebugProc("Type: MUSHROOM SPRING\n");
			break;
		case EnemyType_Sign:
			if (GetCurrentSelectMenu() == EnemyEditMenu_Type) PrintDebugProc("Type: SIGN ->\n");
			else PrintDebugProc("Type: SIGN\n");
			break;
		default:
			break;
		}

		if (GetCurrentSelectMenu() == EnemyEditMenu_CanmoveAxis) PrintDebugProc("### <- ");
		switch (enemy[GetEnemyDebugCtrlID()].canmoveAxis)
		{
		case 0:
			if (GetCurrentSelectMenu() == EnemyEditMenu_CanmoveAxis) PrintDebugProc("CanMoveAxis: X ->\n");
			else PrintDebugProc("CanMoveAxis: X\n");
			break;
		case 1:
			if (GetCurrentSelectMenu() == EnemyEditMenu_CanmoveAxis) PrintDebugProc("CanMoveAxis: Y ->\n");
			else PrintDebugProc("CanMoveAxis: Y\n");
			break;
		case 2:
			if (GetCurrentSelectMenu() == EnemyEditMenu_CanmoveAxis) PrintDebugProc("CanMoveAxis: Z ->\n");
			else PrintDebugProc("CanMoveAxis: Z\n");
			break;
		case -1:
			if (GetCurrentSelectMenu() == EnemyEditMenu_CanmoveAxis) PrintDebugProc("CanMoveAxis: NONE ->\n");
			else PrintDebugProc("CanMoveAxis: NONE\n");
			break;
		default:
			break;
		}

		if (GetCurrentSelectMenu() == EnemyEditMenu_Scale)  PrintDebugProc("### <- ");
		switch (enemy[GetEnemyDebugCtrlID()].scaleAxis)
		{
		case 0:
			if (GetCurrentSelectMenu() == EnemyEditMenu_Scale)  PrintDebugProc("ScaleAxis: X -> 2: Smaller  3: Bigger\n");
			else PrintDebugProc("ScaleAxis: X\n");
			break;
		case 1:
			if (GetCurrentSelectMenu() == EnemyEditMenu_Scale)  PrintDebugProc("ScaleAxis: Y -> 2: Smaller  3: Bigger\n");
			else PrintDebugProc("ScaleAxis: Y\n");
			break;
		case 2:
			if (GetCurrentSelectMenu() == EnemyEditMenu_Scale)  PrintDebugProc("ScaleAxis: Z -> 2: Smaller  3: Bigger\n");
			else PrintDebugProc("ScaleAxis: Z\n");
			break;
		case -1:
			if (GetCurrentSelectMenu() == EnemyEditMenu_Scale)  PrintDebugProc("ScaleAxis: ALL -> 2: Smaller  3: Bigger\n");
			else PrintDebugProc("ScaleAxis: ALL\n");
			break;
		default:
			break;
		}

		if (GetCurrentSelectMenu() == EnemyEditMenu_CanChase)  PrintDebugProc("### <- ");
		switch (enemy[GetEnemyDebugCtrlID()].CanChase)
		{
		case true:
			if (GetCurrentSelectMenu() == EnemyEditMenu_CanChase)  PrintDebugProc("CanChase: true -> \n");
			else PrintDebugProc("CanChase: true\n");
			break;
		case false:
			if (GetCurrentSelectMenu() == EnemyEditMenu_CanChase)  PrintDebugProc("CanChase: false -> \n");
			else PrintDebugProc("CanChase: false\n");
			break;
		default:
			break;
		}
	}
}
