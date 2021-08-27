//=============================================================================
//
// �p�[�e�B�N������ [particle.cpp]
// Author : GP11A132_16_���v�Ԙa��
//
//=============================================================================
#include <time.h>

#include "main.h"
#include "renderer.h"
#include "input.h"
#include "camera.h"
#include "model.h"
#include "shadow.h"
#include "particle.h"
#include "player.h"
#include "enemy.h"
#include "bezier.h"
#include "bullet.h"
#include "debugproc.h"
#include "bezier_point.h"
//*****************************************************************************
// �}�N����`
//*****************************************************************************
#define TEXTURE_MAX			(4)			// �e�N�X�`���̐�

#define	PARTICLE_SIZE_X		(20.0f)		// ���_�T�C�Y
#define	PARTICLE_SIZE_Y		(20.0f)		// ���_�T�C�Y
#define	VALUE_MOVE_PARTICLE	(50.0f)		// �ړ����x


#define	DISP_SHADOW						// �e�̕\��
//#undef DISP_SHADOW

// �p�[�e�B�N���̐F
#define COLOR_R				(1.0f)
#define COLOR_G				(1.0f)
#define COLOR_B				(0.0f)
#define COLOR_A				(0.5f)
#define DIFFUSE				(0.05f)

// �p�[�e�B�N���̍L����}��
#define SUPPRESSION_X				(0.055f)
#define SUPPRESSION_Y				(0.25f)
#define SUPPRESSION_Z				(0.015f)


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT MakeVertexParticle(void);

//*****************************************************************************
// �O���[�o���ϐ�
//*****************************************************************************
static ID3D11Buffer					*g_VertexBuffer = NULL;	// ���_�o�b�t�@

static ID3D11ShaderResourceView		*g_Texture[TEXTURE_MAX] = { NULL };	// �e�N�X�`�����
static int							g_TexNo[EFFECT_MAX];					// �e�N�X�`���ԍ�

static D3DXMATRIX				g_mtxWorldParticle;				// ���[���h�}�g���b�N�X

static PARTICLE					g_aParticle[EFFECT_MAX][MAX_PARTICLE];		// �p�[�e�B�N�����[�N
static BEZ						bez[MAX_BEZ];

static D3DXVECTOR3				g_posBase;						// �r���{�[�h�����ʒu
static float					g_roty = 0.0f;					// �ړ�����
static float					g_spd = 0.0f;					// �ړ��X�s�[�h
static float					count = 0.0f;					// �J�E���g
static bool						bSwitch = false;				// �p�[�e�B�N���X�C�b�`


static int effectNo = EFFECT_DEFAULT;


static char *g_TextureName[] =
{
	"data/TEXTURE/hosi.png",
	"data/TEXTURE/1.png",
	"data/TEXTURE/effect000.jpg",
	"data/TEXTURE/fire.png",


};

//=============================================================================
// ����������
//=============================================================================
HRESULT InitParticle(void)
{
	// ���_���̍쐬
	MakeVertexParticle();

	// �e�N�X�`������
	for (int i = 0; i < TEXTURE_MAX; i++)
	{
		D3DX11CreateShaderResourceViewFromFile(GetDevice(),
			g_TextureName[i],
			NULL,
			NULL,
			&g_Texture[i],
			NULL);
	}


	// �p�[�e�B�N�����[�N�̏�����
	for (int p = 0; p < EFFECT_MAX; p++)
	{
		g_TexNo[p] = 0;

		for (int i = 0; i < MAX_PARTICLE; i++)
		{
			g_aParticle[p][i].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			g_aParticle[p][i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			g_aParticle[p][i].scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
			g_aParticle[p][i].move = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

			ZeroMemory(&g_aParticle[p][i].material, sizeof(g_aParticle[p][i].material));
			g_aParticle[p][i].material.Diffuse = D3DXCOLOR(0.8f, 0.7f, 0.2f, 0.85f);

			g_aParticle[p][i].fSizeX = PARTICLE_SIZE_X;
			g_aParticle[p][i].fSizeY = PARTICLE_SIZE_Y;
			g_aParticle[p][i].nIdxShadow = -1;
			g_aParticle[p][i].nLife = 0;
			g_aParticle[p][i].bUse = false;
			g_aParticle[p][i].hitNo = 0;
		}
	}
	
	for (int b = 0; b < MAX_BEZ; b++)
	{
		bez[b].bez = 0;
		bez[b].finish = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		bez[b].start = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		bez[b].P1 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		bez[b].P2 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		bez[b].color = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.0f);

		bez[b].x = 0.0f;
		bez[b].y = 0.0f;
		bez[b].z = 0.0f;
		bez[b].x2 = 0.0f;
		bez[b].y2 = 0.0f;
		bez[b].z2 = 0.0f;
		bez[b].x3 = 0.0f;
		bez[b].y3 = 0.0f;
		bez[b].z3 = 0.0f;

	}


	return S_OK;
}

//=============================================================================
// �I������
//=============================================================================
void UninitParticle(void)
{
	//�e�N�X�`���̉��
	for (int nCntTex = 0; nCntTex < TEXTURE_MAX; nCntTex++)
	{
		if (g_Texture[nCntTex] != NULL)
		{
			g_Texture[nCntTex]->Release();
			g_Texture[nCntTex] = NULL;
		}
	}

	// ���_�o�b�t�@�̉��
	if (g_VertexBuffer != NULL)
	{
		g_VertexBuffer->Release();
		g_VertexBuffer = NULL;
	}
}

//=============================================================================
// �X�V����
//=============================================================================
void UpdateParticle(void)
{
	PLAYER *player = GetPlayer();
	ENEMY *enemy = GetEnemy();


#ifdef _DEBUG
	if (GetKeyboardTrigger(DIK_E))
	{
		effectNo = (effectNo + 1) % EFFECT_MAX;
		InitParticle();
	}
#endif

	switch (effectNo)
	{
	case EFFECT_DEFAULT:
		// �������
		break;

	case EFFECT_LISSAJOUS:
		// ���T�[�W��
		effectLissajous(g_posBase,8, 9, 7, 50, 35, 50, 500);
		break;

	case EFFECT_SMOKE:
		// ��
		effectSmoke(D3DXVECTOR3(player->pos), D3DXCOLOR(1.16f, 0.8f, 0.48f, 1.0f), 1.7f, 1.7f, 1);
		break;

	case EFFECT_FIRE:
		//��
		effectFire(player->pos, D3DXCOLOR(0.7f, 0.2f, 0.0f, 1.0f), 1.7f, 1.7f, 1);
		break;

	case EFFECT_STAR:
		// �L���L��
		effectStar(g_posBase);
		break;

	case EFFECT_CIRCLE:
		// �T�[�N��
		effectCircle(g_posBase);
		break;

	case EFFECT_BEZIER:
		SetBezier(player->pos, D3DXVECTOR3(0.0f,0.0f,0.0f));
		break;

	case EFFECT_ALL:
		effectLissajous(g_posBase,8, 9, 7, 50, 35, 50, 500);
		effectStar(g_posBase);
		effectCircle(g_posBase);

		break;
	}


	//if (GetKeyboardPress(DIK_SPACE) && !GetKeyboardPress(DIK_LSHIFT))
	//{
	//	effectBezier(player->pos);
	//}

	for (int p = 0; p < EFFECT_MAX; p++)
	{
		for (int i = 0; i < MAX_PARTICLE; i++)
		{

			if (g_aParticle[p][i].bUse)
			{// �g�p��



				g_aParticle[p][i].nLife--;
				if (g_aParticle[p][i].nLife <= 0)
				{
					g_aParticle[p][i].bUse = false;
					ReleaseShadow(g_aParticle[p][i].nIdxShadow);
					g_aParticle[p][i].nIdxShadow = -1;
				}
			}
		}
	}

#ifdef _DEBUG
	//PrintDebugProc("Bez:%d \n", bez[0].bez);
	//PrintDebugProc("Bezier1 X:%f Y:%f Z:%f \n", bez[0].x, bez[0].y, bez[0].z);
	//PrintDebugProc("Bezier2 X:%f Y:%f Z:%f \n", bez[0].x2, bez[0].y2, bez[0].z2);
	//PrintDebugProc("Bezier3 X:%f Y:%f Z:%f \n", bez[0].x3, bez[0].y3, bez[0].z3);
	PrintDebugProc("count:%f \n", count);
	PrintDebugProc("count:%f \n", g_aParticle[EFFECT_CIRCLE][1].count);
	PrintDebugProc("color.a:%f \n", g_aParticle[EFFECT_CIRCLE][1].material.Diffuse.a);

#endif
}

//=============================================================================
// �`�揈��
//=============================================================================
void DrawParticle(void)
{
	D3DXMATRIX mtxView,mtxScale,mtxTranslate;
	CAMERA *cam = GetCamera();

	// ���C�e�B���O�𖳌���
	SetLightEnable(false);

	// ���Z�����ɐݒ�
	SetBlendState(BLEND_MODE_ADD);

	// Z��r����
	SetDepthEnable(false);

	// �t�H�O����
	SetFogEnable(false);

	// ���_�o�b�t�@�ݒ�
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &g_VertexBuffer, &stride, &offset);

	// �v���~�e�B�u�g�|���W�ݒ�
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);



	for (int p = 0; p < EFFECT_MAX; p++)
	{
		// �e�N�X�`���ݒ�
		GetDeviceContext()->PSSetShaderResources(0, 1, &g_Texture[g_TexNo[p]]);

		for (int i = 0; i < MAX_PARTICLE; i++)
		{
			if (g_aParticle[p][i].bUse)
			{
				// ���[���h�}�g���b�N�X�̏�����
				D3DXMatrixIdentity(&g_mtxWorldParticle);

				// �r���[�}�g���b�N�X���擾
				mtxView = cam->mtxView;

				g_mtxWorldParticle._11 = mtxView._11;
				g_mtxWorldParticle._12 = mtxView._21;
				g_mtxWorldParticle._13 = mtxView._31;
				g_mtxWorldParticle._21 = mtxView._12;
				g_mtxWorldParticle._22 = mtxView._22;
				g_mtxWorldParticle._23 = mtxView._32;
				g_mtxWorldParticle._31 = mtxView._13;
				g_mtxWorldParticle._32 = mtxView._23;
				g_mtxWorldParticle._33 = mtxView._33;

				// �X�P�[���𔽉f
				D3DXMatrixScaling(&mtxScale, g_aParticle[p][i].scale.x, g_aParticle[p][i].scale.y, g_aParticle[p][i].scale.z);
				D3DXMatrixMultiply(&g_mtxWorldParticle, &g_mtxWorldParticle, &mtxScale);

				// �ړ��𔽉f
				D3DXMatrixTranslation(&mtxTranslate, g_aParticle[p][i].pos.x, g_aParticle[p][i].pos.y, g_aParticle[p][i].pos.z);
				D3DXMatrixMultiply(&g_mtxWorldParticle, &g_mtxWorldParticle, &mtxTranslate);

				// ���[���h�}�g���b�N�X�̐ݒ�
				SetWorldMatrix(&g_mtxWorldParticle);

				// �}�e���A���ݒ�
				SetMaterial(g_aParticle[p][i].material);

				// �|���S���̕`��
				GetDeviceContext()->Draw(4, 0);
			}
		}
	}

	// ���C�e�B���O��L����
	SetLightEnable(true);

	// �ʏ�u�����h�ɖ߂�
	SetBlendState(BLEND_MODE_ALPHABLEND);

	// Z��r�L��
	SetDepthEnable(true);

	// �t�H�O�L��
	SetFogEnable(true);

}

//=============================================================================
// ���_���̍쐬
//=============================================================================
HRESULT MakeVertexParticle(void)
{
	// ���_�o�b�t�@����
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VERTEX_3D) * 4;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	GetDevice()->CreateBuffer(&bd, NULL, &g_VertexBuffer);

	{//���_�o�b�t�@�̒��g�𖄂߂�
		D3D11_MAPPED_SUBRESOURCE msr;
		GetDeviceContext()->Map(g_VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &msr);

		VERTEX_3D* vertex = (VERTEX_3D*)msr.pData;

		// ���_���W�̐ݒ�
		vertex[0].Position = D3DXVECTOR3(-PARTICLE_SIZE_X / 2, PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[1].Position = D3DXVECTOR3(PARTICLE_SIZE_X / 2, PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[2].Position = D3DXVECTOR3(-PARTICLE_SIZE_X / 2, -PARTICLE_SIZE_Y / 2, 0.0f);
		vertex[3].Position = D3DXVECTOR3(PARTICLE_SIZE_X / 2, -PARTICLE_SIZE_Y / 2, 0.0f);

		// �@���̐ݒ�
		vertex[0].Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		vertex[1].Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		vertex[2].Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		vertex[3].Normal = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

		// ���ˌ��̐ݒ�
		vertex[0].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[1].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[2].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		vertex[3].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		// �e�N�X�`�����W�̐ݒ�
		vertex[0].TexCoord = D3DXVECTOR2(0.0f, 0.0f);
		vertex[1].TexCoord = D3DXVECTOR2(1.0f, 0.0f);
		vertex[2].TexCoord = D3DXVECTOR2(0.0f, 1.0f);
		vertex[3].TexCoord = D3DXVECTOR2(1.0f, 1.0f);

		GetDeviceContext()->Unmap(g_VertexBuffer, 0);
	}

	return S_OK;
}

//=============================================================================
// �}�e���A���J���[�̐ݒ�
//=============================================================================
void SetColorParticle(int pNo, int nIdxParticle, D3DXCOLOR col)
{

	g_aParticle[pNo][nIdxParticle].material.Diffuse = col;
}

//=============================================================================
// ���_���̍쐬
//=============================================================================
int SetParticle(D3DXVECTOR3 pos, D3DXCOLOR col, int nLife)
{
	int nIdxParticle = -1;

	for (int p = 0; p < EFFECT_MAX; p++)
	{
		for (int i = 0; i < MAX_PARTICLE; i++)
		{
			if (!g_aParticle[p][i].bUse)
			{
				g_aParticle[p][i].pos = pos;
				g_aParticle[p][i].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
				g_aParticle[p][i].material.Diffuse = col;
				g_aParticle[p][i].nLife = nLife;
				g_aParticle[p][i].bUse = true;
				nIdxParticle = i;
			}
		}
	}
	return nIdxParticle;
}

PARTICLE *GetParticle()
{
	return &(g_aParticle[0][0]);
}

BEZ *GetBez(void)
{
	return &bez[0];
}



//=============================================================================
// �p�[�e�B�N���p�^�[��
//=============================================================================

// ���T�[�W���Ȑ�
void effectLissajous(D3DXVECTOR3 pos, int sinX, int sinY, int sinZ ,float sizeX, float sizeY, float sizeZ, int Quantity)		// ���T�W�F�[�Ȑ�
{
	D3DXVECTOR3 rot;

	pos.y = 10.0f;
	rot = GetCamera()->rot;
	g_TexNo[EFFECT_LISSAJOUS] = 0;
	float size = 0.5;

	for (int i = 0; i < Quantity; i++)
	{
		if (!g_aParticle[EFFECT_LISSAJOUS][i].bUse)
		{
			g_aParticle[EFFECT_LISSAJOUS][i].nLife = 100;
			g_aParticle[EFFECT_LISSAJOUS][i].bUse = true;
			g_aParticle[EFFECT_LISSAJOUS][i].material.Diffuse = D3DXCOLOR(1.0f, 1.0f, 0.0f, 1.0f);
		}

		g_aParticle[EFFECT_LISSAJOUS][i].count += 0.01f;

		float angle = ((1.0f / D3DX_PI) + (i / D3DX_PI)) + g_aParticle[EFFECT_LISSAJOUS][i].count;

		g_aParticle[EFFECT_LISSAJOUS][i].scale = D3DXVECTOR3(size, size, size);
		g_aParticle[EFFECT_LISSAJOUS][i].pos.x = pos.x - sinf(angle * sinX) * sizeX;
		g_aParticle[EFFECT_LISSAJOUS][i].pos.y = (pos.y - 5.0f) - sinf(angle * sinY) * sizeY;
		g_aParticle[EFFECT_LISSAJOUS][i].pos.z = pos.z - cosf(angle * sinZ) * sizeZ;


		g_aParticle[EFFECT_LISSAJOUS][i].material.Diffuse.a -= 0.02f;

	}
	
}

// ��
int effectFire(D3DXVECTOR3 pos, D3DXCOLOR col, float fSizeX, float fSizeY, int count)
{
	int part = -1;

	int nLife;
  	g_TexNo[EFFECT_FIRE] = 3;
	int c = count;
	float rad;
	D3DXVECTOR3 move;


	for (int i = 30 * c; i < (30 * c) + 30; i++)
	{
		if (!g_aParticle[EFFECT_FIRE][i].bUse)
		{
			nLife = rand() % 60 + 5;

			g_aParticle[EFFECT_FIRE][i].pos = pos;
			g_aParticle[EFFECT_FIRE][i].scale = D3DXVECTOR3(2.0f, 2.0f, 2.0f);
			g_aParticle[EFFECT_FIRE][i].material.Diffuse = col;
			g_aParticle[EFFECT_FIRE][i].nLife = nLife;
			g_aParticle[EFFECT_FIRE][i].bUse = true;

		}
		g_aParticle[EFFECT_FIRE][i].pos.y += 1.0f;
		rad = (FLOAT)(rand() % 628 - 314) / 100.0f;
		move.x = sinf(rad) * fSizeX;
		move.z = cosf(rad) * fSizeY;
		g_aParticle[EFFECT_FIRE][i].pos.x += move.x;
		g_aParticle[EFFECT_FIRE][i].pos.z += move.z;

	}
	return part;
}

// ��
int effectSmoke(D3DXVECTOR3 pos, D3DXCOLOR col, float fSizeX, float fSizeY, int count)
{
	//�����_���̏�����
	//srand((unsigned)time(NULL));

	int part = -1;

	int nLife;
	g_TexNo[EFFECT_SMOKE] = 3;
	int c = count;
	float rad;
	D3DXVECTOR3 move;

	for (int i = 40 * c; i < (40 * c) + 40; i++)
	{
		if (!g_aParticle[EFFECT_SMOKE][i].bUse)
		{
			nLife = rand() % 60 + 20;

			g_aParticle[EFFECT_SMOKE][i].pos = pos;
			g_aParticle[EFFECT_SMOKE][i].scale = D3DXVECTOR3(3.0f, 3.0f, 3.0f);
			g_aParticle[EFFECT_SMOKE][i].material.Diffuse = col;
			g_aParticle[EFFECT_SMOKE][i].nLife = nLife;
			g_aParticle[EFFECT_SMOKE][i].bUse = true;

			g_aParticle[EFFECT_SMOKE][i].pos.x = (pos.x - 24) + rand() % 48;
			g_aParticle[EFFECT_SMOKE][i].pos.z = (pos.z - 24) + rand() % 48;

		}

		g_aParticle[EFFECT_SMOKE][i].pos.y += 1.0f;

		rad = (FLOAT)(rand() % 628 - 314) / 100.0f;
		move.x = sinf(rad) * fSizeX;
		move.z = cosf(rad) * fSizeY;
		g_aParticle[EFFECT_SMOKE][i].pos.x += move.x;
		g_aParticle[EFFECT_SMOKE][i].pos.z += move.z;

	}
	return part;
}

// �L���L��
void effectStar(D3DXVECTOR3 pos)
{
	g_TexNo[EFFECT_STAR] = 0;
	float s = (FLOAT)(rand() % 10)*0.1f;

	for (int i = 0; i < 100; i++)
	{
		if (!g_aParticle[EFFECT_STAR][i].bUse)
		{
			g_aParticle[EFFECT_STAR][i].nLife = rand() % 10;
			g_aParticle[EFFECT_STAR][i].bUse = true;
			g_aParticle[EFFECT_STAR][i].material.Diffuse = D3DXCOLOR(0.5f, 0.3f, 0.0f, 1.0f);
		}

		g_aParticle[EFFECT_STAR][i].material.Diffuse.a -= 0.015f;
		g_aParticle[EFFECT_STAR][i].count += 0.01f;
		float angle = (D3DX_PI / 200) * g_aParticle[EFFECT_STAR][i].count;
		g_aParticle[EFFECT_STAR][i].scale = D3DXVECTOR3(s, s, s);
		g_aParticle[EFFECT_STAR][i].pos.x = pos.x - sinf(angle + 1 * i) * (rand() % 50);
		g_aParticle[EFFECT_STAR][i].pos.z = pos.z - cosf(angle + 1 * i) * (rand() % 50);
		g_aParticle[EFFECT_STAR][i].pos.y = (FLOAT)(rand() % 50);

	}
	
}

// �T�[�N��
int effectCircle(D3DXVECTOR3 pos)
{
	int a = -1;
	float c = 150.0f;
	g_TexNo[EFFECT_CIRCLE] = 2;
	float angle = (D3DX_PI / 200);

	for (int i = 0; i < 200; i++)
	{

		if (!g_aParticle[EFFECT_CIRCLE][i].bUse)
		{
			g_aParticle[EFFECT_CIRCLE][i].nLife = 100;
			g_aParticle[EFFECT_CIRCLE][i].bUse = true;
			g_aParticle[EFFECT_CIRCLE][i].material.Diffuse = D3DXCOLOR(0.5f, 0.3f, 0.0f, 1.0f);

		}

		g_aParticle[EFFECT_CIRCLE][i].count += 2.0f;
		g_aParticle[EFFECT_CIRCLE][i].pos.x = pos.x - sinf(angle + 1 * i) * g_aParticle[EFFECT_CIRCLE][i].count;
		g_aParticle[EFFECT_CIRCLE][i].pos.z = pos.z - cosf(angle + 1 * i) * g_aParticle[EFFECT_CIRCLE][i].count;
		g_aParticle[EFFECT_CIRCLE][i].pos.y = 5.0f;

		g_aParticle[EFFECT_CIRCLE][i].material.Diffuse.a -= 0.015f;

		if (g_aParticle[EFFECT_CIRCLE][i].count >= c)
		{
			g_aParticle[EFFECT_CIRCLE][i].count = 0.0f;
			g_aParticle[EFFECT_CIRCLE][i].material.Diffuse.a = 1.0f;
			g_aParticle[EFFECT_CIRCLE][i].bUse = false;
			effectNo = EFFECT_DEFAULT;
		}

	}

	return a;
}

// �x�W�F�Ȑ�
void effectBezier(D3DXVECTOR3 pos)
{
	PLAYER *player = GetPlayer();

	D3DXVECTOR3 finish[4];
	D3DXVECTOR3 start;
	D3DXVECTOR3 P1;
	D3DXVECTOR3 P2;
	D3DXCOLOR color;
	int max = 200;

	start  =  player->pos;
	start.y = 0.1f;

	P1     = D3DXVECTOR3(-30.0f, 20.0f,  30.0f) + pos;
	P2     = D3DXVECTOR3(-30.0f, 30.0f, -30.0f) + pos;
	finish[0] = D3DXVECTOR3( 30.0f, 70.0f, -30.0f) + pos;
	color = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	effect(start, P1, P2, finish[0], max, color);

	P1 = D3DXVECTOR3( 30.0f, 20.0f, 30.0f) + pos;
	P2 = D3DXVECTOR3(-30.0f, 30.0f, 30.0f) + pos;
	finish[1] = D3DXVECTOR3(-30.0f, 70.0f, -30.0f) + pos;
	color = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
	effect(start, P1, P2, finish[1], max, color);

	P1 = D3DXVECTOR3(30.0f, 20.0f, -30.0f) + pos;
	P2 = D3DXVECTOR3(30.0f, 30.0f, 30.0f) + pos;
	finish[2] = D3DXVECTOR3(-30.0f, 70.0f, 30.0f) + pos;
	color = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	effect(start, P1, P2, finish[2], max, color);

	P1 = D3DXVECTOR3(-30.0f, 20.0f, -30.0f) + pos;
	P2 = D3DXVECTOR3( 30.0f, 30.0f, -30.0f) + pos;
	finish[3] = D3DXVECTOR3(30.0f, 70.0f, 30.0f) + pos;
	color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	effect(start, P1, P2, finish[3], max, color);

}

void ParticleSwitch(D3DXVECTOR3 pos,int No)
{
	switch (No)
	{
	case EFFECT_LISSAJOUS:
		effectNo = EFFECT_LISSAJOUS;
		break;

	case EFFECT_FIRE:
		effectNo = EFFECT_FIRE;
		break;

	case EFFECT_SMOKE:
		effectNo = EFFECT_SMOKE;
		break;

	case EFFECT_STAR:
		effectNo = EFFECT_STAR;

		break;

	case EFFECT_CIRCLE:
		g_posBase = pos;
		effectNo = EFFECT_CIRCLE;
		break;

	case EFFECT_BEZIER:
		effectBezier(pos);

		break;

	case EFFECT_ALL:
		g_posBase = pos;
		effectNo = EFFECT_ALL;

		break;
	}
	
}
