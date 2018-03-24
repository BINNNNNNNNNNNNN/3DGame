#include"Character.h"
#include"Hero.h"

int totalHpForHero[15] = { 616,700,784,869,953,1037,1121,1205,1289,1373,1457,1541,1625,1709,1793 };//Ӣ�۸��ȼ��ܵ�HP
static	int	totalAtkForHero[15] = { 157,162,167,172,177,182,187,192,197,202,207,212,217,222,227 };//Ӣ�۸��ȼ��ܵ�Atk
static	int	totalDefForHero[15] = { 0 };//Ӣ�۸��ȼ��ܵ�Def

Hero::Hero(int modal)
{
	heroModal = modal;
	timeForAttack = -1.0f;
	timeForRun = -0.5f;
	timeForResurrection = 30.0f;
	attackRadius = 1000;
	sightRadius = 1500;

	hp = totalHpForHero[level - 1];
	exp = 0;
	atk = totalAtkForHero[level - 1];
	def = totalDefForHero[level - 1];
}

Hero::~Hero()
{

}

void Hero::Init(LPDIRECT3DDEVICE9	p_d3dDevice, Direct3DInput * p_Input, Terrain * p_Terrain, Camera * p_Camera, std::vector<Character * > * p_Character)
{
	if (heroModal == ARCHER)
		CharacterInit(p_d3dDevice, p_Input, p_Terrain, p_Camera, p_Character, L"GameMedia\\������.X", "walk", "run", "idle", "attack", 0, D3DX_PI * 3 / 2.0f, 0.0f, 1.3f);
	else
		CharacterInit(p_d3dDevice, p_Input, p_Terrain, p_Camera, p_Character, L"GameMedia\\�����.X", "walk", "run", "idle", "attack", 0, D3DX_PI * 3 / 2.0f, 0.0f, 2.0f);
}

void Hero::Control(float f_TimeDelta)
{
	fTimeDelta = f_TimeDelta;

	atk = totalAtkForHero[level - 1];
	def = totalDefForHero[level - 1];


	if (!alive)
		return;


	if (userControl)
		UserControl();
	else
		AutoControl();

}


void Hero::UserControl()
{
	if (pInput->IsKeyDown(DIK_J) && timeForAttack <= 0.0f)
	{
		Attack();
		
		mciSendString(L"play Fight from 0", NULL, 0, NULL);
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


		timeForAttack = 1.0f;
	}
	else if (pInput->IsKeyDown(DIK_W))
	{
		if ((pInput->IsKeyDown(DIK_LSHIFT)) && timeForRun > -1.0f && timeForRun < 1.0f)
		{
			Run();
			timeForRun = -5.0f;
		}
		else if (timeForRun < -1.0f)
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
		if (currentAnim == attack)
			Attack();
		else
			Idle();
	}

	if (pInput->IsKeyDown(DIK_A))
	{
		setFacing(facing + turnSpeed);//��ת

	}
	if (pInput->IsKeyDown(DIK_D))
	{
		setFacing(facing - turnSpeed);//��ת
	}


}


void Hero::AutoControl()
{
	bool isAttack = false, isFollow = false;
	float lenCanAttack, lenCanFollow;
	D3DXVECTOR3 tempTargetPosition = targetPosition;
	//�ܷ񹥻�,�ܷ�׷��
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




void Hero::Update()
{
	if (timeForAttack > 0.0f)
		timeForAttack -= fTimeDelta;
	if (timeForRun > 1.0f)
		timeForRun -= fTimeDelta;
	if (timeForRun < -1.0f)
	{
		timeForRun += fTimeDelta;
		if (timeForRun >= -1.0f)
			timeForRun = 10.0f;
	}

	if (!alive)
	{
		timeForResurrection -= fTimeDelta;
		if (timeForResurrection <= 0.0f)
		{
			alive = true;
			hp = totalHpForHero[level - 1];
			if (team == Red)
			{
				setPosition(D3DXVECTOR3(-7565.6, 0.0, -7190.5));
			}
			else
			{
				setPosition(D3DXVECTOR3(3516.8, 0.0, 3393.6));
			}

		}
	}

	//����HP�ĳ���
	CUSTOMVERTEX *pVertices = NULL;
	HPBackBuffer->Lock(0, 0, (void**)&pVertices, 0);
	pVertices[0] = CUSTOMVERTEX(-totalHpForHero[level - 1] / 2, 0.0f, 0.0f, 0.0f, 1.0f);
	pVertices[1] = CUSTOMVERTEX(-totalHpForHero[level - 1] / 2, 40.0f, 0.0f, 0.0f, 0.0f);
	pVertices[2] = CUSTOMVERTEX(totalHpForHero[level - 1] / 2, 0.0f, 0.0f, 1.0f, 1.0f);
	pVertices[3] = CUSTOMVERTEX(totalHpForHero[level - 1] / 2, 40.0f, 0.0f, 1.0f, 0.0f);
	HPBackBuffer->Unlock();


	HPBuffer->Lock(0, 0, (void**)&pVertices, 0);
	pVertices[0] = CUSTOMVERTEX(-totalHpForHero[level - 1] / 2, 0.0f, 0.0f, 0.0f, 1.0f);
	pVertices[1] = CUSTOMVERTEX(-totalHpForHero[level - 1] / 2, 40.0f, 0.0f, 0.0f, 0.0f);
	pVertices[2] = CUSTOMVERTEX(hp - totalHpForHero[level - 1] / 2, 0.0f, 0.0f, 1.0f, 1.0f);
	pVertices[3] = CUSTOMVERTEX(hp - totalHpForHero[level - 1] / 2, 40.0f, 0.0f, 1.0f, 0.0f);
	HPBuffer->Unlock();


	// ���ù��������ľ���
	D3DXMATRIX matFinal, mx;
	D3DXMatrixIdentity(&matFinal);
	//����λ�úͳ������任���� 
	D3DXMatrixRotationY(&matFinal, -facing);
	D3DXMatrixTranslation(&mx, position.x, position.y, position.z);
	D3DXMatrixMultiply(&matFinal, &matFinal, &mx);
	D3DXMatrixMultiply(&matFinal, &model, &matFinal);


	pd3dDevice->SetTransform(D3DTS_WORLD, &matFinal);

	// ���¹�������
	pAnimController->AdvanceTime(fTimeDelta, NULL);	//���ù���������ʱ��

	UpdateFrameMatrices(pFrameRoot, &matFinal);		//���¿���еı任����

		
													//����ʱ��
	preTime = curTime;
	curTime = preTime + fTimeDelta;

}

void Hero::Draw()
{
	if (!alive)
		return;
	Character::Draw();
}
