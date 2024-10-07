#pragma once
#include "qShader.h"
class qComputeShader : public qShader
{
public:
	qComputeShader(UINT _ThreadPerGroupX, UINT _ThreadPerGroupY, UINT _ThreadPerGroupZ
		, const wstring& _ShaderRelativePath, const string& _FuncName);
	~qComputeShader();

private:
	virtual int Binding() = 0;
	virtual void CalcGroupNum() = 0;
	virtual void Clear() = 0;


public:
	int Execute();

protected:
	// 1 ���� �׷�� ������ ��
	const UINT					m_ThreadPerGroupX;
	const UINT					m_ThreadPerGroupY;
	const UINT					m_ThreadPerGroupZ;

	// �׷� ��
	UINT						m_GroupX;
	UINT						m_GroupY;
	UINT						m_GroupZ;

	// ��� ������ ���� �뵵
	tMtrlConst					m_Const;


private:
	int CreateComputeShader(const wstring& _RelativePath, const string& _FuncName);


private:
	ComPtr<ID3DBlob>				m_CSBlob;
	ComPtr<ID3D11ComputeShader>		m_CS;
};

