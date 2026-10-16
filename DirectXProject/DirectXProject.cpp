#include "Renderer.h"

int main()
{
    Renderer renderer;
    if (!renderer.Initialize())
        return 1;

    renderer.PrintAdapters();

    if (!renderer.InitializeDirect3D12())
        return 1;

    return 0;
}
