#pragma once

#include <windows.h>
#include <dxgi.h>
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
};

class Renderer
{
public:
    bool Initialize();
    void PrintAdapters() const;
    const std::vector<AdapterInfo>& GetAdapters() const;

private:
    void EnumerateAdapters();
    static void PrintAdapter(UINT index, const AdapterInfo& info);
    static const wchar_t* VendorName(UINT vendorId);
    static std::wstring FormatBytes(SIZE_T bytes);

    Microsoft::WRL::ComPtr<IDXGIFactory1> m_factory;
    std::vector<AdapterInfo> m_adapters;
};
