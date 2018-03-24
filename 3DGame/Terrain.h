//=============================================================================
// Name: Terrain.h
//	Des: 三维地形系统类的头文件
//=============================================================================

#pragma once

#include <d3d9.h>
#include <d3dx9.h>
#include <vector>
#include <fstream>
#include  "D3DUtil.h"

class Terrain
{
private:
	LPDIRECT3DDEVICE9				m_pd3dDevice;					//D3D设备
	LPDIRECT3DTEXTURE9				m_pTexture;						//纹理
	LPDIRECT3DINDEXBUFFER9			m_pIndexBuffer;					//顶点缓存
	LPDIRECT3DVERTEXBUFFER9			m_pVertexBuffer;				//索引缓存

	int								m_nCellsPerRow;					// 每行的单元格数
	int								m_nCellsPerCol;					// 每列的单元格数
	int								m_nVertsPerRow;					// 每行的顶点数
	int								m_nVertsPerCol;					// 每列的顶点数
	int								m_nNumVertices;					// 顶点总数
	float							m_fTerrainWidth;				// 地形的宽度
	float							m_fTerrainDepth;				// 地形的深度
	float							m_fCellSpacing;					// 单元格的间距
	float							m_fHeightScale;					// 高度缩放系数
	std::vector<float>				m_vHeightInfo;					// 用于存放高度信息

	//定义一个地形的FVF顶点格式
	struct TERRAINVERTEX
	{
		float _x, _y, _z;
		float _u, _v;
		TERRAINVERTEX(float x, float y, float z, float u, float v)
			:_x(x), _y(y), _z(z), _u(u), _v(v) {}
		static const DWORD FVF = D3DFVF_XYZ | D3DFVF_TEX1;
	};

public:
	Terrain(IDirect3DDevice9 *pd3dDevice);							//构造函数
	virtual ~Terrain(void);											//析构函数

public:
	BOOL LoadTerrainFromFile(wchar_t *pRawFileName, wchar_t *pTextureFile);		//从文件加载高度图和纹理的函数
	BOOL InitTerrain(INT nRows, INT nCols, float fSpace, float fScale, float fUTextureNumbers, float fVTextureNumbers);			//地形初始化函数
	BOOL RenderTerrain(D3DXMATRIX *pMatWorld, BOOL bDrawFrame = FALSE);			//地形渲染函数

	float getPositionHeight(D3DXVECTOR3 pos);
};
