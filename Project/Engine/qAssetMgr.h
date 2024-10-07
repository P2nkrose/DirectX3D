#pragma once

#include "qPathMgr.h"
#include "qTaskMgr.h"

#include "qMesh.h"
#include "qComputeShader.h"
#include "qMaterial.h"
#include "qTexture.h"
#include "qPrefab.h"
#include "qSound.h"
#include "qFSM.h"

class qAsset;

class qAssetMgr : public qSingleton<qAssetMgr>
{
	friend class qTaskMgr;

	SINGLE(qAssetMgr);

public:
	void Init();
	void Tick();

	template<typename T>
	Ptr<T> Load(const wstring& _Key, const wstring& _RelativePath);

	Ptr<qAsset> FindAsset(ASSET_TYPE _Type, const wstring& _Key);
	//void AddAsset(const wstring& _Key, qAsset* _Asset);

	template<typename T>
	Ptr<T> FindAsset(const wstring& _Key);

	template<typename T>
	void AddAsset(const wstring& _Key, Ptr<T> _Asset);

	// _Flags : D3D11_BIND_FLAG
	Ptr<qTexture> CreateTexture(wstring _strKey, UINT _Width, UINT _Height
							  , DXGI_FORMAT _Format, UINT _Flags
							  , D3D11_USAGE _Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT);

	Ptr<qTexture> CreateTexture(wstring _strKey, ComPtr<ID3D11Texture2D> _Tex2D);

public:
	void GetAssetNames(ASSET_TYPE _Type, vector<string>& _vecOut);
	const map<wstring, Ptr<qAsset>>& GetAssets(ASSET_TYPE _Type) { return m_mapAsset[(UINT)_Type]; }
	bool IsChanged() { return m_Changed; }


private:
	void CreateEngineMesh();
	void CreateEngineMaterial();
	void CreateEngineTexture();
	void CreateEngineSprite();
	void CreateEngineGraphicShader();
	void CreateEngineComputeShader();


private:
	void DeleteAsset(ASSET_TYPE _Type, const wstring& _Key);


private:
	map<wstring, Ptr<qAsset>>	m_mapAsset[(UINT)ASSET_TYPE::END];
	bool						m_Changed;		// Editor���� ���� ���� ������ Ȯ��

};


template<typename T>
Ptr<T> qAssetMgr::Load(const wstring& _Key, const wstring& _RelativePath)
{
	// ���� Ű�� ������ �ִ��� Ȯ��
	Ptr<T> Asset = FindAsset<T>(_Key);

	if (nullptr != Asset)
	{
		return Asset;
	}

	// ���� Ű���� ������ ��������
	wstring strFilePath = qPathMgr::GetInst()->GetContentPath();
	strFilePath += _RelativePath;

	Asset = new T;

	// �ε� ���� �� ����ó��
	if (FAILED(Asset->Load(strFilePath)))
	{
		MessageBox(nullptr, L"���� �ε� ����", L"�ε� ����", MB_OK);
		return nullptr;
	}

	// Asset �� �ڽ��� Ű���� ��θ� �˰� ��
	Asset->m_Key = _Key;
	Asset->m_RelativePath = _RelativePath;

	// �ʿ� ���
	ASSET_TYPE type = GetAssetType<T>();
	m_mapAsset[(UINT)type].insert(make_pair(_Key, Asset.Get()));

	// Asset ���� �˸�	
	qTaskMgr::GetInst()->AddTask(tTask{ ASSET_CHANGED });

	// �ε��� ���� �ּ� ��ȯ
	return Asset;
}


template<typename T>
Ptr<T> qAssetMgr::FindAsset(const wstring& _Key)
{
	ASSET_TYPE Type = GetAssetType<T>();

	map<wstring, Ptr<qAsset>>::iterator iter = m_mapAsset[(UINT)Type].find(_Key);

	if (iter == m_mapAsset[(UINT)Type].end())
		return nullptr;

	return (T*)iter->second.Get();
}


template<typename T>
void qAssetMgr::AddAsset(const wstring& _Key, Ptr<T> _Asset)
{
	ASSET_TYPE Type = GetAssetType<T>();

	assert(!FindAsset(Type, _Key).Get());

	_Asset->SetKey(_Key);
	m_mapAsset[(UINT)Type].insert(make_pair(_Key, _Asset.Get()));

	// Asset ���� �˸�
	qTaskMgr::GetInst()->AddTask(tTask{ ASSET_CHANGED });
}

