#include"Character.h"
#include"Soldier.h"

static	int	totalHpForSoldier[15] = { 445,465,485,505,525,545,565,585,605,625,645,665,685,705,725 };//小兵各等级总的HP
static	int	totalAtkForSoldier[15] = { 12,13,14,15,16,17,18,19,20,21,22,23,24,25,26 };//小兵各等级总的Atk
static	int	totalDefForSoldier[15] = { 0 };//小兵各等级总的Def

Soldier::Soldier(int i)
{
	roadIndex = i;
	walkSpeed = 8.0f;

	timeForAttack = -1.0f;
	timeForResurrection = 30.0f;

	attackRadius = 500;
	sightRadius = 800;

	hp = totalHpForSoldier[level - 1];
	exp = 0;
	atk = totalAtkForSoldier[level - 1];
	def = totalDefForSoldier[level - 1];
}

Soldier::~Soldier()
{
	
}

void Soldier::Init(LPDIRECT3DDEVICE9	p_d3dDevice, Direct3DInput * p_Input, Terrain * p_Terrain, Camera * p_Camera, std::vector<Character * > * p_Character)
{
	CharacterInit(p_d3dDevice, p_Input, p_Terrain, p_Camera, p_Character, L"GameMedia\\tiny_4anim - 副本.x", "Walk", "Jog", "Loiter", "Wave", -D3DX_PI / 2.0f, D3DX_PI / 2.0f, 0.0f, 1.5f);
	
	if (team == Red)//红方自下而上
	{
		switch (roadIndex)
		{
		case 0:
			position = road0[0];
			targetPosition = road0[1];
			break;
		case 1:
			position = road1[0];
			targetPosition = road1[1];
			break;
		case 2:
			position = road2[0];
			targetPosition = road2[1];
			break;
		}
	}
	else if(team == Blue)//蓝方自上而下
	{
		switch (roadIndex)
		{
		case 0:
			position = road0[2];
			targetPosition = road0[1];
			break;
		case 1:
			position = road1[1];
			targetPosition = road1[0];
			break;
		case 2:
			position = road2[2];
			targetPosition = road2[1];
			break;
		}
	}

}

void Soldier::Control(float f_TimeDelta)
{
	fTimeDelta = f_TimeDelta;

	atk = totalAtkForSoldier[level - 1];
	def = totalDefForSoldier[level - 1];


	if (!alive)
		return;

	//到了上下路的拐角处
	if (D3DXVec3Length(&(targetPosition - position)) < 2 * modalRadius)
	{
		if (team == Red)
		{
			switch (roadIndex)
			{
			case 0:
				targetPosition = road0[2];
				break;
			case 1:
				targetPosition = road1[1];
				break;
			case 2:
				targetPosition = road2[2];
				break;
			}
		}
		else if(team==Blue)
		{
			switch (roadIndex)
			{
			case 0:
				targetPosition = road0[0];
				break;
			case 1:
				targetPosition = road1[0];
				break;
			case 2:
				targetPosition = road2[0];
				break;
			}
		}
	}

	bool isAttack = false, isFollow = false;
	float lenCanAttack, lenCanFollow;
	D3DXVECTOR3 tempTargetPosition = targetPosition;
	//能否攻击,能否追击
	for (int i = 0; i < (pCharacter)->size(); i++)
	{
		if (!(*pCharacter)[i]->alive)
			continue;
		if ((*pCharacter)[i]->team == team)//同一队
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

	D3DXVECTOR3 vDiff;
	if (tempTargetPosition == targetPosition)
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
		Walk();


}

void Soldier::Update()
{
	if (timeForAttack > 0.0f)
		timeForAttack -= fTimeDelta;

	if (!alive)
	{
		timeForResurrection -= fTimeDelta;
		if (timeForResurrection <= 0.0f)
		{
			alive = true;
			hp = hp = totalHpForSoldier[level - 1];
			if (team == Red)
			{
				switch (roadIndex)
				{
				case 0:
					position = road0[0];
					targetPosition = road0[1];
					break;
				case 1:
					position = road1[0];
					targetPosition = road1[1];
					break;
				case 2:
					position = road2[0];
					targetPosition = road2[1];
					break;
				}
			}
			else if (team == Blue)
			{
				switch (roadIndex)
				{
				case 0:
					position = road0[2];
					targetPosition = road0[1];
					break;
				case 1:
					position = road1[1];
					targetPosition = road1[0];
					break;
				case 2:
					position = road2[2];
					targetPosition = road2[1];
					break;
				}
			}

		}
	}

	//设置HP的长度
	CUSTOMVERTEX *pVertices = NULL;
	HPBackBuffer->Lock(0, 0, (void**)&pVertices, 0);
	pVertices[0] = CUSTOMVERTEX(-totalHpForSoldier[level - 1] / 2, 0.0f, 0.0f, 0.0f, 1.0f);
	pVertices[1] = CUSTOMVERTEX(-totalHpForSoldier[level - 1] / 2, 40.0f, 0.0f, 0.0f, 0.0f);
	pVertices[2] = CUSTOMVERTEX(totalHpForSoldier[level - 1] / 2, 0.0f, 0.0f, 1.0f, 1.0f);
	pVertices[3] = CUSTOMVERTEX(totalHpForSoldier[level - 1] / 2, 40.0f, 0.0f, 1.0f, 0.0f);
	HPBackBuffer->Unlock();


	HPBuffer->Lock(0, 0, (void**)&pVertices, 0);
	pVertices[0] = CUSTOMVERTEX(-totalHpForSoldier[level - 1] / 2, 0.0f, 0.0f, 0.0f, 1.0f);
	pVertices[1] = CUSTOMVERTEX(-totalHpForSoldier[level - 1] / 2, 40.0f, 0.0f, 0.0f, 0.0f);
	pVertices[2] = CUSTOMVERTEX(hp - totalHpForSoldier[level - 1] / 2, 0.0f, 0.0f, 1.0f, 1.0f);
	pVertices[3] = CUSTOMVERTEX(hp - totalHpForSoldier[level - 1] / 2, 40.0f, 0.0f, 1.0f, 0.0f);
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

void Soldier::Draw()
{
	if (!alive)
		return;
	Character::Draw();

}