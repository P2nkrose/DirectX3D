#include "pch.h"
#include "qRaycastCS.h"

#include "qStructuredBuffer.h"

qRaycastCS::qRaycastCS()
	: qComputeShader(32, 32, 1, L"shader\\raycast.fx", "CS_Raycast")
	, m_FaceX(0)
	, m_FaceZ(0)
	, m_Ray{}
	, m_OutBuffer(nullptr)
{
}

qRaycastCS::~qRaycastCS()
{
}

int qRaycastCS::Binding()
{
	if (nullptr == m_OutBuffer)
		return E_FAIL;

	// Raycasting �� ��Ȯ�ϰ� ����ϱ����ؼ� t0 �� ���̸ʵ� ����
	m_Const.btex[0] = !!m_HeightMap.Get();
	m_HeightMap->Binding_CS_SRV(0);

	m_OutBuffer->Binding_CS_UAV(0);

	m_Const.iArr[0] = m_FaceX;
	m_Const.iArr[1] = m_FaceZ;

	m_Const.v4Arr[0] = m_Ray.vStart;
	m_Const.v4Arr[1] = m_Ray.vDir;

	return S_OK;
}

void qRaycastCS::CalcGroupNum()
{
	m_GroupX = (m_FaceX * 2) / m_ThreadPerGroupX;
	m_GroupY = m_FaceZ / m_ThreadPerGroupY;
	m_GroupZ = 1;

	if (0 < (m_FaceX * 2) % m_ThreadPerGroupX)
		m_GroupX += 1;

	if (0 < m_FaceZ % m_ThreadPerGroupY)
		m_GroupY += 1;
}

void qRaycastCS::Clear()
{
	m_OutBuffer->Clear_CS_UAV();
	m_OutBuffer = nullptr;

	m_HeightMap->Clear_CS_SRV();
	m_HeightMap = nullptr;
}