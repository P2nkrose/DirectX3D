#include "pch.h"
#include "qTileMap.h"

#include "qTransform.h"
#include "qAssetMgr.h"

#include "qStructuredBuffer.h"

qTileMap::qTileMap()
	: qRenderComponent(COMPONENT_TYPE::TILEMAP)
	, m_Row(1)
	, m_Col(1)
	, m_AtlasMaxRow(0)
	, m_AtlasMaxCol(0)
	, m_Buffer(nullptr)
{
	SetMesh(qAssetMgr::GetInst()->FindAsset<qMesh>(L"RectMesh"));
	SetMaterial(qAssetMgr::GetInst()->FindAsset<qMaterial>(L"TileMapMtrl"));

	m_Buffer = new qStructuredBuffer;
}

qTileMap::qTileMap(const qTileMap& _Origin)
	: qRenderComponent(_Origin)
	, m_Row(_Origin.m_Row)
	, m_Col(_Origin.m_Col)
	, m_TileSize(_Origin.m_TileSize)
	, m_TileAtlas(_Origin.m_TileAtlas)
	, m_AtlasResolution(_Origin.m_AtlasResolution)
	, m_AtlasTileSize(_Origin.m_AtlasTileSize)
	, m_AtlasTileSliceUV(_Origin.m_AtlasTileSliceUV)
	, m_AtlasMaxRow(_Origin.m_AtlasMaxRow)
	, m_AtlasMaxCol(_Origin.m_AtlasMaxCol)
	, m_vecTileInfo(_Origin.m_vecTileInfo)
	, m_Buffer(nullptr)
{
	m_Buffer = new qStructuredBuffer;
}

qTileMap::~qTileMap()
{
	delete m_Buffer;
}

void qTileMap::Init()
{
	// ��, �� �����ؼ� ����ȭ���� ũ�� ����
	SetRowCol(m_Row, m_Col);
}

void qTileMap::FinalTick()
{
}

void qTileMap::Render()
{
	// Ÿ���� ������ ����ȭ���۸� ���ؼ� t �������Ϳ� ���ε� ��Ų��.
	m_Buffer->SetData(m_vecTileInfo.data(), sizeof(tTileInfo) * m_Row * m_Col);
	m_Buffer->Binding(15);

	GetMaterial()->SetTexParam(TEX_0, m_TileAtlas);
	GetMaterial()->SetScalarParam(INT_1, m_AtlasMaxRow);
	GetMaterial()->SetScalarParam(INT_2, m_AtlasMaxCol);
	GetMaterial()->SetScalarParam(VEC2_1, Vec2(m_Col, m_Row));
	GetMaterial()->SetScalarParam(VEC2_0, m_AtlasTileSliceUV);
	GetMaterial()->Binding();
	Transform()->Binding();
	GetMesh()->Render();
}


void qTileMap::SetRowCol(UINT _Row, UINT _Col)
{
	m_Row = _Row;
	m_Col = _Col;

	ChangeTileMapSize();

	// Ÿ�� ����
	UINT TileCount = m_Row * m_Col;

	// Ÿ�� ������ �����ϴ� ������ ������ ������ Ÿ�ϰ����� �ٸ��� ��������
	if (m_vecTileInfo.size() != TileCount)
	{
		m_vecTileInfo.clear();
		m_vecTileInfo.resize(TileCount);
	}

	// Ÿ�������� ���޹޾Ƽ� t �������Ϳ� ���޽�ų ����ȭ���۰� Ÿ�� ��ü ������ ������� ������ ��������
	if (m_Buffer->GetElementCount() < TileCount)
	{
		m_Buffer->Create(sizeof(tTileInfo), TileCount, SB_TYPE::SRV_ONLY, false);
	}
}

void qTileMap::SetTileSize(Vec2 _Size)
{
	m_TileSize = _Size;

	ChangeTileMapSize();
}


void qTileMap::ChangeTileMapSize()
{
	Vec2 vSize = m_TileSize * Vec2((float)m_Col, (float)m_Row);
	Transform()->SetRelativeScale(vSize.x, vSize.y, 1.f);
}


void qTileMap::SetAtlasTexture(Ptr<qTexture> _Atlas)
{
	m_TileAtlas = _Atlas;
	
	if (nullptr == m_TileAtlas)
		m_AtlasResolution = Vec2(0.f, 0.f);
	else
		m_AtlasResolution = Vec2((float)_Atlas->Width(), (float)_Atlas->Height());

	SetAtlasTileSize(m_AtlasTileSize);
}

void qTileMap::SetAtlasTileSize(Vec2 _TileSize)
{
	m_AtlasTileSize = _TileSize;

	if (nullptr != m_TileAtlas)
	{
		m_AtlasTileSliceUV = m_AtlasTileSize / m_AtlasResolution;

		m_AtlasMaxCol = int(m_AtlasResolution.x / m_AtlasTileSize.x);
		m_AtlasMaxRow = int(m_AtlasResolution.y / m_AtlasTileSize.y);
	}
}



void qTileMap::SaveToFile(FILE* _File)
{
	SaveDataToFile(_File);

	fwrite(&m_Col, sizeof(int), 1, _File);
	fwrite(&m_Row, sizeof(int), 1, _File);

	fwrite(&m_TileSize, sizeof(Vec2), 1, _File);
	fwrite(&m_AtlasTileSize, sizeof(Vec2), 1, _File);



	for (size_t i = 0; i < m_vecTileInfo.size(); ++i)
	{
		fwrite(&m_vecTileInfo[i], sizeof(tTileInfo), 1, _File);
	}

	// ��Ʋ�� �ؽ���
	SaveAssetRef(m_TileAtlas, _File);
}

void qTileMap::LoadFromFile(FILE* _File)
{
	LoadDataFromFile(_File);

	fread(&m_Col, sizeof(int), 1, _File);
	fread(&m_Row, sizeof(int), 1, _File);

	SetRowCol(m_Row, m_Col);

	fread(&m_TileSize, sizeof(Vec2), 1, _File);
	fread(&m_AtlasTileSize, sizeof(Vec2), 1, _File);

	SetTileSize(m_TileSize);

	for (size_t i = 0; i < m_vecTileInfo.size(); ++i)
	{
		fread(&m_vecTileInfo[i], sizeof(tTileInfo), 1, _File);
	}

	// ��Ʋ�� �ؽ���
	LoadAssetRef(m_TileAtlas, _File);
	if (nullptr != m_TileAtlas)
	{
		SetAtlasTexture(m_TileAtlas);
	}
}

