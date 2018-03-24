//=============================================================================
// Name: Camera.h
//	Des: 实现虚拟摄像机类的头文件
//=============================================================================

#pragma once

#include <d3d9.h>
#include <d3dx9.h>

class Camera
{
private:
	D3DXVECTOR3				m_vRightVector;						//右分量向量
	D3DXVECTOR3				m_vUpVector;						//上分量向量
	D3DXVECTOR3				m_vLookVector;						//观察方向向量
	D3DXVECTOR3				m_vCameraPosition;					//摄像机位置的向量
	D3DXVECTOR3				m_vTargetPosition;					//目标观察位置的向量
	D3DXMATRIX				m_matView;							//取景变换矩阵
	D3DXMATRIX				m_matProj;							//投影变换矩阵   
	LPDIRECT3DDEVICE9		m_pd3dDevice;						//Direct3D设备对象

public:
	void CalculateViewMatrix(D3DXMATRIX *pMatrix);				//计算取景变换矩阵
	void GetProjMatrix(D3DXMATRIX *pMatrix);					//返回当前投影矩阵
	void GetCameraPosition(D3DXVECTOR3 *pVector);				//返回当前摄像机位置矩阵
	void GetTargetPosition(D3DXVECTOR3 *pVector);				//返回当前目标位置
	void GetRightVrctor(D3DXVECTOR3 *pVector);					//返回右方向向量	
	void GetUpVector(D3DXVECTOR3 *pVector);						//返回上方向向量
	void GetLookVector(D3DXVECTOR3 *pVector);					//返回观察方向向量

	void SetTargetPosition(D3DXVECTOR3 *pLookat = NULL);		//设置摄像机的目标观察位置向量
	void SetCameraPosition(D3DXVECTOR3 *pVector = NULL);		//设置摄像机所在的位置向量
	void SetRighthVector(D3DXVECTOR3 *vRight);					//设置右方向向量
	void SetUpVector(D3DXVECTOR3 *vUp);							//设置上方向向量
	void SetLookVector(D3DXVECTOR3 *vLook);						//设置观察方向向量
	void SetViewMatrix(D3DXMATRIX *pMatrix = NULL);				//设置取景变换矩阵
	void SetProjMatrix(D3DXMATRIX *pMatrix = NULL);				//设置投影变换矩阵

public:
	void MoveAlongRightVec(float fUnits);						// 沿right向量移动
	void MoveAlongUpVec(float fUnits);							// 沿up向量移动
	void MoveAlongLookVec(float fUnits);						// 沿look向量移动

	void RotationRightVec(float fAngle);						// 绕right向量旋转
	void RotationUpVec(float fAngle);							// 绕up向量旋转
	void RotationLookVec(float fAngle);							// 绕look向量旋转

	void FollowPosition(D3DXVECTOR3 *vCharPos);                 // 摄像机跟随某个位置的上帝视角

public:
	//构造函数和析构函数
	Camera(IDirect3DDevice9 *pd3dDevice);					//构造函数
	virtual ~Camera(void);									//析构函数

};
