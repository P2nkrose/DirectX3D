#include "pch.h"
#include "qTexture.h"

#include "qDevice.h"
#include "qPathMgr.h"

qTexture::qTexture()
	: qAsset(ASSET_TYPE::TEXTURE)
	, m_Desc{}
{
}

qTexture::~qTexture()
{
}


void qTexture::Binding(UINT _RegisterNum)
{
	CONTEXT->VSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
	CONTEXT->HSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
	CONTEXT->DSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
	CONTEXT->GSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
	CONTEXT->PSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
}


void qTexture::Clear(UINT _RegisterNum)
{
	ID3D11ShaderResourceView* pSRV = nullptr;
	CONTEXT->VSSetShaderResources(_RegisterNum, 1, &pSRV);
	CONTEXT->HSSetShaderResources(_RegisterNum, 1, &pSRV);
	CONTEXT->DSSetShaderResources(_RegisterNum, 1, &pSRV);
	CONTEXT->GSSetShaderResources(_RegisterNum, 1, &pSRV);
	CONTEXT->PSSetShaderResources(_RegisterNum, 1, &pSRV);
}


void qTexture::Binding_CS_SRV(UINT _RegisterNum)
{
	m_RecentBindingRegisterNum = _RegisterNum;
	CONTEXT->CSSetShaderResources(_RegisterNum, 1, m_SRV.GetAddressOf());
}

void qTexture::Clear_CS_SRV()
{
	ID3D11ShaderResourceView* pSRV = nullptr;
	CONTEXT->CSSetShaderResources(m_RecentBindingRegisterNum, 1, &pSRV);
}



void qTexture::Binding_CS_UAV(UINT _RegisterNum)
{
	UINT i = -1;
	CONTEXT->CSSetUnorderedAccessViews(_RegisterNum, 1, m_UAV.GetAddressOf(), &i);
	m_RecentBindingRegisterNum = _RegisterNum;
}


void qTexture::Clear_CS_UAV()
{
	UINT i = -1;
	ID3D11UnorderedAccessView* pUAV = nullptr;
	CONTEXT->CSSetUnorderedAccessViews(m_RecentBindingRegisterNum, 1, &pUAV, &i);
}

int qTexture::Load(const wstring& _FilePath)
{
	path filepath = _FilePath;				// ������ �ּ�
	path extention = filepath.extension();	// ������ Ȯ����

	HRESULT hr = S_OK;

	// *.dds
	if (extention == L".dds" || extention == L".DDS")
	{
		hr = LoadFromDDSFile(_FilePath.c_str(), DDS_FLAGS::DDS_FLAGS_NONE, nullptr, m_Image);
	}

	// *.tga
	else if (extention == L".tga" || extention == L".TGA")
	{
		hr = LoadFromTGAFile(_FilePath.c_str(), nullptr, m_Image);
	}

	// WIC == Window Image Component (*.bmp, *.png, *.jpg, *.jpeg)
	else if (extention == L".bmp" || extention == L".png" || extention == L".jpg" || extention == L".jpeg"
		|| extention == L".BMP" || extention == L".PNG" || extention == L".JPG" || extention == L".JPEG")
	{
		hr = LoadFromWICFile(_FilePath.c_str(), WIC_FLAGS::WIC_FLAGS_NONE, nullptr, m_Image);
	}

	// �� �� ���� ������ ���
	else
	{
		return E_FAIL;
	}

	// �ε��� �������� ���
	if (FAILED(hr))
	{
		errno_t ErrNum = GetLastError();
		WCHAR szBuff[255] = {};
		wsprintf(szBuff, L"Error Number : %d", ErrNum);
		MessageBox(nullptr, szBuff, L"�ؽ�ó �ε� ����", MB_OK);
	}

	// SystemMem(ScratchImage) -> GPU(Texture 2D)
	CreateShaderResourceView( DEVICE
							, m_Image.GetImages()
							, m_Image.GetImageCount()
							, m_Image.GetMetadata()
							, m_SRV.GetAddressOf());

	m_SRV->GetResource((ID3D11Resource**)m_Tex2D.GetAddressOf());

	m_Tex2D->GetDesc(&m_Desc);

	return S_OK;
}

int qTexture::Save(const wstring& _FilePath)
{
	// GPU -> System
	CaptureTexture(DEVICE, CONTEXT, m_Tex2D.Get(), m_Image);


	// System -> File
	wstring strRelativePath = qPathMgr::GetInst()->GetRelativePath(_FilePath);
	SetRelativePath(strRelativePath);


	HRESULT hr = E_FAIL;
	if (1 == m_Image.GetMetadata().arraySize)
	{
		// png, jpg, jpeg, bmp, 
		hr = SaveToWICFile(*m_Image.GetImages()
			, WIC_FLAGS_NONE
			, GetWICCodec(WICCodecs::WIC_CODEC_PNG)
			, _FilePath.c_str());
	}

	else
	{
		hr = SaveToDDSFile(m_Image.GetImages()
			, m_Image.GetMetadata().arraySize
			, m_Image.GetMetadata()
			, DDS_FLAGS_NONE
			, _FilePath.c_str());
	}

	return hr;
}

int qTexture::Create(UINT _Width, UINT _Height, DXGI_FORMAT _PixelFormat, UINT _Flags, D3D11_USAGE _Usage)
{
	m_Desc.Width = _Width;		// Depth Stencil �ؽ�ó�� Render Target �ػ󵵿� �ݵ�� ��ġ�ؾ� �Ѵ�.
	m_Desc.Height = _Height;
	m_Desc.Format = _PixelFormat;
	m_Desc.ArraySize = 1;
	m_Desc.BindFlags = _Flags;

	m_Desc.Usage = _Usage;		// System Memory ���� ���� ����

	if (D3D11_USAGE::D3D11_USAGE_DYNAMIC == _Usage)
	{
		m_Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else
	{
		m_Desc.CPUAccessFlags = 0;
	}

	m_Desc.MiscFlags = 0;
	m_Desc.MipLevels = 1;		// ��ȭ���� �̹��� �߰� ����

	m_Desc.SampleDesc.Count = 1;
	m_Desc.SampleDesc.Quality = 0;

	if (FAILED(DEVICE->CreateTexture2D(&m_Desc, nullptr, m_Tex2D.GetAddressOf())))
	{
		return E_FAIL;
	}



	// View ����
	if (m_Desc.BindFlags & D3D11_BIND_RENDER_TARGET)
	{
		DEVICE->CreateRenderTargetView(m_Tex2D.Get(), nullptr, m_RTV.GetAddressOf());
	}

	if (m_Desc.BindFlags & D3D11_BIND_DEPTH_STENCIL)
	{
		DEVICE->CreateDepthStencilView(m_Tex2D.Get(), nullptr, m_DSV.GetAddressOf());
	}

	if (m_Desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
	{
		DEVICE->CreateShaderResourceView(m_Tex2D.Get(), nullptr, m_SRV.GetAddressOf());
	}

	if (m_Desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS)
	{
		DEVICE->CreateUnorderedAccessView(m_Tex2D.Get(), nullptr, m_UAV.GetAddressOf());
	}

	return S_OK;
}

int qTexture::Create(ComPtr<ID3D11Texture2D> _Tex2D)
{
	m_Tex2D = _Tex2D;
	m_Tex2D->GetDesc(&m_Desc);

	// View ����
	if (m_Desc.BindFlags & D3D11_BIND_RENDER_TARGET)
	{
		DEVICE->CreateRenderTargetView(m_Tex2D.Get(), nullptr, m_RTV.GetAddressOf());
	}

	if (m_Desc.BindFlags & D3D11_BIND_DEPTH_STENCIL)
	{
		DEVICE->CreateDepthStencilView(m_Tex2D.Get(), nullptr, m_DSV.GetAddressOf());
	}

	if (m_Desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
	{
		DEVICE->CreateShaderResourceView(m_Tex2D.Get(), nullptr, m_SRV.GetAddressOf());
	}

	if (m_Desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS)
	{
		DEVICE->CreateUnorderedAccessView(m_Tex2D.Get(), nullptr, m_UAV.GetAddressOf());
	}

	return S_OK;
}

int qTexture::CreateArrayTexture(const vector<Ptr<qTexture>>& _vecTex)
{
	m_Desc = _vecTex[0]->GetDesc();
	m_Desc.ArraySize = (UINT)_vecTex.size();
	m_Desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_SHADER_RESOURCE;
	m_Desc.MipLevels = 1;

	if (FAILED(DEVICE->CreateTexture2D(&m_Desc, nullptr, m_Tex2D.GetAddressOf())))
	{
		return E_FAIL;
	}

	// ���� �� �ؽ��ĸ� ������ �迭 �ؽ����� �� ĭ���� �����Ų��.
	for (size_t i = 0; i < _vecTex.size(); ++i)
	{
		UINT Offset = D3D11CalcSubresource(0, i, 1);

		CONTEXT->UpdateSubresource(m_Tex2D.Get(), Offset, nullptr
			, _vecTex[i]->GetPixels()
			, _vecTex[i]->GetRowPitch()
			, _vecTex[i]->GetSlicePitch());
	}

	// Shader Resource View ����
	D3D11_SHADER_RESOURCE_VIEW_DESC tSRVDesc = {};

	tSRVDesc.Format = m_Desc.Format;
	tSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	tSRVDesc.Texture2DArray.MipLevels = 1;
	tSRVDesc.Texture2DArray.MostDetailedMip = 0;
	tSRVDesc.Texture2DArray.ArraySize = m_Desc.ArraySize;

	if (FAILED(DEVICE->CreateShaderResourceView(m_Tex2D.Get(), &tSRVDesc, m_SRV.GetAddressOf())))
		return E_FAIL;

	return S_OK;
}

int qTexture::GenerateMip(UINT _Level)
{
	// CubeTexture �� Mipmap ���� ����
	assert(false == m_Desc.MiscFlags & D3D11_SRV_DIMENSION_TEXTURECUBE);

	m_Tex2D = nullptr;
	m_RTV = nullptr;
	m_DSV = nullptr;
	m_SRV = nullptr;
	m_UAV = nullptr;

	m_Desc.MipLevels = _Level;
	m_Desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	m_Desc.MiscFlags |= D3D11_RESOURCE_MISC_GENERATE_MIPS;

	if (FAILED(DEVICE->CreateTexture2D(&m_Desc, nullptr, m_Tex2D.GetAddressOf())))
	{
		return E_FAIL;
	}

	for (UINT i = 0; i < m_Desc.ArraySize; ++i)
	{
		UINT iSubIdx = D3D11CalcSubresource(0, i, m_Desc.MipLevels);

		CONTEXT->UpdateSubresource(m_Tex2D.Get(), iSubIdx, nullptr
			, m_Image.GetImage(0, i, 0)->pixels
			, m_Image.GetImage(0, i, 0)->rowPitch
			, m_Image.GetImage(0, i, 0)->slicePitch);
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};

	if (2 <= m_Desc.ArraySize)
	{
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		SRVDesc.Texture2DArray.ArraySize = m_Desc.ArraySize;
		SRVDesc.Texture2DArray.MipLevels = m_Desc.MipLevels;
		SRVDesc.Texture2DArray.MostDetailedMip = 0;
	}
	else
	{
		SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		SRVDesc.Texture2D.MipLevels = m_Desc.MipLevels;
		SRVDesc.Texture2D.MostDetailedMip = 0;
	}

	if (FAILED(DEVICE->CreateShaderResourceView(m_Tex2D.Get(), &SRVDesc, m_SRV.GetAddressOf())))
		return E_FAIL;

	CONTEXT->GenerateMips(m_SRV.Get());

	return S_OK;
}
