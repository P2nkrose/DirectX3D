#pragma once
#include "qComponent.h"


class qTransform : public qComponent
{
public:
	CLONE(qTransform);
	qTransform();
	~qTransform();

	virtual void FinalTick() override;
	void Binding();

public:
	void SetRelativePos(Vec3 _Pos) { m_RelativePos = _Pos; }
	void SetRelativeScale(Vec3 _Scale) { m_RelativeScale = _Scale; }
	void SetRelativeRotation(Vec3 _Rot) { m_RelativeRotation = _Rot; }

	void SetRelativePos(float x, float y, float z) { m_RelativePos = Vec3(x, y, z); }
	void SetRelativeScale(float x, float y, float z) { m_RelativeScale = Vec3(x, y, z); }
	void SetRelativeRotation(float x, float y, float z) { m_RelativeRotation = Vec3(x, y, z); }

	void SetWorldMatrix(const Matrix& _matWorld) { m_matWorld = _matWorld; }

	void SetIndependentScale(bool _Set) { m_IndependentScale = _Set; }
	bool IsIndependentScale() { return m_IndependentScale; }
	
	Vec3 GetRelativePos() { return m_RelativePos; }
	Vec3 GetWorldPos() { return m_matWorld.Translation(); }

	Vec3 GetRelativeScale() { return m_RelativeScale; }
	Vec3 GetWorldScale();

	Vec3 GetRelativeRotation() { return m_RelativeRotation; }
	Vec3 GetRelativeDir(DIR _Type) { return m_RelativeDir[_Type]; }
	Vec3 GetWorldDir(DIR _Type) { return m_WorldDir[_Type]; }

	const Matrix& GetWorldMat() { return m_matWorld; }
	const Matrix& GetWorldMatInv() { return m_matWorldInv; }

public:
	qTransform& operator= (const qTransform& _Other);

public:
	virtual void SaveToFile(FILE* _File) override;
	virtual void LoadFromFile(FILE* _File) override;


private:
	Vec3		m_RelativePos;
	Vec3		m_RelativeScale;
	Vec3		m_RelativeRotation;

	Vec3		m_RelativeDir[3];	// 객체 하나당의 방향
	Vec3		m_WorldDir[3];		// 부모의 이동을 더한 월드에서의 방향

	Matrix		m_matWorld;			// 이동, 크기, 회전
	Matrix		m_matWorldInv;		// 월드행렬 역행렬
	bool		m_IndependentScale;	// 부모의 크기에 영향받지 않음
};

