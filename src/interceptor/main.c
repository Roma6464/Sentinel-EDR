#include<stdio.h>
#include<windows.h>




int main()
{
    HMODULE hDLL = LoadLibraryW(L"Sentinel.Hook.dll");
    // Utilise le double antislash \\ pour les chemins en C
    //HMODULE hDLL = LoadLibraryW(L"C:\\Users\\admin_edr\\Documents\\GitHub\\Sentinel-EDR\\Sentinel-EDR\\x64\\Debug\\Sentinel.Hook.dll");
    if(hDLL == NULL)
    {
        printf("Error : %lu\n", GetLastError());
        return 1;
    }
    Sleep(500000);



	FreeLibrary(hDLL);
    return 0;
}
