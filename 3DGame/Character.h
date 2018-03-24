#pragma once
#include <vector>
#include "Camera.h"
#include"Arrow.h"
#include <d3d9.h>
#include <d3dx9.h>
#include "AllocateHierarchy.h"										//骨骼动画的类
#include "Direct3DInput.h"
#include "Terrain.h"

static	int	totalExp[15] = { 100,120,140,160,180,200,220,240,260,280,300,320,340,360,380 };//各等级总的Exp

// 地板的顶点结构
struct CUSTOMVERTEX
{
	float _x, _y, _z;
	float _u, _v;
	CUSTOMVERTEX(float x, float y, float z, float u, float v)
		: _x(x), _y(y), _z(z), _u(u), _v(v) {}
};
#define D3DFVF_CUSTOMVERTEX  (D3DFVF_XYZ | D3DFVF_TEX1)


enum Team { Red, Blue, Creep};//队伍枚举类型
extern  std::vector < Arrow * >			g_vArrow;

class Character
{
public:
	LPDIRECT3DDEVICE9						pd3dDevice;
	Direct3DInput *							pInput;
	Terrain *							    pTerrain;
	Camera *                                pCamera;

	std::vector<Character * > *				pCharacter;

	float									fTimeDelta;

	//骨骼动画
	LPD3DXFRAME								pFrameRoot;
	CAllocateHierarchy*						pAllocateHier;
	LPD3DXANIMATIONCONTROLLER				pAnimController;

	//HP
	LPDIRECT3DVERTEXBUFFER9					HPBuffer;    // HP顶点缓存对象
	LPDIRECT3DTEXTURE9						HPTexture;    // HP纹理对象
	LPDIRECT3DVERTEXBUFFER9					HPBackBuffer;    // HP顶点缓存对象背景
	LPDIRECT3DTEXTURE9						HPBackTexture;    // HP纹理对象背景


	D3DXMATRIX								model;
	DWORD									walk;
	DWORD									run;
	DWORD									idle;
	DWORD									attack;

	DWORD									currentAnim;
	UINT									currentTrack;

	bool									userControl;

	D3DXVECTOR3								position;
	D3DXVECTOR3								targetPosition;

	float									facing;
	float									facingTarget;
	float									alongXRotation;
	float									alongYRotation;
	float									alongZRotation;

	float									scaling;

	float									preTime;
	float									curTime;

	int                                     Type;//防御塔、水晶类型
	//--------------角色属性-------------------//
	int										id;//下标
	Team									team;//队伍
	int										species;//0 hero,1 tower,2 soliders,3 monster

	bool									alive;//是否活着
	int										level;//等级
	int										hp;//血量
	int										exp;//经验值
	int										atk;//攻击力
	int										def;//防御力

	float									walkSpeed;//走路速度
	float									runSpeed; //跑步速度
	float									turnSpeed;//转向速度
	float									autoSpeed;//自动控制的速度
	float									modalRadius;//模型半径
	float									sightRadius;//视野半径
	float									attackRadius;//攻击半径

	float									timeForResurrection;
	float									timeForAttack;
	float									timeForRun;

public:
	Character();
	~Character();


	void setControlByUser(bool userCon);
	void setPosition(D3DXVECTOR3);
	void setPositonChange(float x, float y, float z);
	void setFacing(float);
	void setScaling(float);
	void setWalkSpeed(float wSpeed);
	void setRunSpeed(float rSpeed);
	void setTurnSpeed(float tSpeed);


	void getPosition(D3DXVECTOR3 *);
	void getTargetPosition(D3DXVECTOR3 *);
	void getFacing(D3DXVECTOR3* pV);//通过给定的矩阵来转换3D向量，将结果投射回到w = 1。
	float getScaling();
	float getWalkSpeed();
	float getRunSpeed();
	float getTurnSpeed();
	int getCurrentTrack();//获得当前轨道
	int getCurrentAnim();//获得当前的动作

	DWORD GetAnimIndex(char Index[]);//获得动作索引


	virtual void Character::Init(LPDIRECT3DDEVICE9	p_d3dDevice, Direct3DInput * p_Input, Terrain * p_Terrain, Camera * p_Camera, std::vector<Character * > * p_Character);

	void CharacterInit(LPDIRECT3DDEVICE9	p_d3dDevice, 
		Direct3DInput * p_Input, Terrain * p_Terrain, Camera * p_Camera, 
		std::vector<Character * > * p_Character,
		LPCWSTR filename,//LPCWSTR是一个指向unicode编码字符串的32位指针，所指向字符串是wchar型（wchar_t是C/C++的字符数据类型，是一种扩展的字符存储方式）
		char * walkAnim, char * runAnim, char * idleAnim, char * attackAnim,
		float xRotation, float yRotation, float zRotation, float scal);


	virtual void Control(float f_TimeDelta);
	void UserControl();
	void AutoControl();

	virtual void Update();
	void ChangeTrack(DWORD name);//改变轨道
	virtual void Draw();//绘制

	void Walk();
	void Run();
	void Idle();
	void Attack();

	bool BeingBlocked();//是否撞墙or撞人

};