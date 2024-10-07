#pragma once
#include "qAsset.h"

class qMesh : public qAsset
{
public:
	qMesh();
	~qMesh();

public:
	int Create(Vtx* _VtxSysMem, UINT _VtxCount, UINT* _IdxSysMem, UINT _IdxCount);
	void Binding();
	void Render();
	void Render_Particle(UINT _Count);

	void* GetVtxSysMem() { return m_VtxSysMem; }

	virtual int Load(const wstring& _FilePath) { return S_OK; }
	virtual int Save(const wstring& _FilePath) { return S_OK; }


private:
	ComPtr<ID3D11Buffer>	m_VB = nullptr;			// Vertex Buffer ���ؽ� ����
	ComPtr<ID3D11Buffer>	m_IB = nullptr;			// Index Buffer  �ε��� ����

	UINT					m_VtxCount;
	UINT					m_IdxCount;

	D3D11_BUFFER_DESC		m_VBDesc;
	D3D11_BUFFER_DESC		m_IBDesc;

	void*					m_VtxSysMem;
	void*					m_IdxSysMem;
};

