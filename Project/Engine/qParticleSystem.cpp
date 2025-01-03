#include "pch.h"
#include "qParticleSystem.h"

#include "qTimeMgr.h"

#include "qDevice.h"
#include "qAssetMgr.h"
#include "qStructuredBuffer.h"

#include "qTransform.h"


qParticleSystem::qParticleSystem()
	: qRenderComponent(COMPONENT_TYPE::PARTICLESYSTEM)
	, m_ParticleBuffer(nullptr)
	, m_SpawnCountBuffer(nullptr)
	, m_ModuleBuffer(nullptr)
	, m_Time(0.f)
	, m_BurstTime(0.f)
	, m_Module{}
	, m_MaxParticleCount(1000)
{
	// Mesh / Material 
	SetMesh(qAssetMgr::GetInst()->FindAsset<qMesh>(L"PointMesh"));
	SetMaterial(qAssetMgr::GetInst()->FindAsset<qMaterial>(L"ParticleRenderMtrl"), 0);

	// ParticleTick ComputeShader
	m_TickCS = (qParticleTickCS*)qAssetMgr::GetInst()->FindAsset<qComputeShader>(L"ParticleTickCS").Get();

	m_ParticleTex = qAssetMgr::GetInst()->FindAsset<qTexture>(L"texture\\particle\\TX_GlowScene_2.png");

	m_ParticleBuffer = new qStructuredBuffer;
	m_ParticleBuffer->Create(sizeof(tParticle), m_MaxParticleCount, SB_TYPE::SRV_UAV, true);

	m_SpawnCountBuffer = new qStructuredBuffer;
	m_SpawnCountBuffer->Create(sizeof(tSpawnCount), 1, SB_TYPE::SRV_UAV, true, nullptr);

	// 파티클 기능(모듈) 정보 세팅
	// Spawn Module
	m_Module.Module[(UINT)PARTICLE_MODULE::SPAWN] = true;
	m_Module.SpawnRate = 20;
	m_Module.vSpawnColor = Vec4(0.24f, 0.67f, 0.87f, 1.f);
	m_Module.MinLife = 1.f;
	m_Module.MaxLife = 5.f;
	m_Module.vSpawnMinScale = Vec3(50.f, 5.f, 1.f);
	m_Module.vSpawnMaxScale = Vec3(500.f, 50.f, 1.f);

	m_Module.SpawnShape = 1;
	m_Module.SpawnShapeScale.x = 500.f;

	m_Module.BlockSpawnShape = 1;
	m_Module.BlockSpawnShapeScale.x = 0.f;

	m_Module.SpaceType = 1; // Local Space 

	// Spawn Burst Module
	m_Module.Module[(UINT)PARTICLE_MODULE::SPAWN_BURST] = true;
	m_Module.SpawnBurstRepeat = true;
	m_Module.SpawnBurstCount = 100;
	m_Module.SpawnBurstRepeatTime = 3.f;

	// Scale Module
	m_Module.Module[(UINT)PARTICLE_MODULE::SCALE] = true;
	m_Module.StartScale = 1.f;
	m_Module.EndScale = 1.f;

	// AddVelocity Module
	m_Module.Module[(UINT)PARTICLE_MODULE::ADD_VELOCITY] = true;
	m_Module.AddVelocityType = 1;
	m_Module.AddVelocityFixedDir = Vec3(0.f, 1.f, 0.f);
	m_Module.AddMinSpeed = 100.f;
	m_Module.AddMaxSpeed = 500.f;

	// Drag Module (감속)
	m_Module.Module[(UINT)PARTICLE_MODULE::DRAG] = false;
	m_Module.DestNormalizedAge = 1.f;
	m_Module.LimitSpeed = 0.f;

	// Noise Force Module
	m_Module.Module[(UINT)PARTICLE_MODULE::NOISE_FORCE] = false;
	m_Module.NoiseForceTerm = 0.3f;
	m_Module.NoiseForceScale = 100.f;

	// Render Module
	m_Module.Module[(UINT)PARTICLE_MODULE::RENDER] = true;
	m_Module.EndColor = Vec3(1.f, 0.f, 0.f);
	m_Module.FadeOut = true;
	m_Module.FadeOutStartRatio = 0.9f;
	m_Module.VelocityAlignment = true;

	m_ModuleBuffer = new qStructuredBuffer;
	m_ModuleBuffer->Create(sizeof(tParticleModule), 1, SB_TYPE::SRV_UAV, true, &m_Module);
	
}

qParticleSystem::qParticleSystem(const qParticleSystem& _Other)
	: qRenderComponent(_Other)
	, m_TickCS(_Other.m_TickCS)
	, m_ParticleBuffer(nullptr)
	, m_SpawnCountBuffer(nullptr)
	, m_ModuleBuffer(nullptr)
	, m_ParticleTex(_Other.m_ParticleTex)
	, m_Time(0.f)
	, m_BurstTime(0.f)
	, m_MaxParticleCount(_Other.m_MaxParticleCount)
	, m_Module(_Other.m_Module)
{
	assert(_Other.m_ParticleBuffer && _Other.m_SpawnCountBuffer && _Other.m_ModuleBuffer);

	m_ParticleBuffer = new qStructuredBuffer(*_Other.m_ParticleBuffer);
	m_SpawnCountBuffer = new qStructuredBuffer(*_Other.m_SpawnCountBuffer);
	m_ModuleBuffer = new qStructuredBuffer(*_Other.m_ModuleBuffer);
}

qParticleSystem::~qParticleSystem()
{
	DELETE(m_ParticleBuffer);
	DELETE(m_SpawnCountBuffer);
	DELETE(m_ModuleBuffer);
}

void qParticleSystem::FinalTick()
{
	// SpawnCount 계산
	CaculateSpawnCount();

	// ComputeShader
	m_TickCS->SetParticleWorldPos(Transform()->GetWorldPos());
	m_TickCS->SetParticleBuffer(m_ParticleBuffer);
	m_TickCS->SetSpawnCount(m_SpawnCountBuffer);
	m_TickCS->SetModuleBuffer(m_ModuleBuffer);

	m_TickCS->Execute();
}

void qParticleSystem::Render()
{
	// 위치정보 바인딩
	Transform()->Binding();

	// 파티클 버퍼 바인딩
	m_ParticleBuffer->Binding(20);	// t20

	// 모듈 버퍼 바인딩
	m_ModuleBuffer->Binding(21);	// t21

	// 재질정보 바인딩
	GetMaterial(0)->SetTexParam(TEX_0, m_ParticleTex);
	GetMaterial(0)->Binding();


	// 렌더링
	GetMesh()->Render_Particle(m_MaxParticleCount);

	// 파티클 버퍼 바인딩 해제
	m_ParticleBuffer->Clear(20);
	m_ModuleBuffer->Clear(21);

}

void qParticleSystem::render_shadowmap()
{
}

void qParticleSystem::CaculateSpawnCount()
{
	m_Time += EngineDT;
	tSpawnCount count = {  };

	if (m_Module.Module[(UINT)PARTICLE_MODULE::SPAWN])
	{
		// SpawnRate 에 따른 이번 Tick 에서 생성시킬 파티클의 수
		float Term = 1.f / (float)m_Module.SpawnRate;
		UINT SpawnCount = 0;

		if (Term < m_Time)
		{
			float Value = m_Time / Term;
			SpawnCount = (UINT)Value;
			m_Time -= (float)SpawnCount * Term;
		}

		count.SpawnCount = SpawnCount;
	}

	if (m_Module.Module[(UINT)PARTICLE_MODULE::SPAWN_BURST])
	{
		UINT BurstCount = 0;

		if (0.f == m_BurstTime)
		{
			BurstCount = m_Module.SpawnBurstCount;
		}

		m_BurstTime += EngineDT;

		if (m_Module.SpawnBurstRepeat && m_Module.SpawnBurstRepeatTime <= m_BurstTime)
		{
			m_BurstTime -= m_Module.SpawnBurstRepeatTime;
			BurstCount += m_Module.SpawnBurstCount;
		}

		count.SpawnCount += BurstCount;
	}

	// SpawnCount 를 Buffer 에 전달	
	m_SpawnCountBuffer->SetData(&count);
}


void qParticleSystem::SaveToFile(FILE* _File)
{
	SaveDataToFile(_File);

	fwrite(&m_MaxParticleCount, sizeof(UINT), 1, _File);
	fwrite(&m_Module, sizeof(tParticleModule), 1, _File);

	SaveAssetRef(m_ParticleTex, _File);
}

void qParticleSystem::LoadFromFile(FILE* _File)
{
	LoadDataFromFile(_File);

	fread(&m_MaxParticleCount, sizeof(UINT), 1, _File);
	fread(&m_Module, sizeof(tParticleModule), 1, _File);

	LoadAssetRef(m_ParticleTex, _File);
}
