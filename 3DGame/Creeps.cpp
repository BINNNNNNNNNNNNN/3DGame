#include "Character.h"
#include "Creeps.h"
using namespace std;

static	int	totalHpForCreeps[15] = { 800,810,820,830,840,850,860,870,880,890,900,920,940,960,1000 };//各等级总的HP
static	int	totalAtkForCreeps[15] = { 54,55,56,57,58,59,60,61,62,63,64,65,66,67,68 };//各等级总的Atk
static	int	totalDefForCreeps[15] = { 0 };//各等级总的Def

Creeps::Creeps(D3DXVECTOR3 pos) :
	timeForResurrection(50.0f),
	timeForAttack(2.0f),
	areaRaidus(500.0f)
{
	areaCenterPosition = pos;
	sightRadius = 1000;
	attackRadius = 500;

	position = pos;

	hp = totalHpForCreeps[level - 1];
	exp = 0;
	atk = totalAtkForCreeps[level - 1];
	def = totalDefForCreeps[level - 1];

}

Creeps:: ~Creeps()
{

}

void Creeps::Control(float f_TimeDelta)
{
	fTimeDelta = f_TimeDelta;

	atk = totalAtkForCreeps[level - 1];
	def = totalDefForCreeps[level - 1];


	bool isAttack = false, isFollow = false;
	float lenCanAttack, lenCanFollow;
	D3DXVECTOR3 tempTargetPosition = targetPosition;

	//能否攻击,能否追击
	for (int i = 0; i < (pCharacter)->size(); i++)
	{
		if (!(*pCharacter)[i]->alive)
			continue;
		if ((*pCharacter)[i]->team == team)
			continue;
		float length = D3DXVec3Length(&((*pCharacter)[i]->position - position));
		if (length < attackRadius)
		{
			tempTargetPosition = (*pCharacter)[i]->position;
			isAttack = true;
			break;
		}
		else if (length < sightRadius)
		{
			tempTargetPosition = (*pCharacter)[i]->position;
			isFollow = true;
		}
	}

	//无目标，回领地中央
	if (!isAttack && !isFollow)
		targetPosition = areaCenterPosition;

	D3DXVECTOR3 vDiff;
	//离开了领地，回去
	if (D3DXVec3Length(&(position - areaCenterPosition)) < areaRaidus)
	{
		targetPosition = areaCenterPosition;
		tempTargetPosition = targetPosition;
	}
	else if (tempTargetPosition == targetPosition)
		D3DXVec3Subtract(&vDiff, &targetPosition, &position);
	else
		D3DXVec3Subtract(&vDiff, &tempTargetPosition, &position);

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
		setFacing(facing - turnSpeed);
	if (facing - facingTarget > 3 * turnSpeed)
		setFacing(facing - turnSpeed);
	else
		setFacing(facingTarget);
	if (facing < facingTarget && (-facing + facingTarget > 2 * turnSpeed || -facingTarget - 2 * D3DX_PI + facing > 2 * turnSpeed))
		if (facingTarget - facing > 3 * turnSpeed)
			setFacing(facing + turnSpeed);
		else
			setFacing(facingTarget);

	if (isAttack && timeForAttack <= 0.0f)
	{
		Attack();

		bool ok = false;
		for (int i = 0; i < g_vArrow.size(); i++)
		{
			if (g_vArrow[i]->isDestroy)
			{
				g_vArrow[i]->Init(team, id, 500.0f, position, facing);
				ok = true;
				break;
			}
		}
		if (!ok)
		{
			Arrow * temp = new Arrow(pd3dDevice);
			g_vArrow.push_back(temp);

			g_vArrow[g_vArrow.size() - 1]->Init(team, id, 500.0f, position, facing);

		}

		timeForAttack = 3.0f;
	}

	if (!isAttack)
		Idle();

}

void Creeps::Init(LPDIRECT3DDEVICE9	p_d3dDevice, Direct3DInput * p_Input, Terrain * p_Terrain, Camera * p_Camera, std::vector<Character * > * p_Character)
{
	CharacterInit(p_d3dDevice, p_Input, p_Terrain, p_Camera, p_Character, L"GameMedia\\moshou11.X", "walk", "run", "ready", "magicAttack", -D3DX_PI / 2.0f, 0.0f, 0.0f, 15.0f);
}



void Creeps::Update()
{

	if (timeForAttack > 0.0f)
		timeForAttack -= fTimeDelta;

	if (!alive)
	{
		timeForResurrection -= fTimeDelta;
		if (timeForResurrection <= 0.0f)
		{
			alive = true;
			hp = totalHpForCreeps[level - 1];
			setPosition(areaCenterPosition);

		}
	}

	//设置HP的长度
	CUSTOMVERTEX *pVertices = NULL;
	HPBackBuffer->Lock(0, 0, (void**)&pVertices, 0);
	pVertices[0] = CUSTOMVERTEX(-totalHpForCreeps[level - 1] / 2, 0.0f, 0.0f, 0.0f, 1.0f);
	pVertices[1] = CUSTOMVERTEX(-totalHpForCreeps[level - 1] / 2, 40.0f, 0.0f, 0.0f, 0.0f);
	pVertices[2] = CUSTOMVERTEX(totalHpForCreeps[level - 1] / 2, 0.0f, 0.0f, 1.0f, 1.0f);
	pVertices[3] = CUSTOMVERTEX(totalHpForCreeps[level - 1] / 2, 40.0f, 0.0f, 1.0f, 0.0f);
	HPBackBuffer->Unlock();


	HPBuffer->Lock(0, 0, (void**)&pVertices, 0);
	pVertices[0] = CUSTOMVERTEX(-totalHpForCreeps[level - 1] / 2, 0.0f, 0.0f, 0.0f, 1.0f);
	pVertices[1] = CUSTOMVERTEX(-totalHpForCreeps[level - 1] / 2, 40.0f, 0.0f, 0.0f, 0.0f);
	pVertices[2] = CUSTOMVERTEX(hp - totalHpForCreeps[level - 1] / 2, 0.0f, 0.0f, 1.0f, 1.0f);
	pVertices[3] = CUSTOMVERTEX(hp - totalHpForCreeps[level - 1] / 2, 40.0f, 0.0f, 1.0f, 0.0f);
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


void Creeps::Draw()
{
	if (!alive)
		return;
	Character::Draw();
}