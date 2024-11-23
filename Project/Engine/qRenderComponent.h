#pragma once
#include "qComponent.h"

struct tMtrlSet
{
	Ptr<qMaterial> pSharedMtrl;		// ���� ���׸���
	Ptr<qMaterial> pDynamicMtrl;	// ���� ���׸����� ���纻
	Ptr<qMaterial> pCurMtrl;		// ���� ����� ���׸���
};

class qRenderComponent : public qComponent
{
public:
	qRenderComponent(COMPONENT_TYPE _Type);
	qRenderComponent(const qRenderComponent& _Origin);
	~qRenderComponent();

public:
	virtual qRenderComponent* Clone() PURE;
	virtual void FinalTick() PURE;
	virtual void Render() PURE;
	
	virtual void render_shadowmap();

public:
	void SetMesh(Ptr<qMesh> _Mesh);
	Ptr<qMesh> GetMesh() { return m_Mesh; }
	
	void SetMaterial(Ptr<qMaterial> _Mtrl, UINT _idx);
	Ptr<qMaterial> GetMaterial(UINT _idx);
	Ptr<qMaterial> GetSharedMaterial(UINT _idx);


	// ���� ���� ���� �� ��ȯ
	Ptr<qMaterial> GetDynamicMaterial(UINT _idx);

	UINT GetMaterialCount() { return (UINT)m_vecMtrls.size(); }

public:
	void SetFrustumCheck(bool _Check) { m_FrustumCheck = _Check; }
	bool IsFrustumCheck() { return m_FrustumCheck; }


protected:
	void SaveDataToFile(FILE* _File);
	void LoadDataFromFile(FILE* _File);


private:
	Ptr<qMesh>					m_Mesh;

	vector<tMtrlSet>			m_vecMtrls;		// ����

	bool						m_FrustumCheck; // ����ü üũ�� ���������� ��������
};

	