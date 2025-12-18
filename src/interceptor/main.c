#include<stdio.h>
#include<windows.h>



int __stdcall WinMain(HINSTANCE hIstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
    HMODULE hDLL = LoadLibrary("");
    if(hDLL == NULL)
    {
        return 1;
    }
    return 0;
}
