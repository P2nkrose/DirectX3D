#pragma once
#include "qEntity.h"

#include "Ptr.h"
#include "qTexture.h"

class qMRT : public qEntity
{
public:
	CLONE_DISABLE(qMRT);
	qMRT();
	~qMRT();

public:
	Ptr<qTexture> GetRT(int _Index) { return m_arrRT[_Index]; }

	void Create(int _RTCount, Ptr<qTexture>* _arrRT, Ptr<qTexture> _DSTex);
	void SetClearColor(Vec4* _Color, bool _Independent);
	void OMSet();

	void Clear() { ClearRT(); ClearDS(); }
	void ClearRT();
	void ClearDS();



private:
	Ptr<qTexture>		m_arrRT[8];			// �ִ� ���� ������ Render Target ������ 8��
	Vec4				m_ClearColor[8];
	int					m_RTCount;

	Ptr<qTexture>		m_DSTex;			// ���� �ؽ���. ��� �����ϴ�.

	D3D11_VIEWPORT		m_ViewPort;			// MRT ����Ʈ ���� ��
};

