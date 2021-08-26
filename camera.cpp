//=============================================================================
//
// カメラ処理 [camera.cpp]
// Author : GP11B132 14 ゴショウケン
//
//=============================================================================
#include "main.h"
#include "input.h"
#include "camera.h"
#include "player.h"
#include "ground.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	POS_X_CAM		(0.0f)			// カメラの初期位置(X座標)
#define	POS_Y_CAM		(200.0f)		// カメラの初期位置(Y座標)
#define	POS_Z_CAM		(-800.0f)		// カメラの初期位置(Z座標)

#define	POS_X_ROTATEWORLD_CAM		(0.0f)			// カメラの視点位置(X座標)
#define	POS_Y_ROTATEWORLD_CAM		(1400.0f)		// カメラの視点位置(Y座標)
#define	POS_Z_ROTATEWORLD_CAM		(-2000.0f)		// カメラの視点位置(Z座標)
#define CAMERA_ROUTE_SEGMENT		(10)

#define	VIEW_ANGLE		(D3DXToRadian(45.0f))							// ビュー平面の視野角
#define	VIEW_ASPECT		((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT)	// ビュー平面のアスペクト比	
#define	VIEW_NEAR_Z		(20.0f)											// ビュー平面のNearZ値
#define	VIEW_FAR_Z		(80000.0f)										// ビュー平面のFarZ値

#define	VALUE_MOVE_CAMERA	(2.0f)										// カメラの移動量
#define	VALUE_ROTATE_CAMERA	(D3DX_PI * 0.01f)							// カメラの回転量

#define VIEW_SIGHTVIEWING_SPEED (0.0013)

#define VIEW_ROTX_MAX   (D3DX_PI * 0.45f)
#define VIEW_ROTX_MIN   (-D3DX_PI * 0.45f)

static MOIVE_TRANSFORM cam_move_tbl[] = { //camera movie point
	{240, D3DXVECTOR3(1149, -251, 17072),          D3DXVECTOR3(14077, 4108, 7609),         oneObject},
	{240, D3DXVECTOR3(1149, -251, 17072),          D3DXVECTOR3(14077, 4108, 7609),         oneObject},
	{240, D3DXVECTOR3(1326.0f, 956.0f, -1270.0f),  D3DXVECTOR3(1326.0f, 956.0f, -3190.0f), oneObject},//dummy
	//{D3DXVECTOR3(300.0f, 956.0f,-300.0f),   D3DXVECTOR3(.0f, 0.0f, 0.0f), twoPointMoveandLookAt, 120},
	//{D3DXVECTOR3(-200.0f, 200.0f, -200.0f), D3DXVECTOR3(-100.0f, 0.0f, -100.0f), twoPointMoveandLookAt, 120},
	//{D3DXVECTOR3(-400.0f, 80.0f, 200.0f),   D3DXVECTOR3(0.0f, 0.0f, 0.0f), twoPointMoveandLookAt, 120},
	//{D3DXVECTOR3(500.0f, 100.0f, 500.0f),   D3DXVECTOR3(200.0f, 500.0f, -100.0f), twoPointMoveandLookAt, 120},
	//{D3DXVECTOR3(0.0f, 100.0f, 0.0f),       D3DXVECTOR3(0.0f, 0.0f, 0.0f), twoPointMoveandLookAt, 120},
	//{D3DXVECTOR3(0.0f, 100.0f, 0.0f),       D3DXVECTOR3(0.0f, 0.0f, 0.0f), twoPointMoveandLookAt, 120},//dummy
	//{D3DXVECTOR3(0.0f, 100.0f, 0.0f),       D3DXVECTOR3(0.0f, 0.0f, 0.0f), twoPointMoveandLookAt, 120},//dummy

};

enum MOVE_TBL_DATA_NUM
{
	TDN_cam_move_tbl = sizeof(cam_move_tbl) / sizeof(TRANSFORM),

};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
static void InputUpdate(void);
static void PhysicsUpdate(CAMERA &camera);
static void MovieModeUpdate();

//*****************************************************************************
// グローバル変数
//*****************************************************************************
static CAMERA			g_Camera;		// カメラデータ

static float screenWHelf = SCREEN_WIDTH * 0.5f;
static float screenHHelf = SCREEN_HEIGHT * 0.5f;

static bool  skipMouseDetect = true;

static D3DXVECTOR3 cameraPosRecord;
static D3DXVECTOR3 cameraRotRecord;
static bool bezirmoveflag;
static bool WorldRotateMode;
static bool goroback;
static D3DXVECTOR3* route;
static float		r_time;
static float		r_speed;

//=============================================================================
// 初期化処理
//=============================================================================
void InitCamera(void)
{
	//カメラの視点の設定
	g_Camera.pos = D3DXVECTOR3(POS_X_CAM, POS_Y_CAM, POS_Z_CAM);

	//カメラの注視点の設定
	g_Camera.at =  D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_Camera.up =  D3DXVECTOR3(0.0f, 1.0f, 0.0f);
	g_Camera.rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_Camera.rot.x -= D3DX_PI * 0.1f;

	// 視点と注視点の距離を計算
	float vx, vz;
	vx = g_Camera.pos.x - g_Camera.at.x;
	vz = g_Camera.pos.z - g_Camera.at.z;
	g_Camera.len = sqrtf(vx * vx + vz * vz);

	g_Camera.TPMode = true;
	g_Camera.canMove = true;

	//physics
	g_Camera.speed = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_Camera.maxMoveSpeed = 15.0f;
	g_Camera.decelerate = 0.8f;

	// 階層アニメーション用のメンバー変数の初期化
	g_Camera.tbl_adr = cam_move_tbl;
	g_Camera.TDN = TDN_cam_move_tbl;
	g_Camera.move_time = 0.0f;
	g_Camera.MovieMode = false;

	//world move camera change parameter
	bezirmoveflag = false;
	route = new D3DXVECTOR3[CAMERA_ROUTE_SEGMENT + 1];
	r_time = 0.0f;
	r_speed = 0.7f;
}


//=============================================================================
// カメラの終了処理
//=============================================================================
void UninitCamera(void)
{

}


//=============================================================================
// カメラの更新処理
//=============================================================================
void UpdateCamera(void)
{
	
	//================Input
	InputUpdate();

	//================Physics
	PhysicsUpdate(g_Camera);

	//================Movie Like Update
	MovieModeUpdate();

#ifdef _DEBUG	// デバッグ情報を表示する
	//char* str = GetDebugStr();
	//sprintf(&str[strlen(str)], " CrotX:%.2f CrotY:%.2f", g_Camera.rot.x, g_Camera.rot.y);
	//char* str = GetDebugStr();
	//sprintf(&str[strlen(str)], " CposX : %.2f  CposY : %.2f  CposZ : %.2f ", g_Camera.pos.x, g_Camera.pos.y, g_Camera.pos.z);
#endif

}


//=============================================================================
// カメラの更新
//=============================================================================
void SetCamera(void)
{
	// ビューマトリックス設定→ビューイング変換用
	D3DXMatrixLookAtLH(&g_Camera.mtxView, &g_Camera.pos, &g_Camera.at, &D3DXVECTOR3(0.0f, 1.0f, 0.0f));

	SetViewMatrix(&g_Camera.mtxView);


	float det;
	D3DXMatrixInverse(&g_Camera.mtxInvView, &det, &g_Camera.mtxView);


	// プロジェクションマトリックス設定→投影変換用（奥行き設定）                                             手前        奥
	D3DXMatrixPerspectiveFovLH(&g_Camera.mtxProjection, 1.0f, (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, VIEW_NEAR_Z, VIEW_FAR_Z);

	SetProjectionMatrix(&g_Camera.mtxProjection);
}


//=============================================================================
// カメラの取得
//=============================================================================
CAMERA *GetCamera(void)
{
	return &g_Camera;
}

void InputUpdate(void) {

	// TPMode or FPMode
	if (GetKeyboardTrigger(DIK_TAB))
	{
		g_Camera.TPMode = !g_Camera.TPMode;
	}
	if (GetKeyboardTrigger(DIK_M))
	{
		g_Camera.MovieMode = !g_Camera.MovieMode;
	}

	if (GetKeyboardTrigger(DIK_NUMPAD1))
	{
		g_Camera.canMove = !g_Camera.canMove;
	}

	if (g_Camera.MovieMode || !g_Camera.canMove) return;

	//FP
	D3DXVECTOR3 dir = g_Camera.at - g_Camera.pos;
	//dir = D3DXVECTOR3{ dir.x, 0.0f, dir.z }; // don't want to fly (move according to dir.y)
	D3DXVECTOR3 Ndir = { dir.z, 0.0f, -dir.x };
	D3DXVec3Normalize(&dir, &dir);
	D3DXVec3Normalize(&Ndir, &Ndir);

	//======カメラ旋回
	if (GetKeyboardPress(DIK_Q))
	{//「左」
		g_Camera.rot.y -= VALUE_ROTATE_CAMERA;
		if (g_Camera.rot.y < -D3DX_PI)
		{
			g_Camera.rot.y += D3DX_PI * 2.0f;
		}
	}

	if (GetKeyboardPress(DIK_E))
	{//「右」
		g_Camera.rot.y += VALUE_ROTATE_CAMERA;
		if (g_Camera.rot.y > D3DX_PI)
		{
			g_Camera.rot.y -= D3DX_PI * 2.0f;
		}
	}

	if (GetKeyboardPress(DIK_R))
	{//「上」
		g_Camera.rot.x += VALUE_ROTATE_CAMERA;
		if (g_Camera.rot.x < -D3DX_PI)
		{
			g_Camera.rot.x -= D3DX_PI * 2.0f;
		}

		if (g_Camera.rot.x > VIEW_ROTX_MAX) g_Camera.rot.x = VIEW_ROTX_MAX;
	}

	if (GetKeyboardPress(DIK_F))
	{//「下」
		g_Camera.rot.x -= VALUE_ROTATE_CAMERA;
		if (g_Camera.rot.x < -D3DX_PI)
		{
			g_Camera.rot.x -= D3DX_PI * 2.0f;
		}
		if (g_Camera.rot.x < VIEW_ROTX_MIN) g_Camera.rot.x = VIEW_ROTX_MIN;
	}

	//==============マウスでカメラを動かす FP
	if (CheckMouseUpdate()) {
		if (skipMouseDetect == false) {
			float dx = GetMousedX();
			float dy = GetMousedY();
			g_Camera.rot.y += D3DX_PI * VIEW_SIGHTVIEWING_SPEED * dx;
			g_Camera.rot.x -= D3DX_PI * VIEW_SIGHTVIEWING_SPEED * dy;
			if (g_Camera.rot.y < -D3DX_PI) g_Camera.rot.y += D3DX_PI * 2.0f;
			if (g_Camera.rot.y > D3DX_PI)  g_Camera.rot.y -= D3DX_PI * 2.0f;
			//g_Camera.rot.y = D3DX_PI * ((float)GetMousePosX() - screenWHelf) / screenWHelf; // 180 ~ -180
			//g_Camera.rot.x = D3DX_PI * -0.5f * ((float)GetMousePosY() - screenHHelf) / screenHHelf; // 90 ~ -90 
			if (g_Camera.rot.x > VIEW_ROTX_MAX) g_Camera.rot.x = VIEW_ROTX_MAX;
			if (g_Camera.rot.x < VIEW_ROTX_MIN) g_Camera.rot.x = VIEW_ROTX_MIN;
		}
		if (skipMouseDetect)skipMouseDetect = false;
	}

	//=============== FPMODE
	if (!g_Camera.TPMode) {

		if (GetKeyboardPress(DIK_W))
		{// 視点移動「前」
			//g_Camera.pos.z += VALUE_MOVE_CAMERA;
			g_Camera.speed += dir * VALUE_MOVE_CAMERA;
		}

		if (GetKeyboardPress(DIK_S))
		{// 視点移動「後ろ」
			g_Camera.speed -= dir * VALUE_MOVE_CAMERA;
		}

		if (GetKeyboardPress(DIK_A))
		{// 視点移動「左」
			//g_Camera.pos.x -= VALUE_MOVE_CAMERA;
			//g_Camera.pos.z += VALUE_MOVE_CAMERA * sinf(g_Camera.rot.y);
			//g_Camera.pos.x -= VALUE_MOVE_CAMERA * cosf(g_Camera.rot.y);

			g_Camera.speed -= Ndir * VALUE_MOVE_CAMERA;
		}

		if (GetKeyboardPress(DIK_D))
		{// 視点移動「右」
			//g_Camera.pos.x += VALUE_MOVE_CAMERA;
			//g_Camera.pos.z -= VALUE_MOVE_CAMERA * sinf(g_Camera.rot.y);
			//g_Camera.pos.x += VALUE_MOVE_CAMERA * cosf(g_Camera.rot.y);
			g_Camera.speed += Ndir * VALUE_MOVE_CAMERA;
		}

		//Sprint
		if (GetKeyboardPress(DIK_LSHIFT))
		{
			g_Camera.maxMoveSpeed = 40.0f;
		}
		else {
			g_Camera.maxMoveSpeed = 15.0f;
		}

		//カメラの座標を更新 FP
		g_Camera.at.x = g_Camera.pos.x + sinf(g_Camera.rot.y) * (g_Camera.len *  cosf(g_Camera.rot.x));
		g_Camera.at.z = g_Camera.pos.z + cosf(g_Camera.rot.y) * (g_Camera.len *  cosf(g_Camera.rot.x));
		g_Camera.at.y = g_Camera.pos.y + sinf(g_Camera.rot.x) * g_Camera.len;

	}
	//=============== TPMODE
	else {
		if (GetKeyboardPress(DIK_Z) || GetWheel() > 0)
		{// 近づく
			g_Camera.len -= VALUE_MOVE_CAMERA * 10.0f;
			if (g_Camera.len < 100.0f)g_Camera.len = 100.0f;
			if (GetWheel() >= 15) { // double speed => one more time
				g_Camera.len -= VALUE_MOVE_CAMERA * 10.0f;
				if (g_Camera.len < 100.0f)g_Camera.len = 100.0f;
				if (GetWheel() > 0) SetWheel(-1);
			}
			if (GetWheel() >= 30) { // double speed => one more time
				g_Camera.len -= VALUE_MOVE_CAMERA * 10.0f;
				if (g_Camera.len < 100.0f)g_Camera.len = 100.0f;
				if (GetWheel() > 0) SetWheel(-1);
			}
		}

		if (GetKeyboardPress(DIK_X) || GetWheel() < 0)
		{// 離れる
			g_Camera.len += VALUE_MOVE_CAMERA * 10.0f;
			if (GetWheel() <= -15) { // double speed => one more time
				g_Camera.len += VALUE_MOVE_CAMERA * 10.0f;
				if (g_Camera.len < 100.0f)g_Camera.len = 100.0f;
				if (GetWheel() < 0) SetWheel(1);
			}
			if (GetWheel() <= -30) { // double speed => one more time
				g_Camera.len += VALUE_MOVE_CAMERA * 10.0f;
				if (g_Camera.len < 100.0f)g_Camera.len = 100.0f;
				if (GetWheel() < 0) SetWheel(1);
			}
		}

		//キャラクターの座標に向く TP
		if (!WorldRotateMode)
		{
			PLAYER *obj;
			obj = GetPlayer();
			g_Camera.at = obj->pos;
		}
		else
		{
			g_Camera.at = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		}
		

		//カメラの座標を更新 TP
		g_Camera.pos.x = g_Camera.at.x - sinf(g_Camera.rot.y) * (g_Camera.len *  cosf(g_Camera.rot.x));
		g_Camera.pos.z = g_Camera.at.z - cosf(g_Camera.rot.y) * (g_Camera.len *  cosf(g_Camera.rot.x));
		g_Camera.pos.y = g_Camera.at.y - sinf(g_Camera.rot.x) * g_Camera.len;
	}

	if (bezirmoveflag) {

		// エネミー０番だけテーブルに従って座標移動（線形補間）
		int nowNo = r_time;		// 整数分であるテーブル番号を取り出している
		int maxNo = CAMERA_ROUTE_SEGMENT + 1;			// 登録テーブル数を数えている
		int nextNo = (nowNo + 1) % maxNo;			// 移動先テーブルの番号を求めている
		D3DXVECTOR3	pos = route[nextNo] - route[nowNo];// XYZ移動量を計算している
		float nowTime = r_time - nowNo;	// 時間部分である少数を取り出している
		pos *= nowTime;								// 現在の移動量を計算している
		r_time += r_speed;	// 時間を進めている
		// 計算して求めた移動量を現在の移動テーブルXYZに足している＝表示座標を求めている
		g_Camera.pos = route[nowNo] + pos;

		if ((int)r_time >= CAMERA_ROUTE_SEGMENT)		// 登録テーブル最後まで移動したか？
		{
			r_time = CAMERA_ROUTE_SEGMENT;
			if (goroback)
			{
				WorldRotateMode = true;
				ShowCursor(true);
			}
			else
			{
				bezirmoveflag = false;
				ShowCursor(false);
			}
		}
	}

	// カメラを初期に戻す
	if (GetKeyboardPress(DIK_P))
	{
		//UninitCamera();
		//InitCamera();
	}
}

void PhysicsUpdate(CAMERA & camera)
{
	if (g_Camera.MovieMode) return;

	D3DXVECTOR3 dir = g_Camera.speed;
	D3DXVec3Normalize(&dir, &dir);
	D3DXVECTOR3 Ndir = { dir.z, 0.0f, -dir.x };

	if (D3DXVec3Length(&g_Camera.speed) > 0) {
		g_Camera.speed -= dir * g_Camera.decelerate;
	}
	if (D3DXVec3Length(&g_Camera.speed) <= g_Camera.decelerate) {
		g_Camera.speed = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}
	if (D3DXVec3Length(&g_Camera.speed) >= g_Camera.maxMoveSpeed) {
		g_Camera.speed = dir * g_Camera.maxMoveSpeed;
	}

	//Update Speed
	g_Camera.pos += g_Camera.speed;

	//char* str = GetDebugStr();
	//sprintf(&str[strlen(str)], "  g_Camera.speed.x: %.2f  g_Camera.speed.z: %.2f", g_Camera.speed.x, g_Camera.speed.z);
}

void MovieModeUpdate()
{
	if (!g_Camera.MovieMode) return;
	////================階層アニメーション更新
	
	// 移動処理
	int		index = (int)g_Camera.move_time;
	float	time = g_Camera.move_time - index;
	int		size = g_Camera.TDN; // sizeof(g_Camera.tbl_adr) / sizeof(TRANSFORM)

	float dt = 1.0f / g_Camera.tbl_adr[index].frame;	// 1フレームで進める時間
	g_Camera.move_time += dt;					// アニメーションの合計時間に足す

	if (index > (size - 2))	// ゴールをオーバーしていたら、ゴールへ戻す
	{
		g_Camera.move_time = 0.0f;
		index = 0;
	}

	if (size != 1) {
		// 座標を求める	X = StartX + (EndX - StartX) * 今の時間
		D3DXVECTOR3 vec = g_Camera.tbl_adr[index + 1].pos - g_Camera.tbl_adr[index].pos;
		D3DXVECTOR3 deltaAt = g_Camera.tbl_adr[index + 1].at - g_Camera.tbl_adr[index].at;

		MOIVE_LOOKAT_TYPE type = g_Camera.tbl_adr[index].type;

		PLAYER *obj;
		obj = GetPlayer();
		D3DXVECTOR3 dis;
		float len ;
		D3DXVECTOR3 dir;
		switch (type)
		{
		case onePoint:
			g_Camera.pos = g_Camera.tbl_adr[index].pos;
			g_Camera.at = g_Camera.tbl_adr[index].at;
			break;
		case twoPointMove:
			g_Camera.pos = g_Camera.tbl_adr[index].pos + vec * time;
			g_Camera.at = g_Camera.tbl_adr[index].at;
			break;
		case twoPointLookAt:
			g_Camera.pos = g_Camera.tbl_adr[index].pos;
			g_Camera.at = g_Camera.tbl_adr[index].at + deltaAt * time;
			break;
		case twoPointMoveandLookAt:
			g_Camera.pos = g_Camera.tbl_adr[index].pos + vec * time;
			g_Camera.at = g_Camera.tbl_adr[index].at + deltaAt * time;
			break;
		case oneObject:
			//キャラクターの座標に向く TP
			g_Camera.pos = g_Camera.tbl_adr[index].pos;
			g_Camera.at = obj->pos;
			break;
		case twoObject:
			//キャラクターの座標に向く TP
			g_Camera.pos = g_Camera.tbl_adr[index].pos + vec * time;
			g_Camera.at = obj->pos;
			break;
		
		default:
			break;
		}
		
	}

}

void SetCameraWorldRotateView(bool b_goback) {
	r_time = 0.0f;
	bezirmoveflag == true;
	goroback = b_goback;

	if (b_goback)//player view -> world rotate view
	{
		g_Camera.at = D3DXVECTOR3(0, 0, 0);
		bezirmoveflag = true;
		cameraPosRecord = GetCamera()->pos;
		cameraRotRecord = GetCamera()->rot;
		D3DXVECTOR3 ep = D3DXVECTOR3(POS_X_ROTATEWORLD_CAM, POS_Y_ROTATEWORLD_CAM, POS_Z_ROTATEWORLD_CAM);
		D3DXVECTOR3 cp1 = D3DXVECTOR3(  
			(POS_X_ROTATEWORLD_CAM - cameraPosRecord.x) / 3 + cameraPosRecord.x,
			(POS_Y_ROTATEWORLD_CAM - cameraPosRecord.y) / 3 + cameraPosRecord.y, 
			(POS_Z_ROTATEWORLD_CAM - cameraPosRecord.z) / 3 + cameraPosRecord.z);
		D3DXVECTOR3 cp2 = D3DXVECTOR3(
			(POS_X_ROTATEWORLD_CAM - cameraPosRecord.x) * 2 / 3 + cameraPosRecord.x,
			(POS_Y_ROTATEWORLD_CAM - cameraPosRecord.y) * 2 / 3 + cameraPosRecord.y,
			(POS_Z_ROTATEWORLD_CAM - cameraPosRecord.z) * 2 / 3 + cameraPosRecord.z);
		

		ZeroMemory(&route, sizeof(route));
		route = new D3DXVECTOR3[CAMERA_ROUTE_SEGMENT + 1];
		route = GetThreePowerBeizerList(cameraPosRecord, cp1, cp2, ep, CAMERA_ROUTE_SEGMENT);
		route[CAMERA_ROUTE_SEGMENT] = ep;

	}
	else
	{
		g_Camera.at = GetPlayer()->pos;
		WorldRotateMode = false;
		
		HWND name;
		name = GetForegroundWindow();
		ShowCursor(false);
		AdjustDeltaRot();

		auto st = D3DXVECTOR3(POS_X_ROTATEWORLD_CAM, POS_Y_ROTATEWORLD_CAM, POS_Z_ROTATEWORLD_CAM);
		D3DXVECTOR3 ep = cameraPosRecord;
		D3DXVECTOR3 cp1 = D3DXVECTOR3(
			(ep.x - st.x) / 3 + st.x,
			(ep.y - st.y) / 3 + st.y,
			(ep.z - st.z) / 3 + st.z);
		D3DXVECTOR3 cp2 = D3DXVECTOR3(
			(ep.x - st.x) * 2 / 3 + st.x,
			(ep.y - st.y) * 2 / 3 + st.y,
			(ep.z - st.z) * 2 / 3 + st.z);
		
		ZeroMemory(&route, sizeof(route));
		route = new D3DXVECTOR3[CAMERA_ROUTE_SEGMENT + 1];
		route = GetThreePowerBeizerList(st, cp1, cp2, ep, CAMERA_ROUTE_SEGMENT);
		route[CAMERA_ROUTE_SEGMENT] = ep;
	}
	
}

// <summary>
// 四つの点を組み合わせ、ベジエ曲線を作る
// </summary>
// <param name="startPoint"></param>　開始点
// <param name="controlPoint1"></param>　中間点１
// <param name="controlPoint2"></param>　中間点２
// <param name="endPoint"></param>　結末点
// <param name="segmentNum"></param>　何段線がありますか
// <returns></returns>
// 
D3DXVECTOR3* GetThreePowerBeizerList(D3DXVECTOR3 startPoint, D3DXVECTOR3 controlPoint1, D3DXVECTOR3 controlPoint2, D3DXVECTOR3 endPoint, int segmentNum)
{
	D3DXVECTOR3* path = new D3DXVECTOR3[segmentNum + 1];

	for (int i = 0; i < segmentNum; i++)
	{
		float t = i / (float)segmentNum;
		D3DXVECTOR3 pixel = CalculateThreePowerBezierPoint(t, startPoint,
			controlPoint1, controlPoint2, endPoint);
		path[i] = pixel;
	}
	return path;
}

// <summary>
// 割れている点を一つずつ計算
// </summary>
// <param name="t"></param>　どんな程度を進んでいる
// <param name="p0"></param> 開始点
// <param name="p1"></param> 中間点１
// <param name="p2"></param> 中間点２
// <param name="p3"></param> 結末点
// <returns></returns>
D3DXVECTOR3 CalculateThreePowerBezierPoint(float t, D3DXVECTOR3 p0, D3DXVECTOR3 p1, D3DXVECTOR3 p2, D3DXVECTOR3 p3)
{
	float u = 1 - t;
	float tt = t * t;
	float uu = u * u;
	float ttt = tt * t;
	float uuu = uu * u;

	D3DXVECTOR3 p = uuu * p0;
	p += 3 * t * uu * p1;
	p += 3 * tt * u * p2;
	p += ttt * p3;

	return p;
}

bool GetWorldRotateMode() {
	return WorldRotateMode;
}

void SetWorldRotateMode(bool wrm) {
	WorldRotateMode = wrm;
}