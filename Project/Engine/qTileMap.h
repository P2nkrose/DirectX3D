#pragma once
#include "qRenderComponent.h"

class qStructuredBuffer;

struct tTileInfo
{
	int ImgIdx;
	int padding[3];
};



class qTileMap : public qRenderComponent
{
public:
	CLONE(qTileMap);
	qTileMap();
	qTileMap(const qTileMap& _Origin);
	~qTileMap();

public:
	virtual void Init() override;
	virtual void FinalTick() override;
	virtual void Render() override;

public:
	void SetRowCol(UINT _Row, UINT _Col);
	void SetTileSize(Vec2 _Size);
	void SetAtlasTexture(Ptr<qTexture> _Atlas);
	void SetAtlasTileSize(Vec2 _TileSize);

public:
	virtual void SaveToFile(FILE* _File) override;
	virtual void LoadFromFile(FILE* _File) override;

	
private:
	void ChangeTileMapSize();


private:
	int						m_Row;				// TileMap �� �� ����
	int						m_Col;				// TileMap �� �� ����
	Vec2					m_TileSize;			// Tile 1���� ũ��

	Ptr<qTexture>			m_TileAtlas;        // Tile ���� �̹������� �����ϰ� �ִ� ��Ʋ�� �ؽ���
	Vec2					m_AtlasResolution;  // Atlas �ؽ��� �ػ�
	Vec2					m_AtlasTileSize;    // Atlas �ؽ��� ������ Ÿ�� 1���� ũ��
	Vec2					m_AtlasTileSliceUV; // Atlas �ؽ��� ������ Ÿ�� 1���� ũ�⸦ UV �� ��ȯ�� ũ��(SliceUV)

	int						m_AtlasMaxRow;      // Atlas �ؽ��İ� �����ϰ� �ִ� Ÿ���� �ִ� �� ����
	int						m_AtlasMaxCol;      // Atlas �ؽ��İ� �����ϰ� �ִ� Ÿ���� �ִ� �� ����

	vector<tTileInfo>       m_vecTileInfo;      // ������ Ÿ���� ����
	qStructuredBuffer*		m_Buffer;			// ������ Ÿ�������� t �������ͷ� �����ϱ� ���� ����ȭ����
};

