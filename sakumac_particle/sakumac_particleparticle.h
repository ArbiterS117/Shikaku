//=============================================================================
//
// �p�[�e�B�N������ [particle.h]
// Author : GP11A132_16_���v�Ԙa��
//
//=============================================================================
#pragma once

//*****************************************************************************
// �}�N����`
//*****************************************************************************

#define	MAX_PARTICLE		(2000)		// �p�[�e�B�N���ő吔
#define MAX_BEZ				(256)

//*****************************************************************************
// �\���̒�`
//*****************************************************************************
typedef struct
{
	D3DXVECTOR3		pos;			// �ʒu
	D3DXVECTOR3		rot;			// ��]
	D3DXVECTOR3		scale;			// �X�P�[��
	D3DXVECTOR3		move;			// �ړ���
	MATERIAL		material;		// �}�e���A��
	float			fSizeX;			// ��
	float			fSizeY;			// ����
	int				nIdxShadow;		// �eID
	int				nLife;			// ����
	bool			bUse;			// �g�p���Ă��邩�ǂ���
	int				hitNo;			// �����蔻��
	float			count;			// �J�E���g

} PARTICLE;

typedef struct
{
	int bez;
	D3DXVECTOR3 finish;
	D3DXVECTOR3 start;
	D3DXVECTOR3 P1;
	D3DXVECTOR3 P2;
	D3DXCOLOR color;

	float x;
	float y;
	float z;
	float x2;
	float y2;
	float z2;
	float x3;
	float y3;
	float z3;

} BEZ;

enum
{
	EFFECT_DEFAULT,
	EFFECT_LISSAJOUS,
	EFFECT_FIRE,
	EFFECT_SMOKE,
	EFFECT_STAR,
	EFFECT_CIRCLE,
	EFFECT_BEZIER,
	EFFECT_ALL,
	EFFECT_MAX,
};

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitParticle(void);
void UninitParticle(void);
void UpdateParticle(void);
void DrawParticle(void);

int SetParticle(D3DXVECTOR3 pos, D3DXCOLOR col, int nLife);
void SetColorParticle(int nIdxParticle, D3DXCOLOR col);
PARTICLE *GetParticle();
BEZ *GetBez(void);

void effectLissajous(D3DXVECTOR3 pos, int sinX, int sinY, int sinZ, float sizeX, float sizeY, float sizeZ, int Quantity);
void effectStar(D3DXVECTOR3 pos);
int effectFire(D3DXVECTOR3 pos, D3DXCOLOR col, float fSizeX, float fSizeY, int count);
int effectSmoke(D3DXVECTOR3 pos, D3DXCOLOR col, float fSizeX, float fSizeY, int count);
int effectCircle(D3DXVECTOR3 pos);
void effectBezier(D3DXVECTOR3 pos);
void ParticleSwitch(D3DXVECTOR3 pos,int No);