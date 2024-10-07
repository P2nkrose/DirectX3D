#pragma once
#include "qComponent.h"

#include "qFlipBook.h"

class qFlipBookComponent : public qComponent
{
public:
	CLONE(qFlipBookComponent);
	qFlipBookComponent();
	qFlipBookComponent(const qFlipBookComponent& _Origin);
	~qFlipBookComponent();

public:
	virtual void FinalTick() override;


public:
	void Reset();
	void Binding();
	static void Clear();

public:
	virtual void SaveToFile(FILE* _File) override;
	virtual void LoadFromFile(FILE* _File) override;

public:
	void AddFlipBook(int _Idx, Ptr<qFlipBook> _FlipBook);
	Ptr<qFlipBook> FindFlipBook(const wstring& _Key);
	void Play(int _FliBookIdx, float _FPS, bool _Repeat);

	Ptr<qSprite> GetCurSprite() { return m_CurFrmSprite; }
	void SetCurSprite(Ptr<qSprite> _Sprite) { m_CurFrmSprite = _Sprite; }

	Ptr<qFlipBook> GetCurFlipBook() { return m_CurFlipBook; }
	void SetCurFlipBook(Ptr<qFlipBook> _FlipBook) { m_CurFlipBook = _FlipBook; }


// �߰�
public:
	vector<Ptr<qFlipBook>>& GetVecFlipBook() { return m_vecFlipBook; }
	bool IsCurFlipBookFinished() { return m_Finish; }

public:
	int GetMaxFrm() { return m_MaxSpriteIndex; }
	int GetCurFrmIdx() { return m_CurSpriteIndex; }

	bool GetRepeat() { return m_Repeat; }
	void SetRepeat(bool _Repeat);

	float GetFPS() { return m_FPS; }
	void SetFPS(float _fps) { m_FPS = _fps; }

private:
	vector<Ptr<qFlipBook>>			m_vecFlipBook;		// FlipBook ������Ʈ�� ������ ��� FlipBook ���
	Ptr<qFlipBook>					m_CurFlipBook;		// ���� ������� FlipBook
	Ptr<qSprite>					m_CurFrmSprite;		// ���� ������� FlipBook ���� ���� ������ �ε����� �ش��ϴ� ��������Ʈ
	int								m_CurSpriteIndex;	// ���� ����߿� FlipBook ���� ���° Sprite�� ��������� �ε��� ���
	int								m_MaxSpriteIndex;	// ���� ������� FlipBook �� �ִ� ������
	float							m_FPS;				// ���� ������� FlipBook �� �ʴ� ������ ���� ��
	float							m_AccTime;			// ���� �ð��� üũ
	bool							m_Repeat;
	bool							m_Finish;			// FlipBook ����� ���� �����ߴ��� ����

};

