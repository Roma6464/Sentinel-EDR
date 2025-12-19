#include<stdio.h>
#include<windows.h>




int __stdcall WinMain(HINSTANCE hIstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
    HMODULE hDLL = LoadLibrary("vhook.dll");
    if(hDLL == NULL)
    {
        return 1;
    }
    Sleep(2000);



	FreeLibrary(hDLL);
    return 0;
}
