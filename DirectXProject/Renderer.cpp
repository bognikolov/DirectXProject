#include "Renderer.h"

#include <iostream>
#include <iomanip>
#include <sstream>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")

using Microsoft::WRL::ComPtr;

bool Renderer::Initialize()
{
    if (!CheckResult(CreateDXGIFactory1(IID_PPV_ARGS(&m_factory)), L"CreateDXGIFactory1"))
        return false;

    EnumerateAdapters();
    return true;
}

bool Renderer::InitializeDirect3D12()
{
    return CreateDevice()
        && CreateCommandQueue()
        && CreateCommandAllocator()
        && CreateCommandList();
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
            info.adapter = adapter;

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

bool Renderer::CreateDevice()
{
    for (const AdapterInfo& info : m_adapters)
    {
        if (info.software)
            continue;

        HRESULT hr = D3D12CreateDevice(info.adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device));
        if (SUCCEEDED(hr))
        {
            std::wcout << L"ID3D12Device created on adapter: " << info.name << L"\n";
            return true;
        }

        std::wcerr << L"Adapter \"" << info.name << L"\" cannot create a Direct3D 12 device: 0x"
                   << std::hex << static_cast<unsigned long>(hr) << std::dec << L"\n";
    }

    std::wcerr << L"No hardware adapter with Direct3D 12 support was found\n";
    return false;
}

bool Renderer::CreateCommandQueue()
{
    D3D12_COMMAND_QUEUE_DESC desc{};
    desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    desc.NodeMask = 0;

    HRESULT hr = m_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_commandQueue));
    if (!CheckResult(hr, L"ID3D12Device::CreateCommandQueue"))
        return false;

    std::wcout << L"ID3D12CommandQueue created\n";
    return true;
}

bool Renderer::CreateCommandAllocator()
{
    HRESULT hr = m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator));
    if (!CheckResult(hr, L"ID3D12Device::CreateCommandAllocator"))
        return false;

    std::wcout << L"ID3D12CommandAllocator created\n";
    return true;
}

bool Renderer::CreateCommandList()
{
    HRESULT hr = m_device->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        m_commandAllocator.Get(),
        nullptr,
        IID_PPV_ARGS(&m_commandList));
    if (!CheckResult(hr, L"ID3D12Device::CreateCommandList"))
        return false;

    if (!CheckResult(m_commandList->Close(), L"ID3D12GraphicsCommandList::Close"))
        return false;

    std::wcout << L"ID3D12GraphicsCommandList created\n";
    return true;
}

bool Renderer::CheckResult(HRESULT hr, const wchar_t* operation)
{
    if (FAILED(hr))
    {
        std::wcerr << operation << L" failed: 0x"
                   << std::hex << static_cast<unsigned long>(hr) << std::dec << L"\n";
        return false;
    }
    return true;
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
