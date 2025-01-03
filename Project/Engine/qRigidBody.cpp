#include "pch.h"
#include "qRigidBody.h"

#include "qTimeMgr.h"
#include "qTransform.h"

qRigidBody::qRigidBody()
	: qComponent(COMPONENT_TYPE::RIGIDBODY)
	, m_Mass(1.f)
	, m_InitWalkSpeed(0.f)
	, m_MaxWalkSpeed(0.f)
	, m_MaxGravitySpeed(0.f)
	, m_Friction(500.f)
	, m_GravityAccel(980.f)
	, m_UseGravity(false)
	, m_Ground(true)
	, m_JumpSpeed(600.f)
	, m_DoubleJumpSpeed(500.f)
	, m_GroundFunc(nullptr)
	, m_AirFunc(nullptr)
	, m_GroundInst(nullptr)
	, m_AirInst(nullptr)
	, m_GroundDelegate(nullptr)
	, m_AirDelegate(nullptr)
{
}

qRigidBody::qRigidBody(const qRigidBody& _Origin)
	: qComponent(_Origin)
	, m_Mass(_Origin.m_Mass)
	, m_InitWalkSpeed(_Origin.m_InitWalkSpeed)
	, m_MaxWalkSpeed(_Origin.m_MaxWalkSpeed)
	, m_MaxGravitySpeed(_Origin.m_MaxGravitySpeed)
	, m_Friction(_Origin.m_Friction)
	, m_GravityAccel(_Origin.m_GravityAccel)
	, m_UseGravity(_Origin.m_UseGravity)
	, m_JumpSpeed(_Origin.m_JumpSpeed)
	, m_DoubleJumpSpeed(_Origin.m_DoubleJumpSpeed)
	, m_Ground(_Origin.m_Ground)
	, m_GroundFunc(nullptr)
	, m_AirFunc(nullptr)
	, m_GroundInst(nullptr)
	, m_AirInst(nullptr)
	, m_GroundDelegate(nullptr)
	, m_AirDelegate(nullptr)
{
}

qRigidBody::~qRigidBody()
{
}

void qRigidBody::FinalTick()
{
	Vec3 vObjPos = GetOwner()->Transform()->GetRelativePos();

	// F = M x A
	Vec3 vAccel = m_Force / m_Mass;

	// 가속도
	// 중력을 사용할 때, 공중에서 힘이 적용된 경우
	if (m_UseGravity && !m_Ground)
	{
		m_Velocity += vAccel * DT * 0.5f;

	}
	else
	{
		m_Velocity += vAccel * DT * 1.f;
	}

	// 최대속도 제한
	if (0.f != m_MaxWalkSpeed && m_MaxWalkSpeed < m_Velocity.Length())
	{
		m_Velocity.Normalize();
		m_Velocity *= m_MaxWalkSpeed;
	}

	// 최소 속도 보정
	if (m_InitWalkSpeed != 0.f && m_Velocity.Length() < 10.f && vAccel.x != 0.f && vAccel.y != 0.f && vAccel.z != 0.f)
	{
		Vec3 vForceDir = m_Force;
		vForceDir.Normalize();
		m_Velocity = vForceDir * m_InitWalkSpeed;
	}

	// 만약 RigidBody 에 적용된 힘이 없으면서, 속도는 있는 경우
	// 마찰에 의해서 현재 속도를 줄인다.

	if (m_Force.x == 0.f && m_Force.y == 0.f && m_Force.z == 0.f)
	{
		float Speed = m_Velocity.Length();


		// 중력 기능을 켰고, 공중 상태인 경우
		if (m_UseGravity && !m_Ground)
		{
			// 마찰을 더 적게 적용한다.
			Speed -= m_Friction * DT * 0.2f;
		}
		else
		{
			Speed -= m_Friction * DT;
		}

		// 마찰에 의해서 속도가 역으로 생길 수는 없기 때문에,
		// 감소된 속력의 크기가 음수가 되지 않게 보정한다.
		if (Speed < 0)
			Speed = 0.f;

		if (m_Velocity.x == 0.f && m_Velocity.y == 0.f && m_Velocity.z == 0.f)
		{

		}
		else
		{
			m_Velocity.Normalize();
		}

		m_Velocity *= Speed;
	}


	// 중력 가속도에 의한 속도 증가
	if (m_UseGravity && !m_Ground)
	{
		m_VelocityByGravity += Vec3(0.0f, -1.f, 0.f) * m_GravityAccel * DT;

		if (0.f != m_MaxGravitySpeed && m_MaxGravitySpeed < m_VelocityByGravity.Length())
		{
			m_VelocityByGravity.Normalize();
			m_VelocityByGravity *= m_MaxGravitySpeed;
		}
	}

	if (m_UseGravity && m_Wall)
	{
		m_VelocityByGravity = Vec3(0.f, 0.f, 0.f);
	}

	// 최종 속도
	Vec3 vFinalVelocity = m_Velocity + m_VelocityByGravity;


	// 현재 속도에 따른 이동
	// 속도 = 거리 / 시간
	vObjPos += vFinalVelocity * DT;
	GetOwner()->Transform()->SetRelativePos(vObjPos);

	// 이번 프레임 힘 초기화
	m_Force = Vec3(0.f, 0.f, 0.f);
	m_AddVelocity = Vec3(0.f, 0.f, 0.f);

}




void qRigidBody::jump()
{
	if (L"Player" == GetOwner()->GetName())
	{
		m_VelocityByGravity += Vec3(0.f, 1.4f, 0.f) * m_JumpSpeed;
	}

	SetGround(false);
}

void qRigidBody::doublejump()
{
}


void qRigidBody::SaveToFile(FILE* _File)
{
	fwrite(&m_Velocity, sizeof(Vec3), 1, _File);
	fwrite(&m_VelocityByGravity, sizeof(Vec3), 1, _File);
	fwrite(&m_AddVelocity, sizeof(Vec3), 1, _File);

	fwrite(&m_Mass, sizeof(float), 1, _File);
	fwrite(&m_Force, sizeof(Vec3), 1, _File);

	fwrite(&m_InitWalkSpeed, sizeof(float), 1, _File);
	fwrite(&m_MaxWalkSpeed, sizeof(float), 1, _File);
	fwrite(&m_MaxGravitySpeed, sizeof(float), 1, _File);

	fwrite(&m_Friction, sizeof(float), 1, _File);

	fwrite(&m_GravityAccel, sizeof(float), 1, _File);
	fwrite(&m_UseGravity, sizeof(bool), 1, _File);
	fwrite(&m_Ground, sizeof(bool), 1, _File);

	fwrite(&m_JumpSpeed, sizeof(float), 1, _File);
	fwrite(&m_DoubleJumpSpeed, sizeof(float), 1, _File);



}

void qRigidBody::LoadFromFile(FILE* _File)
{
	fread(&m_Velocity, sizeof(Vec3), 1, _File);
	fread(&m_VelocityByGravity, sizeof(Vec3), 1, _File);
	fread(&m_AddVelocity, sizeof(Vec3), 1, _File);

	fread(&m_Mass, sizeof(float), 1, _File);
	fread(&m_Force, sizeof(Vec3), 1, _File);

	fread(&m_InitWalkSpeed, sizeof(float), 1, _File);
	fread(&m_MaxWalkSpeed, sizeof(float), 1, _File);
	fread(&m_MaxGravitySpeed, sizeof(float), 1, _File);

	fread(&m_Friction, sizeof(float), 1, _File);

	fread(&m_GravityAccel, sizeof(float), 1, _File);
	fread(&m_UseGravity, sizeof(bool), 1, _File);
	fread(&m_Ground, sizeof(bool), 1, _File);

	fread(&m_JumpSpeed, sizeof(float), 1, _File);
	fread(&m_DoubleJumpSpeed, sizeof(float), 1, _File);
}