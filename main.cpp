//=============================================================================
//
// メイン処理 [main.cpp]
// Author : GP11B132 14 ゴショウケン
// sakumac
//=============================================================================
#include "main.h"
#include "renderer.h"
#include "camera.h"
#include "input.h"
#include "sound.h"

#include "title.h"
#include "tutorial.h"
#include "gameScene.h"
#include "result.h"
#include "fade.h"
#include "debugproc.h"
//#include "light.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define CLASS_NAME		"AppClass"				    // ウインドウのクラス名
#define WINDOW_NAME		"ALICE and Sorcerer's Card"	// ウインドウのキャプション名

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
void Uninit(void);
void Update(void);
void Draw(void);

void InitFPSTimer();
void FPSTimerCount();
void mouseUpdate();
void checkFocus(HWND hwnd);
//*****************************************************************************
// グローバル変数:
//*****************************************************************************
bool g_mouseActive = true;
long g_MouseX = 0;
long g_MouseY = 0;
long g_MousedX = 0;
long g_MousedY = 0;
int  g_Wheel   = 0;

#ifdef _DEBUG
int		g_CountFPS;							// FPSカウンタ
char	g_DebugStr[2048] = WINDOW_NAME;		// デバッグ文字表示用

#endif

MODE g_Mode = MODE_GAME;					// 起動時の画面を設定
//MODE g_Mode = MODE_TITLE;
//MODE g_Mode = MODE_RESULT;
//MODE g_Mode = MODE_TUTORIAL;

// 時間計測用
DWORD dwExecLastTime;
DWORD dwFPSLastTime;
DWORD dwCurrentTime;
DWORD dwFrameCount;
DWORD dwPerFrameTimer;

//=============================================================================
// メイン関数
//=============================================================================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);	// 無くても良いけど、警告が出る（未使用宣言）
	UNREFERENCED_PARAMETER(lpCmdLine);		// 無くても良いけど、警告が出る（未使用宣言）

	WNDCLASSEX	wcex = {
		sizeof(WNDCLASSEX),
		CS_CLASSDC,
		WndProc,
		0,
		0,
		hInstance,
		NULL,
		LoadCursor(NULL, IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		NULL,
		CLASS_NAME,
		NULL
	};
	HWND		hWnd;
	MSG			msg;

	// ウィンドウクラスの登録
	RegisterClassEx(&wcex);

	// ウィンドウの作成
	hWnd = CreateWindow(CLASS_NAME,
		WINDOW_NAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,																		// ウィンドウの左座標
		CW_USEDEFAULT,																		// ウィンドウの上座標
		SCREEN_WIDTH + GetSystemMetrics(SM_CXDLGFRAME) * 2,									// ウィンドウ横幅
		SCREEN_HEIGHT + GetSystemMetrics(SM_CXDLGFRAME) * 2 + GetSystemMetrics(SM_CYCAPTION),	// ウィンドウ縦幅
		NULL,
		NULL,
		hInstance,
		NULL);

	//ウインドウモード
	/*bool mode = true;

	int id = MessageBox(NULL, "Open in Window mode ?", "Open mode", MB_YESNOCANCEL | MB_ICONQUESTION);
	switch (id)
	{
	case IDYES:
		mode = true;
		break;

	case IDNO:
		mode = false;
		break;

	case IDCANCEL:

	default:
		return -1;
		break;
	}*/

	// 初期化処理(ウィンドウを作成してから行う)
	if (FAILED(Init(hInstance, hWnd, TRUE)))
	{
		return -1;
	}
	//Hide cursor
	ShowCursor(false);

	// フレームカウント初期化
	InitFPSTimer();

	// ウインドウの表示(初期化処理の後に呼ばないと駄目)
	ShowWindow(hWnd, nCmdShow);
	//SetForegroundWindow(hWnd);
	//SetFocus(hWnd);
	UpdateWindow(hWnd);

	// メッセージループ
	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{// PostQuitMessage()が呼ばれたらループ終了
				break;
			}
			else
			{
				// メッセージの翻訳と送出
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			FPSTimerCount();
			dwCurrentTime = timeGetTime();

			if ((dwCurrentTime - dwFPSLastTime) >= 1000)	// 1秒ごとに実行
			{
#ifdef _DEBUG
				g_CountFPS = dwFrameCount;
#endif
				dwFPSLastTime = dwCurrentTime;				// FPSを測定した時刻を保存
				dwFrameCount = 0;							// カウントをクリア
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))	// 1/60秒ごとに実行
			{
				dwExecLastTime = dwCurrentTime;	// 処理した時刻を保存

#ifdef _DEBUG	// デバッグ版の時だけFPSを表示する
				wsprintf(g_DebugStr, WINDOW_NAME);
				wsprintf(&g_DebugStr[strlen(g_DebugStr)], " FPS:%d", g_CountFPS);
#endif
				checkFocus(hWnd);
				Update();			// 更新処理
				Draw();				// 描画処理

#ifdef _DEBUG	// デバッグ版の時だけ表示する
				//wsprintf(&g_DebugStr[strlen(g_DebugStr)], " MX:%d MY:%d", GetMousePosX(), GetMousePosY());
				SetWindowText(hWnd, g_DebugStr);
#endif

				dwFrameCount++;
			}
		}
	}

	timeEndPeriod(1);				// 分解能を戻す

	// ウィンドウクラスの登録を解除
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	// 終了処理
	Uninit();

	return (int)msg.wParam;
}

//=============================================================================
// プロシージャ
//=============================================================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hWnd);
			break;
		}
		break;

	case WM_MOUSEMOVE:
		/*POINT CPoint;
		GetCursorPos(&CPoint);
		g_MouseX = CPoint.x;
		g_MouseY = CPoint.y;

		RECT rect;
		GetClientRect(hWnd, &rect);

		if (CPoint.x > rect.right || CPoint.x < -rect.left || CPoint.y > rect.bottom || CPoint.y < rect.top) {
			SetCursorPos(SCREEN_CENTER_X, SCREEN_CENTER_Y);
			GetCursorPos(&CPoint);
			g_MouseX = CPoint.x;
			g_MouseY = CPoint.y;
			g_MouseXPrev = g_MouseX;
			g_MouseYPrev = g_MouseY;
		}*/
		
		break;

	case WM_MOUSEWHEEL: {
		int delta = GET_WHEEL_DELTA_WPARAM(wParam);
		if (delta > 0)
		{
			//Mouse Wheel Up
			g_Wheel += 10;
		}
		else
		{
			//Mouse Wheel Down
			g_Wheel -= 10;
		}
	}

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	bool result;
	result = InitRenderer(hInstance, hWnd, bWindow);

	// カメラ処理の初期化 (3D)
	InitCamera();

	// 入力処理の初期化
	result = InitInput(hInstance, hWnd);

	// サウンドの初期化処理
	result = InitSound(hWnd);

	// フェードの初期化
	InitFade();

	// 最初のモードをセット
	SetMode(g_Mode);	// ここはSetModeのままで！ initMode is inside

	return result;
}

//=============================================================================
// 終了処理
//=============================================================================
void Uninit(void)
{
	// 終了のモードをセット
	SetMode(MODE_MAX);

	// フェードの終了処理
	UninitFade();

	// サウンド終了処理
	UninitSound();

	// 入力の終了処理
	UninitInput();

	// カメラの終了処理
	UninitCamera();

	// レンダリングの終了処理
	UninitRenderer();

}

//=============================================================================
// 更新処理
//=============================================================================
void Update(void)
{
	// 入力の更新処理
	UpdateInput();

	// カメラ更新
	UpdateCamera();

	// サウンドの更新
	UpdateSound();


	// モードによって処理を分ける
	switch (g_Mode)
	{
	case MODE_TITLE:		// タイトル画面の更新
		UpdateTitle();
		break;

	case MODE_TUTORIAL:	    // ゲーム説明画面
		UpdateTutorial();
		break;

	case MODE_GAME:			// ゲーム画面の更新
		UpdateGameScene();
		break;

	case MODE_RESULT:		// リザルト画面の更新
		UpdateResult();
		break;
	}

	// フェード処理の更新
	UpdateFade();

	//===== sys
	if (g_Wheel > 0) g_Wheel -= 1;
	if (g_Wheel < 0) g_Wheel += 1;
}

//=============================================================================
// 描画処理
//=============================================================================
void Draw(void)
{
	// バックバッファクリア
	Clear();

	// カメラをセット
	SetCamera();

	// モードによって処理を分ける
	switch (g_Mode)
	{
	case MODE_TITLE:		// タイトル画面の描画
		DrawTitle();
		break;

	case MODE_TUTORIAL:     // ゲーム説明画面
		DrawTutorial();
		break;

	case MODE_GAME:			// ゲーム画面の描画
		DrawGameScene();
		break;

	case MODE_RESULT:		// リザルト画面の描画
		DrawResult();
		break;
	}

	// フェード描画
	DrawFade();

#ifdef _DEBUG
	// デバッグ表示
	DrawDebugProc();
#endif

	// バックバッファ、フロントバッファ入れ替え
	Present();
}


long GetMousePosX(void)
{
	return g_MouseX;
}

long GetMousePosY(void)
{
	return g_MouseY;
}

long GetMousedX(void)
{
	return g_MousedX;
}

long GetMousedY(void)
{
	return g_MousedY;
}

long GetWheel(void)
{
	return g_Wheel;
}

long SetWheel(int i)
{
	return g_Wheel += i;
}

bool CheckMouseUpdate(void)
{
	//if (g_MouseXPrev != g_MouseX || g_MouseYPrev != g_MouseY) return true;
	if (g_MousedX != 0 || g_MousedY != 0) return true;
	else return false;
}

void InitFPSTimer() {
	timeBeginPeriod(1);	// 分解能を設定
	dwExecLastTime = dwFPSLastTime = timeGetTime();	// システム時刻をミリ秒単位で取得
	dwCurrentTime = dwFrameCount = dwPerFrameTimer = 0;
}

void FPSTimerCount() {

	dwCurrentTime = timeGetTime();					// システム時刻を取得

	if ((dwCurrentTime - dwFPSLastTime) >= 1000)	// 1秒ごとに実行
	{
#ifdef _DEBUG
		g_CountFPS = dwFrameCount;
#endif
		dwFPSLastTime = dwCurrentTime;				// FPSを測定した時刻を保存
		dwFrameCount = 0;							// カウントをクリア
	}

	dwPerFrameTimer = dwCurrentTime - dwExecLastTime;

}

#ifdef _DEBUG
char* GetDebugStr(void)
{
	return g_DebugStr;
}

#endif

//=============================================================================
// モードの設定
//=============================================================================
void SetMode(MODE mode)
{
	// モードを変える前に全部メモリを解放しちゃう

	// タイトル画面の終了処理
	UninitTitle();

	// ゲーム説明画面の終了処理
	UninitTutorial();
	// ゲーム画面の終了処理
	UninitGameScene();

	// リザルト画面の終了処理
	UninitResult();


	g_Mode = mode;	// 次のモードをセットしている

	switch (g_Mode)
	{
	case MODE_TITLE:
		// タイトル画面の初期化
		InitTitle();
		break;

	case MODE_TUTORIAL:
		// ゲーム説明画面の初期化
		InitTutorial();
		break;

	case MODE_GAME:
		// ゲーム画面の初期化
		InitGameScene();
		break;

	case MODE_RESULT:
		// リザルト画面の初期化
		InitResult();
		break;

		// ゲーム終了時の処理
	case MODE_MAX:
		break;
	}
}

//=============================================================================
// モードの取得
//=============================================================================
MODE GetMode(void)
{
	return g_Mode;
}

void mouseUpdate()
{

	POINT CPoint;
	GetCursorPos(&CPoint);
	g_MouseX = CPoint.x;
	g_MouseY = CPoint.y;

	g_MousedX = g_MouseX - SCREEN_CENTER_X;
	g_MousedY = g_MouseY - SCREEN_CENTER_Y;

	SetCursorPos(SCREEN_CENTER_X, SCREEN_CENTER_Y);


}

void checkFocus(HWND hwnd)
{
	HWND name;
	name = GetForegroundWindow();
	if (hwnd == name)
		g_mouseActive = true;
	else
		g_mouseActive = false;


	if (g_mouseActive) { mouseUpdate(); ShowCursor(false); }
	else ShowCursor(true);

}
