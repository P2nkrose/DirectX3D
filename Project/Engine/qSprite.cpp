#include "pch.h"
#include "qSprite.h"

#include "qAssetMgr.h"
#include "qTexture.h"

qSprite::qSprite()
	: qAsset(ASSET_TYPE::SPRITE)
{
}

qSprite::~qSprite()
{
}



void qSprite::SetLeftTop(Vec2 _LeftTop)
{
	Vec2 AtlasResolution = Vec2((float)m_Atlas->Width(), (float)m_Atlas->Height());
	m_LeftTopUV = _LeftTop / AtlasResolution;
}

void qSprite::SetSlice(Vec2 _Slice)
{
	Vec2 AtlasResolution = Vec2((float)m_Atlas->Width(), (float)m_Atlas->Height());
	m_SliceUV = _Slice / AtlasResolution;
}

void qSprite::SetBackground(Vec2 _Background)
{
	Vec2 AtlasResolution = Vec2((float)m_Atlas->Width(), (float)m_Atlas->Height());
	m_BackgroundUV = _Background / AtlasResolution;
}

void qSprite::SetOffset(Vec2 _Offset)
{
	Vec2 AtlasResolution = Vec2((float)m_Atlas->Width(), (float)m_Atlas->Height());
	m_OffsetUV = _Offset / AtlasResolution;
}


void qSprite::Create(Ptr<qTexture> _Atlas, Vec2 _LeftTopPixel, Vec2 _SlicePixel)
{
	m_Atlas = _Atlas;
	assert(m_Atlas);

	UINT width = m_Atlas->Width();
	UINT height = m_Atlas->Height();

	m_LeftTopUV = Vec2(_LeftTopPixel.x / (float)width, _LeftTopPixel.y / (float)height);
	m_SliceUV = Vec2(_SlicePixel.x / (float)width, _SlicePixel.y / (float)height);
}

int qSprite::Save(const wstring& _FilePath)
{
	FILE* File = nullptr;
	_wfopen_s(&File, _FilePath.c_str(), L"wb");

	if (nullptr == File)
		return E_FAIL;

	// Atlas �������� ����
	SaveAssetRef(m_Atlas, File);

	fwrite(&m_LeftTopUV, sizeof(Vec2), 1, File);
	fwrite(&m_SliceUV, sizeof(Vec2), 1, File);
	fwrite(&m_BackgroundUV, sizeof(Vec2), 1, File);
	fwrite(&m_OffsetUV, sizeof(Vec2), 1, File);

	fclose(File);

	return S_OK;
}

int qSprite::Load(const wstring& _FilePath)
{
	FILE* File = nullptr;
	_wfopen_s(&File, _FilePath.c_str(), L"rb");

	if (nullptr == File)
	{
		return E_FAIL;
	}

	// Atlas Ű, ��� �ҷ�����
	LoadAssetRef(m_Atlas, File);

	fread(&m_LeftTopUV, sizeof(Vec2), 1, File);
	fread(&m_SliceUV, sizeof(Vec2), 1, File);
	fread(&m_BackgroundUV, sizeof(Vec2), 1, File);
	fread(&m_OffsetUV, sizeof(Vec2), 1, File);

	fclose(File);

	return S_OK;
}
