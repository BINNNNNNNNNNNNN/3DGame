#pragma once
#include"Character.h"

class Creeps :public Character
{
public:
	D3DXVECTOR3 areaCenterPosition;//�������λ��
	float       timeForResurrection;//����ʱ��
	float       timeForAttack;////���𹥻���ʱ����
	float	    areaRaidus;//��ذ뾶


public:
	Creeps(D3DXVECTOR3 pos);
	~Creeps();

	void Init(LPDIRECT3DDEVICE9	p_d3dDevice, Direct3DInput * p_Input, Terrain * p_Terrain, Camera * p_Camera, std::vector<Character * > * p_Character);
	void Control(float f_TimeDelta);
	void Update();
	void Draw();
};