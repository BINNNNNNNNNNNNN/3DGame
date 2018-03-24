#pragma once
#include"Character.h"


class Tower:public Character
{
private:
	float timeForAttack;

public:
	Tower(int i);
	~Tower();

	void Init(LPDIRECT3DDEVICE9	p_d3dDevice, Direct3DInput * p_Input, Terrain * p_Terrain, Camera * p_Camera, std::vector<Character * > * p_Character);
	void Control(float f_TimeDelta);
	void Update();
	void Draw();
};