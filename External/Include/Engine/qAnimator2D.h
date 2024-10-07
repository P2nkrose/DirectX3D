#pragma once
#include "qComponent.h"

class qFlipBook;

class qAnimator2D : public qComponent
{
public:
	CLONE(qAnimator2D);
	qAnimator2D();
	~qAnimator2D();

public:
	virtual void FinalTick() override;


private:
	map<wstring, qFlipBook*>	m_mapAnim;	// Animator2D ������Ʈ�� ������ ��� FlipBook ���
	qFlipBook*					m_CurAnim;	// ���� ������� FlipBook
	int							m_CurFrm;	// ���� ����߿� FlipBook ���� ���° Sprite�� ��������� �ε��� ���
};

