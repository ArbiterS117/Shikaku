//=============================================================================
//
// モデルの処理 [model.cpp]
// Author :GP11B132 14 ゴショウケン
//
// DX11_MODEL INSTANCE[num] => LOAD() => DrawModel() : Use Matrix to move,scale,rotate,billboard   [don't forget UnLoad] 
//
//=============================================================================
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include "main.h"
#include "model.h"
#include "camera.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
//#define MODEL_FILENAME		"data/MODEL/miku_01.obj"
#define	VALUE_MOVE_MODEL	(0.50f)					// 移動速度
#define	RATE_MOVE_MODEL		(0.20f)					// 移動慣性係数
#define	VALUE_ROTATE_MODEL	(PI * 0.05f)		    // 回転速度
#define	RATE_ROTATE_MODEL	(0.20f)					// 回転慣性係数
#define	SCALE_MODEL			(10.0f)					// 回転慣性係数


//*****************************************************************************
// 構造体定義
//*****************************************************************************

// マテリアル構造体
struct MODEL_MATERIAL
{
	char						Name[256];
	MATERIAL					Material;
	char						TextureName[256];
};

// 描画サブセット構造体
struct SUBSET
{
	unsigned short	StartIndex;
	unsigned short	IndexNum;
	MODEL_MATERIAL	Material;
};

// モデル構造体
struct MODEL
{
	VERTEX_3D		*VertexArray;
	unsigned short   VertexNum;
	unsigned short	*IndexArray;
	unsigned short	 IndexNum;
	SUBSET			*SubsetArray;
	unsigned short	 SubsetNum;
	unsigned short	 PointNum;
};



//*****************************************************************************
// グローバル変数
//*****************************************************************************


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void LoadObj(char *FileName, MODEL *Model, DX11_MODEL *D3DXModel, bool oriClr = false);
void LoadMaterial(char *FileName, MODEL_MATERIAL **MaterialArray, unsigned short *MaterialNum , bool oriClr = false);

//=============================================================================
// 初期化処理
//=============================================================================
void LoadModel(char *FileName, DX11_MODEL *Model, bool oriClr)
{
	MODEL model;

	LoadObj(FileName, &model, Model, oriClr);

	// 頂点バッファ生成
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(VERTEX_3D) * model.VertexNum;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = model.VertexArray;

		GetDevice()->CreateBuffer(&bd, &sd, &Model->VertexBuffer);
	}

	// インデックスバッファ生成
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(unsigned short) * model.IndexNum;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.pSysMem = model.IndexArray;

		GetDevice()->CreateBuffer(&bd, &sd, &Model->IndexBuffer);
	}

	// サブセット設定
	{
		Model->SubsetArray = new DX11_SUBSET[model.SubsetNum];
		Model->SubsetNum = model.SubsetNum;

		for (unsigned short i = 0; i < model.SubsetNum; i++)
		{
			Model->SubsetArray[i].StartIndex = model.SubsetArray[i].StartIndex;
			Model->SubsetArray[i].IndexNum = model.SubsetArray[i].IndexNum;

			Model->SubsetArray[i].Material.Material = model.SubsetArray[i].Material.Material;

			D3DX11CreateShaderResourceViewFromFile(GetDevice(),
				model.SubsetArray[i].Material.TextureName,
				NULL,
				NULL,
				&Model->SubsetArray[i].Material.Texture,
				NULL);
		}
	}

	delete[] model.VertexArray;
	delete[] model.IndexArray;
	delete[] model.SubsetArray;
}


//=============================================================================
// 終了処理
//=============================================================================
void UnloadModel(DX11_MODEL *Model)
{
	if (Model->VertexBuffer) { Model->VertexBuffer->Release(); Model->VertexBuffer = NULL;}
	if (Model->IndexBuffer)  { Model->IndexBuffer->Release(); Model->IndexBuffer = NULL;}
	if (Model->SubsetArray)  { delete[] Model->SubsetArray; Model->SubsetArray = NULL;}
	if (Model->Vertexlist) { delete[] Model->Vertexlist; Model->Vertexlist = NULL; }
	if (Model->Indexlist) { delete[] Model->Indexlist; Model->Indexlist = NULL; }
}


//=============================================================================
// 描画処理
//=============================================================================
void DrawModel(DX11_MODEL *Model)
{

	// 頂点バッファ設定
	UINT stride = sizeof(VERTEX_3D);
	UINT offset = 0;
	GetDeviceContext()->IASetVertexBuffers(0, 1, &Model->VertexBuffer, &stride, &offset);

	// インデックスバッファ設定
	GetDeviceContext()->IASetIndexBuffer(Model->IndexBuffer, DXGI_FORMAT_R16_UINT, 0);

	// プリミティブトポロジ設定
	GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (unsigned short i = 0; i < Model->SubsetNum; i++)
	{
		// マテリアル設定
		SetMaterial(Model->SubsetArray[i].Material.Material);

		// テクスチャ設定
		GetDeviceContext()->PSSetShaderResources(0, 1, &Model->SubsetArray[i].Material.Texture);

		// ポリゴン描画
		GetDeviceContext()->DrawIndexed(Model->SubsetArray[i].IndexNum, Model->SubsetArray[i].StartIndex, 0);
	}


}

//モデル読込////////////////////////////////////////////
void LoadObj(char *FileName, MODEL *Model, DX11_MODEL *D3DXModel, bool oriClr)
{

	D3DXVECTOR3	*positionArray;
	D3DXVECTOR3	*normalArray;
	D3DXVECTOR2	*texcoordArray;

	unsigned short	positionNum = 0;
	unsigned short	normalNum = 0;
	unsigned short	texcoordNum = 0;
	unsigned short	vertexNum = 0;
	unsigned short	indexNum = 0;
	unsigned short	in = 0;
	unsigned short	subsetNum = 0;

	MODEL_MATERIAL	*materialArray = NULL;
	unsigned short	materialNum = 0;

	char str[256];
	char *s;
	char c;


	FILE *file;
	file = fopen(FileName, "rt");
	if (file == NULL)
	{
		printf("エラー:LoadModel %s \n", FileName);
		return;
	}



	//要素数カウント
	while (true)
	{
		fscanf(file, "%s", str);

		if (feof(file) != 0)
			break;

		if (strcmp(str, "v") == 0)
		{
			positionNum++;
		}
		else if (strcmp(str, "vn") == 0)
		{
			normalNum++;
		}
		else if (strcmp(str, "vt") == 0)
		{
			texcoordNum++;
		}
		else if (strcmp(str, "usemtl") == 0)
		{
			subsetNum++;
		}
		else if (strcmp(str, "f") == 0)
		{
			in = 0;

			do
			{
				fscanf(file, "%s", str);
				vertexNum++;
				in++;
				c = fgetc(file);
			} while (c != '\n' && c != '\r');

			//四角は三角に分割
			if (in == 4)
				in = 6;

			indexNum += in;
		}
	}


	//メモリ確保
	positionArray = new D3DXVECTOR3[positionNum];
	normalArray = new D3DXVECTOR3[normalNum];
	texcoordArray = new D3DXVECTOR2[texcoordNum];
	
	Model->VertexArray = new VERTEX_3D[vertexNum];
	Model->VertexNum = vertexNum;

	Model->IndexArray = new unsigned short[indexNum];
	Model->IndexNum = indexNum;

	Model->SubsetArray = new SUBSET[subsetNum];
	Model->SubsetNum = subsetNum;


	//要素読込
	D3DXVECTOR3 *position = positionArray;
	D3DXVECTOR3 *normal = normalArray;
	D3DXVECTOR2 *texcoord = texcoordArray;

	unsigned short vc = 0; // vertex count
	unsigned short ic = 0; // index count
	unsigned short sc = 0; // material count

	//MeshCollider
	D3DXModel->VertexNum = positionNum;
	D3DXModel->IndexNum = indexNum;
	D3DXModel->Vertexlist = new D3DXVECTOR3[positionNum];
	D3DXModel->Indexlist = new unsigned short[indexNum];

	fseek(file, 0, SEEK_SET);

	while (true)
	{
		fscanf(file, "%s", str);

		if (feof(file) != 0)
			break;

		if (strcmp(str, "mtllib") == 0)
		{
			//マテリアルファイル
			fscanf(file, "%s", str);

			char path[256];
			strcpy(path, "data/model/");
			strcat(path, str);

			LoadMaterial(path, &materialArray, &materialNum, oriClr);
		}
		else if (strcmp(str, "o") == 0)
		{
			//オブジェクト名
			fscanf(file, "%s", str);
		}
		else if (strcmp(str, "v") == 0)
		{
			//頂点座標
			fscanf(file, "%f", &position->x);
			fscanf(file, "%f", &position->y);
			fscanf(file, "%f", &position->z);
			position->x *= SCALE_MODEL;
			position->y *= SCALE_MODEL;
			position->z *= SCALE_MODEL;
			position++;
		}
		else if (strcmp(str, "vn") == 0)
		{
			//法線
			fscanf(file, "%f", &normal->x);
			fscanf(file, "%f", &normal->y);
			fscanf(file, "%f", &normal->z);
			normal++;
		}
		else if (strcmp(str, "vt") == 0)
		{
			//テクスチャ座標
			fscanf(file, "%f", &texcoord->x);
			fscanf(file, "%f", &texcoord->y);
			texcoord->y = 1.0f - texcoord->y;
			texcoord++;
		}
		else if (strcmp(str, "usemtl") == 0)
		{
			//マテリアル
			fscanf(file, "%s", str);

			if (sc != 0)
				Model->SubsetArray[sc - 1].IndexNum = ic - Model->SubsetArray[sc - 1].StartIndex;

			Model->SubsetArray[sc].StartIndex = ic;


			for (unsigned short i = 0; i < materialNum; i++)
			{
				if (strcmp(str, materialArray[i].Name) == 0)
				{
					Model->SubsetArray[sc].Material.Material = materialArray[i].Material;
					strcpy(Model->SubsetArray[sc].Material.TextureName, materialArray[i].TextureName);
					strcpy(Model->SubsetArray[sc].Material.Name, materialArray[i].Name);

					break;
				}
			}

			sc++;

		}
		else if (strcmp(str, "f") == 0)
		{
			//面
			in = 0;

			do
			{
				fscanf(file, "%s", str);

				s = strtok(str, "/");
				Model->VertexArray[vc].Position = positionArray[atoi(s) - 1];
				D3DXModel->Indexlist[ic] = atoi(s) - 1;

				if (s[strlen(s) + 1] != '/')
				{
					//テクスチャ座標が存在しない場合もある
					s = strtok(NULL, "/");
					Model->VertexArray[vc].TexCoord = texcoordArray[atoi(s) - 1];
				}
				s = strtok(NULL, "/");
				Model->VertexArray[vc].Normal = normalArray[atoi(s) - 1];

				Model->VertexArray[vc].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

				Model->IndexArray[ic] = vc;

				ic++;
				vc++;

				in++;
				c = fgetc(file);
			} while (c != '\n' && c != '\r');

			//四角は三角に分割
			if (in == 4)
			{
				Model->IndexArray[ic] = vc - 4;
				D3DXModel->Indexlist[ic] = D3DXModel->Indexlist[ic - 4]; // point 1
				ic++;
				Model->IndexArray[ic] = vc - 2;
				D3DXModel->Indexlist[ic] = D3DXModel->Indexlist[ic - 3]; // point 3 -> 4 1 3 to make triangle face
				ic++;
			}
		}
	}


	if (sc != 0)
		Model->SubsetArray[sc - 1].IndexNum = ic - Model->SubsetArray[sc - 1].StartIndex;

	for (int i = 0; i < positionNum; i++) {
		D3DXModel->Vertexlist[i] = positionArray[i];
	}
	//for (int i = 0; i < indexNum; i++) {
	//	D3DXModel->Indexlist[i] = Model->IndexArray[i];
	//}


	delete[] positionArray;
	delete[] normalArray;
	delete[] texcoordArray;
	delete[] materialArray;

	fclose(file);
}




//マテリアル読み込み///////////////////////////////////////////////////////////////////
void LoadMaterial(char *FileName, MODEL_MATERIAL **MaterialArray, unsigned short *MaterialNum, bool oriClr)
{
	char str[256];

	FILE *file;
	file = fopen(FileName, "rt");
	if (file == NULL)
	{
		printf("エラー:LoadMaterial %s \n", FileName);
		return;
	}

	MODEL_MATERIAL *materialArray;
	unsigned short materialNum = 0;

	//要素数カウント
	while (true)
	{
		fscanf(file, "%s", str);

		if (feof(file) != 0)
			break;


		if (strcmp(str, "newmtl") == 0)
		{
			materialNum++;
		}
	}


	//メモリ確保
	materialArray = new MODEL_MATERIAL[materialNum];
	ZeroMemory(materialArray, sizeof(MODEL_MATERIAL)*materialNum);


	//要素読込
	int mc = -1;

	fseek(file, 0, SEEK_SET);

	while (true)
	{
		fscanf(file, "%s", str);

		if (feof(file) != 0)
			break;


		if (strcmp(str, "newmtl") == 0)
		{
			//マテリアル名
			mc++;
			fscanf(file, "%s", materialArray[mc].Name);
			strcpy(materialArray[mc].TextureName, "");
		}
		else if (strcmp(str, "Ka") == 0)
		{
			//アンビエント
			fscanf(file, "%f", &materialArray[mc].Material.Ambient.r);
			fscanf(file, "%f", &materialArray[mc].Material.Ambient.g);
			fscanf(file, "%f", &materialArray[mc].Material.Ambient.b);
			materialArray[mc].Material.Ambient.a = 1.0f;
		}
		else if (strcmp(str, "Kd") == 0)
		{
			//ディフューズ
			if (oriClr) {
				fscanf(file, "%f", &materialArray[mc].Material.Diffuse.r);
				fscanf(file, "%f", &materialArray[mc].Material.Diffuse.g);
				fscanf(file, "%f", &materialArray[mc].Material.Diffuse.b);
			}
			else {
				materialArray[mc].Material.Diffuse.r = 0.9f;
				materialArray[mc].Material.Diffuse.g = 0.9f;
				materialArray[mc].Material.Diffuse.b = 0.9f;
			}
			materialArray[mc].Material.Diffuse.a = 1.0f;
		}
		else if (strcmp(str, "Ks") == 0)
		{
			//スペキュラ
			fscanf(file, "%f", &materialArray[mc].Material.Specular.r);
			fscanf(file, "%f", &materialArray[mc].Material.Specular.g);
			fscanf(file, "%f", &materialArray[mc].Material.Specular.b);
			materialArray[mc].Material.Specular.a = 1.0f;
		}
		else if (strcmp(str, "Ns") == 0)
		{
			//スペキュラ強度
			fscanf(file, "%f", &materialArray[mc].Material.Shininess);
		}
		else if (strcmp(str, "d") == 0)
		{
			//アルファ
			fscanf(file, "%f", &materialArray[mc].Material.Diffuse.a);
		}
		else if (strcmp(str, "map_Kd") == 0)
		{
			//テクスチャ
			fscanf(file, "%s", str);

			char path[256];
			strcpy(path, "data/model/");
			strcat(path, str);

			strcat(materialArray[mc].TextureName, path);
		}
	}


	*MaterialArray = materialArray;
	*MaterialNum = materialNum;

	fclose(file);
}

