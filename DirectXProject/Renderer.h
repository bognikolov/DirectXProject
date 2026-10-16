#pragma once

#include <windows.h>
#include <dxgi.h>
#include <d3d12.h>
#include <wrl/client.h>
#include <string>
#include <vector>

struct AdapterInfo
{
    std::wstring name;
    UINT vendorId;
    UINT deviceId;
    UINT subSysId;
    UINT revision;
    SIZE_T dedicatedVideoMemory;
    SIZE_T dedicatedSystemMemory;
    SIZE_T sharedSystemMemory;
    bool software;
    LUID luid;
    std::wstring driverVersion;
    UINT outputCount;
    Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
};

class Renderer
{
public:
    bool Initialize();
    bool InitializeDirect3D12();
    void PrintAdapters() const;
    const std::vector<AdapterInfo>& GetAdapters() const;

private:
    void EnumerateAdapters();
    bool CreateDevice();
    bool CreateCommandQueue();
    bool CreateCommandAllocator();
    bool CreateCommandList();

    static bool CheckResult(HRESULT hr, const wchar_t* operation);
    static void PrintAdapter(UINT index, const AdapterInfo& info);
    static const wchar_t* VendorName(UINT vendorId);
    static std::wstring FormatBytes(SIZE_T bytes);

    Microsoft::WRL::ComPtr<IDXGIFactory1> m_factory;
    std::vector<AdapterInfo> m_adapters;

    Microsoft::WRL::ComPtr<ID3D12Device> m_device;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;
};
