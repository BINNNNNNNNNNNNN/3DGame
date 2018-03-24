//=============================================================================
// Name: FireParticleClass.cpp
//	Des: ������ϵͳ���Դ�ļ�
//=============================================================================

#include"Arrow.h"
#include"d3dx9.h"
#include<tchar.h>

//-------------------------------------------------------------------------------------------------
// Desc: ���캯��
//-------------------------------------------------------------------------------------------------
Arrow::Arrow(LPDIRECT3DDEVICE9 pd3dDevice)
{
	m_pd3dDevice = pd3dDevice;
	m_pVertexBuffer = NULL;
	for (int i = 0; i < 4; i++)
	{
		m_pTexture[i] = NULL;
	}
}
//-------------------------------------------------------------------------------------------------
// Name:   Arrow::Init( )
// Desc: ����ϵͳ��ʼ������
//-------------------------------------------------------------------------------------------------
HRESULT Arrow::Init(int team, int heroindex, float radius, D3DXVECTOR3 pos, float fac)
{
	m_team = team;
	index = heroindex;
	ArrowRadius = radius;
	position = pos;
	facing = fac;
	isDestroy = false;
	//��ʼ������������
	m_Arrow.RotationX = (rand() % 100) / 50.0f*D3DX_PI;
	m_Arrow.RotationY= (rand() % 100) / 50.0f*D3DX_PI;
	m_Arrow.ArrowSpeed = 30.0f;
	m_Arrow.RotationSpeed = 30.0f + rand() % 10 + 10.0f;
	m_Arrow.TextureIndex = rand() % 4;
	m_Arrow.livetime = 0.0f;

	//���������Ӷ��㻺��
	m_pd3dDevice->CreateVertexBuffer(4 * sizeof(ARROWPOINTVERTEX), 0,
		D3DFVF_POINTVERTEX, D3DPOOL_MANAGED, &m_pVertexBuffer, NULL);
	//�������Ӷ��㻺��
	ARROWPOINTVERTEX vertices[]=
	{
		{-300.0f,0.0f,0.0f,0.0f,1.0f},
		{-300.0f,600.0f,0.0f,0.0f,0.0f},
		{300.0f,0.0f,0.0f,1.0f,1.0f},
		{300.f,600.0f,0.0f,1.0f,0.0f}
	};
	//����
	void* pVertices;
	m_pVertexBuffer->Lock(0, sizeof(vertices), (void**)&pVertices, 0);

	//����
	memcpy(pVertices, vertices, sizeof(vertices));
	
	//����
	m_pVertexBuffer->Unlock();

	//����4�ּ�����
	D3DXCreateTextureFromFile(m_pd3dDevice, _T("GameMedia\\arrow1.png"), &m_pTexture[0]);
	D3DXCreateTextureFromFile(m_pd3dDevice, _T("GameMedia\\arrow2.png"), &m_pTexture[1]);
	D3DXCreateTextureFromFile(m_pd3dDevice, _T("GameMedia\\arrow3.png"), &m_pTexture[2]);
	D3DXCreateTextureFromFile(m_pd3dDevice, _T("GameMedia\\arrow4.png"), &m_pTexture[3]);

	return S_OK;
}

//-------------------------------------------------------------------------------------------------
// Name:  Arrow::Update( )
// Desc: ����ϵͳ���º���
//-------------------------------------------------------------------------------------------------
HRESULT Arrow::Update(float fElapsedTime)
{
	if (isDestroy)
		return   S_FALSE;

	D3DXMATRIX m;
	D3DXVECTOR3 pV = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	D3DXVec3TransformCoord(&pV, &pV, D3DXMatrixRotationY(&m, -facing));
	position += m_Arrow.ArrowSpeed*pV;
	m_Arrow.livetime += fElapsedTime;

	if (m_Arrow.livetime > LIFETIME)
	{
		isDestroy = true;
	}

	//���������Ƕ�
	m_Arrow.RotationY += m_Arrow.RotationSpeed * fElapsedTime;
	m_Arrow.RotationX += m_Arrow.RotationSpeed * fElapsedTime;


	return S_OK;
}


//-------------------------------------------------------------------------------------------------
// Name:  Arrow::Render( )
// Desc: ����ϵͳ��Ⱦ����
//-------------------------------------------------------------------------------------------------
HRESULT Arrow::Render()
{
	if (isDestroy)
		return S_FALSE;

	//��������Ч��
	m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, false);

	//��������״̬
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);	//��������ɫ��ϵĵ�һ����������ɫֵ�������
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);		//������ɫ��ϵĵ�һ��������ֵ��ȡ������ɫֵ
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);			//��С����״̬���������������
	m_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);			//�Ŵ����״̬���������������

																					//����Alpha���ϵ��
	m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, true);						//��Alpha���
	m_pd3dDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);						//Դ���ϵ����Ϊ1
	m_pd3dDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);					//Ŀ����ϵ����Ϊ1

																					//�����޳�ģʽΪ���޳��κ���
	m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	//��Ⱦ��
	//���첢���õ�ǰ�����ӵ��������
	static D3DXMATRIX matYaw, matPitch, matTrans, matWorld;
	D3DXMatrixRotationY(&matYaw, m_Arrow.RotationY);
	D3DXMatrixRotationX(&matPitch, m_Arrow.RotationX);
	D3DXMatrixTranslation(&matTrans, position.x+m_Arrow.x, position.y+m_Arrow.y+500, position.z+m_Arrow.z);
	matWorld = matYaw * matPitch * matTrans;
	m_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);

	//��Ⱦ��ǰ������
	m_pd3dDevice->SetTexture(0, m_pTexture[m_Arrow.TextureIndex]);			//��������
	m_pd3dDevice->SetStreamSource(0, m_pVertexBuffer, 0, sizeof(ARROWPOINTVERTEX));  //�Ѱ����ļ�������Ϣ�Ķ��㻺�����Ⱦ��ˮ�������  
	m_pd3dDevice->SetFVF(D3DFVF_POINTVERTEX);									//����FVF�����ʽ
	m_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);						//����


	//�ָ������Ⱦ״̬��Alpha��� ���޳�״̬������
	m_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	m_pd3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pd3dDevice->SetRenderState(D3DRS_LIGHTING, true);

	return S_OK;

}


//-------------------------------------------------------------------------------------------------
// Desc: ��������
//-------------------------------------------------------------------------------------------------
Arrow::~Arrow()
{
	SAFE_RELEASE(m_pVertexBuffer);
	for (int i = 0; i < 4; i++)
	{
		SAFE_RELEASE(m_pTexture[i]);
	}
}