/*******************************************************************************
* タイトル:		EnemyFile
* プログラム名:	EnemyFile.cpp
* 作成者:		GP11B132 14 ゴショウケン

UPDATE STRUCT ====> Input()內UpgradeSaveData  =>  Update Struct && Init()內 UpgradeLoadData  =>  Input()內SaveData  => 恢復 saveData && loadData

********************************************************************************/

/***************************************************************************
* インクルードファイル****
*******************************************************************************/
#include "EnemyFile.h"
#include "model.h"

/*******************************************************************************
* マクロ定義
*******************************************************************************/



/*******************************************************************************
* 構造体定義
*******************************************************************************/

struct OLDENEMYDATA // for update ENEMY_STRUCT
{
	bool		        Use;			    // 使用しているかどうか
	D3DXVECTOR3			pos;				// モデルの位置
	D3DXVECTOR3			rot;				// モデルの向き(回転)
	D3DXVECTOR3			scl;				// モデルの大きさ(スケール)

	DX11_MODEL			model;				// モデル情報

	D3DXMATRIX			mtxWorld;			// ワールドマトリックス

	//===physics
	D3DXVECTOR3     speed;                  // 移動スピード
	float           maxMoveSpeed;
	float           decelerate;
	float           gravityScale;

	//===other
	int             SPID;                // the "ONLY ONE" ID : manual setting (手動設定) 0 = default : can not use for searching  """""""under constuction""""""
	int             enemyID;
	int             colliderIdx;            // colliderのインデックス番号
	int				shadowIdx;			    // 影のインデックス番号
	int             enemyType;

	bool            canMove;
	int             canmoveAxis;            // 0: X  1: Y  2: Z  -1: none
	D3DXVECTOR3     CaculatedPos;
	int             scaleAxis;              // 0: X  1: Y  2: Z  -1: all

	//線形移動 tmep
	float LMtime;//Liner move time

};

/*******************************************************************************
* プロトタイプ宣言
*******************************************************************************/



/*******************************************************************************
マクロ定義
*******************************************************************************/



/*******************************************************************************
* グローバル変数
*******************************************************************************/
ENEMYSAVEDATA	save;		// セーブデータ作成場所
OLDENEMYDATA    Osave[MAX_ENEMY];      // 古い版セーブデータ作成場所

/*******************************************************************************
関数名:	void SaveData( void )
引数:	void
戻り値:	void
説明:	セーブデータを作成し、ファイルへ出力する
*******************************************************************************/
void SaveEnemyData(void)
{
	{	// プレイヤーデータをセーブする
		ENEMY *enemy = GetEnemy();

		// プレイヤーの人数分セーブする
		for (int i = 0; i < MAX_ENEMY; i++)
		{
			save.enemy[i] = enemy[i];
		}
	}

	// SAVEDATA構造体ごと全部をファイルに出力する
	FILE *fp;

	printf("\nセーブ開始・・・");
	fp = fopen("saveEnemydata.bin", "wb");			// ファイルをバイナリ書き込みモードでOpenする

	if (fp != NULL)								// ファイルがあれば書き込み、無ければ無視
	{
		fwrite(&save, 1, sizeof(ENEMYSAVEDATA), fp);	// 指定したアドレスから指定したバイト数分ファイルへ書き込む
		fclose(fp);								// Openしていたファイルを閉じる
		//int id = MessageBox(NULL, "Enemy File load success ", "Open mode", MB_YESNOCANCEL | MB_ICONQUESTION);
	}
	else
	{
		//int id = MessageBox(NULL, "Enemy File load fail ", "Open mode", MB_YESNOCANCEL | MB_ICONQUESTION);
	}

}


/*******************************************************************************
関数名:	void LoadData( void )
引数:	void
戻り値:	void
説明:	セーブデータをファイルから読み込む
*******************************************************************************/
bool LoadEnemyData(void)
{
	ENEMY *enemy = GetEnemy();
	// ファイルからセーブデータを読み込む
	FILE* fp;

	printf("\nロード開始・・・");
	fp = fopen("saveEnemydata.bin", "rb");	// ファイルをバイナリ読み込みモードでOpenする

	if (fp != NULL)						// ファイルがあれば書き込み、無ければ無視
	{
		fread(&save, 1, sizeof(ENEMYSAVEDATA), fp);	// 指定したアドレスへ指定したバイト数分ファイルから読み込む
		fclose(fp);								// Openしていたファイルを閉じる

		// セーブデータから各ワークへロードする
		{	// プレイヤーデータをロードする
			ENEMY *enemy = GetEnemy();
			DX11_MODEL temp;
			int collideridx;
			int ChaceCollideridx;

			// プレイヤーの人数分ロードする
			for (int i = 0; i < MAX_ENEMY; i++)
			{
				temp = enemy[i].model;
				collideridx = enemy[i].BodyColliderIdx;
				ChaceCollideridx = enemy[i].ChaseColliderIdx;

				enemy[i] = save.enemy[i];

				enemy[i].model = temp;
				enemy[i].BodyColliderIdx = collideridx;
				enemy[i].ChaseColliderIdx = ChaceCollideridx;
			}
		}

		//int id = MessageBox(NULL, "Enemy File load success ", "Enemy File", MB_OK );
		return true;
	}
	else
	{
		//int id = MessageBox(NULL, "Enemy File load fail ", "Enemy File", MB_OK );
		return false;
	}


}


//===================================================================================
//===================================================================================
//===================================================================================
void UpgradeSaveEnemyData(void)
{
	{	// プレイヤーデータをセーブする
		ENEMY *enemy = GetEnemy();

		// プレイヤーの人数分セーブする
		for (int i = 0; i < MAX_ENEMY; i++)
		{
			Osave[i].Use           = enemy[i].Use;
			Osave[i].pos           = enemy[i].pos;
			Osave[i].rot           = enemy[i].rot;
			Osave[i].scl           = enemy[i].scl;
			Osave[i].enemyType     = enemy[i].enemyType;
			Osave[i].canMove       = enemy[i].CanMove;
			Osave[i].canmoveAxis   = enemy[i].canmoveAxis;
			Osave[i].CaculatedPos  = enemy[i].CaculatedPos;

			Osave[i].model = enemy[i].model;
			Osave[i].mtxWorld = enemy[i].mtxWorld;
			Osave[i].enemyID = enemy[i].enemyID;
			Osave[i].shadowIdx = enemy[i].shadowIdx;
		
		}
	}

	// SAVEDATA構造体ごと全部をファイルに出力する
	FILE *fp;

	printf("\nセーブ開始・・・");
	fp = fopen("saveUGEnemydata.bin", "wb");			// ファイルをバイナリ書き込みモードでOpenする

	if (fp != NULL)								// ファイルがあれば書き込み、無ければ無視
	{
		fwrite(&Osave, 1, sizeof(OLDENEMYDATA) * MAX_ENEMY, fp);	// 指定したアドレスから指定したバイト数分ファイルへ書き込む
		fclose(fp);								// Openしていたファイルを閉じる
		//int id = MessageBox(NULL, "Enemy File load success ", "Open mode", MB_YESNOCANCEL | MB_ICONQUESTION);
	}
	else
	{
		//int id = MessageBox(NULL, "Enemy File load fail ", "Open mode", MB_YESNOCANCEL | MB_ICONQUESTION);
	}

}

bool UpgradeLoadEnemyData(void)
{
	ENEMY *enemy = GetEnemy();
	// ファイルからセーブデータを読み込む
	FILE* fp;

	printf("\nロード開始・・・");
	fp = fopen("saveUGEnemydata.bin", "rb");	// ファイルをバイナリ読み込みモードでOpenする

	if (fp != NULL)						// ファイルがあれば書き込み、無ければ無視
	{
		fread(&Osave, 1, sizeof(OLDENEMYDATA) * MAX_ENEMY, fp);	// 指定したアドレスへ指定したバイト数分ファイルから読み込む
		fclose(fp);								// Openしていたファイルを閉じる

		// セーブデータから各ワークへロードする
		{	// プレイヤーデータをロードする
			ENEMY *enemy = GetEnemy();
			DX11_MODEL temp;
			int collideridx;
			int ChaceCollideridx;

			// プレイヤーの人数分ロードする
			for (int i = 0; i < MAX_ENEMY; i++)
			{
				temp = enemy[i].model;
				collideridx = enemy[i].BodyColliderIdx;
				ChaceCollideridx = enemy[i].ChaseColliderIdx;

				enemy[i].Use           = Osave[i].Use;
				enemy[i].pos           = Osave[i].pos;
				enemy[i].rot           = Osave[i].rot;
				enemy[i].scl           = Osave[i].scl;
				enemy[i].enemyType     = Osave[i].enemyType;
				enemy[i].CanMove       = Osave[i].canMove;
				enemy[i].canmoveAxis   = Osave[i].canmoveAxis;
				enemy[i].CaculatedPos  = Osave[i].CaculatedPos;
				enemy[i].model         = Osave[i].model;
				enemy[i].mtxWorld      = Osave[i].mtxWorld;
				enemy[i].enemyID       = Osave[i].enemyID;
				enemy[i].shadowIdx     = Osave[i].shadowIdx;

				enemy[i].model = temp;
				enemy[i].BodyColliderIdx = collideridx;
				enemy[i].ChaseColliderIdx = ChaceCollideridx;
			}
		}

		//int id = MessageBox(NULL, "Enemy File load success ", "Enemy File", MB_OK );
		return true;
	}
	else
	{
		//int id = MessageBox(NULL, "Enemy File load fail ", "Enemy File", MB_OK );
		return false;
	}


}
