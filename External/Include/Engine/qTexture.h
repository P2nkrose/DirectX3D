#pragma once
#include "qAsset.h"

class qTexture : public qAsset
{
public:
	CLONE_DISABLE(qTexture);
	qTexture();
	~qTexture();

public:
	// Graphic Shader - t register
	void Binding(UINT _RegisterNum);
	static void Clear(UINT _RegisterNum);

	// Compute Shader - t register
	void Binding_CS_SRV(UINT _RegisterNum);
	void Clear_CS_SRV();

	// Compute Shader - u register
	void Binding_CS_UAV(UINT _RegisterNum);
	void Clear_CS_UAV();

	virtual int Load(const wstring& _FilePath) override;
	virtual int Save(const wstring& _FilePath) override;

public:
	// _Flags : D3D11_BIND_FLAG
	int Create(UINT _Width, UINT _Height, DXGI_FORMAT _PixelFormat, UINT _Flags, D3D11_USAGE _Usage = D3D11_USAGE_DEFAULT);
	int Create(ComPtr<ID3D11Texture2D> _Tex2D);

public:
	// 어레이 텍스처 만들기
	int CreateArrayTexture(const vector<Ptr<qTexture>>& _vecTex);
	int GenerateMip(UINT _Level);
	

public:
	ComPtr<ID3D11Texture2D>				GetTex2D() { return m_Tex2D; }
	ComPtr<ID3D11RenderTargetView>		GetRTV()   { return m_RTV; }
	ComPtr<ID3D11DepthStencilView>		GetDSV()   { return m_DSV; }
	ComPtr<ID3D11ShaderResourceView>	GetSRV()   { return m_SRV; }
	ComPtr<ID3D11UnorderedAccessView>	GetUAV()   { return m_UAV; }
	const D3D11_TEXTURE2D_DESC&			GetDesc()  { return m_Desc; }


	const TexMetadata& GetMetaData() { return m_Image.GetMetadata(); }
	tPixel* GetPixels() { return (tPixel*)m_Image.GetPixels(); }
	size_t GetRowPitch() { return m_Image.GetImages()->rowPitch; }
	size_t GetSlicePitch() { return m_Image.GetImages()->slicePitch; }


public:
	UINT Width() { return m_Desc.Width; }
	UINT Height() { return m_Desc.Height; }
	bool IsCubeMap() { return m_Desc.MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE; }

	UINT GetArraySize() { return m_Desc.ArraySize; }
	

private:
	ScratchImage						m_Image;
	ComPtr<ID3D11Texture2D>				m_Tex2D;

	ComPtr<ID3D11RenderTargetView>		m_RTV;		// Render Target View
	ComPtr<ID3D11DepthStencilView>		m_DSV;		// Depth Stencil View
	ComPtr<ID3D11ShaderResourceView>	m_SRV;		// Shader Resource View
	ComPtr<ID3D11UnorderedAccessView>	m_UAV;		// Unordered Access View

	D3D11_TEXTURE2D_DESC				m_Desc;

	UINT								m_RecentBindingRegisterNum;

};

