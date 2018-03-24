#pragma once

#include"Character.h"

#define ARCHER  1 //弓箭手
#define MONKEY 2 //没头的孙悟空

class Hero :public Character
{
private:
	int heroModal;

public:
	Hero(int modal);
	~Hero();

	void Init(LPDIRECT3DDEVICE9	p_d3dDevice, Direct3DInput * p_Input, Terrain * p_Terrain, Camera * p_Camera, std::vector<Character * > * p_Character);
	void Control(float f_TimeDelta);
	void Update();
	void Draw();

	void UserControl();
	void AutoControl();

};