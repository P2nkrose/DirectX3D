#include "pch.h"
#include "qMeshRender.h"

#include "qTransform.h"
#include "qFlipBookComponent.h"
#include "qSprite.h"

qMeshRender::qMeshRender()
	: qRenderComponent(COMPONENT_TYPE::MESHRENDER)
{
}

qMeshRender::~qMeshRender()
{
}

void qMeshRender::FinalTick()
{
}

void qMeshRender::Render()
{

	// FlipBookComponent �� ������ ���� ������� Sprite ������ Binding �ϰ� �Ѵ�.
	if (FlipBookComponent())
		FlipBookComponent()->Binding();
	else
		qFlipBookComponent::Clear();

	// ��ġ, ũ��, ȸ�� �������� ���ε�
	Transform()->Binding();

	// ���� ���ε� (���� ���, ���̴� ���)
	GetMaterial(0)->Binding();

	for (UINT i = 0; i < GetMesh()->GetSubsetCount(); ++i)
	{
		// ���� ���ε�(���� ���, ���̴� ���)
		if (!GetMaterial(i))
			continue;

		GetMaterial(i)->Binding();

		// ���ؽ�����, �ε������� ���ε� �� ������ ȣ��
		GetMesh()->Render(i);
	}
}


void qMeshRender::SaveToFile(FILE* _File)
{
	SaveDataToFile(_File);
}

void qMeshRender::LoadFromFile(FILE* _File)
{
	LoadDataFromFile(_File);
}
