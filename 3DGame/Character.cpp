#include "Character.h"
#include <vector>


Character::Character() :
	pd3dDevice(NULL),
	pInput(NULL),
	pTerrain(NULL),
	pCamera(NULL),
	pCharacter(NULL),
	fTimeDelta(0.0f),
	pFrameRoot(NULL),
	pAllocateHier(NULL),
	pAnimController(NULL),
	HPBuffer(NULL),
	HPTexture(NULL),
	HPBackBuffer(NULL),
	HPBackTexture(NULL),
	//	model
	currentAnim(0),
	currentTrack(0),
	userControl(false),
	position(D3DXVECTOR3(1000, 0, 2000)),
	targetPosition(D3DXVECTOR3(1000, 0, 2000)),
	facing(0.0f),
	scaling(1.0f),
	preTime(0.0f),
	curTime(0.0f),
	team(Red),
	alive(true),
	level(1),
	hp(100),
	exp(0),
	atk(0),
	def(0),
	walkSpeed(12.0f),
	runSpeed(36.0f),
	turnSpeed(0.03f),
	modalRadius(50.25f),
	sightRadius(200.0f),
	attackRadius(100.0f)
{
	D3DXMatrixIdentity(&model);

}

Character::~Character()
{

}

void Character::setControlByUser(bool userCon)
{
	userControl = userCon;
}

void Character::setPosition(D3DXVECTOR3 pos)
{
	position = pos;
}

void Character::setPositonChange(float x, float y, float z)
{
	position.x += x;
	position.y += y;
	position.z += z;
}

void Character::setFacing(float fac)
{
	facing = fac;
	while (facing > 2 * D3DX_PI)//保持在0~2π的范围内
		facing -= 2 * D3DX_PI;
	while (facing < 0)
		facing += 2 * D3DX_PI;
}

void Character::setScaling(float sca)
{
	scaling = sca;

	D3DXMATRIX mx;
	D3DXMatrixScaling(&mx, scaling, scaling, scaling);
	model = mx;
	D3DXMatrixRotationX(&mx, alongXRotation);
	D3DXMatrixMultiply(&model, &model, &mx);
	D3DXMatrixRotationY(&mx, alongYRotation);
	D3DXMatrixMultiply(&model, &model, &mx);
	D3DXMatrixRotationZ(&mx, alongZRotation);
	D3DXMatrixMultiply(&model, &model, &mx);
}

void Character::setWalkSpeed(float wSpeed)
{
	walkSpeed = wSpeed;
}
void Character::setRunSpeed(float rSpeed)
{
	runSpeed = rSpeed;
}
void Character::setTurnSpeed(float tSpeed)
{
	turnSpeed = tSpeed;
}

void Character::getPosition(D3DXVECTOR3 * pos)
{
	*pos = position;
}

void Character::getTargetPosition(D3DXVECTOR3 * tar)
{
	*tar = targetPosition;
}

void Character::getFacing(D3DXVECTOR3* pV)
{
	D3DXMATRIX m;
	*pV = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
	D3DXVec3TransformCoord(pV, pV, D3DXMatrixRotationY(&m, -facing));
}

float Character::getScaling()
{
	return scaling;
}

float Character::getWalkSpeed()
{
	return walkSpeed;
}

float Character::getRunSpeed()
{
	return runSpeed;
}
float Character::getTurnSpeed()
{
	return turnSpeed;
}

int Character::getCurrentTrack()
{
	return currentTrack;
}

int Character::getCurrentAnim()
{
	return currentAnim;
}

void Character::CharacterInit(LPDIRECT3DDEVICE9	p_d3dDevice, Direct3DInput * p_Input, Terrain * p_Terrain, Camera * p_Camera, std::vector<Character * > * p_Character, LPCWSTR filename, char * walkAnim, char * runAnim, char * idleAnim, char * attackAnim, float xRotation, float yRotation, float zRotation, float scal)
{
	pd3dDevice = p_d3dDevice;
	pInput = p_Input;
	pTerrain = p_Terrain;
	pCamera = p_Camera;
	pCharacter = p_Character;
	alongXRotation = xRotation;
	alongYRotation = yRotation;
	alongZRotation = zRotation;


	// -----------------------------【创建HP的顶点缓存】--------------------------------
	pd3dDevice->CreateVertexBuffer(4 * sizeof(CUSTOMVERTEX), 0,
		D3DFVF_CUSTOMVERTEX, D3DPOOL_MANAGED, &HPBuffer, NULL);
	pd3dDevice->CreateVertexBuffer(4 * sizeof(CUSTOMVERTEX), 0,
		D3DFVF_CUSTOMVERTEX, D3DPOOL_MANAGED, &HPBackBuffer, NULL);


	// -----------------------------【创建HP纹理】---------------------------------------
	if (team == Red)
		D3DXCreateTextureFromFileEx(pd3dDevice, L"GameMedia\\bloodRed.png", 0, 0, 6, 0, D3DFMT_X8R8G8B8,
			D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0xFF000000, 0, 0, &HPTexture);
	else if (team == Blue)
		D3DXCreateTextureFromFileEx(pd3dDevice, L"GameMedia\\bloodBlue.png", 0, 0, 6, 0, D3DFMT_X8R8G8B8,
			D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0xFF000000, 0, 0, &HPTexture);
	else
		D3DXCreateTextureFromFileEx(pd3dDevice, L"GameMedia\\bloodYellow.png", 0, 0, 6, 0, D3DFMT_X8R8G8B8,
			D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0xFF000000, 0, 0, &HPTexture);

	D3DXCreateTextureFromFileEx(pd3dDevice, L"GameMedia\\bloodBack.png", 0, 0, 6, 0, D3DFMT_X8R8G8B8,
		D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0xFF000000, 0, 0, &HPBackTexture);


	// 创建骨骼动画
	pAllocateHier = new CAllocateHierarchy();
	D3DXLoadMeshHierarchyFromX(filename, D3DXMESH_MANAGED, pd3dDevice,
		pAllocateHier, NULL, &pFrameRoot, &pAnimController);
	SetupBoneMatrixPointers(pFrameRoot, pFrameRoot);

	walk = GetAnimIndex(walkAnim);
	run = GetAnimIndex(runAnim);
	idle = GetAnimIndex(idleAnim);
	attack = GetAnimIndex(attackAnim);

	LPD3DXANIMATIONSET pAnimationSet = NULL;
	pAnimController->GetAnimationSet(idle, &pAnimationSet);
	pAnimController->SetTrackAnimationSet(currentTrack, pAnimationSet);
	pAnimationSet->Release();
	pAnimController->UnkeyAllTrackEvents(currentTrack);
	pAnimController->SetTrackEnable(currentTrack, TRUE);
	pAnimController->KeyTrackSpeed(currentTrack, 1.0f, 0.01f, 0.25f, D3DXTRANSITION_LINEAR);
	pAnimController->KeyTrackWeight(currentTrack, 1.0f, 0.05f, 0.25f, D3DXTRANSITION_LINEAR);

	currentAnim = idle;

	curTime = preTime = timeGetTime();

	setScaling(scal);
}

void Character::Init(LPDIRECT3DDEVICE9	p_d3dDevice, Direct3DInput * p_Input, Terrain * p_Terrain, Camera * p_Camera, std::vector<Character * > * p_Character)
{

}

void Character::Update()
{
	//设置HP的长度
	CUSTOMVERTEX *pVertices = NULL;
	HPBackBuffer->Lock(0, 0, (void**)&pVertices, 0);
	pVertices[0] = CUSTOMVERTEX(-100, 0.0f, 0.0f, 0.0f, 1.0f);
	pVertices[1] = CUSTOMVERTEX(-100, 40.0f, 0.0f, 0.0f, 0.0f);
	pVertices[2] = CUSTOMVERTEX(100, 0.0f, 0.0f, 1.0f, 1.0f);
	pVertices[3] = CUSTOMVERTEX(100, 40.0f, 0.0f, 1.0f, 0.0f);
	HPBackBuffer->Unlock();


	HPBuffer->Lock(0, 0, (void**)&pVertices, 0);
	pVertices[0] = CUSTOMVERTEX(-100, 0.0f, 0.0f, 0.0f, 1.0f);
	pVertices[1] = CUSTOMVERTEX(-100, 40.0f, 0.0f, 0.0f, 0.0f);
	pVertices[2] = CUSTOMVERTEX(hp - 100, 0.0f, 0.0f, 1.0f, 1.0f);
	pVertices[3] = CUSTOMVERTEX(hp - 100, 40.0f, 0.0f, 1.0f, 0.0f);
	HPBuffer->Unlock();


	// 设置骨骼动画的矩阵
	D3DXMATRIX matFinal, mx;
	D3DXMatrixIdentity(&matFinal);
	//根据位置和朝向计算变换矩阵 
	D3DXMatrixRotationY(&matFinal, -facing);
	D3DXMatrixTranslation(&mx, position.x, position.y, position.z);
	D3DXMatrixMultiply(&matFinal, &matFinal, &mx);
	D3DXMatrixMultiply(&matFinal, &model, &matFinal);


	pd3dDevice->SetTransform(D3DTS_WORLD, &matFinal);

	// 更新骨骼动画
	pAnimController->AdvanceTime(fTimeDelta, NULL);	//设置骨骼动画的时间

	UpdateFrameMatrices(pFrameRoot, &matFinal);		//更新框架中的变换矩阵

													


	//更新时间
	preTime = curTime;
	curTime = preTime + fTimeDelta;

}


void Character::ChangeTrack(DWORD name)
{
	if (currentAnim == name)
	{
		LPD3DXANIMATIONSET pTrack;
		pAnimController->GetTrackAnimationSet(currentTrack, &pTrack);
		D3DXTRACK_DESC td;
		pAnimController->GetTrackDesc(currentTrack, &td);
		if (td.Position > pTrack->GetPeriod() / 4)  // come within the change delta of the end
			return;
	}		//要查看下当前轨道进行的程度，以免在轨道过渡时


	DWORD dwNewTrack = (currentTrack == 0 ? 1 : 0);
	LPD3DXANIMATIONSET pAnimationSet = NULL;


	pAnimController->GetAnimationSet(name, &pAnimationSet);
	pAnimController->SetTrackAnimationSet(dwNewTrack, pAnimationSet);

	if (pAnimationSet)
		pAnimationSet->Release();

	pAnimController->UnkeyAllTrackEvents(currentTrack);
	pAnimController->UnkeyAllTrackEvents(dwNewTrack);

	pAnimController->KeyTrackEnable(currentTrack, FALSE, curTime + 0.125f);
	pAnimController->KeyTrackSpeed(currentTrack, 0.0f, curTime, 0.125f, D3DXTRANSITION_LINEAR);
	pAnimController->KeyTrackWeight(currentTrack, 0.0f, curTime, 0.125f, D3DXTRANSITION_LINEAR);

	pAnimController->SetTrackEnable(dwNewTrack, TRUE);
	pAnimController->KeyTrackSpeed(dwNewTrack,	1.0f, curTime, 0.125f, D3DXTRANSITION_LINEAR);
	pAnimController->KeyTrackWeight(dwNewTrack, 1.0f, curTime, 0.125f, D3DXTRANSITION_LINEAR);

	currentAnim = name;
	currentTrack = dwNewTrack;
}


void Character::Draw()
{

	//-----------------------------【利用公告板技术准备绘制HP】--------------------------
	//取得当前的取景变换矩阵
	D3DXMATRIX matView;
	pCamera->CalculateViewMatrix(&matView);


	//根据取景变换矩阵来计算并构造公告板矩阵
	D3DXMATRIX matBillboard;
	D3DXMatrixIdentity(&matBillboard);

	matBillboard._11 = matView._11;
	matBillboard._12 = matView._12;
	matBillboard._13 = -matView._13;
	matBillboard._14 = 0.0f;

	matBillboard._21 = matView._21;
	matBillboard._22 = matView._22;
	matBillboard._23 = -matView._23;
	matBillboard._24 = 0.0f;

	matBillboard._31 = matView._31;
	matBillboard._32 = matView._32;
	matBillboard._33 = -matView._33;
	matBillboard._34 = 0.0f;

	matBillboard._41 = position.x;
	matBillboard._42 = position.y + 1500;
	matBillboard._43 = position.z;
	matBillboard._44 = 1.0f;
	

	//创建一个矩阵，右乘之前的公告板矩阵，并设置成世界矩阵
	D3DXMATRIX matHP;
	D3DXMatrixIdentity(&matHP);
	matHP = matBillboard * matHP;
	pd3dDevice->SetTransform(D3DTS_WORLD, &matHP);

	//-----------------------------【正式绘制HP血量】-----------------------------
	pd3dDevice->SetTexture(0, HPBackTexture);
	pd3dDevice->SetStreamSource(0, HPBackBuffer, 0, sizeof(CUSTOMVERTEX));
	pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
	pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	pd3dDevice->SetTexture(0, HPTexture);
	pd3dDevice->SetStreamSource(0, HPBuffer, 0, sizeof(CUSTOMVERTEX));
	pd3dDevice->SetFVF(D3DFVF_CUSTOMVERTEX);
	pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	DrawFrame(pd3dDevice, pFrameRoot);

}

DWORD Character::GetAnimIndex(char Index[])
{
	HRESULT hr;
	LPD3DXANIMATIONSET pAS;
	DWORD dwRet = -1;

	for (DWORD i = 0; i < pAnimController->GetNumAnimationSets(); ++i)
	{
		hr = pAnimController->GetAnimationSet(i, &pAS);
		if (FAILED(hr))
			continue;

		if (pAS->GetName() &&//获取动画集
			!strncmp(pAS->GetName(), Index, min(strlen(pAS->GetName()), strlen(Index))))//比较名字是否相同
		{
			dwRet = i;
			pAS->Release();
			break;
		}
		pAS->Release();
	}
	return dwRet;
}

void Character::Walk()
{
	ChangeTrack(walk);

	D3DXVECTOR3 vFacing;
	getFacing(&vFacing);
	setPositonChange(walkSpeed * vFacing.x, 0.0, walkSpeed * vFacing.z);
	if (BeingBlocked())
		setPositonChange(-walkSpeed * vFacing.x, 0.0, -walkSpeed * vFacing.z);
}
void Character::Run()
{

	ChangeTrack(run);

	D3DXVECTOR3 vFacing;
	getFacing(&vFacing);
	setPositonChange(runSpeed * vFacing.x, 0.0, runSpeed * vFacing.z);
	if (BeingBlocked())
		setPositonChange(-runSpeed * vFacing.x, 0.0, -runSpeed * vFacing.z);
}
void Character::Idle()
{
	ChangeTrack(idle);
}
void Character::Attack()
{
	ChangeTrack(attack);
}

bool Character::BeingBlocked()
{
	bool blocked = false;
	D3DXVECTOR3 vFacing;
	getFacing(&vFacing);
	setPositonChange(modalRadius * vFacing.x, 0.0, modalRadius * vFacing.z);
	//撞墙
	if (pTerrain->getPositionHeight(position / 100) > 200)
		blocked = true;
	setPositonChange(-modalRadius * vFacing.x, 0.0, -modalRadius * vFacing.z);
	//撞人
	std::vector<Character *> ::iterator it;
	for (it = (*pCharacter).begin(); !blocked && it != (*pCharacter).end(); it++)
	{
		if (*it == this)
			continue;
		D3DXVECTOR3 opPosition;
		(*it)->getPosition(&opPosition);
		opPosition -= position;//距离小于两个modalRadius
		if (D3DXVec3Length(&opPosition)  < 2 * modalRadius)
			blocked = true;
	}

	return blocked;
}

void Character::Control(float f_TimeDelta)
{
	fTimeDelta = f_TimeDelta;


	if (!alive)
		return;


	if (userControl)
		UserControl();
	else
		AutoControl();

}

void	Character::UserControl()
{
	if (pInput->IsKeyDown(DIK_J))
	{
		Attack();
	}
	else if (pInput->IsKeyDown(DIK_W))
	{
		if (pInput->IsKeyDown(DIK_LSHIFT))//按SHIFT键加速快跑
		{
			Run();
		}
		else
		{
			Walk();
		}
	}
	else
	{
		Idle();
	}

	if (pInput->IsKeyDown(DIK_A) && (currentAnim == walk || currentAnim == run))
	{
		setFacing(facing + turnSpeed);

	}
	if (pInput->IsKeyDown(DIK_D) && (currentAnim == walk || currentAnim == run))
	{
		setFacing(facing - turnSpeed);
	}
}

void 	Character::AutoControl()
{

	D3DXVECTOR3	temp = targetPosition - position;
	int length = D3DXVec3Length(&temp);
	if (length < modalRadius * 2)
	{
		bool OK = true;
		{
			D3DXVECTOR3 newTarPos(rand() % 2000 + 800, 0, rand() % 2000 + 800);
			targetPosition = newTarPos;

		}
		autoSpeed = rand() % 2 == 0 ? walkSpeed : runSpeed;

	}
	D3DXVECTOR3 vDiff;
	D3DXVec3Subtract(&vDiff, &targetPosition, &position);
	D3DXVec3Normalize(&vDiff, &vDiff);

	if (vDiff.z == 0.f)
	{
		if (vDiff.x > 0.f)
			facingTarget = 0.0f;
		else
			facingTarget = D3DX_PI;
	}
	else if (vDiff.z > 0.f)
		facingTarget = acosf(vDiff.x);
	else
		facingTarget = acosf(-vDiff.x) + D3DX_PI;

	if (facing > facingTarget && (facing - facingTarget > 2 * turnSpeed || facingTarget + 2 * D3DX_PI - facing > 2 * turnSpeed))
		setFacing(facing - turnSpeed);//向右
	if (facing - facingTarget > 3 * turnSpeed)
		setFacing(facing - turnSpeed);
	else
		setFacing(facingTarget);
	if (facing < facingTarget && (-facing + facingTarget > 2 * turnSpeed || -facingTarget - 2 * D3DX_PI + facing > 2 * turnSpeed))
		if (facingTarget - facing > 3 * turnSpeed)
			setFacing(facing + turnSpeed);//向左
		else
			setFacing(facingTarget);

	
	if (autoSpeed == walkSpeed)
		Walk();
	else if (autoSpeed == runSpeed)
		Run();

}
