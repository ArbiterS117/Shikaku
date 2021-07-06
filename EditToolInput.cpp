//=============================================================================
//
// ゲームエディターモードの入力処理 [EditToolInput.cpp]
// Author : GP11B132 14 ゴショウケン
//
//=============================================================================
#include "EditToolInput.h"
#include "groundFile.h"
#include "EnemyFile.h"
#include "input.h"
#include "EditTool.h"
#include "collider3D.h"
#include "camera.h"


static int editModeMoveValue = VALUE_MOVE;

static int NewPlatformType = 0; // for right klick new Platform
static int NewEnemyType = 0; // for right klick new Enemy

int GetNewPlatformType() {
	return NewPlatformType;
}
int GetNewEnemyType() {
	return NewEnemyType;
}

void LoadAndRebuildData()
{
	bool result;
	//Platform
	PLATFORM *platform = GetPlatform();
	for (int i = 0; i < MAX_PLATFORM; i++) {
		ReleasePlatform(i);
		SetPlatFormDebugCtrlID(-1);
	}
	result = LoadGroundData();
	if (result == true) { // have data
						  //rebuild data & collider
		RebuildAllPlatformAfterLoad();
	}
	//Enemy
	ENEMY *enemy = GetEnemy();
	for (int i = 0; i < MAX_ENEMY; i++) {
		ReleaseEnemy(i);
		SetEnemyDebugCtrlID(-1);
	}
	result = LoadEnemyData();
	//result = UpgradeLoadEnemyData();
	if (result == true) { // have data
						  //rebuild data & collider
		RebuildAllEnemyAfterLoad();
	}
	
}
//=============================================================================
//=============================================================================
// MainEditMenuInputUpdate
//=============================================================================
//=============================================================================
void MainEditInputUpdate()
{
	//===Select
	if (GetKeyboardTrigger(DIK_LEFT)) {
		if (GetCurrentSelectMenu() >= EditMenu_Platform && GetCurrentSelectMenu() <= EditMenu_Enemy) {
			AddCurrentSelectMenu();
			if (GetCurrentSelectMenu() > EditMenu_Enemy) SetCurrentSelectMenu(EditMenu_Platform, false);
		}
		else if (GetCurrentSelectMenu() == EditMenu_PlatformType) {
			NewPlatformType--;
			if (NewPlatformType < 0) NewPlatformType = PlatformType_MAX - 1;
		}
		else if (GetCurrentSelectMenu() == EditMenu_EnemyType) {
			NewEnemyType--;
			if (NewEnemyType < 0) NewEnemyType = EnemyType_MAX - 1;
		}
	}
	if (GetKeyboardTrigger(DIK_RIGHT)) {
		if (GetCurrentSelectMenu() >= EditMenu_Platform && GetCurrentSelectMenu() <= EditMenu_Enemy) {
			MinusCurrentSelectMenu();
			if (GetCurrentSelectMenu() < EditMenu_Platform) SetCurrentSelectMenu(EditMenu_Enemy, false);
		}
		else if (GetCurrentSelectMenu() == EditMenu_PlatformType) {
			NewPlatformType++;
			if (NewPlatformType >= PlatformType_MAX) NewPlatformType = 0;
		}
		else if (GetCurrentSelectMenu() == EditMenu_EnemyType) {
			NewEnemyType++;
			if (NewEnemyType >= EnemyType_MAX) NewEnemyType = 0;
		}
	}
	if (GetKeyboardTrigger(DIK_UP)) {
		     if (GetCurrentSelectMenu() == EditMenu_Platform)     SetCurrentSelectMenu(EditMenu_PlatformType);
		else if (GetCurrentSelectMenu() == EditMenu_Enemy)        SetCurrentSelectMenu(EditMenu_EnemyType);   
		else if (GetCurrentSelectMenu() == EditMenu_PlatformType) SetCurrentSelectMenu(EditMenu_Platform);    
		else if (GetCurrentSelectMenu() == EditMenu_EnemyType)    SetCurrentSelectMenu(EditMenu_Enemy);       
	}
	if (GetKeyboardTrigger(DIK_DOWN)) {
		     if (GetCurrentSelectMenu() == EditMenu_Platform)     SetCurrentSelectMenu(EditMenu_PlatformType);
		else if (GetCurrentSelectMenu() == EditMenu_Enemy)        SetCurrentSelectMenu(EditMenu_EnemyType);
		else if (GetCurrentSelectMenu() == EditMenu_PlatformType) SetCurrentSelectMenu(EditMenu_Platform);
		else if (GetCurrentSelectMenu() == EditMenu_EnemyType)    SetCurrentSelectMenu(EditMenu_Enemy);
	}
	//===SaveLoad
	if (GetKeyboardTrigger(DIK_9)) {
		SaveGroundData();
		SaveEnemyData();
		//UpgradeSaveEnemyData();
		SetSLProcAppearTimer(SLProcAppearTime);
	}
	if (GetKeyboardTrigger(DIK_8)) {
		LoadAndRebuildData();
	}
	//=== Shoot Raycast
	D3DXVECTOR3 pos;
	if (IsMouseLeftTriggered()) {
		SetRayCastBulletDir(GetCamera()->rot);
		SetRayCastBulletisShooting(true);
	}
	if (IsMouseRightTriggered()) {
		PLATFORM *plat = GetPlatform();	
		ENEMY *enemy = GetEnemy();
		SetRayCastBulletisShooting(false);
		switch (GetCurrentSelectMenu())
		{
		case EditMenu_Platform :
		case EditMenu_PlatformType :
			// New Platform

			SetCurrentSelectMenu(PlatformEditMenu_Type);
			SetRayCastObjDis(VALUE_RAYCASTOBJDIS_DEFULT);
			pos.x = GetCamera()->pos.x + sinf(GetCamera()->rot.y) * (VALUE_RAYCASTOBJDIS_DEFULT *  cosf(GetCamera()->rot.x));
			pos.y = GetCamera()->pos.y + sinf(GetCamera()->rot.x) *  VALUE_RAYCASTOBJDIS_DEFULT;
			pos.z = GetCamera()->pos.z + cosf(GetCamera()->rot.y) * (VALUE_RAYCASTOBJDIS_DEFULT *  cosf(GetCamera()->rot.x));
			SetPlatFormDebugCtrlID(SetPlatform(pos, D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(20.0f, 20.0f, 20.0f), NewPlatformType));
			plat[GetPlatFormDebugCtrlID()].canmoveAxis = -1;
			plat[GetPlatFormDebugCtrlID()].colorType = 1;

			break;
		case EditMenu_Enemy :
		case EditMenu_EnemyType :
			// New Enemy
			SetCurrentSelectMenu(EnemyEditMenu_Type);
			SetRayCastObjDis(VALUE_RAYCASTOBJDIS_DEFULT);
			pos.x = GetCamera()->pos.x + sinf(GetCamera()->rot.y) * (VALUE_RAYCASTOBJDIS_DEFULT *  cosf(GetCamera()->rot.x));
			pos.y = GetCamera()->pos.y + sinf(GetCamera()->rot.x) *  VALUE_RAYCASTOBJDIS_DEFULT;
			pos.z = GetCamera()->pos.z + cosf(GetCamera()->rot.y) * (VALUE_RAYCASTOBJDIS_DEFULT *  cosf(GetCamera()->rot.x));
			SetEnemyDebugCtrlID(SetEnemy(pos, D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(30.0f, 30.0f, 30.0f), NewEnemyType));
			enemy[GetEnemyDebugCtrlID()].canmoveAxis = -1;

			break;
		default:
			break;
		}
	}
}
//=============================================================================
//=============================================================================
// PlatFormModeInputUpdate
//=============================================================================
//=============================================================================
void PlatFormModeInputUpdate()
{
	PLATFORM *platform = GetPlatform();

	if (GetGameSceneEditMode()) { //============ IN Platform EDIT MODE
		PLATFORM &CtrlPlatform = platform[GetPlatFormDebugCtrlID()];
		D3DXVECTOR3 dir = GetCamera()->at - GetCamera()->pos;
		//dir = D3DXVECTOR3{ dir.x, 0.0f, dir.z }; // don't want to fly (move according to dir.y)
		D3DXVECTOR3 Ndir = { dir.z, 0.0f, -dir.x };
		D3DXVec3Normalize(&dir, &dir);
		D3DXVec3Normalize(&Ndir, &Ndir);

		float dis = GetRayCastObjDis();
		if (GetWheel() > 0) {
			dis += 20.0f;
		}
		if (GetWheel() > 15) {
			dis += 20.0f;
			SetWheel(-1);
		}
		if (GetWheel() > 30) {
			dis += 20.0f;
			SetWheel(-1);
		}
		if (GetWheel() < 0) {
			dis -= 20.0f;
		}
		if (GetWheel() < -15) {
			dis -= 20.0f;
			SetWheel(1);
		}
		if (GetWheel() < -30) {
			dis -= 20.0f;
			SetWheel(1);
		}
		SetRayCastObjDis(dis);

		D3DXVECTOR3 pos;
		pos.x = GetCamera()->pos.x + sinf(GetCamera()->rot.y) * (GetRayCastObjDis() *  cosf(GetCamera()->rot.x));
		pos.y = GetCamera()->pos.y + sinf(GetCamera()->rot.x) *  GetRayCastObjDis() ;
		pos.z = GetCamera()->pos.z + cosf(GetCamera()->rot.y) * (GetRayCastObjDis() *  cosf(GetCamera()->rot.x));
		CtrlPlatform.pos = pos;

		if (GetKeyboardTrigger(DIK_UP)) {
			MinusCurrentSelectMenu();
			if (GetCurrentSelectMenu() <= PlatformEditMenu_MIN) SetCurrentSelectMenu(PlatformEditMenu_MAX - 1, false);
		}
		if (GetKeyboardTrigger(DIK_DOWN)) {
			AddCurrentSelectMenu();
			if (GetCurrentSelectMenu() >= PlatformEditMenu_MAX) SetCurrentSelectMenu(PlatformEditMenu_MIN + 1, false);
		}

		if (GetKeyboardTrigger(DIK_RIGHT)) {
			if (GetPlatFormDebugCtrlID() != -1) {
				switch (GetCurrentSelectMenu())
				{
				case PlatformEditMenu_Type:
					CtrlPlatform.platformType += 1;
					if (CtrlPlatform.platformType >= PlatformType_MAX) {
						CtrlPlatform.platformType = 0;
					}
					break;
				case PlatformEditMenu_CanmoveAxis:
					CtrlPlatform.canmoveAxis += 1;
					if (CtrlPlatform.canmoveAxis > 2) {
						CtrlPlatform.canmoveAxis = -1;
					}
					break;
				case PlatformEditMenu_CanspinAxis:
					CtrlPlatform.canspinAxis += 1;
					if (CtrlPlatform.canspinAxis > 2) {
						CtrlPlatform.canspinAxis = -1;
					}
					break;
				case PlatformEditMenu_Scale:
					CtrlPlatform.scaleAxis += 1;
					if (CtrlPlatform.scaleAxis > 2) {
						CtrlPlatform.scaleAxis = -1;
					}
					break;
				case PlatformEditMenu_Color:
					CtrlPlatform.colorType += 1;
					if (CtrlPlatform.colorType > 3) {
						CtrlPlatform.colorType = 0;
					}
					break;					
				default:
					break;
				}
				int type = CtrlPlatform.platformType;
				SetScaleCollider3D(CtrlPlatform.colliderIdx, D3DXVECTOR3(CtrlPlatform.scl.x * PlatformColliderSize[type].x,
					CtrlPlatform.scl.y* PlatformColliderSize[type].y, CtrlPlatform.scl.z* PlatformColliderSize[type].z));
				float maxR = max((max((CtrlPlatform.scl.x* PlatformColliderSize[CtrlPlatform.platformType].x),
					(CtrlPlatform.scl.y* PlatformColliderSize[CtrlPlatform.platformType].y))),
					(CtrlPlatform.scl.z* PlatformColliderSize[CtrlPlatform.platformType].z));
				SetScaleCollider3D(CtrlPlatform.CircleRangeColliderIdx, maxR * RANGE_COLLIDER_SIZE_RATE);
				UpdatePlatformModel(GetPlatFormDebugCtrlID());
			}
		}

		if (GetKeyboardTrigger(DIK_LEFT)) {
			if (GetPlatFormDebugCtrlID() != -1) {
				switch (GetCurrentSelectMenu())
				{
				case PlatformEditMenu_Type:
					CtrlPlatform.platformType -= 1;
					if (CtrlPlatform.platformType < 0) {
						CtrlPlatform.platformType = PlatformType_MAX - 1;
					}
					break;
				case PlatformEditMenu_CanmoveAxis:
					CtrlPlatform.canmoveAxis -= 1;
					if (CtrlPlatform.canmoveAxis < -1) {
						CtrlPlatform.canmoveAxis = 2;
					}
					break;
				case PlatformEditMenu_CanspinAxis:
					CtrlPlatform.canspinAxis -= 1;
					if (CtrlPlatform.canspinAxis < -1) {
						CtrlPlatform.canspinAxis = 2;
					}
					break;
				case PlatformEditMenu_Scale:
					CtrlPlatform.scaleAxis -= 1;
					if (CtrlPlatform.scaleAxis < -1) {
						CtrlPlatform.scaleAxis = 2;
					}
					break;
				case PlatformEditMenu_Color:
					CtrlPlatform.colorType -= 1;
					if (CtrlPlatform.colorType < 0) {
						CtrlPlatform.colorType = 3;
					}
					break;
				default:
					break;
				}
				int type = CtrlPlatform.platformType;
				SetScaleCollider3D(CtrlPlatform.colliderIdx, D3DXVECTOR3(CtrlPlatform.scl.x * PlatformColliderSize[type].x,
					CtrlPlatform.scl.y* PlatformColliderSize[type].y, CtrlPlatform.scl.z* PlatformColliderSize[type].z));
				float maxR = max((max((CtrlPlatform.scl.x* PlatformColliderSize[CtrlPlatform.platformType].x), 
					(CtrlPlatform.scl.y* PlatformColliderSize[CtrlPlatform.platformType].y))),
					(CtrlPlatform.scl.z* PlatformColliderSize[CtrlPlatform.platformType].z));
				SetScaleCollider3D(CtrlPlatform.CircleRangeColliderIdx, maxR * RANGE_COLLIDER_SIZE_RATE);
				UpdatePlatformModel(GetPlatFormDebugCtrlID());
			}
		}

		//===========Rot
		if (GetPlatFormDebugCtrlID() != -1) {
			if (GetCurrentSelectMenu() == PlatformEditMenu_Rot) {
				if (CtrlPlatform.rot.x < D3DX_PI *  0.175)if (GetKeyboardPress(DIK_U))   CtrlPlatform.rot.x += 0.01 * D3DX_PI;
				if (CtrlPlatform.rot.x > D3DX_PI * -0.175)if (GetKeyboardPress(DIK_J))   CtrlPlatform.rot.x -= 0.01 * D3DX_PI;
				if (CtrlPlatform.rot.y > D3DX_PI * -0.5)if (GetKeyboardPress(DIK_H))   CtrlPlatform.rot.y -= 0.01 * D3DX_PI;
				if (CtrlPlatform.rot.y < D3DX_PI *  0.5)if (GetKeyboardPress(DIK_K))   CtrlPlatform.rot.y += 0.01 * D3DX_PI;
				//if (GetKeyboardPress(DIK_Y))   CtrlPlatform.rot.z -= 0.01 * D3DX_PI;
				//if (GetKeyboardPress(DIK_I))   CtrlPlatform.rot.z += 0.01 * D3DX_PI;
				if (CtrlPlatform.rot.x >  D3DX_PI * 2) CtrlPlatform.rot.x -= D3DX_PI * 2;
				if (CtrlPlatform.rot.x < -D3DX_PI * 2) CtrlPlatform.rot.x += D3DX_PI * 2;
				if (CtrlPlatform.rot.y >  D3DX_PI * 2) CtrlPlatform.rot.y -= D3DX_PI * 2;
				if (CtrlPlatform.rot.y < -D3DX_PI * 2) CtrlPlatform.rot.y += D3DX_PI * 2;
				if (CtrlPlatform.rot.z >  D3DX_PI * 2) CtrlPlatform.rot.z -= D3DX_PI * 2;
				if (CtrlPlatform.rot.z < -D3DX_PI * 2) CtrlPlatform.rot.z += D3DX_PI * 2;
				SetPositionCollider3D(CtrlPlatform.colliderIdx, CtrlPlatform.pos, CtrlPlatform.rot);
			}
		}

		if (GetKeyboardPress(DIK_3)) {
			if (CtrlPlatform.scaleAxis == -1) {
				CtrlPlatform.scl.x += VALUE_EDIT_SCALE_SPEED;
				CtrlPlatform.scl.y += VALUE_EDIT_SCALE_SPEED;
				CtrlPlatform.scl.z += VALUE_EDIT_SCALE_SPEED;
			}
			else if (CtrlPlatform.scaleAxis == 0)CtrlPlatform.scl.x += VALUE_EDIT_SCALE_SPEED;
			else if (CtrlPlatform.scaleAxis == 1)CtrlPlatform.scl.y += VALUE_EDIT_SCALE_SPEED;
			else if (CtrlPlatform.scaleAxis == 2)CtrlPlatform.scl.z += VALUE_EDIT_SCALE_SPEED;

			int type = CtrlPlatform.platformType;
			SetScaleCollider3D(CtrlPlatform.colliderIdx, D3DXVECTOR3(CtrlPlatform.scl.x * PlatformColliderSize[type].x,
				CtrlPlatform.scl.y* PlatformColliderSize[type].y, CtrlPlatform.scl.z* PlatformColliderSize[type].z));
			float maxR = max((max((CtrlPlatform.scl.x* PlatformColliderSize[CtrlPlatform.platformType].x), (CtrlPlatform.scl.y* PlatformColliderSize[CtrlPlatform.platformType].y))), (CtrlPlatform.scl.z* PlatformColliderSize[CtrlPlatform.platformType].z));
			SetScaleCollider3D(CtrlPlatform.CircleRangeColliderIdx, maxR * RANGE_COLLIDER_SIZE_RATE);
		}

		if (GetKeyboardPress(DIK_2)) {
			if (CtrlPlatform.scaleAxis == -1) {
				if (CtrlPlatform.scl.x > VALUE_EDIT_SCALE_SPEED)CtrlPlatform.scl.x -= VALUE_EDIT_SCALE_SPEED;
				if (CtrlPlatform.scl.y > VALUE_EDIT_SCALE_SPEED)CtrlPlatform.scl.y -= VALUE_EDIT_SCALE_SPEED;
				if (CtrlPlatform.scl.z > VALUE_EDIT_SCALE_SPEED)CtrlPlatform.scl.z -= VALUE_EDIT_SCALE_SPEED;
			}
			else if (CtrlPlatform.scaleAxis == 0)CtrlPlatform.scl.x -= VALUE_EDIT_SCALE_SPEED;
			else if (CtrlPlatform.scaleAxis == 1)CtrlPlatform.scl.y -= VALUE_EDIT_SCALE_SPEED;
			else if (CtrlPlatform.scaleAxis == 2)CtrlPlatform.scl.z -= VALUE_EDIT_SCALE_SPEED;
			int type = CtrlPlatform.platformType;
			SetScaleCollider3D(CtrlPlatform.colliderIdx, D3DXVECTOR3(CtrlPlatform.scl.x * PlatformColliderSize[type].x,
				CtrlPlatform.scl.y* PlatformColliderSize[type].y, CtrlPlatform.scl.z* PlatformColliderSize[type].z));
			float maxR = max((max((CtrlPlatform.scl.x* PlatformColliderSize[CtrlPlatform.platformType].x), (CtrlPlatform.scl.y* PlatformColliderSize[CtrlPlatform.platformType].y))), (CtrlPlatform.scl.z* PlatformColliderSize[CtrlPlatform.platformType].z));
			SetScaleCollider3D(CtrlPlatform.CircleRangeColliderIdx, maxR * RANGE_COLLIDER_SIZE_RATE);
		}

		if (GetKeyboardPress(DIK_NUMPAD8)) {
			editModeMoveValue++;
		}
		if (GetKeyboardPress(DIK_NUMPAD9)) {
			editModeMoveValue--;
			if (editModeMoveValue < 1) editModeMoveValue = 1;
		}

		if (IsMouseLeftTriggered()) {
			if (GetPlatFormDebugCtrlID() != -1) { // grab something => place
				SetCurrentSelectMenu(GetPreSelectMenu());
				SetPlatFormDebugCtrlID(-1);
			}
		}
		if (IsMouseRightTriggered()) {
			if (GetPlatFormDebugCtrlID() != -1) { // grab SomeThing => delete
				ReleasePlatform(GetPlatFormDebugCtrlID());
				SetCurrentSelectMenu(GetPreSelectMenu());
				SetPlatFormDebugCtrlID(-1);
			}
		}
	}
}
//=============================================================================
//=============================================================================
// PlatFormModeInputUpdate
//=============================================================================
//=============================================================================
void EnemyModeInputUpdate()
{
	ENEMY *enemy = GetEnemy();

	if (GetGameSceneEditMode()) { //============ IN Platform EDIT MODE
		D3DXVECTOR3 dir = GetCamera()->at - GetCamera()->pos;
		//dir = D3DXVECTOR3{ dir.x, 0.0f, dir.z }; // don't want to fly (move according to dir.y)
		D3DXVECTOR3 Ndir = { dir.z, 0.0f, -dir.x };
		D3DXVec3Normalize(&dir, &dir);
		D3DXVec3Normalize(&Ndir, &Ndir);

		float dis = GetRayCastObjDis();
		if (GetWheel() > 0) {
			dis += 20.0f;
		}
		if (GetWheel() > 15) {
			dis += 20.0f;
			SetWheel(-1);
		}
		if (GetWheel() > 30) {
			dis += 20.0f;
			SetWheel(-1);
		}
		if (GetWheel() < 0) {
			dis -= 20.0f;
		}
		if (GetWheel() < -15) {
			dis -= 20.0f;
			SetWheel(1);
		}
		if (GetWheel() < -30) {
			dis -= 20.0f;
			SetWheel(1);
		}
		SetRayCastObjDis(dis);

		D3DXVECTOR3 pos;
		pos.x = GetCamera()->pos.x + sinf(GetCamera()->rot.y) * (GetRayCastObjDis() *  cosf(GetCamera()->rot.x));
		pos.y = GetCamera()->pos.y + sinf(GetCamera()->rot.x) *  GetRayCastObjDis();
		pos.z = GetCamera()->pos.z + cosf(GetCamera()->rot.y) * (GetRayCastObjDis() *  cosf(GetCamera()->rot.x));
		enemy[GetEnemyDebugCtrlID()].pos = pos;
		enemy[GetEnemyDebugCtrlID()].OriginPos = pos;
		SetEnemyAnimOriPos(GetEnemyDebugCtrlID(),pos);

		if (GetKeyboardTrigger(DIK_UP)) {
			MinusCurrentSelectMenu();
			if (GetCurrentSelectMenu() <= EnemyEditMenu_MIN) SetCurrentSelectMenu(EnemyEditMenu_MAX - 1, false);
		}
		if (GetKeyboardTrigger(DIK_DOWN)) {
			AddCurrentSelectMenu();
			if (GetCurrentSelectMenu() >= EnemyEditMenu_MAX) SetCurrentSelectMenu(EnemyEditMenu_MIN + 1, false);
		}

		if (GetKeyboardTrigger(DIK_RIGHT)) {
			if (GetEnemyDebugCtrlID() != -1) {
				int t;
				switch (GetCurrentSelectMenu())
				{
				case EnemyEditMenu_Type:
					t = enemy[GetEnemyDebugCtrlID()].enemyType;
					ChangeEnemyType(GetEnemyDebugCtrlID(), t + 1);
					if (enemy[GetEnemyDebugCtrlID()].enemyType >= EnemyType_MAX) {
						ChangeEnemyType(GetEnemyDebugCtrlID(), 0);
					}
					break;
				case EnemyEditMenu_CanmoveAxis:
					enemy[GetEnemyDebugCtrlID()].canmoveAxis += 1;
					if (enemy[GetEnemyDebugCtrlID()].canmoveAxis > 2) {
						enemy[GetEnemyDebugCtrlID()].canmoveAxis = -1;
					}
					break;
				case EnemyEditMenu_Scale:
					enemy[GetEnemyDebugCtrlID()].scaleAxis += 1;
					if (enemy[GetEnemyDebugCtrlID()].scaleAxis > 2) {
						enemy[GetEnemyDebugCtrlID()].scaleAxis = -1;
					}
					break;
				case EnemyEditMenu_CanChase:
					enemy[GetEnemyDebugCtrlID()].CanChase = !enemy[GetEnemyDebugCtrlID()].CanChase;
					break;
				default:
					break;
				}
				int type = enemy[GetEnemyDebugCtrlID()].enemyType;
				SetScaleCollider3D(enemy[GetEnemyDebugCtrlID()].BodyColliderIdx, D3DXVECTOR3(enemy[GetEnemyDebugCtrlID()].scl.x * EnemyColliderSize[type].x,
					enemy[GetEnemyDebugCtrlID()].scl.y* EnemyColliderSize[type].y, enemy[GetEnemyDebugCtrlID()].scl.z* EnemyColliderSize[type].z));
				UpdateEnemyModel(GetEnemyDebugCtrlID());
			}
		}

		if (GetKeyboardTrigger(DIK_LEFT)) {
			if (GetEnemyDebugCtrlID() != -1) {
				int t;
				switch (GetCurrentSelectMenu())
				{
				case EnemyEditMenu_Type:
					t = enemy[GetEnemyDebugCtrlID()].enemyType;
					ChangeEnemyType(GetEnemyDebugCtrlID(), t - 1);
					if (enemy[GetEnemyDebugCtrlID()].enemyType < 0) {
						enemy[GetEnemyDebugCtrlID()].enemyType = EnemyType_MAX - 1;
					}
					break;
				case EnemyEditMenu_CanmoveAxis:
					enemy[GetEnemyDebugCtrlID()].canmoveAxis -= 1;
					if (enemy[GetEnemyDebugCtrlID()].canmoveAxis < -1) {
						enemy[GetEnemyDebugCtrlID()].canmoveAxis = 2;
					}
					break;
				case EnemyEditMenu_Scale:
					enemy[GetEnemyDebugCtrlID()].scaleAxis -= 1;
					if (enemy[GetEnemyDebugCtrlID()].scaleAxis < -1) {
						enemy[GetEnemyDebugCtrlID()].scaleAxis = 2;
					}
					break;
				case EnemyEditMenu_CanChase:
					enemy[GetEnemyDebugCtrlID()].CanChase = !enemy[GetEnemyDebugCtrlID()].CanChase;
				default:
					break;
				}
			}
			UpdateEnemyModel(GetEnemyDebugCtrlID());
			int type = enemy[GetEnemyDebugCtrlID()].enemyType;
			SetScaleCollider3D(enemy[GetEnemyDebugCtrlID()].BodyColliderIdx, D3DXVECTOR3(enemy[GetEnemyDebugCtrlID()].scl.x * EnemyColliderSize[type].x,
				enemy[GetEnemyDebugCtrlID()].scl.y* EnemyColliderSize[type].y, enemy[GetEnemyDebugCtrlID()].scl.z* EnemyColliderSize[type].z));
		}

		if (GetKeyboardPress(DIK_3)) {
			if (enemy[GetEnemyDebugCtrlID()].scaleAxis == -1) {
				enemy[GetEnemyDebugCtrlID()].scl.x += VALUE_EDIT_SCALE_SPEED;
				enemy[GetEnemyDebugCtrlID()].scl.y += VALUE_EDIT_SCALE_SPEED;
				enemy[GetEnemyDebugCtrlID()].scl.z += VALUE_EDIT_SCALE_SPEED;
			}
			else if (enemy[GetEnemyDebugCtrlID()].scaleAxis == 0)enemy[GetEnemyDebugCtrlID()].scl.x += VALUE_EDIT_SCALE_SPEED;
			else if (enemy[GetEnemyDebugCtrlID()].scaleAxis == 1)enemy[GetEnemyDebugCtrlID()].scl.y += VALUE_EDIT_SCALE_SPEED;
			else if (enemy[GetEnemyDebugCtrlID()].scaleAxis == 2)enemy[GetEnemyDebugCtrlID()].scl.z += VALUE_EDIT_SCALE_SPEED;

			int type = enemy[GetEnemyDebugCtrlID()].enemyType;
			SetScaleCollider3D(enemy[GetEnemyDebugCtrlID()].BodyColliderIdx, D3DXVECTOR3(enemy[GetEnemyDebugCtrlID()].scl.x * EnemyColliderSize[type].x,
				enemy[GetEnemyDebugCtrlID()].scl.y* EnemyColliderSize[type].y, enemy[GetEnemyDebugCtrlID()].scl.z* EnemyColliderSize[type].z));
			SetScaleCollider3D(enemy[GetEnemyDebugCtrlID()].GroundCheckColliderIdx, D3DXVECTOR3(enemy[GetEnemyDebugCtrlID()].scl.x * EnemyColliderSize[type].x,
				enemy[GetEnemyDebugCtrlID()].scl.y* EnemyColliderSize[type].y * ENEMY_GROUND_CHECK_THIN, enemy[GetEnemyDebugCtrlID()].scl.z* EnemyColliderSize[type].z));
			SetEnemyAnimOriScl(GetEnemyDebugCtrlID(), enemy[GetEnemyDebugCtrlID()].scl);
		}

		if (GetKeyboardPress(DIK_2)) {
			if (enemy[GetEnemyDebugCtrlID()].scaleAxis == -1) {
				if (enemy[GetEnemyDebugCtrlID()].scl.x > VALUE_EDIT_SCALE_SPEED)enemy[GetEnemyDebugCtrlID()].scl.x -= VALUE_EDIT_SCALE_SPEED;
				if (enemy[GetEnemyDebugCtrlID()].scl.y > VALUE_EDIT_SCALE_SPEED)enemy[GetEnemyDebugCtrlID()].scl.y -= VALUE_EDIT_SCALE_SPEED;
				if (enemy[GetEnemyDebugCtrlID()].scl.z > VALUE_EDIT_SCALE_SPEED)enemy[GetEnemyDebugCtrlID()].scl.z -= VALUE_EDIT_SCALE_SPEED;
			}
			else if (enemy[GetEnemyDebugCtrlID()].scaleAxis == 0)enemy[GetEnemyDebugCtrlID()].scl.x -= VALUE_EDIT_SCALE_SPEED;
			else if (enemy[GetEnemyDebugCtrlID()].scaleAxis == 1)enemy[GetEnemyDebugCtrlID()].scl.y -= VALUE_EDIT_SCALE_SPEED;
			else if (enemy[GetEnemyDebugCtrlID()].scaleAxis == 2)enemy[GetEnemyDebugCtrlID()].scl.z -= VALUE_EDIT_SCALE_SPEED;
			int type = enemy[GetEnemyDebugCtrlID()].enemyType;
			SetScaleCollider3D(enemy[GetEnemyDebugCtrlID()].BodyColliderIdx, D3DXVECTOR3(enemy[GetEnemyDebugCtrlID()].scl.x * EnemyColliderSize[type].x,
				enemy[GetEnemyDebugCtrlID()].scl.y* EnemyColliderSize[type].y, enemy[GetEnemyDebugCtrlID()].scl.z* EnemyColliderSize[type].z));
			SetScaleCollider3D(enemy[GetEnemyDebugCtrlID()].GroundCheckColliderIdx, D3DXVECTOR3(enemy[GetEnemyDebugCtrlID()].scl.x * EnemyColliderSize[type].x,
				enemy[GetEnemyDebugCtrlID()].scl.y* EnemyColliderSize[type].y * ENEMY_GROUND_CHECK_THIN, enemy[GetEnemyDebugCtrlID()].scl.z* EnemyColliderSize[type].z));
			SetEnemyAnimOriScl(GetEnemyDebugCtrlID(), enemy[GetEnemyDebugCtrlID()].scl);
		}

		if (GetKeyboardPress(DIK_NUMPAD8)) {
			editModeMoveValue++;
		}
		if (GetKeyboardPress(DIK_NUMPAD9)) {
			editModeMoveValue--;
			if (editModeMoveValue < 1) editModeMoveValue = 1;
		}

		if (IsMouseLeftTriggered()) {
			if (GetEnemyDebugCtrlID() != -1) { // grab something => place
				ENEMY *enemy = GetEnemy();
				enemy[GetEnemyDebugCtrlID()].speed = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
				SetCurrentSelectMenu(GetPreSelectMenu());
				SetEnemyDebugCtrlID(-1);
			}
		}
		if (IsMouseRightTriggered()) {
			if (GetEnemyDebugCtrlID() != -1) { // grab SomeThing => delete
				ReleaseEnemy(GetEnemyDebugCtrlID());
				SetCurrentSelectMenu(GetPreSelectMenu());
				SetEnemyDebugCtrlID(-1);
			}
		}
	}
}
