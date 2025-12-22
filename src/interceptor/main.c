#include<stdio.h>
#include<windows.h>




int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    HMODULE hDLL = LoadLibraryW(L"Sentinel.Hook.dll");
    if(hDLL == NULL)
    {
        return 1;
    }
    Sleep(500000);



	FreeLibrary(hDLL);
    return 0;
}
