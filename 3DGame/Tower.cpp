#include"Tower.h"
#include "Character.h"

static int totalHpForTower[15] = { 4000,4200,4400,4600,4800,5000,5200,5400,5600,5800,6000,6200,6400,6600,6800 };//防御塔各等级总的HP
static int HpForCrystal = 10000;//水晶HP
static int totalAtkForTower[15] = { 152,156,160,164,168,172,176,180,184,188,192,196,200,204,208 };//防御塔各等级总的Atk
static int AtkForCrystal = 500;//水晶Atk
static int totalDefForTower[15] = { 0 };//防御塔各等级总的Def
static int DefForCrystal = 100;


Tower::Tower(int i)
{
	timeForAttack = -1.0f;
	attackRadius = 2000;
	hp = totalHpForTower[level - 1];
	exp = 0;
	atk = totalAtkForTower[level - 1];
	def = totalDefForTower[level - 1];
	Type = i;
}

void Tower::Init(LPDIRECT3DDEVICE9	p_d3dDevice, Direct3DInput * p_Input, Terrain * p_Terrain, Camera * p_Camera, std::vector<Character * > * p_Character)
{
	if (Type == 0 || Type == 13)
		CharacterInit(p_d3dDevice, p_Input, p_Terrain, p_Camera, p_Character, L"GameMedia\\水晶.X", "idle", "idle", "idle", "idle", -D3DX_PI / 2.0f, D3DX_PI / 2.0f, 0.0f, 1.0f);
	else
		CharacterInit(p_d3dDevice, p_Input, p_Terrain, p_Camera, p_Character, L"GameMedia\\防御塔.X", "idle", "idle", "idle", "idle", -D3DX_PI / 2.0f, D3DX_PI / 2.0f, 0.0f, 1.0f);
}

void Tower::Control(float f_TimeDelta)
{
	fTimeDelta = f_TimeDelta;

	atk = totalAtkForTower[level - 1];
	def = totalDefForTower[level - 1];
	
	if (!alive)
		return;

	bool isAttack = false;
	float lenCanAttack;
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
	}

	D3DXVECTOR3 vDiff;
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
	
	while (facingTarget > 2 * D3DX_PI)//保持在0~2π
		facingTarget -= 2 * D3DX_PI;
	while (facingTarget < 0)
		facingTarget += 2 * D3DX_PI;

	if (isAttack && timeForAttack <= 0.0f)
	{
		Attack();

		bool ok = false;
		for (int i = 0; i < g_vArrow.size(); i++)
		{
			if (g_vArrow[i]->isDestroy)
			{
				g_vArrow[i]->Init(team, id, 500.0f, position, facingTarget);
				ok = true;
				break;
			}
		}
		if (!ok)
		{
			Arrow * temp = new Arrow(pd3dDevice);
			g_vArrow.push_back(temp);

			g_vArrow[g_vArrow.size() - 1]->Init(team, id, 500.0f, position, facingTarget);

		}

		timeForAttack = 3.0f;
	}

}


void Tower::Update()
{
	if (timeForAttack > 0.0f)
		timeForAttack -= fTimeDelta;

	//设置防御塔HP的长度
	CUSTOMVERTEX *pVertices = NULL;
	HPBackBuffer->Lock(0, 0, (void**)&pVertices, 0);
	pVertices[0] = CUSTOMVERTEX(-totalHpForTower[level - 1] / 2, 0.0f, 0.0f, 0.0f, 1.0f);
	pVertices[1] = CUSTOMVERTEX(-totalHpForTower[level - 1] / 2, 40.0f, 0.0f, 0.0f, 0.0f);
	pVertices[2] = CUSTOMVERTEX(totalHpForTower[level - 1] / 2, 0.0f, 0.0f, 1.0f, 1.0f);
	pVertices[3] = CUSTOMVERTEX(totalHpForTower[level - 1] / 2, 40.0f, 0.0f, 1.0f, 0.0f);
	HPBackBuffer->Unlock();


	HPBuffer->Lock(0, 0, (void**)&pVertices, 0);
	pVertices[0] = CUSTOMVERTEX(-totalHpForTower[level - 1] / 2, 0.0f, 0.0f, 0.0f, 1.0f);
	pVertices[1] = CUSTOMVERTEX(-totalHpForTower[level - 1] / 2, 40.0f, 0.0f, 0.0f, 0.0f);
	pVertices[2] = CUSTOMVERTEX(hp - totalHpForTower[level - 1] / 2, 0.0f, 0.0f, 1.0f, 1.0f);
	pVertices[3] = CUSTOMVERTEX(hp - totalHpForTower[level - 1] / 2, 40.0f, 0.0f, 1.0f, 0.0f);
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


void Tower::Draw()
{
	if (!alive)
		return;
	Character::Draw();
}


Tower::~Tower()
{

}