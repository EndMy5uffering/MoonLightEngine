#include "Graphics.h"

#pragma comment(lib, "d3d11.lib")

Graphics::Graphics(HWND hWnd)
{
    HRESULT hr{};// <- needed for macros
    DXGI_SWAP_CHAIN_DESC sd = {};
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 0;
    sd.BufferDesc.RefreshRate.Denominator = 0;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = 1;
    sd.OutputWindow = hWnd;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sd.Flags = 0;

    GEXEPT_CHECK(D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &sd,
        &_pSwap,
        &_pDevice,
        nullptr,
        &_pContext
    ));
    
    Microsoft::WRL::ComPtr<ID3D11Resource> pBackBuffer;
    GEXEPT_CHECK(_pSwap->GetBuffer(0, __uuidof(ID3D11Resource), &pBackBuffer));
    GEXEPT_CHECK(_pDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &_pTarget));
    pBackBuffer->Release();
}

void Graphics::EndFrame()
{
    HRESULT hr{};
    if (FAILED( hr = _pSwap->Present(1u, 0u)))
    {
        if (hr == DXGI_ERROR_DEVICE_REMOVED) 
        {
            throw GEXEPT( _pDevice->GetDeviceRemovedReason() );
        }
        else
        {
            GEXEPT_CHECK(hr);
        }
    }

}


//Graphics exception

const char* Graphics::GExcept::what() const noexcept {

    std::ostringstream oss;
    oss << GetType() << "\n"
        << "[CODE]: " << this->GetErrorCode() << "\n"
        << "[DESCRIPTION]: " << this->GetErrorString() << "\n";
        //<< "[ERROR STRING]: " << this->GetError /*For this i need the dxerr.h <- see tutorial 15*/
    
    return this->whatBuffer.c_str();
}

HRESULT Graphics::GExcept::GetErrorCode() const noexcept
{
    return this->hr;
}

std::string Graphics::GExcept::GetErrorString() const noexcept
{
    return std::system_category().message(this->hr);
}

Graphics::InfoException::InfoException(int line, const char* file, std::vector< std::string > info) noexcept: MLException{ line, file }
{
    for (const std::string& infos : info) 
    {
        this->_info += infos;
        this->_info.push_back('\n');
    }
}

const char* Graphics::InfoException::what() const noexcept 
{
    std::ostringstream oss;
    oss << GetType() << std::endl
        << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
    oss << GetOriginString();
    whatBuffer = oss.str();
    return whatBuffer.c_str();
}

std::string Graphics::InfoException::GetErrorInfo() const noexcept
{
    return _info;
}

//------------------
