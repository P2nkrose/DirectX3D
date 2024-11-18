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
	, m_BrushIdx(-1)
	, m_BrushScale(Vec2(0.2f, 0.2f))
	, m_IsHeightMapCreated(false)
{
	SetFace(m_FaceX, m_FaceZ);

	Init();
}

qLandScape::~qLandScape()
{
	if (nullptr != m_RaycastOut)
		delete m_RaycastOut;
}

void qLandScape::FinalTick()
{
	// �귯�� �ٲٱ�
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
			// ���̸� ����
			m_HeightMapCS->SetBrushPos(m_RaycastOut);
			m_HeightMapCS->SetBrushScale(m_BrushScale);
			m_HeightMapCS->SetHeightMap(m_HeightMap);
			m_HeightMapCS->SetBrushTex(m_vecBrush[m_BrushIdx]);
			m_HeightMapCS->Execute();
		}
	}
}

void qLandScape::Render()
{
	Transform()->Binding();

	//GetMaterial()->GetShader()->SetRSType(RS_TYPE::WIRE_FRAME);

	// ������ �� ����
	GetMaterial()->SetScalarParam(INT_0, m_FaceX);
	GetMaterial()->SetScalarParam(INT_1, m_FaceZ);
	

	// ������ �����ų ���̸�
	GetMaterial()->SetTexParam(TEX_0, m_HeightMap);

	// ���� ���ε�
	GetMaterial()->Binding();

	// ������
	GetMesh()->Render();
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
	// ���� ���� ī�޶� ��������
	qCamera* pCam = qRenderMgr::GetInst()->GetPOVCam();
	if (nullptr == pCam)
		return false;

	// ����ȭ���� Ŭ����
	m_Out = {};
	m_Out.Distance = 0xffffffff;
	m_RaycastOut->SetData(&m_Out, 1);

	// ī�޶� �������� ���콺�� ���ϴ� Ray ������ ������
	tRay ray = pCam->GetRay();

	// LandScape �� WorldInv ��� ������
	const Matrix& matWorldInv = Transform()->GetWorldMatInv();

	// ���� ���� Ray ������ LandScape �� Local �������� ������
	ray.vStart = XMVector3TransformCoord(ray.vStart, matWorldInv);
	ray.vDir = XMVector3TransformNormal(ray.vDir, matWorldInv);
	ray.vDir.Normalize();

	// Raycast ��ǻƮ ���̴��� �ʿ��� ������ ����
	m_RaycastCS->SetRayInfo(ray);
	m_RaycastCS->SetFace(m_FaceX, m_FaceZ);
	m_RaycastCS->SetOutBuffer(m_RaycastOut);
	m_RaycastCS->SetHeightMap(m_HeightMap);

	// ��ǻƮ���̴� ����
	m_RaycastCS->Execute();

	// ��� Ȯ��
	m_RaycastOut->GetData(&m_Out);

	return m_Out.Success;
}


void qLandScape::SaveToFile(FILE* _File)
{
}

void qLandScape::LoadFromFile(FILE* _File)
{
}


