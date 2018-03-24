//=============================================================================
// Name: SkyBox.h
//	Des: ��ά��պ�ϵͳ���ͷ�ļ�
//=============================================================================
#pragma once
#include "D3DUtil.h"

//Ϊ��պ��ඨ��һ��FVF�����ʽ
struct SKYBOXVERTEX
{
	float	x, y, z;
	float	u, v;
};
#define D3DFVF_SKYBOX D3DFVF_XYZ|D3DFVF_TEX1


class SkyBox
{
private:
	LPDIRECT3DDEVICE9				m_pd3dDevice;					//D3D�豸����
	LPDIRECT3DVERTEXBUFFER9	m_pVertexBuffer;						//���㻺�����
	LPDIRECT3DTEXTURE9			m_pTexture[5];						//5�������ӿڶ���,Χ��һ�����ⶥ�ĺ�����
	float										m_Length;			//��պб߳�

public:
	SkyBox(LPDIRECT3DDEVICE9  pDevice);							//���캯��
	virtual ~SkyBox(void);											//��������

public:
	BOOL	InitSkyBox(float Length);								//��ʼ����պк���
	BOOL	LoadSkyTextureFromFile(wchar_t *pFrontTextureFile, wchar_t *pBackTextureFile, wchar_t *pLeftTextureFile, wchar_t *pRightTextureFile, wchar_t *pTopTextureFile);  //���ļ�������պ���������ϵ�����
	void		RenderSkyBox(D3DXMATRIX *pMatWorld, BOOL bRenderFrame); //��Ⱦ��պ�,���ݵ�һ�������趨��պ�������󣬵ڶ�������ѡ���Ƿ���Ⱦ���߿�

};
