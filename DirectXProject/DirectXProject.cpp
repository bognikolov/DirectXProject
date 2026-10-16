#include "Renderer.h"

int main()
{
    Renderer renderer;
    if (!renderer.Initialize())
        return 1;

    renderer.PrintAdapters();
    return 0;
}
