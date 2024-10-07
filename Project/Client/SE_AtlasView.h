#pragma once
#include "SE_Sub.h"


class SE_AtlasView : public SE_Sub
{
public:
	friend class SE_Detail;
	SE_AtlasView();
	~SE_AtlasView();

public:
	virtual void Init() override;
	virtual void Update() override;

public:
	void SetAtlasTex(Ptr<qTexture> _Tex);

public:
	ImVec2 GetLeftTop() { return m_MouseLT; }
	ImVec2 GetRightBottom() { return m_MouseRB; }


private:
	void WheelCheck();
	void SelectCheck();
	void DrawSelectRect();
	void CalcSpriteSize(Vec2 _PixelPos);
	bool IsPixelOk(Vec2 _PixelPos);


private:
	Ptr<qTexture>		m_AtlasTex;

	ImVec2				ImageRectMin;		// Image ���� �»�� ���� ��ġ
	ImVec2				m_MousePos;			// ���� ���콺 ��ġ

	ImVec2				m_MouseLT;			// ���콺 ��ġ�� �ش��ϴ� Texture�� �ȼ� ��ǥ Left Top
	ImVec2				m_MouseRB;			// ���콺 ��ġ�� �ش��ϴ� Texture�� �ȼ� ��ǥ Right Bottom

	set<Vec2>			m_PixelID;			// ��ϵ� ���� �ִ� �ȼ����� Ȯ���ϴ� �뵵

	float				m_WidthSize;
	float				m_WheelScale;
	float				m_Ratio;			// ���� �ػ� ��� ��� �̹����� ����


};

