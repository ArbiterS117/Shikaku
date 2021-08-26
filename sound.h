//=============================================================================
//
// サウンド処理 [sound.h]
//
//=============================================================================
#ifndef _SOUND_H_
#define _SOUND_H_

#include <windows.h>
#include "xaudio2.h"						// サウンド処理で必要

//*****************************************************************************
// サウンドファイル
//*****************************************************************************
enum
{
	SOUND_LABEL_BGM_1,	// BGM0
	SOUND_LABEL_BGM_2,	// BGM0
	SOUND_LABEL_BGM_3,	// BGM0
	SOUND_LABEL_BGM_4,	// BGM0
	SOUND_LABEL_BGM_5,	// BGM0
	SOUND_LABEL_BGM_sample000,	// BGM0
	SOUND_LABEL_BGM_sample001,	// BGM1
	SOUND_LABEL_BGM_sample002,	// BGM2
	SOUND_LABEL_SE_start,	    // スタート
	SOUND_LABEL_SE_jump,	    // ジャンプ
	SOUND_LABEL_SE_throwcard,   // カード投げる
	SOUND_LABEL_SE_hit,		    // 撃った
	SOUND_LABEL_SE_sword,    	// ソード 
	SOUND_LABEL_SE_aqua,	    // アクアジャンプ 
	SOUND_LABEL_SE_hit2,		// 撃った2
	SOUND_LABEL_SE_usecard,     // カードアビリティ発動 
	SOUND_LABEL_SE_getStar,     // Star 
	SOUND_LABEL_MAX,
};

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
bool InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(int label);
void StopSound(int label);
void StopSound(void);

#endif
