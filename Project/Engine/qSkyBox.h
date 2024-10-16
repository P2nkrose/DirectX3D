#pragma once
#include "qRenderComponent.h"

class qSkyBox : public qRenderComponent
{
public:
	CLONE(qSkyBox);
	qSkyBox();
	~qSkyBox();

public:
	virtual void FinalTick() override;
	virtual void Render() override;

	virtual void SaveToFile(FILE* _File) override;   // ���Ͽ� ������Ʈ�� ������ ����
	virtual void LoadFromFile(FILE* _File) override; // ���Ͽ� ������Ʈ�� ������ �ε�

private:

};

