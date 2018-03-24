#pragma once
#include"D3DUtil.h"
#include<d3d9.h>
#include<d3dx9.h>
using namespace std;

//-------------------------------------------------------------------------------------------------
//���ӵ�FVF����ṹ�Ͷ����ʽ
//-------------------------------------------------------------------------------------------------
//extern struct POINTVERTEX;
struct ARROWPOINTVERTEX
{
	float x, y, z;								//����λ��
	float u, v;									// ������������
};
#define D3DFVF_POINTVERTEX (D3DFVF_XYZ|D3DFVF_TEX1)

#define LIFETIME 1.0f                              //�ܴ��ʱ��
#define ARROW_NUMBER     1						   //������Ŀ


//-------------------------------------------------------------------------------------------------
// Desc: ���ӽṹ��Ķ���
//-------------------------------------------------------------------------------------------------
struct    ARROW
{
	float  x, y, z;                             //����λ��
	float RotationX;                            //������X����ת�Ƕ�
	float RotationY;                            //������Y����ת�Ƕ�
	float ArrowSpeed;                           //���ٶ�
	float RotationSpeed;                        //��ת�ٶ�      
	int   TextureIndex;                         //����������

	bool isAlive;                               //�Ƿ����
	float livetime;                             //���ʱ��
};

//-------------------------------------------------------------------------------------------------
// Desc: ����ϵͳ��Ķ���
//-------------------------------------------------------------------------------------------------
class Arrow
{
public:
	LPDIRECT3DDEVICE9                 m_pd3dDevice;         //D3D�豸����
	ARROW                             m_Arrow;              //����������
	LPDIRECT3DVERTEXBUFFER9           m_pVertexBuffer;      //���Ӷ��㻺�� 
	LPDIRECT3DTEXTURE9                m_pTexture[4];        //������������

	int                               m_team;               //����
	int                               index;                //�ĸ��±�����﷢����
	int                               ArrowRadius;          //���뾶
	D3DXVECTOR3                       position;
	float                             facing;
	bool                              isDestroy;


public:
	Arrow(LPDIRECT3DDEVICE9 pd3dDevice);              //���캯��
	~Arrow();                                         //���캯��
	HRESULT  Init(int team, int index, float radius, D3DXVECTOR3 pos, float fac);    //����ϵͳ��ʼ������
	HRESULT  Update(float fElapsedTime);               //����ϵͳ���º���
	HRESULT  Render();                                 //����ϵͳ��Ⱦ����
};