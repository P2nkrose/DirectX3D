#include "pch.h"
#include "qRenderComponent.h"

#include "qAssetMgr.h"
#include "qLevelMgr.h"
#include "qLevel.h"

#include "qTransform.h"

qRenderComponent::qRenderComponent(COMPONENT_TYPE _Type)
	: qComponent(_Type)
	, m_FrustumCheck(true)
{
}

qRenderComponent::qRenderComponent(const qRenderComponent& _Origin)
	: qComponent(_Origin)
	, m_Mesh(_Origin.m_Mesh)
	, m_FrustumCheck(_Origin.m_FrustumCheck)
{
	m_vecMtrls.resize(_Origin.m_vecMtrls.size());

	for (size_t i = 0; i < _Origin.m_vecMtrls.size(); ++i)
	{
		m_vecMtrls[i].pCurMtrl = _Origin.m_vecMtrls[i].pCurMtrl;
		m_vecMtrls[i].pSharedMtrl = _Origin.m_vecMtrls[i].pSharedMtrl;

		// ���� ������Ʈ�� ���������� �����ϰ� �ְ�, ���� ��������� ���������� �ƴѰ��
		if (_Origin.m_vecMtrls[i].pSharedMtrl != _Origin.m_vecMtrls[i].pCurMtrl)
		{
			assert(_Origin.m_vecMtrls[i].pDynamicMtrl.Get());

			// ���� ���� ������Ʈ�� ������ ���������� �����Ѵ�.
			GetDynamicMaterial(i);

			// ���� ����������Ʈ�� �������� ���� ���� ������ ���������� �����Ѵ�.
			*m_vecMtrls[i].pDynamicMtrl.Get() = *_Origin.m_vecMtrls[i].pDynamicMtrl.Get();
		}
		else
		{
			m_vecMtrls[i].pCurMtrl = m_vecMtrls[i].pSharedMtrl;
		}
	}
}

qRenderComponent::~qRenderComponent()
{
}

void qRenderComponent::render_shadowmap()
{
	// ������ ShadowMapMtrl �� �̹� Binding �Ǿ� �ִ� �� ����� ��
	// �ڽ��� ������ Mesh �� ������ ��û�� �ϸ� �ȴ�.
	Transform()->Binding();
	
	// ��� ����� �޽��� ���̸� �����Ѵ�.
	for (int i = 0; i < GetMesh()->GetSubsetCount(); ++i)
	{
		GetMesh()->Render(i);
	}
}

void qRenderComponent::SetMesh(Ptr<qMesh> _Mesh)
{
	m_Mesh = _Mesh;

	if (!m_vecMtrls.empty())
	{
		m_vecMtrls.clear();
		vector<tMtrlSet> vecMtrls;
		m_vecMtrls.swap(vecMtrls);
	}

	if (nullptr != m_Mesh)
		m_vecMtrls.resize(m_Mesh->GetSubsetCount());
}

void qRenderComponent::SetMaterial(Ptr<qMaterial> _Mtrl, UINT _idx)
{
	// ������ ����Ǹ� ������ ���纻 �޾Ƶ� DynamicMaterial �� �����Ѵ�.
	m_vecMtrls[_idx].pSharedMtrl = _Mtrl;
	m_vecMtrls[_idx].pCurMtrl = _Mtrl;
	m_vecMtrls[_idx].pDynamicMtrl = nullptr;
}

Ptr<qMaterial> qRenderComponent::GetMaterial(UINT _idx)
{
	return m_vecMtrls[_idx].pCurMtrl;
}


Ptr<qMaterial> qRenderComponent::GetSharedMaterial(UINT _idx)
{
	// ���������� �������°����� ���� ��������� ������������ ȸ���ϵ��� �Ѵ�
	m_vecMtrls[_idx].pCurMtrl = m_vecMtrls[_idx].pSharedMtrl;

	if (m_vecMtrls[_idx].pDynamicMtrl.Get())
	{
		m_vecMtrls[_idx].pDynamicMtrl = nullptr;
	}

	return m_vecMtrls[_idx].pSharedMtrl;
}

Ptr<qMaterial> qRenderComponent::GetDynamicMaterial(UINT _idx)
{
	qLevel* pCurLevel = qLevelMgr::GetInst()->GetCurrentLevel();
	if (pCurLevel->GetState() != LEVEL_STATE::PLAY)
		return nullptr;

	// ���� ������ ���� -> Nullptr ��ȯ
	if (nullptr == m_vecMtrls[_idx].pSharedMtrl)
	{
		m_vecMtrls[_idx].pCurMtrl = nullptr;
		m_vecMtrls[_idx].pDynamicMtrl = nullptr;
		return m_vecMtrls[_idx].pCurMtrl;
	}

	if (nullptr == m_vecMtrls[_idx].pDynamicMtrl)
	{
		m_vecMtrls[_idx].pDynamicMtrl = m_vecMtrls[_idx].pSharedMtrl->Clone();
		m_vecMtrls[_idx].pDynamicMtrl->SetName(m_vecMtrls[_idx].pSharedMtrl->GetName() + L"_Clone");
		m_vecMtrls[_idx].pCurMtrl = m_vecMtrls[_idx].pDynamicMtrl;
	}

	return m_vecMtrls[_idx].pCurMtrl;
}

void qRenderComponent::SaveDataToFile(FILE* _File)
{
	// �޽� �������� ����
	SaveAssetRef(m_Mesh, _File);

	// ���� �������� ����
	UINT iMtrlCount = GetMaterialCount();
	fwrite(&iMtrlCount, sizeof(UINT), 1, _File);

	for (UINT i = 0; i < iMtrlCount; ++i)
	{
		SaveAssetRef(m_vecMtrls[i].pSharedMtrl, _File);
	}
}

void qRenderComponent::LoadDataFromFile(FILE* _File)
{
	// �޽� �������� �ҷ�����
	LoadAssetRef(m_Mesh, _File);

	// ���� �������� �ҷ�����
	UINT iMtrlCount = GetMaterialCount();
	fread(&iMtrlCount, sizeof(UINT), 1, _File);

	for (UINT i = 0; i < iMtrlCount; ++i)
	{
		LoadAssetRef(m_vecMtrls[i].pSharedMtrl, _File);
	}
}
