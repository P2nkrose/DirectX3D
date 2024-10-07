#include "pch.h"
#include "qLevelSaveLoad.h"

#include <Engine/qLevelMgr.h>
#include <Engine/qLevel.h>
#include <Engine/qLayer.h>
#include <Engine/qGameObject.h>
#include <Engine/components.h>

#include <Scripts/qScriptMgr.h>


void qLevelSaveLoad::SaveLevel(const wstring& _FilePath, qLevel* _Level)
{

	assert(_Level && _Level->GetState() == LEVEL_STATE::STOP);

	FILE* File = nullptr;
	_wfopen_s(&File, _FilePath.c_str(), L"wb");

	// Level �̸� ����
	SaveWString(_Level->GetName(), File);

	// Level �ȿ� �ִ� Layer �� ����
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		qLayer* pLayer = _Level->GetLayer(i);

		// Layer �̸� ����
		SaveWString(pLayer->GetName(), File);

		// Layer �ȿ� �ִ� GameObject �� ����
		const vector<qGameObject*>& vecParents = pLayer->GetParentObjects();

		// Layer �� ������ ParentObject ���� ����
		size_t count = vecParents.size();
		fwrite(&count, sizeof(size_t), 1, File);

		// GameObject ����
		for (size_t i = 0; i < vecParents.size(); ++i)
		{
			SaveGameObject(File, vecParents[i]);
		}
	}

	fclose(File);
}

void qLevelSaveLoad::SaveGameObject(FILE* _File, qGameObject* _Object)
{
	// GameObject �� �̸� ����
	SaveWString(_Object->GetName(), _File);

	// Component�� ���� ����
	UINT i = 0;
	for (; i < (UINT)COMPONENT_TYPE::END; ++i)
	{
		qComponent* pComponent = _Object->GetComponent((COMPONENT_TYPE)i);

		if (nullptr == pComponent)
			continue;

		// Component Type �� ����
		COMPONENT_TYPE Type = pComponent->GetComponentType();
		fwrite(&Type, sizeof(COMPONENT_TYPE), 1, _File);

		pComponent->SaveToFile(_File);
	}

	// COMPONENT_TYPE::END ����
	fwrite(&i, sizeof(COMPONENT_TYPE), 1, _File);

	// Script ���� ����
	// Script ���� ����
	const vector<qScript*> vecScripts = _Object->GetScripts();
	size_t ScriptCount = vecScripts.size();
	fwrite(&ScriptCount, sizeof(size_t), 1, _File);

	for (size_t i = 0; i < ScriptCount; ++i)
	{
		// Script �� �̸� ����
		wstring ScriptName = qScriptMgr::GetScriptName(vecScripts[i]);
		SaveWString(ScriptName, _File);

		// Script ���� ����
		vecScripts[i]->SaveToFile(_File);
	}

	// Child ���� ����
	const vector<qGameObject*>& vecChild = _Object->GetChildren();
	size_t ChildCount = vecChild.size();
	fwrite(&ChildCount, sizeof(size_t), 1, _File);

	for (size_t i = 0; i < vecChild.size(); ++i)
	{
		SaveGameObject(_File, vecChild[i]);
	}

}

qLevel* qLevelSaveLoad::LoadLevel(const wstring& _FilePath)
{
	FILE* File = nullptr;
	_wfopen_s(&File, _FilePath.c_str(), L"rb");
	
	// Level ����
	qLevel* pNewLevel = new qLevel;

	// Level �̸� �ҷ�����
	wstring LevelName;
	LoadWString(LevelName, File);
	pNewLevel->SetName(LevelName);

	// Level �ȿ� �ִ� Layer ���� �ҷ�����
	for (UINT i = 0; i < MAX_LAYER; ++i)
	{
		qLayer* pLayer = pNewLevel->GetLayer(i);
		
		// Layer �̸� �ҷ�����
		wstring LayerName;
		LoadWString(LayerName, File);
		pLayer->SetName(LayerName);

		// Layer �� ������ ParentObject ���� �ҷ�����
		size_t count = 0;
		fread(&count, sizeof(size_t), 1, File);

		// GameObject �ҷ��ͼ� Layer �� ����ֱ�
		for (size_t i = 0; i < count; ++i)
		{
			qGameObject* pLoadedObject = LoadGameObject(File);
			pLayer->AddObject(pLoadedObject, false);
		}
	}

	fclose(File);
	
	return pNewLevel;
}

qGameObject* qLevelSaveLoad::LoadGameObject(FILE* _File)
{
	qGameObject* pObject = new qGameObject;

	// GameObject �� �̸� �ε�
	wstring Name;
	LoadWString(Name, _File);
	pObject->SetName(Name);

	// Component ���� �ε�
	COMPONENT_TYPE Type = COMPONENT_TYPE::END;
	while (true)
	{
		// ����Ǿ� �ִ� ������ � ������Ʈ���� Ȯ��
		fread(&Type, sizeof(COMPONENT_TYPE), 1, _File);

		// ���� Ÿ�� ������ END �� ���, ����� ������Ʈ ������ ���̴�.
		if (COMPONENT_TYPE::END == Type)
			break;

		// ����� Ÿ�Կ� �´� ������Ʈ�� ���� ��Ű��, ������ ���� ������ ������ �����͸� �д´�.
		qComponent* pComponent = GetComponent(Type);

		// ������ ������Ʈ�� ������Ʈ�� �־��ش�.
		pObject->AddComponent(pComponent);

		// ���� ����� ������ �о�ͼ� �����Ѵ�.
		pComponent->LoadFromFile(_File);
	}

	// Script ���� �ε�
	size_t ScriptCount = 0;
	fread(&ScriptCount, sizeof(size_t), 1, _File);

	for (size_t i = 0; i < ScriptCount; ++i)
	{
		// Script �� �̸� �б�
		wstring ScriptName;
		LoadWString(ScriptName, _File);

		// ���� Script �̸����� �̸��� �ش��ϴ� Script ����
		qScript* pScript = qScriptMgr::GetScript(ScriptName);
		pScript->LoadFromFile(_File);

		pObject->AddComponent(pScript);
	}

	// Child ���� �ε�
	size_t ChildCount = 0;
	fread(&ChildCount, sizeof(size_t), 1, _File);

	for (size_t i = 0; i < ChildCount; ++i)
	{
		qGameObject* pChildObject = LoadGameObject(_File);
		pObject->AddChild(pChildObject);
	}

	return pObject;
}

qComponent* qLevelSaveLoad::GetComponent(COMPONENT_TYPE _Type)
{
	switch (_Type)
	{
	case COMPONENT_TYPE::TRANSFORM:
		return new qTransform;

	case COMPONENT_TYPE::COLLIDER2D:
		return new qCollider2D;

	case COMPONENT_TYPE::COLLIDER3D:

		break;
	case COMPONENT_TYPE::LIGHT2D:
		return  new qLight2D;

	case COMPONENT_TYPE::LIGHT3D:
		break;

	case COMPONENT_TYPE::FLIPBOOKCOMPONENT:
		return new qFlipBookComponent;

	case COMPONENT_TYPE::ANIMATOR3D:
		break;

	case COMPONENT_TYPE::STATEMACHINE:
		return new qFSM;

	case COMPONENT_TYPE::RIGIDBODY:
		return new qRigidBody;

	case COMPONENT_TYPE::CAMERA:
		return  new qCamera;

	case COMPONENT_TYPE::MESHRENDER:
		return  new qMeshRender;

	case COMPONENT_TYPE::TILEMAP:
		return  new qTileMap;

	case COMPONENT_TYPE::PARTICLESYSTEM:
		return new qParticleSystem;
		

	case COMPONENT_TYPE::DECAl:
		break;

	case COMPONENT_TYPE::SKYBOX:
		break;

	case COMPONENT_TYPE::LANDSCAPE:
		break;
	}

	return nullptr;
}
