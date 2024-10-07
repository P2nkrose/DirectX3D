#include "pch.h"
#include "qTestLevel.h"

#include <Engine/qAssetMgr.h>
#include <Engine/assets.h>

#include <Engine/qCollisionMgr.h>
#include <Engine/qLevelMgr.h>

#include <Engine/qLevel.h>
#include <Engine/qLayer.h>
#include <Engine/qGameObject.h>
#include <Engine/components.h>

#include <Scripts/qPlayerScript.h>
#include <Scripts/qMissileScript.h>
#include <Scripts/qCameraMoveScript.h>
#include <Scripts/qPlatformScript.h>

#include <Engine/qSetColorCS.h>
#include <Engine/qStructuredBuffer.h>

#include "qLevelSaveLoad.h"

void qTestLevel::CreateTestLevel()
{
	// Material
	Ptr<qMaterial> pMtrl = qAssetMgr::GetInst()->FindAsset<qMaterial>(L"Std2DMtrl");
	Ptr<qMaterial> pAlphaBlendMtrl = qAssetMgr::GetInst()->FindAsset<qMaterial>(L"Std2DAlphaBlendMtrl");
	Ptr<qMaterial> pDebugShapeMtrl = qAssetMgr::GetInst()->FindAsset<qMaterial>(L"DebugShapeMtrl");

	Ptr<qTexture> pTexture = qAssetMgr::GetInst()->FindAsset<qTexture>(L"texture\\Character.png");


	pAlphaBlendMtrl->SetTexParam(TEX_0, pTexture);

	// �׽�Ʈ Ŀ��

	

	// Sound ���


	//// ��ƼŬ �ּ�
	//// ��ǻƮ ���̴� �׽�Ʈ�� �ؽ��� ����
	//Ptr<qTexture> pTestTex = qAssetMgr::GetInst()->CreateTexture(L"ComputeShaderTestTex"
	//									, 1026, 1026, DXGI_FORMAT_R8G8B8A8_UNORM
	//									, D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS);


	//qSetColorCS cs;
	//cs.SetTargetTexture(pTestTex);
	//cs.SetClearColor(Vec4(0.f, 1.f, 0.f, 1.f));
	//cs.Execute();
	//pMtrl->SetTexParam(TEX_0, pTestTex);

	//// Set Data 
	//qStructuredBuffer* pBuffer = new qStructuredBuffer;
	//pBuffer->Create(sizeof(tParticle), 1, SB_TYPE::SRV_UAV, true);

	//tParticle Particle = {};
	//tParticle Particle2 = {};

	//Particle.Active = true;
	//Particle.vColor = Vec4(1.f, 0.f, 0.f, 1.f);
	//pBuffer->SetData(&Particle);

	//pBuffer->GetData(&Particle2);
	//delete pBuffer;


	//wstring StrLevelLoadPath = qPathMgr::GetInst()->GetContentPath();
	//StrLevelLoadPath += L"level\\Temp.lv";
	//qLevel* pLoadedLevel = qLevelSaveLoad::LoadLevel(StrLevelLoadPath);
	//
	//ChangeLevel(pLoadedLevel, LEVEL_STATE::PLAY);
	//
	//return;

	// Level ����
	qLevel* pLevel = new qLevel;
	pLevel->SetName(L"testlevel");

	qLevelMgr::GetInst()->InsertLevel(L"testlevel", pLevel);

	//ChangeLevel(pLevel, LEVEL_STATE::STOP);

	pLevel->GetLayer(0)->SetName(L"Default");
	pLevel->GetLayer(1)->SetName(L"Background");
	pLevel->GetLayer(2)->SetName(L"Platform");
	pLevel->GetLayer(3)->SetName(L"Player");
	pLevel->GetLayer(4)->SetName(L"Monster");
	pLevel->GetLayer(5)->SetName(L"PlayerProjectile");		// �÷��̾� ����ü
	pLevel->GetLayer(6)->SetName(L"MonsterProjectile");		// ���� ����ü
	pLevel->GetLayer(7)->SetName(L"Tile");					


	// ī�޶� ������Ʈ
	qGameObject* CamObj = new qGameObject;
	CamObj->SetName(L"MainCamera");
	CamObj->AddComponent(new qTransform);
	CamObj->AddComponent(new qCamera);
	CamObj->AddComponent(new qCameraMoveScript);

	// �켱������ 0 : Main Camera�� ����
	CamObj->Camera()->SetPriority(0);

	// ī�޶� ���̾� ���� (31�� ���̾� ���� ��� ���̾ �Կ�)
	// 31�� UI��
	CamObj->Camera()->SetLayerAll();
	CamObj->Camera()->SetLayer(31, false);
	CamObj->Camera()->SetFar(100000.f);
	CamObj->Camera()->SetProjType(ORTHOGRAPHIC);
	//CamObj->Camera()->SetProjType(PERSPECTIVE);

	pLevel->AddObject(0, CamObj);


	// ------------------------------------------------

	qGameObject* pObject = nullptr;


	// ���� ������Ʈ �߰�
	pObject = new qGameObject;
	pObject->SetName(L"PointLight 1");
	pObject->AddComponent(new qTransform);
	pObject->AddComponent(new qLight2D);

	pObject->Light2D()->SetLightType(LIGHT_TYPE::POINT);
	pObject->Light2D()->SetLightColor(Vec3(1.f, 1.f, 1.f));
	pObject->Light2D()->SetRadius(1000.f);
	pObject->Transform()->SetRelativePos(Vec3(0.f, 0.f, 100.f));

	pLevel->AddObject(0, pObject);


	// �÷���
	qGameObject* pGround = new qGameObject;
	pGround->SetName(L"Platform");
	pGround->AddComponent(new qTransform);
	pGround->AddComponent(new qCollider2D);
	pGround->AddComponent(new qPlatformScript);

	pGround->Transform()->SetRelativePos(0.f, -400.0f, 100.f);
	pGround->Transform()->SetRelativeScale(600.f, 100.f, 1.f);

	pGround->Collider2D()->SetIndependentScale(false);
	pGround->Collider2D()->SetOffset(Vec3(0.f, 0.f, 0.f));
	pGround->Collider2D()->SetScale(Vec3(1.f, 1.f, 1.f));


	//pGround->MeshRender()->SetMesh(qAssetMgr::GetInst()->FindAsset<qMesh>(L"RectMesh"));
	//pGround->MeshRender()->SetMaterial(pMtrl);

	pLevel->AddObject(2, pGround);




	// �÷��̾� ������Ʈ
	qGameObject* pPlayer = new qGameObject;
	pPlayer->SetName(L"Player");
	pPlayer->AddComponent(new qTransform);
	pPlayer->AddComponent(new qMeshRender);
	pPlayer->AddComponent(new qCollider2D);
	pPlayer->AddComponent(new qFlipBookComponent);
	pPlayer->AddComponent(new qPlayerScript);
	pPlayer->AddComponent(new qRigidBody);

	// ������ٵ�
	pPlayer->RigidBody()->SetMass(1.f);
	pPlayer->RigidBody()->SetMaxWalkSpeed(300.f);
	pPlayer->RigidBody()->SetFriction(100.f);

	pPlayer->RigidBody()->UseGravity(true);
	pPlayer->RigidBody()->SetMaxGravitySpeed(1500.f);
	pPlayer->RigidBody()->SetJumpSpeed(400.f);

	pPlayer->RigidBody()->SetGround(false);


	pPlayer->Transform()->SetRelativePos(0.f, 0.0f, 100.f);
	pPlayer->Transform()->SetRelativeScale(200.f, 200.f, 1.f);

	pPlayer->Collider2D()->SetIndependentScale(false);
	pPlayer->Collider2D()->SetOffset(Vec3(0.f, 0.f, 0.f));
	pPlayer->Collider2D()->SetScale(Vec3(1.f, 1.f, 1.f));

	pPlayer->MeshRender()->SetMesh(qAssetMgr::GetInst()->FindAsset<qMesh>(L"RectMesh"));
	pPlayer->MeshRender()->SetMaterial(pMtrl);
	
	Ptr<qFlipBook> pFlipBook = qAssetMgr::GetInst()->FindAsset<qFlipBook>(L"Animation\\Link_MoveDown.flip");
	Ptr<qFlipBook> pDeath = qAssetMgr::GetInst()->FindAsset<qFlipBook>(L"Animation\\death_idle.flip");
	
	pPlayer->FlipBookComponent()->AddFlipBook(5, pFlipBook);
	pPlayer->FlipBookComponent()->AddFlipBook(6, pDeath);

	pPlayer->FlipBookComponent()->Play(6, 10, true);

	pLevel->AddObject(3, pPlayer);


	//qGameObject* pClonePlayer = pPlayer->Clone();


	// ======================================================

	// Child ������Ʈ
	//qGameObject* pChild = new qGameObject;
	//pChild->SetName(L"Child");

	//pChild->AddComponent(new qTransform);
	//pChild->AddComponent(new qMeshRender);
	//pChild->AddComponent(new qCollider2D);

	//pChild->Transform()->SetRelativePos(400.f, 0.f, 0.f);
	//pChild->Transform()->SetRelativeScale(100.f, 100.f, 1.f);
	//pChild->Transform()->SetIndependentScale(true);

	//pChild->Collider2D()->SetOffset(Vec3(0.f, 0.f, 0.f));
	//pChild->Collider2D()->SetScale(Vec3(1.2f, 1.2f, 1.f));

	//pChild->MeshRender()->SetMesh(qAssetMgr::GetInst()->FindAsset<qMesh>(L"RectMesh"));
	//pChild->MeshRender()->SetMaterial(pMtrl);

	//pObject->AddChild(pChild);

	// ======================================================




	// Monster Object
	qGameObject* pMonster = new qGameObject;
	pMonster->SetName(L"Monster");
	
	pMonster->AddComponent(new qTransform);
	pMonster->AddComponent(new qMeshRender);
	pMonster->AddComponent(new qCollider2D);
	
	pMonster->Transform()->SetRelativePos(-400.f, 0.f, 100.f);
	pMonster->Transform()->SetRelativeScale(150.f, 150.f, 1.f);
	
	pMonster->Collider2D()->SetOffset(Vec3(0.f, 0.f, 0.f));
	pMonster->Collider2D()->SetScale(Vec3(1.2f, 1.2f, 1.f));

	
	pMonster->MeshRender()->SetMesh(qAssetMgr::GetInst()->FindAsset<qMesh>(L"RectMesh"));
	pMonster->MeshRender()->SetMaterial(pMtrl);
	
	pLevel->AddObject(4, pMonster);



	// TileMap Object
	qGameObject* pTileMapObj = new qGameObject;
	pTileMapObj->SetName(L"TileMap");
	
	pTileMapObj->AddComponent(new qTransform);
	pTileMapObj->AddComponent(new qTileMap);
	
	pTileMapObj->Transform()->SetRelativePos(Vec3(-500.f, 250.f, 500.f));
	
	pTileMapObj->TileMap()->SetRowCol(20, 20);
	pTileMapObj->TileMap()->SetTileSize(Vec2(64.f, 64.f));
	
	Ptr<qTexture> pTileAtlas = qAssetMgr::GetInst()->FindAsset<qTexture>(L"texture\\TILE.bmp");
	pTileMapObj->TileMap()->SetAtlasTexture(pTileAtlas);
	pTileMapObj->TileMap()->SetAtlasTileSize(Vec2(64.f, 64.f));
	
	pLevel->AddObject(2, pTileMapObj);


	// Particle Object
	//qGameObject* pParticleObj = new qGameObject;
	//
	//pParticleObj->SetName(L"Particle");
	//pParticleObj->AddComponent(new qTransform);
	//pParticleObj->AddComponent(new qParticleSystem);
	//
	//pParticleObj->Transform()->SetRelativePos(Vec3(0.f, 0.f, 0.f));
	//
	//pLevel->AddObject(0, pParticleObj);



	// Post Process Object : ��ó���� ������Ʈ
	qGameObject* pGrayFilterObj = new qGameObject;
	pGrayFilterObj->SetName(L"GrayFilter");
	pGrayFilterObj->AddComponent(new qTransform);
	pGrayFilterObj->AddComponent(new qMeshRender);

	pGrayFilterObj->Transform()->SetRelativeScale(150.f, 150.f, 1.f);

	pGrayFilterObj->MeshRender()->SetMesh(qAssetMgr::GetInst()->FindAsset<qMesh>(L"RectMesh"));
	pGrayFilterObj->MeshRender()->SetMaterial(qAssetMgr::GetInst()->FindAsset<qMaterial>(L"DistortionMtrl"));

	pLevel->AddObject(0, pGrayFilterObj);


	// Level Save
	//wstring strLevelPath = qPathMgr::GetInst()->GetContentPath();
	//strLevelPath += L"level\\Temp.lv";
	//qLevelSaveLoad::SaveLevel(strLevelPath, pLevel);
	
	//ChangeLevel(pLevel, LEVEL_STATE::STOP);



	// �浹 ����
	//qCollisionMgr::GetInst()->CollisionCheck(3, 4);		// Player vs Monster
	//qCollisionMgr::GetInst()->CollisionCheck(2, 3);		// Player vs Platform
	//qCollisionMgr::GetInst()->CollisionCheck(5, 4);		// Player Projectile vs Monster


	//pMtrl->Save(L"material\\std2d.mtrl");

	// ���� ����
	//m_CurLevel->Begin();
}

void qTestLevel::CreatePrefab()
{
	//qGameObject* pProto = new qGameObject;
	//pProto->SetName(L"Missile");
	//pProto->AddComponent(new qTransform);
	//pProto->AddComponent(new qMeshRender);
	//pProto->AddComponent(new qMissileScript);
	//
	//pProto->Transform()->SetRelativeScale(100.f, 100.f, 1.f);
	//
	//pProto->MeshRender()->SetMesh(qAssetMgr::GetInst()->FindAsset<qMesh>(L"RectMesh"));
	//pProto->MeshRender()->SetMaterial(qAssetMgr::GetInst()->FindAsset<qMaterial>(L"Std2DMtrl"));
	//
	//Ptr<qPrefab> pPrefab = new qPrefab;
	//pPrefab->SetProtoObject(pProto);
	//
	//qAssetMgr::GetInst()->AddAsset<qPrefab>(L"MissilePref", pPrefab);
	//
	//wstring FilePath = qPathMgr::GetInst()->GetContentPath();
	//FilePath += L"prefab\\Missile.pref";
	//pPrefab->Save(FilePath);
}
