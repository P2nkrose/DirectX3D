#include "pch.h"
#include "qLight3D.h"

#include "qRenderMgr.h"
#include "qTransform.h"

#include "qAssetMgr.h"
#include "qCamera.h"
#include "qMRT.h"

qLight3D::qLight3D()
	: qComponent(COMPONENT_TYPE::LIGHT3D)
	, m_Info{}
	, m_LightIdx(-1)
	, m_Cam(nullptr)
	, m_ShadowMapMRT(nullptr)
{
	// ���� ���� ī�޶�
	m_Cam = new qGameObject;
	m_Cam->AddComponent(new qCamera);
	m_Cam->AddComponent(new qTransform);

	SetLightType(LIGHT_TYPE::DIRECTIONAL);
}

qLight3D::qLight3D(const qLight3D& _Origin)
	: qComponent(_Origin)
	, m_Info(_Origin.m_Info)
	, m_LightIdx(-1)
{
	// ī�޶� ����
	m_Cam = _Origin.m_Cam->Clone();

	// ���� Ÿ�� ����
	SetLightType(m_Info.Type);

}

qLight3D::~qLight3D()
{
	if (nullptr != m_Cam)
		delete m_Cam;

	if (nullptr != m_ShadowMapMRT)
		delete m_ShadowMapMRT;
}


void qLight3D::FinalTick()
{
	m_Info.WorldPos = Transform()->GetWorldPos();
	m_Info.WorldDir = Transform()->GetWorldDir(DIR::FRONT);

	// ������ ��ġ����
	if (m_Info.Type == LIGHT_TYPE::DIRECTIONAL)
	{
		Transform()->SetRelativePos(m_TargetPos + -m_Info.WorldDir * 10000.f);
	}

	// PointLight, SphereMesh, r = 0.5f
	Transform()->SetRelativeScale(m_Info.Radius * 2.f, m_Info.Radius * 2.f, m_Info.Radius * 2.f);

	// �ڽ��� RenderMgr �� ��Ͻ�Ŵ
	m_LightIdx = qRenderMgr::GetInst()->RegisterLight3D(this);

	// DebugShape
	if (m_Info.Type == LIGHT_TYPE::POINT)
	{
		DrawDebugSphere(Transform()->GetWorldMat(), Vec4(0.f, 1.f, 0.f, 1.f), 0.f, true);
	}
}


void qLight3D::Render()
{
	Transform()->Binding();

	// ���� �ε���	
	m_LightMtrl->SetScalarParam(INT_0, m_LightIdx);

	// ���� ī�޶�� ������ų�� ����� ViewProj ���
	m_LightMtrl->SetScalarParam(MAT_0, m_Cam->Camera()->GetViewMat() * m_Cam->Camera()->GetProjMat());

	// ShadowMap ����
	m_LightMtrl->SetTexParam(TEX_2, m_ShadowMapMRT->GetRT(0));

	// ���� ���ε�
	m_LightMtrl->Binding();

	// ������
	m_VolumeMesh->Render();
}


void qLight3D::CreateShadowMap()
{
	// ī�޶��� Transform �� Light3D �� Transform ������ ����
	*m_Cam->Transform() = *Transform();

	// ���� ī�޶��� ��ġ�� �������� ViewMat, ProjMat �� ����Ѵ�.
	m_Cam->Camera()->FinalTick();

	// ���� ī�޶��� View Proj ������ ������� ���������� �����Ѵ�.
	g_Trans.matView = m_Cam->Camera()->GetViewMat();
	g_Trans.matProj = m_Cam->Camera()->GetProjMat();
	g_Trans.matViewInv = m_Cam->Camera()->GetViewMatInv();
	g_Trans.matProjInv = m_Cam->Camera()->GetProjMatInv();

	// MRT ����
	m_ShadowMapMRT->Clear();
	m_ShadowMapMRT->OMSet();

	// ShdowMap Mtrl Binding
	m_ShadowMapMtrl->Binding();

	m_Cam->Camera()->SortGameObject_ShadowMap();
	m_Cam->Camera()->render_shadowmap();
}


void qLight3D::SetLightType(LIGHT_TYPE _Type)
{
	m_Info.Type = _Type;

	if (m_Info.Type == LIGHT_TYPE::DIRECTIONAL)
	{
		m_VolumeMesh = qAssetMgr::GetInst()->FindAsset<qMesh>(L"RectMesh");
		m_LightMtrl = qAssetMgr::GetInst()->FindAsset<qMaterial>(L"DirLightMtrl");

		// ShadowMap Mtrl
		m_ShadowMapMtrl = qAssetMgr::GetInst()->FindAsset<qMaterial>(L"DirLightShadowMapMtrl");

		// ���� ī�޶� �ɼ� ����
		m_Cam->Camera()->SetProjType(ORTHOGRAPHIC);
		m_Cam->Camera()->SetWidth(8192);
		m_Cam->Camera()->SetHeight(8192);
		m_Cam->Camera()->SetLayerAll();
		m_Cam->Camera()->SetLayer(31, false);
		m_Cam->Camera()->SetScale(1.f);

		// 8192, 8192 �ػ� ShadowMap ����
		Ptr<qTexture> pShadowMap = new qTexture;
		pShadowMap->Create(8192, 8192, DXGI_FORMAT_R32_FLOAT, D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE);

		Ptr<qTexture> pShdowMapDepth = new qTexture;
		pShdowMapDepth->Create(8192, 8192, DXGI_FORMAT_D24_UNORM_S8_UINT, D3D11_BIND_DEPTH_STENCIL);

		// MRT ����
		if (nullptr == m_ShadowMapMRT)
			m_ShadowMapMRT = new qMRT;

		m_ShadowMapMRT->Create(1, &pShadowMap, pShdowMapDepth);
		Vec4 vClearColor = Vec4(-1.f, 0.f, 0.f, 0.f);
		m_ShadowMapMRT->SetClearColor(&vClearColor, true);


	}

	else if (m_Info.Type == LIGHT_TYPE::POINT)
	{
		m_VolumeMesh = qAssetMgr::GetInst()->FindAsset<qMesh>(L"SphereMesh");
		m_LightMtrl = qAssetMgr::GetInst()->FindAsset<qMaterial>(L"PointLightMtrl");
	}

	else if (m_Info.Type == LIGHT_TYPE::SPOT)
	{
		m_VolumeMesh = qAssetMgr::GetInst()->FindAsset<qMesh>(L"ConeMesh");
		m_LightMtrl = qAssetMgr::GetInst()->FindAsset<qMaterial>(L"SpotLightMtrl");
	}
}








void qLight3D::SetRadius(float _Radius)
{
	m_Info.Radius = _Radius;

	m_Info.Radius = _Radius;
}


void qLight3D::SaveToFile(FILE* _File)
{
	fwrite(&m_Info, sizeof(tLightInfo), 1, _File);
}

void qLight3D::LoadFromFile(FILE* _File)
{
	fread(&m_Info, sizeof(tLightInfo), 1, _File);
}



