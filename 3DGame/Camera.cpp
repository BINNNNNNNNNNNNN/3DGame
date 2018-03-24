//=============================================================================
// Name: Camera.cpp
//	Des: 实现虚拟摄像机的类的源文件
//=============================================================================
#include "Camera.h"
#include <fstream>
using namespace std;

#ifndef WINDOW_WIDTH
#define WINDOW_WIDTH	800					//窗口宽度
#define WINDOW_HEIGHT	600					//窗口高度
#endif

//-----------------------------------------------------------------------------
// Desc: 构造函数
//-----------------------------------------------------------------------------
Camera::Camera(IDirect3DDevice9 *pd3dDevice)
{
	m_pd3dDevice = pd3dDevice;
	m_vRightVector = D3DXVECTOR3(1.0f, 0.0f, 0.0f);				//默认右向量与X正半轴重合
	m_vUpVector = D3DXVECTOR3(0.0f, 1.0f, 0.0f);				//默认上向量与Y正半轴重合
	m_vLookVector = D3DXVECTOR3(0.0f, 0.0f, 1.0f);				//默认观察向量与Z正半轴重合
	m_vCameraPosition = D3DXVECTOR3(0.0f, 0.0f, -250.0f);			//默认摄像机坐标为(0.0f, 0.0f, -250.0f)
	m_vTargetPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);				//默认观察目标位置为(0.0f, 0.0f, 0.0f);

}


//-----------------------------------------------------------------------------
// Name：Camera::CalculateViewMatrix( )
// Desc: 根据给定的矩阵计算出取景变换矩阵
//-----------------------------------------------------------------------------
void Camera::CalculateViewMatrix(D3DXMATRIX *pMatrix)
{
	//1.先把3个向量都规范化并使其相互垂直，成为一组正交矩阵
	D3DXVec3Normalize(&m_vLookVector, &m_vLookVector);					//规范化观察分量
	D3DXVec3Cross(&m_vUpVector, &m_vLookVector, &m_vRightVector);		//上向量与观察向量垂直
	D3DXVec3Normalize(&m_vUpVector, &m_vUpVector);						//规范化上向量
	D3DXVec3Cross(&m_vRightVector, &m_vUpVector, &m_vLookVector);		//右向量与上向量垂直
	D3DXVec3Normalize(&m_vRightVector, &m_vRightVector);				//规范化右向量


	// 2.创建出取景变换矩阵
	//依次写出取景变换矩阵的第一行
	pMatrix->_11 = m_vRightVector.x;									// Rx
	pMatrix->_12 = m_vUpVector.x;										// Ux
	pMatrix->_13 = m_vLookVector.x;										// Lx
	pMatrix->_14 = 0.0f;
	//依次写出取景变换矩阵的第二行
	pMatrix->_21 = m_vRightVector.y;									// Ry
	pMatrix->_22 = m_vUpVector.y;										// Uy
	pMatrix->_23 = m_vLookVector.y;										// Ly
	pMatrix->_24 = 0.0f;
	//依次写出取景变换矩阵的第三行
	pMatrix->_31 = m_vRightVector.z;									// Rz
	pMatrix->_32 = m_vUpVector.z;										// Uz
	pMatrix->_33 = m_vLookVector.z;										// Lz
	pMatrix->_34 = 0.0f;
	//依次写出取景变换矩阵的第四行
	pMatrix->_41 = -D3DXVec3Dot(&m_vRightVector, &m_vCameraPosition);    // -P*R
	pMatrix->_42 = -D3DXVec3Dot(&m_vUpVector, &m_vCameraPosition);       // -P*U
	pMatrix->_43 = -D3DXVec3Dot(&m_vLookVector, &m_vCameraPosition);     // -P*L
	pMatrix->_44 = 1.0f;
}

//-----------------------------------------------------------------------------
// Name：GetProjMatrix()
// Desc: 返回当前投影矩阵
//-----------------------------------------------------------------------------
void Camera::GetProjMatrix(D3DXMATRIX *pMatrix)
{

	*pMatrix = m_matProj;

}

//-----------------------------------------------------------------------------
// Name：GetCameraPosition()
// Desc: 返回当前摄像机位置矩阵
//-----------------------------------------------------------------------------
void Camera::GetCameraPosition(D3DXVECTOR3 *pVector)
{

	*pVector = m_vCameraPosition;

}

//-----------------------------------------------------------------------------
// Name：GetTargetPosition()
// Desc: 返回当前目标位置
//-----------------------------------------------------------------------------
void Camera::GetTargetPosition(D3DXVECTOR3 *pVector)
{

	*pVector = m_vTargetPosition;

}

//-----------------------------------------------------------------------------
// Name：GetRightVrctor()
// Desc: 返回右方向向量	
//-----------------------------------------------------------------------------
void Camera::GetRightVrctor(D3DXVECTOR3 *pVector)
{

	*pVector = m_vRightVector;

}

//-----------------------------------------------------------------------------
// Name：GetUpVector()
// Desc: 返回上方向向量	
//-----------------------------------------------------------------------------
void Camera::GetUpVector(D3DXVECTOR3 *pVector)
{

	*pVector = m_vUpVector;

}

//-----------------------------------------------------------------------------
// Name：GetLookVector()
// Desc: 返回观察方向向量	
//-----------------------------------------------------------------------------
void Camera::GetLookVector(D3DXVECTOR3 *pVector)
{

	*pVector = m_vLookVector;

}


//-----------------------------------------------------------------------------
// Name：Camera::SetTargetPosition( )
// Desc: 设置摄像机的观察位置
//-----------------------------------------------------------------------------
void Camera::SetTargetPosition(D3DXVECTOR3 *pLookat)
{
	//先看看pLookat是否为默认值NULL
	if (pLookat != NULL)  m_vTargetPosition = (*pLookat);
	else m_vTargetPosition = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	m_vLookVector = m_vTargetPosition - m_vCameraPosition;				//观察点位置减摄像机位置，得到观察方向向量
	D3DXVec3Normalize(&m_vLookVector, &m_vLookVector);					//规范化m_vLookVector向量

	//正交并规范化m_vUpVector和m_vRightVector
	D3DXVec3Cross(&m_vUpVector, &m_vLookVector, &m_vRightVector);
	D3DXVec3Normalize(&m_vUpVector, &m_vUpVector);
	D3DXVec3Cross(&m_vRightVector, &m_vUpVector, &m_vLookVector);
	D3DXVec3Normalize(&m_vRightVector, &m_vRightVector);
}

//-----------------------------------------------------------------------------
// Name：Camera::SetCameraPosition( )
// Desc: 设置摄像机所在的位置
//-----------------------------------------------------------------------------
void Camera::SetCameraPosition(D3DXVECTOR3 *pVector)
{
	D3DXVECTOR3 V = D3DXVECTOR3(0.0f, 0.0f, -250.0f);
	m_vCameraPosition = pVector ? (*pVector) : V;						//三目运算符，如果pVector为真的话，
																		//返回*pVector的值（即m_vCameraPosition=*pVector），
																		//否则返回V的值（即m_vCameraPosition=V）
}

//-----------------------------------------------------------------------------
// Name：Camera::SetViewMatrix( )
// Desc: 设置取景变换矩阵
//-----------------------------------------------------------------------------
void Camera::SetViewMatrix(D3DXMATRIX *pMatrix)
{
	//根据pMatrix的值先做一下判断
	if (pMatrix) m_matView = *pMatrix;
	else CalculateViewMatrix(&m_matView);
	m_pd3dDevice->SetTransform(D3DTS_VIEW, &m_matView);
	//把取景变换矩阵的值分下来分别给右分量，上分量，和观察分量
	m_vRightVector = D3DXVECTOR3(m_matView._11, m_matView._12, m_matView._13);
	m_vUpVector = D3DXVECTOR3(m_matView._21, m_matView._22, m_matView._23);
	m_vLookVector = D3DXVECTOR3(m_matView._31, m_matView._32, m_matView._33);
}

//-----------------------------------------------------------------------------
// Name：Camera::SetProjMatrix( )
// Desc: 设置投影变换矩阵
//-----------------------------------------------------------------------------
void Camera::SetProjMatrix(D3DXMATRIX *pMatrix)
{
	//判断值有没有，没有的话就计算一下
	if (pMatrix != NULL) m_matProj = *pMatrix;
	else D3DXMatrixPerspectiveFovLH(&m_matProj, D3DX_PI / 4.0f, (float)((double)WINDOW_WIDTH / WINDOW_HEIGHT), 1.0f, 300000.0f);//视截体远景设大一点，这样就不怕看不到远处的物体了
	m_pd3dDevice->SetTransform(D3DTS_PROJECTION, &m_matProj);		//设置投影变换矩阵
}

//-----------------------------------------------------------------------------
// Name：Camera::SetRighthVector( )
// Desc: 设置右方向向量
//-----------------------------------------------------------------------------
void Camera::SetRighthVector(D3DXVECTOR3 *vRight)					//设置右方向向量
{
	m_vRightVector = *vRight;
}

//-----------------------------------------------------------------------------
// Name：Camera::SetUpVector( )
// Desc: 设置上方向向量
//-----------------------------------------------------------------------------
void Camera::SetUpVector(D3DXVECTOR3 *vUp)							//设置上方向向量
{
	m_vUpVector = *vUp;
}

//-----------------------------------------------------------------------------
// Name：Camera::SetLookVector( )
// Desc: 设置观察方向向量
//-----------------------------------------------------------------------------
void Camera::SetLookVector(D3DXVECTOR3 *vLook)						//设置观察方向向量
{
	m_vLookVector = *vLook;
}

//-----------------------------------------------------------------------------
// Name：Camera::MoveAlongRightVec( )
// Desc: 沿右向量平移fUnits个单位
//-----------------------------------------------------------------------------
void Camera::MoveAlongRightVec(float fUnits)
{
	//直接乘以fUnits的量来累加就行了
	m_vCameraPosition += m_vRightVector * fUnits;
	m_vTargetPosition += m_vRightVector * fUnits;
}

//-----------------------------------------------------------------------------
// Name：Camera::MoveAlongUpVec( )
// Desc:  沿上向量平移fUnits个单位
//-----------------------------------------------------------------------------
void Camera::MoveAlongUpVec(float fUnits)
{
	//直接乘以fUnits的量来累加就行了
	m_vCameraPosition += m_vUpVector * fUnits;
	m_vTargetPosition += m_vUpVector * fUnits;
}

//-----------------------------------------------------------------------------
// Name：Camera::MoveAlongLookVec( )
// Desc:  沿观察向量平移fUnits个单位
//-----------------------------------------------------------------------------
void Camera::MoveAlongLookVec(float fUnits)
{
	//直接乘以fUnits的量来累加就行了
	m_vCameraPosition += m_vLookVector * fUnits;
	m_vTargetPosition += m_vLookVector * fUnits;
}

//-----------------------------------------------------------------------------
// Name：Camera::RotationRightVec( )
// Desc:  沿右向量旋转fAngle个弧度单位的角度
//-----------------------------------------------------------------------------
void Camera::RotationRightVec(float fAngle)
{
	D3DXMATRIX R;
	D3DXMatrixRotationAxis(&R, &m_vRightVector, fAngle);				//创建出绕m_vRightVector旋转fAngle个角度的R矩阵
	D3DXVec3TransformCoord(&m_vUpVector, &m_vUpVector, &R);				//让m_vUpVector向量绕m_vRightVector旋转fAngle个角度
	D3DXVec3TransformCoord(&m_vLookVector, &m_vLookVector, &R);			//让m_vLookVector向量绕m_vRightVector旋转fAngle个角度

	m_vTargetPosition = m_vLookVector * D3DXVec3Length(&m_vCameraPosition);//更新一下观察点的新位置（方向乘以模=向量）
}

//-----------------------------------------------------------------------------
// Name：Camera::RotationUpVec( )
// Desc:  沿上向量旋转fAngle个弧度单位的角度
//-----------------------------------------------------------------------------
void Camera::RotationUpVec(float fAngle)
{
	D3DXMATRIX R;
	D3DXMatrixRotationAxis(&R, &m_vUpVector, fAngle);					//创建出绕m_vUpVector旋转fAngle个角度的R矩阵
	D3DXVec3TransformCoord(&m_vRightVector, &m_vRightVector, &R);		//让m_vRightVector向量绕m_vUpVector旋转fAngle个角度
	D3DXVec3TransformCoord(&m_vLookVector, &m_vLookVector, &R);			//让m_vLookVector向量绕m_vUpVector旋转fAngle个角度

	m_vTargetPosition = m_vLookVector * D3DXVec3Length(&m_vCameraPosition);//更新一下观察点的新位置（方向乘以模=向量）
}

//-----------------------------------------------------------------------------
// Name：Camera::RotationLookVec( )
// Desc:  沿观察向量旋转fAngle个弧度单位的角度
//-----------------------------------------------------------------------------
void Camera::RotationLookVec(float fAngle)
{
	D3DXMATRIX R;
	D3DXMatrixRotationAxis(&R, &m_vLookVector, fAngle);					//创建出绕m_vLookVector旋转fAngle个角度的R矩阵
	D3DXVec3TransformCoord(&m_vRightVector, &m_vRightVector, &R);		//让m_vRightVector向量绕m_vLookVector旋转fAngle个角度
	D3DXVec3TransformCoord(&m_vUpVector, &m_vUpVector, &R);				//让m_vUpVector向量绕m_vLookVector旋转fAngle个角度

	m_vTargetPosition = m_vLookVector * D3DXVec3Length(&m_vCameraPosition);//更新一下观察点的新位置（方向乘以模=向量）
}


//-----------------------------------------------------------------------------
// Name：Camera::FollowPosition( )
// Desc:  摄像机跟随某个位置的上帝视角
//-----------------------------------------------------------------------------
void Camera::FollowPosition(D3DXVECTOR3 *vCharPos)                 // 摄像机跟随某个位置的上帝视角
{


	static bool haveLoad = false;
	static float x[4], y[4], z[4];
	if (!haveLoad)
	{
		ifstream infile("GameMedia\\CameraFollow.txt");
		for (int i = 0; i < 4; i++)
			infile >> x[i] >> y[i] >> z[i];
		haveLoad = true;
	}
	m_vRightVector = D3DXVECTOR3(x[0], y[0], z[0]);				//默认右向量与X正半轴重合
	m_vUpVector = D3DXVECTOR3(x[1], y[1], z[1]);				//默认上向量与Y正半轴重合
	m_vLookVector = D3DXVECTOR3(x[2], y[2], z[2]);				//默认观察向量与Z正半轴重合
	m_vCameraPosition = D3DXVECTOR3(x[3], y[3], z[3]);			//默认摄像机坐标为(0.0f, 0.0f, -250.0f)

	//	m_vCameraPosition   =	D3DXVECTOR3(-595.0f, 4244.3f, -4085.7f);	//默认摄像机坐标为(0.0f, 0.0f, -250.0f)
	//	m_vTargetPosition   =	D3DXVECTOR3(0.0f, 0.0f, 0.0f);				//默认观察目标位置为(0.0f, 0.0f, 0.0f);
	//5327.3   -    5922.3 = -595
	//4244.3   -    0.0    = 4244.3
	//1554.3   -    5640.0 = -4085.7
	m_vCameraPosition += *vCharPos;
}

//-----------------------------------------------------------------------------
// Desc: 析构函数
//-----------------------------------------------------------------------------
Camera::~Camera(void)
{
}
