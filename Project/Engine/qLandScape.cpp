#include "pch.h"
#include "qLandScape.h"

#include "qAssetMgr.h"
#include "qKeyMgr.h"
#include "qCamera.h"
#include "qRenderMgr.h"
#include "qStructuredBuffer.h"

#include "qTransform.h"

qLandScape::qLandScape()
	: qRenderComponent(COMPONENT_TYPE::LANDSCAPE)
	, m_FaceX(1)
	, m_FaceZ(1)
	, m_MinLevel(0)
	, m_MaxLevel(4)
	, m_MaxLevelRange(2000.f)
	, m_MinLevelRange(6000.f)
	, m_BrushIdx(-1)
	, m_BrushScale(Vec2(0.2f, 0.2f))
	, m_IsHeightMapCreated(false)
	, m_WeightMap(nullptr)
	, m_WeightWidth(0)
	, m_WeightHeight(0)
	, m_WeightIdx(0)
	, m_Mode(LANDSCAPE_MODE::SPLATING)
{
	SetFace(m_FaceX, m_FaceZ);

	Init();
}

qLandScape::~qLandScape()
{
	if (nullptr != m_RaycastOut)
		delete m_RaycastOut;

	if (nullptr != m_WeightMap)
		delete m_WeightMap;
}

void qLandScape::FinalTick()
{
	// 모드 전환
	if (KEY_TAP(KEY::_6))
	{
		if (HEIGHTMAP == m_Mode)
			m_Mode = SPLATING;
		else if (SPLATING == m_Mode)
			m_Mode = NONE;
		else
			m_Mode = HEIGHTMAP;
	}


	// 브러쉬 바꾸기
	if (KEY_TAP(KEY::_7))
	{
		++m_BrushIdx;
		if (m_vecBrush.size() <= m_BrushIdx)
			m_BrushIdx = 0;
	}

	if (m_IsHeightMapCreated && KEY_PRESSED(KEY::LBTN))
	{
		Raycasting();

		if (m_Out.Success)
		{
			// 높이맵 설정
			m_HeightMapCS->SetBrushPos(m_RaycastOut);
			m_HeightMapCS->SetBrushScale(m_BrushScale);
			m_HeightMapCS->SetHeightMap(m_HeightMap);
			m_HeightMapCS->SetBrushTex(m_vecBrush[m_BrushIdx]);
			m_HeightMapCS->Execute();
		}
	}

	// 가중치 인덱스 바꾸기
	if (KEY_TAP(KEY::_8))
	{
		++m_WeightIdx;
		if (m_ColorTex->GetArraySize() <= m_WeightIdx)
			m_WeightIdx = 0;
	}

	if (NONE == m_Mode)
		return;

	Raycasting();

	if (HEIGHTMAP == m_Mode)
	{
		if (m_IsHeightMapCreated && KEY_PRESSED(KEY::LBTN))
		{
			if (m_Out.Success)
			{
				// 높이맵 설정
				m_HeightMapCS->SetBrushPos(m_RaycastOut);
				m_HeightMapCS->SetBrushScale(m_BrushScale);
				m_HeightMapCS->SetHeightMap(m_HeightMap);
				m_HeightMapCS->SetBrushTex(m_vecBrush[m_BrushIdx]);
				m_HeightMapCS->Execute();
			}
		}
	}

	else if (SPLATING == m_Mode)
	{
		if (KEY_PRESSED(KEY::LBTN) && m_WeightWidth != 0 && m_WeightHeight != 0)
		{
			if (m_Out.Success)
			{
				m_WeightMapCS->SetBrushPos(m_RaycastOut);
				m_WeightMapCS->SetBrushScale(m_BrushScale);
				m_WeightMapCS->SetBrushTex(m_vecBrush[m_BrushIdx]);
				m_WeightMapCS->SetWeightMap(m_WeightMap);
				m_WeightMapCS->SetWeightIdx(m_WeightIdx);
				m_WeightMapCS->SetWeightMapWidthHeight(m_WeightWidth, m_WeightHeight);

				m_WeightMapCS->Execute();
			}
		}
	}
}

void qLandScape::Render()
{
	Transform()->Binding();

	//GetMaterial()->GetShader()->SetRSType(RS_TYPE::WIRE_FRAME);

	// 지형의 면 개수
	GetMaterial()->SetScalarParam(INT_0, m_FaceX);
	GetMaterial()->SetScalarParam(INT_1, m_FaceZ);
	
	// 지형 모드
	GetMaterial()->SetScalarParam(INT_2, (int)m_Mode);

	// 텍스쳐 배열 개수
	GetMaterial()->SetScalarParam(INT_3, (int)m_ColorTex->GetArraySize());

	// 테셀레이션 레벨
	GetMaterial()->SetScalarParam(VEC4_0, Vec4(m_MinLevel, m_MaxLevel, m_MinLevelRange, m_MaxLevelRange));

	// 카메라 월드 위치
	qCamera* pCam = qRenderMgr::GetInst()->GetPOVCam();
	GetMaterial()->SetScalarParam(VEC4_1, pCam->Transform()->GetWorldPos());


	// 지형에 적용시킬 높이맵
	GetMaterial()->SetTexParam(TEX_0, m_HeightMap);

	// 지형 색상 및 노말 텍스쳐
	GetMaterial()->SetTexParam(TEXARR_0, m_ColorTex);
	GetMaterial()->SetTexParam(TEXARR_1, m_NormalTex);

	// Brush 정보
	GetMaterial()->SetTexParam(TEX_1, m_vecBrush[m_BrushIdx]);
	GetMaterial()->SetScalarParam(VEC2_0, m_BrushScale);
	GetMaterial()->SetScalarParam(VEC2_1, m_Out.Location);
	GetMaterial()->SetScalarParam(FLOAT_0, (float)m_Out.Success);

	// 가중치 해상도
	GetMaterial()->SetScalarParam(VEC2_2, Vec2(m_WeightWidth, m_WeightHeight));

	// WeightMap t20 바인딩
	m_WeightMap->Binding(20);

	// 재질 바인딩
	GetMaterial()->Binding();

	
	// 렌더링
	GetMesh()->Render();


	// WeightMap 버퍼 바인딩 클리어
	m_WeightMap->Clear(20);
}

void qLandScape::SetFace(int _X, int _Z)
{
	m_FaceX = _X;
	m_FaceZ = _Z;

	CreateMesh();
	SetMaterial(qAssetMgr::GetInst()->FindAsset<qMaterial>(L"LandScapeMtrl"));
}


int qLandScape::Raycasting()
{
	// 현재 시점 카메라 가져오기
	qCamera* pCam = qRenderMgr::GetInst()->GetPOVCam();
	if (nullptr == pCam)
		return false;

	// 구조화버퍼 클리어
	m_Out = {};
	m_Out.Distance = 0xffffffff;
	m_RaycastOut->SetData(&m_Out);

	// 카메라가 시점에서 마우스를 향하는 Ray 정보를 가져옴
	tRay ray = pCam->GetRay();

	// LandScape 의 WorldInv 행렬 가져옴
	const Matrix& matWorldInv = Transform()->GetWorldMatInv();

	// 월드 기준 Ray 정보를 LandScape 의 Local 공간으로 데려감
	ray.vStart = XMVector3TransformCoord(ray.vStart, matWorldInv);
	ray.vDir = XMVector3TransformNormal(ray.vDir, matWorldInv);
	ray.vDir.Normalize();

	// Raycast 컴퓨트 쉐이더에 필요한 데이터 전달
	m_RaycastCS->SetRayInfo(ray);
	m_RaycastCS->SetFace(m_FaceX, m_FaceZ);
	m_RaycastCS->SetOutBuffer(m_RaycastOut);
	m_RaycastCS->SetHeightMap(m_HeightMap);

	// 컴퓨트쉐이더 실행
	m_RaycastCS->Execute();

	// 결과 확인
	m_RaycastOut->GetData(&m_Out);

	return m_Out.Success;
}


void qLandScape::SaveToFile(FILE* _File)
{
}

void qLandScape::LoadFromFile(FILE* _File)
{
}


