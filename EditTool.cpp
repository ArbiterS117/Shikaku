//=============================================================================
//
// ゲームエディターモードの処理 [EditTool.cpp]
// Author : GP11B132 14 ゴショウケン
//
//=============================================================================

#include "EditTool.h"
#include "input.h"
#include "camera.h"
#include "Collider3D.h"
#include "DebugProc.h"
#include "platform.h"
#include "groundFile.h"
#include "EditToolInput.h"
#include "EditToolDebugProc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
//RayCast
static void EditModeCollisionUpdate();
//MainProcess
static void InputUpdate();
static void DebugProc();

static int SLProcAppearTimer = 0;
//*****************************************************************************
// グローバル変数
//*****************************************************************************
//======Edit Mode
//MainEditMode
static bool EditMode = false;

static int CurrentSelectMenu;
static int PreSelectMenu;

//GrabThings
static int PlatFormDebugCtrlID = -1;       // Now controlling plat  ID
static int EnemyDebugCtrlID = -1;          // Now controlling enemy ID

//====EditMode
int EditRayCastCIndex;
D3DXVECTOR3 rayCastBulletDir;
bool rayCastBulletisShooting = false;
int rayCastBulletShootTimer = 0;
static float rayCastObjDis = VALUE_RAYCASTOBJDIS_DEFULT;

//=============================================================================
// 初期化処理
//=============================================================================
void InitEditTool(void)
{
	EditRayCastCIndex = CreateCollider3DCircle(collider3DTag_platEditRaycast, GetCamera()->pos, 60.0f, 0);
	COLLIDER3D *collider;
	collider = GetCollider();
	collider[EditRayCastCIndex].canDraw = false; // do Not Draw collider

	//EditTool
	CurrentSelectMenu = PreSelectMenu = EditMenu_Platform;
}
//=============================================================================
// 終了処理
//=============================================================================
void UninitEditTool(void)
{
}
//=============================================================================
// 更新処理
//=============================================================================
void UpdateEditTool(void)
{
	InputUpdate();
	EditModeCollisionUpdate();
}
//=============================================================================
// 描画処理
//=============================================================================
void DrawEditTool(void)
{
	DebugProc();
}
//=============================================================================
// other
//=============================================================================
void InputUpdate() {
	//Enter EditMode => TAB
	if (GetCamera()->TPMode) {
		EditMode = false;
	}
	else EditMode = true;
	if (!GetCamera()->canMove) EditMode = false;

	if (EditMode == false) { // Exit EditMode
		if (PlatFormDebugCtrlID != -1) {
			PLATFORM *platform = GetPlatform();
			ReleasePlatform(PlatFormDebugCtrlID);
			SetCurrentSelectMenu(PreSelectMenu);
			PlatFormDebugCtrlID = -1;
		}
		if (EnemyDebugCtrlID != -1) {
			ENEMY *enemy = GetEnemy();
			ReleaseEnemy(EnemyDebugCtrlID);
			SetCurrentSelectMenu(PreSelectMenu);
			EnemyDebugCtrlID = -1;
		}
		return;
	}

	if (CurrentSelectMenu > EditMenu_MIN && CurrentSelectMenu < EditMenu_MAX) { // MainEditMenuActive
		MainEditInputUpdate();
	}
	else if (CurrentSelectMenu > PlatformEditMenu_MIN && CurrentSelectMenu < PlatformEditMenu_MAX) { // PlatFormModeMenuActive
		PlatFormModeInputUpdate();
	}
	else if (CurrentSelectMenu > EnemyEditMenu_MIN && CurrentSelectMenu < EnemyEditMenu_MAX) { // EnemyModeMenuActive
		EnemyModeInputUpdate();
	}
	
}

void DebugProc() {
	PrintDebugProc("TAB: EditMode 1: Draw collider Area 5: Draw Chase Area\n");
	if (GetGameSceneEditMode()) {
		SLProcAppearTimer -= 1;
		if (SLProcAppearTimer >= 0) {
			PrintDebugProc("Edit Mode           SAVE Success !!\n");
		}
		else {
			SLProcAppearTimer = 0;
			PrintDebugProc("Edit Mode\n");
		}

		if (CurrentSelectMenu > EditMenu_MIN && CurrentSelectMenu < EditMenu_MAX) {
			MainEditDebugProc();
		}
		else if (CurrentSelectMenu > PlatformEditMenu_MIN && CurrentSelectMenu < PlatformEditMenu_MAX) {
			PlatFormModeDebugProc();
		}
		else if (CurrentSelectMenu > EnemyEditMenu_MIN && CurrentSelectMenu < EnemyEditMenu_MAX) {
			EnemyModeDebugProc();
		}
	
	}

	//============Debug
	PrintDebugProc("\n");
	PrintDebugProc("                           CurrentSelectMenu: %d\n", CurrentSelectMenu);
	PrintDebugProc("                           PreSelectMenu: %d\n", PreSelectMenu);
	PrintDebugProc("                           PlatFormDebugCtrlID: %d\n", PlatFormDebugCtrlID);
	PrintDebugProc("                           EnemyDebugCtrlID: %d\n", EnemyDebugCtrlID);
}

void EditModeCollisionUpdate()
{
	//collider position
	SetPositionCollider3D(EditRayCastCIndex, GetCamera()->pos);

	//rayCastBullet
	if (rayCastBulletisShooting) {
		//Timer
		rayCastBulletShootTimer += 1;
		if (rayCastBulletShootTimer >= VALUE_RAYCASTBULLET_SHOOTTIME) {
			rayCastBulletShootTimer = 0;
			rayCastBulletisShooting = false;
		}

		//Move
		D3DXVECTOR3 pos;
		pos.x = GetCamera()->pos.x + sinf(rayCastBulletDir.y) * (rayCastBulletShootTimer * VALUE_RAYCASTBULLET_SPEED *  cosf(rayCastBulletDir.x));
		pos.z = GetCamera()->pos.z + cosf(rayCastBulletDir.y) * (rayCastBulletShootTimer * VALUE_RAYCASTBULLET_SPEED *  cosf(rayCastBulletDir.x));
		pos.y = GetCamera()->pos.y + sinf(rayCastBulletDir.x) *  rayCastBulletShootTimer * VALUE_RAYCASTBULLET_SPEED;

		SetPositionCollider3D(EditRayCastCIndex, pos);
	}
	else {
		rayCastBulletShootTimer = 0;
	}

	//collision
	if (GetGameSceneEditMode()) {
		int targetID;
		if (rayCastBulletisShooting) {
			//====PlatForm
			if (CheckHitByTagReturnMasterID(EditRayCastCIndex, collider3DTag_normalPlatform, &targetID)) {
				PLATFORM *platform = GetPlatform();
				PlatFormDebugCtrlID = targetID;
				platform[PlatFormDebugCtrlID].scaleAxis = -1; // reset scaleAxis
				rayCastBulletisShooting = false;
				rayCastBulletShootTimer = 0;
				// Dis
				D3DXVECTOR3 LengthV = (platform[PlatFormDebugCtrlID].pos - GetCamera()->pos);
				rayCastObjDis = D3DXVec3Length(&LengthV);
				//Reset SelectedID (メニューは上から一番)
				SetCurrentSelectMenu(PlatformEditMenu_Type);
				return;
			}
			else {
				PlatFormDebugCtrlID = -1;
			}
			//====Enemy
			if (CheckHitByTagReturnMasterID(EditRayCastCIndex, collider3DTag_enemy, &targetID)) {
				ENEMY *enemy = GetEnemy();
				EnemyDebugCtrlID = targetID;
				enemy[EnemyDebugCtrlID].scaleAxis = -1; // reset scaleAxis
				rayCastBulletisShooting = false;
				rayCastBulletShootTimer = 0;
				// Dis
				D3DXVECTOR3 LengthV = (enemy[EnemyDebugCtrlID].pos - GetCamera()->pos);
				rayCastObjDis = D3DXVec3Length(&LengthV);
				//Reset SelectedID (メニューは上から一番)
				SetCurrentSelectMenu(EnemyEditMenu_Type);
				return;
			}
			else {
				EnemyDebugCtrlID = -1;
			}

		}
	}

}
//========================================================
//=================変数コントロール=======================
//========================================================
#pragma region 変数コントロール

bool GetGameSceneEditMode()
{
	return EditMode;
}

int GetPlatFormDebugCtrlID(void)
{
	return PlatFormDebugCtrlID;
}

void SetPlatFormDebugCtrlID(int num)
{
	PlatFormDebugCtrlID = num;
}

int GetEnemyDebugCtrlID(void)
{
	return EnemyDebugCtrlID;
}

void SetEnemyDebugCtrlID(int num)
{
	EnemyDebugCtrlID = num;
}

int GetCurrentSelectMenu()
{
	return CurrentSelectMenu;
}

void SetCurrentSelectMenu(int num, bool setPre) {
	if(setPre)PreSelectMenu = CurrentSelectMenu;
	CurrentSelectMenu = num;
}

int GetPreSelectMenu()
{
	return PreSelectMenu;
}

void SetPreSelectMenu(int num)
{
	PreSelectMenu = num;
}

void AddCurrentSelectMenu() {
	CurrentSelectMenu++;
}

void MinusCurrentSelectMenu() {
	CurrentSelectMenu--;
}

int GetRayCastObjDis()
{
	return rayCastObjDis;
}

void SetRayCastObjDis(float dis)
{
	rayCastObjDis = dis;
}

void SetRayCastBulletDir(D3DXVECTOR3 dir)
{
	rayCastBulletDir = dir;
}

void SetRayCastBulletisShooting(bool b)
{
	rayCastBulletisShooting = b;
}

int GetSLProcAppearTimer()
{
	return SLProcAppearTimer;
}

void SetSLProcAppearTimer(int num)
{
	SLProcAppearTimer = num;
}

#pragma endregion