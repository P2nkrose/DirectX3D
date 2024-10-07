#pragma once
#include "qComponent.h"

typedef void(*CALL_BACK)(void);
typedef void(qGameObject::* DELEGATE)(void);

class qRigidBody : public qComponent
{
public:
	CLONE(qRigidBody);

	qRigidBody();
	qRigidBody(const qRigidBody& _Origin);
	~qRigidBody();


public:
	virtual void FinalTick() override;
	virtual void SaveToFile(FILE* _File) override;
	virtual void LoadFromFile(FILE* _File) override;


public:
	void AddForce(Vec3 _vForce) { m_Force += _vForce; }						// �� �߰�

	void SetMass(float _Mass) { m_Mass = _Mass; }							// ���� ����
	void SetInitialWalkSpeed(float _Speed) { m_InitWalkSpeed = _Speed; }	// �ʱ� �̵��ӵ� ����
	void SetMaxWalkSpeed(float _Speed) { m_MaxWalkSpeed = _Speed; }			// �ִ� �̵��ӵ� ����
	void SetMaxGravitySpeed(float _Speed) { m_MaxGravitySpeed = _Speed; }	// �ִ� �߷°��ӵ� ����
	void SetFriction(float _Friction) { m_Friction = _Friction; }
	void SetJumpSpeed(float _Speed) { m_JumpSpeed = _Speed; }

	void jump();
	void doublejump();

	float GetMass() { return m_Mass; }
	float GetInitWalkSpeed() { return m_InitWalkSpeed; }
	float GetMaxWalkSpeed() { return m_MaxWalkSpeed; }
	float GetFriction() { return m_Friction; }
	Vec3  GetGravityVelocity() { return m_VelocityByGravity; }


	void SetGroundFunc(void(*_pFunc)(void)) { m_GroundFunc = _pFunc; }
	void SetAirFunc(void(*_pFunc)(void)) { m_AirFunc = _pFunc; }


	void SetGroundDelegate(qGameObject* _Inst, DELEGATE _MemFunc)
	{
		m_GroundInst = _Inst;
		m_GroundDelegate = _MemFunc;
	}

	void SetAirDelegate(qGameObject* _Inst, DELEGATE _MemFunc)
	{
		m_AirInst = _Inst;
		m_AirDelegate = _MemFunc;
	}


	void UseGravity(bool _Use)
	{
		m_UseGravity = _Use;
		if (!m_UseGravity)
			m_VelocityByGravity = Vec3(0.f, 0.f, 0.f);
	}

	void SetGround(bool _Ground)
	{
		m_Ground = _Ground;

		if (m_Ground)
		{
			m_VelocityByGravity = Vec3(0.f, 0.f, 0.f);

			if (nullptr != m_GroundFunc)
				m_GroundFunc();

			if (m_GroundInst && m_GroundDelegate)
				(m_GroundInst->*m_GroundDelegate)();
		}
		else
		{
			if (nullptr != m_AirFunc)
				m_AirFunc();

			if (m_AirInst && m_AirDelegate)
				(m_AirInst->*m_AirDelegate)();
		}
	}

	void SetWall(bool _Wall)
	{
		if (m_Wall == _Wall)
		{
			return;
		}

		m_Wall = _Wall;
	}



	bool IsGround() { return m_Ground; }
	bool IsWall() { return m_Wall; }


public:
	void SetVelocityX(float _XSpeed) { m_Velocity.x = _XSpeed; }



private:
	Vec3				m_Velocity;				// �ӵ� (����, ũ�������� ���� �ִ�. �׷��� vector �� �θ���.
	Vec3				m_VelocityByGravity;	// �߷¿� ���ؼ� �����ϴ� �ӵ�
	Vec3				m_AddVelocity;			// ���� �ӵ��� ������ �ӵ�

	float				m_Mass;					// ����
	Vec3				m_Force;				// ��

	float				m_InitWalkSpeed;		// �ּ� ���� �ӷ�
	float				m_MaxWalkSpeed;			// �ִ� ���� �ӷ�
	float				m_MaxGravitySpeed;		// �߷����� �߻��� ���ϼӵ� ����

	float				m_Friction;				// ������

	float				m_GravityAccel;			// �߷� ���ӵ�
	bool				m_UseGravity;			// �߷� ��� <-> �̻�� bool
	bool				m_Ground;				// �� ���� ���ִ��� üũ
	bool				m_Wall;					// ���� �ε������� üũ

	float				m_JumpSpeed;			// ���� �ӷ�
	float				m_DoubleJumpSpeed;		// �������� �ӷ�




	// Ground, Air, Rope   On / Off ȣ���ų �Լ�������
	CALL_BACK			m_GroundFunc;
	CALL_BACK			m_AirFunc;

	// Delegate
	qGameObject*		m_GroundInst;
	DELEGATE			m_GroundDelegate;

	qGameObject*		m_AirInst;
	DELEGATE			m_AirDelegate;
};

