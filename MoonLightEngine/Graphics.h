#pragma once

#include "MyWin.h"
#include "MLException.h"
#include <d3d11.h>
#include <wrl.h>
#include <vector>

#define GEXEPT_CHECK(errorno) if(FAILED(hr = errorno)) throw Graphics::GExcept{__LINE__, __FILE__, errorno};
#define GEXEPT(errorno) Graphics::GExcept{__LINE__, __FILE__, errorno};
//NEEDED INFO MANAGER FROM TUTORIAL 15
#define GFX_THROW_INFO_ONLY(call) infoManager.Set(); (call); {auto v = infoManager.GetMessages(); if(!v.empty()) {throw Graphics::InfoException( __LINE__,__FILE__,v);}}

class Graphics
{
public:

	//Graphics exception------
	class GExcept : public MLException{
	public:
		Graphics::GExcept(int line, const char* file, HRESULT hr) noexcept : MLException{ line, file }, hr{ hr } {}
		const char* what() const noexcept override;
		const char* GetType() const noexcept { return "Graphics exception"; }
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
	private:
		HRESULT hr;
	};
	class DeviceRemovedException : public Graphics::GExcept {
	public:
		Graphics::DeviceRemovedException(int line, const char* file, HRESULT hr) noexcept : Graphics::GExcept{ line, file, hr} {}
		const char* GetType() const noexcept { return "Graphics exception DeviceRemovedException"; }
	};
	class InfoException : public MLException 
	{
	public:
		InfoException(int line, const char* file, std::vector< std::string > info) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept { return "Graphics info exception"; }
		std::string GetErrorInfo() const noexcept;
	private:
		std::string _info;

	};
	//------------------------

	Graphics(HWND hWnd);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics() = default;
	void EndFrame();
	void ClearBuffer(float r, float g, float b) noexcept 
	{
		const float color[] = { r, g, b, 1.0f };
		_pContext->ClearRenderTargetView(_pTarget.Get(), color);
	}

	void DrawTestTriangle() 
	{

		HRESULT hr;
		namespace wrl = Microsoft::WRL;

		struct Vertex 
		{
			float x;
			float y;
		};

		const Vertex vetices[] =
		{
			{  0.0f,  0.5f },
			{  0.5f, -0.5f },
			{ -0.5f, -0.5f }
		};


		wrl::ComPtr<ID3D11Buffer> pVertexBuffer;
		D3D11_BUFFER_DESC desc{ 0 };
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = 0u;
		desc.MiscFlags = 0u;
		desc.ByteWidth = sizeof(vetices);
		desc.StructureByteStride = sizeof(Vertex);
		D3D11_SUBRESOURCE_DATA data{ 0 };
		data.pSysMem = vetices;

		GEXEPT_CHECK( _pDevice->CreateBuffer(&desc, &data, &pVertexBuffer) );
		const UINT stride = sizeof(Vertex);
		const UINT offset = 0u;
		this->_pContext->IASetVertexBuffers(0u, 1u, &pVertexBuffer, &stride, &offset);
		this->_pContext->Draw( 3u, 0u );
	}

private:
	Microsoft::WRL::ComPtr< ID3D11Device > _pDevice;
	Microsoft::WRL::ComPtr < IDXGISwapChain > _pSwap;
	Microsoft::WRL::ComPtr < ID3D11DeviceContext > _pContext;
	Microsoft::WRL::ComPtr < ID3D11RenderTargetView > _pTarget;
};

