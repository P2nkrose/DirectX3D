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
	if (!GetMesh() || !GetMaterial() || !GetMaterial()->GetShader())
		return;


	// FlipBookComponent �� ������ ���� ������� Sprite ������ Binding �ϰ� �Ѵ�.
	if (FlipBookComponent())
		FlipBookComponent()->Binding();
	else
		qFlipBookComponent::Clear();

	// ��ġ, ũ��, ȸ�� �������� ���ε�
	Transform()->Binding();

	// ���� ���ε� (���� ���, ���̴� ���)
	GetMaterial()->Binding();

	// ���ؽ�����, �ε������� ���ε� �� ������ ȣ��
	GetMesh()->Render();

	//qTexture::Clear(0);
	//qTexture::Clear(1);
}


void qMeshRender::SaveToFile(FILE* _File)
{
	SaveDataToFile(_File);
}

void qMeshRender::LoadFromFile(FILE* _File)
{
	LoadDataFromFile(_File);
}
