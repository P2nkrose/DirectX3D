#pragma once


class qEntity
{
public:
	qEntity();
	qEntity(const qEntity& _Other);
	virtual ~qEntity();

public:
	// Entity ���� �߻�Ŭ������ ����
	virtual qEntity* Clone() = 0;

	void SetName(const wstring& _Name) { m_Name = _Name; }
	const wstring& GetName() { return m_Name; }
	UINT GetID() { return m_ID; }


private:
	static UINT		g_ID;

private:
	wstring			m_Name;
	const UINT		m_ID;		// ���ȭ�� ������ ������ �̴ϼȶ������� �������ִ�.


};

