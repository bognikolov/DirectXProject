#include <windows.h>
#include <dxgi.h>
#include <wrl/client.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>

#pragma comment(lib, "dxgi.lib")

using Microsoft::WRL::ComPtr;

static const wchar_t* VendorName(UINT id)
{
    switch (id)
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

static std::wstring FormatBytes(SIZE_T bytes)
{
    const double MB = 1024.0 * 1024.0;
    const double GB = MB * 1024.0;
    std::wstringstream ss;
    ss << std::fixed << std::setprecision(2);
    if (bytes >= GB) ss << bytes / GB << L" GB";
    else             ss << bytes / MB << L" MB";
    return ss.str();
}

int main()
{
    ComPtr<IDXGIFactory1> factory;
    HRESULT hr = CreateDXGIFactory1(IID_PPV_ARGS(&factory));
    if (FAILED(hr))
    {
        std::wcerr << L"CreateDXGIFactory1 failed: 0x" << std::hex << hr << L"\n";
        return 1;
    }

    ComPtr<IDXGIAdapter1> adapter;
    for (UINT i = 0; factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i)
    {
        DXGI_ADAPTER_DESC1 desc{};
        if (FAILED(adapter->GetDesc1(&desc)))
            continue;

        std::wcout << L"==== Adapter " << i << L" ====\n";
        std::wcout << L"Description        : " << desc.Description << L"\n";
        std::wcout << L"Vendor             : " << VendorName(desc.VendorId)
            << L" (0x" << std::hex << desc.VendorId << std::dec << L")\n";
        std::wcout << L"Device ID          : 0x" << std::hex << desc.DeviceId << std::dec << L"\n";
        std::wcout << L"Subsystem ID       : 0x" << std::hex << desc.SubSysId << std::dec << L"\n";
        std::wcout << L"Revision           : " << desc.Revision << L"\n";
        std::wcout << L"Dedicated VRAM     : " << FormatBytes(desc.DedicatedVideoMemory) << L"\n";
        std::wcout << L"Dedicated sys RAM  : " << FormatBytes(desc.DedicatedSystemMemory) << L"\n";
        std::wcout << L"Shared sys RAM     : " << FormatBytes(desc.SharedSystemMemory) << L"\n";
        std::wcout << L"Type               : "
            << ((desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) ? L"Software" : L"Hardware") << L"\n";
        std::wcout << L"LUID               : " << std::hex << desc.AdapterLuid.HighPart
            << L":" << desc.AdapterLuid.LowPart << std::dec << L"\n";

        LARGE_INTEGER drv{};
        if (SUCCEEDED(adapter->CheckInterfaceSupport(__uuidof(IDXGIDevice), &drv)))
        {
            std::wcout << L"Driver version     : "
                << HIWORD(drv.HighPart) << L"." << LOWORD(drv.HighPart) << L"."
                << HIWORD(drv.LowPart) << L"." << LOWORD(drv.LowPart) << L"\n";
        }

        UINT outputs = 0;
        ComPtr<IDXGIOutput> output;
        while (adapter->EnumOutputs(outputs, &output) != DXGI_ERROR_NOT_FOUND)
        {
            ++outputs;
            output.Reset();
        }
        std::wcout << L"Outputs            : " << outputs << L"\n\n";

        adapter.Reset();
    }

    return 0;
}