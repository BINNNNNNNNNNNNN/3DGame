#pragma once
#include"D3DUtil.h"
#include<d3d9.h>
#include<d3dx9.h>
using namespace std;

//-------------------------------------------------------------------------------------------------
//粒子的FVF顶点结构和顶点格式
//-------------------------------------------------------------------------------------------------
//extern struct POINTVERTEX;
struct ARROWPOINTVERTEX
{
	float x, y, z;								//顶点位置
	float u, v;									// 顶点纹理坐标
};
#define D3DFVF_POINTVERTEX (D3DFVF_XYZ|D3DFVF_TEX1)

#define LIFETIME 1.0f                              //总存活时间
#define ARROW_NUMBER     1						   //粒子数目


//-------------------------------------------------------------------------------------------------
// Desc: 粒子结构体的定义
//-------------------------------------------------------------------------------------------------
struct    ARROW
{
	float  x, y, z;                             //坐标位置
	float RotationX;                            //绕自身X轴旋转角度
	float RotationY;                            //绕自身Y轴旋转角度
	float ArrowSpeed;                           //箭速度
	float RotationSpeed;                        //旋转速度      
	int   TextureIndex;                         //纹理索引数

	bool isAlive;                               //是否存在
	float livetime;                             //存活时间
};

//-------------------------------------------------------------------------------------------------
// Desc: 粒子系统类的定义
//-------------------------------------------------------------------------------------------------
class Arrow
{
public:
	LPDIRECT3DDEVICE9                 m_pd3dDevice;         //D3D设备对象
	ARROW                             m_Arrow;              //箭粒子数组
	LPDIRECT3DVERTEXBUFFER9           m_pVertexBuffer;      //粒子顶点缓存 
	LPDIRECT3DTEXTURE9                m_pTexture[4];        //冰箭纹理数组

	int                               m_team;               //队伍
	int                               index;                //哪个下标的人物发出的
	int                               ArrowRadius;          //箭半径
	D3DXVECTOR3                       position;
	float                             facing;
	bool                              isDestroy;


public:
	Arrow(LPDIRECT3DDEVICE9 pd3dDevice);              //构造函数
	~Arrow();                                         //构造函数
	HRESULT  Init(int team, int index, float radius, D3DXVECTOR3 pos, float fac);    //粒子系统初始化函数
	HRESULT  Update(float fElapsedTime);               //粒子系统更新函数
	HRESULT  Render();                                 //粒子系统渲染函数
};