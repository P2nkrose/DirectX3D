#include "pch.h"
#include "qSkyBox.h"

#include "qTransform.h"
#include "qAssetMgr.h"

qSkyBox::qSkyBox()
	: qRenderComponent(COMPONENT_TYPE::SKYBOX)
{
	SetMesh(qAssetMgr::GetInst()->FindAsset<qMesh>(L"SphereMesh"));
	SetMaterial(qAssetMgr::GetInst()->FindAsset<qMaterial>(L"SkyBoxMtrl"));
}

qSkyBox::~qSkyBox()
{
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
	//Ptr<qTexture> Tex = qAssetMgr::GetInst()->FindAsset<qTexture>(L"texture\\skybox\\Sky02.jpg");

	Transform()->Binding();

	//GetMaterial()->SetTexParam(TEX_0, Tex);

	GetMaterial()->Binding();
	

	GetMesh()->Render();
}

void qSkyBox::SaveToFile(FILE* _File)
{
}

void qSkyBox::LoadFromFile(FILE* _File)
{
}
