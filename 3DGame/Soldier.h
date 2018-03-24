#pragma once
#include"Character.h"

static D3DXVECTOR3 road0[] = { D3DXVECTOR3(-9381.6,0.0,-1954.3),D3DXVECTOR3(-8917.1,0.0,7584.4),D3DXVECTOR3(1969.8,0.0,8977.7) };//上路
static D3DXVECTOR3 road1[] = { D3DXVECTOR3(-3880.3,0.0,-3510.7), D3DXVECTOR3(3771.8,0.0,4063.0) };//中路
static D3DXVECTOR3 road2[] = { D3DXVECTOR3(-1384.9,0.0,-8887.3) , D3DXVECTOR3(8498.9,0.0,-7611.2) , D3DXVECTOR3(9042.4,0.0, 2215.5) };//下路

class Soldier :public Character
{
private:
	int roadIndex;
	float timeForAttack;
	float timeForResurrection;


public:
	Soldier(int i);
	~Soldier();

	void Init(LPDIRECT3DDEVICE9	p_d3dDevice, Direct3DInput * p_Input, Terrain * p_Terrain, Camera * p_Camera, std::vector<Character * > * p_Character);
	void Control(float f_TimeDelta);
	void Update();
	void Draw();

};
