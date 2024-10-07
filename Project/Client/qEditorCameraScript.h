#pragma once
#include <Engine/qScript.h>


class qEditorCameraScript : public qScript
{
public:
	CLONE(qEditorCameraScript);
	qEditorCameraScript();
	~qEditorCameraScript();


public:
	virtual void Tick() override;


public:
	virtual void SaveToFile(FILE* _File) override {}
	virtual void LoadFromFile(FILE* _File) override {}


private:
	void OrthoGraphicMove();		// ���� ����
	void PerspectiveMove();			// ���� ����

private:
	float		m_Speed;

};

