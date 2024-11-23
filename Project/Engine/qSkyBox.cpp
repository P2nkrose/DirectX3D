#include "pch.h"
#include "qSkyBox.h"

#include "qTransform.h"
#include "qAssetMgr.h"

qSkyBox::qSkyBox()
	: qRenderComponent(COMPONENT_TYPE::SKYBOX)
	, m_Type(SPHERE)
{
	SetSkyBoxType(m_Type);

	SetFrustumCheck(false);
}

qSkyBox::qSkyBox(const qSkyBox& _Origin)
	: qRenderComponent(_Origin)
	, m_Type(_Origin.m_Type)
	, m_SkyBoxTex(_Origin.m_SkyBoxTex)
{
	SetSkyBoxType(m_Type);
}

qSkyBox::~qSkyBox()
{
}


void qSkyBox::SetSkyBoxType(SKYBOX_TYPE _Type)
{
	m_Type = _Type;

	if (SKYBOX_TYPE::SPHERE == m_Type)
		SetMesh(qAssetMgr::GetInst()->FindAsset<qMesh>(L"SphereMesh"));
	else if (SKYBOX_TYPE::CUBE == m_Type)
		SetMesh(qAssetMgr::GetInst()->FindAsset<qMesh>(L"CubeMesh"));

	// Mesh �� ����Ǹ� ������ �ٽ� �����ؾ� �Ѵ�.
	SetMaterial(qAssetMgr::GetInst()->FindAsset<qMaterial>(L"SkyBoxMtrl"), 0);
}



void qSkyBox::FinalTick()
{
	// 1. ���� ī�޶� �� �ִ� �þ߰Ÿ���, Transform �� Scale �� �����Ѵ�.
	//  - Play -> Main Cam
	//  - Pause, Stop -> Editor Cam

	// 2. SkyBox ��ġ�� ���� ī�޶� ��ġ�� �ű��.
}

void qSkyBox::Render()
{
	Transform()->Binding();

	GetMaterial(0)->SetScalarParam(INT_0, (int)m_Type);

	if (m_Type == SPHERE)
	{
		if (!m_SkyBoxTex->IsCubeMap())
			GetMaterial(0)->SetTexParam(TEX_0, m_SkyBoxTex);
		else
			GetMaterial(0)->SetTexParam(TEX_0, nullptr);
	}

	else if (m_Type == CUBE)
	{
		if (m_SkyBoxTex->IsCubeMap())
			GetMaterial(0)->SetTexParam(TEXCUBE_0, m_SkyBoxTex);
		else
			GetMaterial(0)->SetTexParam(TEXCUBE_0, nullptr);
	}

	GetMaterial(0)->Binding();
	GetMesh()->Render(0);
}

void qSkyBox::render_shadowmap()
{
}

void qSkyBox::SaveToFile(FILE* _File)
{
}

void qSkyBox::LoadFromFile(FILE* _File)
{
}

