#pragma once
#include "qComponent.h"

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
	void SetMesh(Ptr<qMesh> _Mesh) { m_Mesh = _Mesh; }
	void SetMaterial(Ptr<qMaterial> _Mtrl);

	Ptr<qMesh> GetMesh() { return m_Mesh; }
	Ptr<qMaterial> GetMaterial() { return m_Mtrl; }
	Ptr<qMaterial> GetSharedMtrl();


	// ���� ���� ���� �� ��ȯ
	Ptr<qMaterial> GetDynamicMaterial();

public:
	void SetFrustumCheck(bool _Check) { m_FrustumCheck = _Check; }
	bool IsFrustumCheck() { return m_FrustumCheck; }


protected:
	void SaveDataToFile(FILE* _File);
	void LoadDataFromFile(FILE* _File);


private:
	Ptr<qMesh>					m_Mesh;

	Ptr<qMaterial>				m_Mtrl;			// ���� ������� ����
	Ptr<qMaterial>				m_SharedMtrl;	// ���� ���� (������)
	Ptr<qMaterial>				m_DynamicMtrl;	// �ӽ�(����) ����

	bool						m_FrustumCheck; // ����ü üũ�� ���������� ��������
};

	