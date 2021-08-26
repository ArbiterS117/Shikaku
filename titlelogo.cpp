//=============================================================================
//
// titlelogo��ʏ��� [titlelogo.cpp]
// Author : GP11B132 14 �S�V���E�P��
//
//=============================================================================
#include "titlelogo.h"
#include "input.h"
#include "fade.h"
#include "UISprite.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define MAX_UISPRITE                (3)

#define INTRO_TEXTURE_WIDTH         (960.0f)
#define INTRO_TEXTURE_HEIGHT        (546.0f)
#define UISPRITE_INTROA             "data/TEXTURE/result/teamlogo.png"
#define UISPRITE_INTROB             "data/TEXTURE/result/teamlogo.png"
#define UISPRITE_INTROC             "data/TEXTURE/result/teamlogo.png"
//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************


//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static DX11_UISPRITE		        g_UISprite[MAX_UISPRITE]; // UI�X�v���C�g���

static int lv = 0;
//=============================================================================
// ����������
//=============================================================================
HRESULT InitTitlelogo(void)
{
	LoadUISprite(UISPRITE_INTROA, &g_UISprite[0], INTRO_TEXTURE_WIDTH, INTRO_TEXTURE_HEIGHT, SCREEN_CENTER_X, SCREEN_CENTER_Y);
	LoadUISprite(UISPRITE_INTROB, &g_UISprite[1], INTRO_TEXTURE_WIDTH, INTRO_TEXTURE_HEIGHT, SCREEN_CENTER_X, SCREEN_CENTER_Y);
	LoadUISprite(UISPRITE_INTROC, &g_UISprite[2], INTRO_TEXTURE_WIDTH, INTRO_TEXTURE_HEIGHT, SCREEN_CENTER_X, SCREEN_CENTER_Y);

	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitTitlelogo(void)
{
	// �X�v���C�g�̉������
	for (int i = 0; i < MAX_UISPRITE; i++) {
		UnloadUISprite(&g_UISprite[i]);
	}

}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateTitlelogo(void)
{
	// �{�^����������A�X�e�[�W��؂�ւ���
	if (lv >= 0) {
		if (GetKeyboardTrigger(DIK_RETURN) || IsMouseLeftTriggered()) SetFade(FADE_OUT, MODE_TITLE);
	}
	else {
		if (GetKeyboardTrigger(DIK_RETURN) || IsMouseLeftTriggered()) lv++;
	}

}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawTitlelogo(void)
{
	// 2D�`��Ȃ̂Ő[�x����
	SetDepthEnable(false);
	SetLightEnable(false);

	if (lv == 0)DrawUISprite(&g_UISprite[0]);
	if (lv == 1)DrawUISprite(&g_UISprite[0]);
	if (lv >= 2)DrawUISprite(&g_UISprite[0]);

	SetDepthEnable(true);
	SetLightEnable(true);

}

