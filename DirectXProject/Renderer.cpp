#include "Renderer.h"

#include <iostream>
#include <iomanip>
#include <sstream>

#pragma comment(lib, "dxgi.lib")

using Microsoft::WRL::ComPtr;

bool Renderer::Initialize()
{
    HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&m_factory));
    if (FAILED(hr))
    {
        std::wcerr << L"CreateDXGIFactory1 failed: 0x" << std::hex << hr << std::dec << L"\n";
        return false;
    }

    EnumerateAdapters();
    return true;
}

void Renderer::EnumerateAdapters()
{
    m_adapters.clear();

    ComPtr<IDXGIAdapter1> adapter;
    for (UINT i = 0; m_factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i)
    {
        DXGI_ADAPTER_DESC1 desc{};
        if (SUCCEEDED(adapter->GetDesc1(&desc)))
        {
            AdapterInfo info{};
            info.name = desc.Description;
            info.vendorId = desc.VendorId;
            info.deviceId = desc.DeviceId;
            info.subSysId = desc.SubSysId;
            info.revision = desc.Revision;
            info.dedicatedVideoMemory = desc.DedicatedVideoMemory;
            info.dedicatedSystemMemory = desc.DedicatedSystemMemory;
            info.sharedSystemMemory = desc.SharedSystemMemory;
            info.software = (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) != 0;
            info.luid = desc.AdapterLuid;

            LARGE_INTEGER driver{};
            if (SUCCEEDED(adapter->CheckInterfaceSupport(__uuidof(IDXGIDevice), &driver)))
            {
                std::wstringstream ss;
                ss << HIWORD(driver.HighPart) << L"." << LOWORD(driver.HighPart) << L"."
                   << HIWORD(driver.LowPart) << L"." << LOWORD(driver.LowPart);
                info.driverVersion = ss.str();
            }

            UINT outputs = 0;
            ComPtr<IDXGIOutput> output;
            while (adapter->EnumOutputs(outputs, &output) != DXGI_ERROR_NOT_FOUND)
            {
                ++outputs;
                output.Reset();
            }
            info.outputCount = outputs;

            m_adapters.push_back(info);
        }
        adapter.Reset();
    }
}

void Renderer::PrintAdapters() const
{
    for (UINT i = 0; i < m_adapters.size(); ++i)
        PrintAdapter(i, m_adapters[i]);
}

const std::vector<AdapterInfo>& Renderer::GetAdapters() const
{
    return m_adapters;
}

void Renderer::PrintAdapter(UINT index, const AdapterInfo& info)
{
    std::wcout << L"==== Adapter " << index << L" ====\n";
    std::wcout << L"Description        : " << info.name << L"\n";
    std::wcout << L"Vendor             : " << VendorName(info.vendorId)
               << L" (0x" << std::hex << info.vendorId << std::dec << L")\n";
    std::wcout << L"Device ID          : 0x" << std::hex << info.deviceId << std::dec << L"\n";
    std::wcout << L"Subsystem ID       : 0x" << std::hex << info.subSysId << std::dec << L"\n";
    std::wcout << L"Revision           : " << info.revision << L"\n";
    std::wcout << L"Dedicated VRAM     : " << FormatBytes(info.dedicatedVideoMemory) << L"\n";
    std::wcout << L"Dedicated sys RAM  : " << FormatBytes(info.dedicatedSystemMemory) << L"\n";
    std::wcout << L"Shared sys RAM     : " << FormatBytes(info.sharedSystemMemory) << L"\n";
    std::wcout << L"Type               : " << (info.software ? L"Software" : L"Hardware") << L"\n";
    std::wcout << L"LUID               : " << std::hex << info.luid.HighPart
               << L":" << info.luid.LowPart << std::dec << L"\n";
    if (!info.driverVersion.empty())
        std::wcout << L"Driver version     : " << info.driverVersion << L"\n";
    std::wcout << L"Outputs            : " << info.outputCount << L"\n\n";
}

const wchar_t* Renderer::VendorName(UINT vendorId)
{
    switch (vendorId)
    {
    case 0x10DE: return L"NVIDIA";
    case 0x1002:
    case 0x1022: return L"AMD";
    case 0x8086: return L"Intel";
    case 0x1414: return L"Microsoft";
    case 0x5143: return L"Qualcomm";
    default:     return L"Unknown";
    }
}

std::wstring Renderer::FormatBytes(SIZE_T bytes)
{
    const double MB = 1024.0 * 1024.0;
    const double GB = MB * 1024.0;
    std::wstringstream ss;
    ss << std::fixed << std::setprecision(2);
    if (bytes >= GB) ss << bytes / GB << L" GB";
    else             ss << bytes / MB << L" MB";
    return ss.str();
}
