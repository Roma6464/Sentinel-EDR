// dllmain.cpp : Définit le point d'entrée de l'application DLL.
//freopen->redirige stdout vers la fenentre cree par allocConsole
#include "pch.h"
#include <MinHook.h>
#include <stdio.h>

#if _M_X64
#pragma comment(lib, "minhook.x64.lib")
#else 
#pragma comment(lib, "minhook.x86.lib")
#endif

//VirtualAlloc
//VirtualProtect
//WriteProcessMemory
//CreateRemoteThread


typedef LPVOID(WINAPI* PVIRTUALALLOCEX)(
    HANDLE hProcess,
    LPVOID lpAddress,
    SIZE_T dwSize,
    DWORD  flAllocationType,
    DWORD  flProtect
    );

typedef BOOL(WINAPI* PVIRTUALPROTECTEX)(
    HANDLE hProcess,
    LPVOID lpAddress,
    SIZE_T dwSize,
    DWORD  flNewProtect,
    PDWORD lpflOldProtect
    );

typedef BOOL(WINAPI* PWRITEPROCESSMEMORY)(
    HANDLE  hProcess,
    LPVOID  lpBaseAddress,
    LPCVOID lpBuffer,
    SIZE_T  nSize,
    SIZE_T* lpNumberOfBytesWritten
    );

typedef HANDLE(WINAPI* PCREATEREMOTETHREAD)(
    HANDLE                 hProcess,
    LPSECURITY_ATTRIBUTES  lpThreadAttributes,
    SIZE_T                 dwStackSize,
    LPTHREAD_START_ROUTINE lpStartAddress,
    LPVOID                 lpParameter,
    DWORD                  dwCreationFlags,
    LPDWORD                lpThreadId
    );

PVIRTUALALLOCEX pOriginalPVIRTUALALLOCEX = NULL;
PVIRTUALPROTECTEX pOriginalPVIRTUALPROTECTEX = NULL;
PWRITEPROCESSMEMORY pOriginalPWRITEPROCESSMEMORY = NULL;
PCREATEREMOTETHREAD pOriginalPCREATEREMOTETHREAD = NULL;


LPVOID WINAPI DetourPvirtualAlallocex(
    HANDLE hProcess,
    LPVOID lpAddress,
    SIZE_T dwSize,
    DWORD  flAllocationType,
    DWORD  flProtect
)
{
    if (flProtect == PAGE_EXECUTE_READWRITE)
    {
        printf("PAGE_EXECUTE_READWRITE permission detected in PVIRTUALALLOCEX function call\n");
        //if PAGE_EXECUTE_READWRITE prevent the call.
    }


    return pOriginalPVIRTUALALLOCEX(hProcess,lpAddress,dwSize,flAllocationType,flProtect);

}


BOOL WINAPI DetourPvirtualProtectex(
    HANDLE hProcess,
    LPVOID lpAddress,
    SIZE_T dwSize,
    DWORD  flNewProtect,
    PDWORD lpflOldProtect
)
{
    if (flNewProtect == PAGE_EXECUTE_READWRITE)
    {
        printf("PAGE_EXECUTE_READWRITE permission detected in PVIRTUALPROTECTEX function call\n");
        //if PAGE_EXECUTE_READWRITE prevent the call.
    }

    return pOriginalPVIRTUALPROTECTEX(hProcess,lpAddress,dwSize,flNewProtect,lpflOldProtect);

}

BOOL WINAPI DetourPwriteProcessMemory(
    HANDLE  hProcess,
    LPVOID  lpBaseAddress,
    LPCVOID lpBuffer,
    SIZE_T  nSize,
    SIZE_T* lpNumberOfBytesWritten
)
{
    return pOriginalPWRITEPROCESSMEMORY(hProcess, lpBaseAddress, lpBuffer, nSize, lpNumberOfBytesWritten);
}

HANDLE WINAPI DetourPcreateRemoteThread(
    HANDLE                 hProcess,
    LPSECURITY_ATTRIBUTES  lpThreadAttributes,
    SIZE_T                 dwStackSize,
    LPTHREAD_START_ROUTINE lpStartAddress,
    LPVOID                 lpParameter,
    DWORD                  dwCreationFlags,
    LPDWORD                lpThreadId
)
{
    return pOriginalPCREATEREMOTETHREAD(hProcess,lpThreadAttributes,dwStackSize,lpStartAddress,
        lpParameter,dwCreationFlags,lpThreadId);
}



void InitializeHooks()
{
    MH_STATUS status = MH_Initialize();
    if (status != MH_OK)
    {
        printf("Minhook init failed. Error : %s\n", MH_StatusToString(status));
        return;

    }
    else
    {
        printf("Minhook init succeed. Code : %s\n", MH_StatusToString(status));
    }
    MH_STATUS status_VirtualAllocEx = MH_CreateHookApi(L"kernel32","VirtualAllocEx",
        &DetourPvirtualAlallocex,(LPVOID*)(&pOriginalPVIRTUALALLOCEX));
    if (status_VirtualAllocEx != MH_OK)
    {
        printf("Failed to hook VirtualAllocEx. Error :%s\n",MH_StatusToString(status_VirtualAllocEx));
    }
    else
    {
        printf("Success to hook VirtualAllocEx.\n");
    }

    MH_STATUS status_VirtualProtectEx = MH_CreateHookApi(L"kernel32", "VirtualProtectEx",
        &DetourPvirtualProtectex, (LPVOID*)(&pOriginalPVIRTUALPROTECTEX));
    if (status_VirtualProtectEx != MH_OK)
    {
        printf("Failed to hook VirtualProtectEx. Error :%s \n", MH_StatusToString(status_VirtualProtectEx));
    }
    else
    {
        printf("Success to hook VirtualProtectEx.\n");
    }

    MH_STATUS status_WriteProcessMemory = MH_CreateHookApi(L"kernel32", "WriteProcessMemory",
        &DetourPwriteProcessMemory, (LPVOID*)(&pOriginalPWRITEPROCESSMEMORY));
    if (status_WriteProcessMemory != MH_OK)
    {
        printf("Failed to hook WriteProcessMemory. Error :%s\n", MH_StatusToString(status_WriteProcessMemory));
    }
    else
    {
        printf("Success to hook WriteProcessMemory.\n");
    }

    MH_STATUS status_CreateRemoteThread = MH_CreateHookApi(L"kernel32", "CreateRemoteThread",
        &DetourPcreateRemoteThread, (LPVOID*)(&pOriginalPCREATEREMOTETHREAD));
    if (status_CreateRemoteThread != MH_OK)
    {
        printf("Failed to hook CreateRemoteThread. Error :%s\n", MH_StatusToString(status_CreateRemoteThread));
    }
    else
    {
        printf("Success to hook CreateRemoteThread.\n");
    }

 
    status = MH_EnableHook(MH_ALL_HOOKS);
    if (status != MH_OK) {
        printf("[ERREUR] Impossible d'activer les hooks : %s\n", MH_StatusToString(status));
    }
    else {
        printf("EDR : Tous les hooks sont actifs. Surveillance en cours...\n");
    }
}



void CreateConsole()
{
	FreeConsole();

    if (AllocConsole())
    {
        FILE *file;
        freopen_s(&file, "CONOUT$", "w", stdout);
        freopen_s(&file, "CONOUT$", "w", stderr);
        freopen_s(&file, "CONIN$", "r", stdin);

		printf("Console allocated\n");
    }
}
//Thread separe appele par DLLMain qui va s'occuper de l'initialisation des hooks
DWORD WINAPI MainFunction(LPVOID lpParam)
{
    CreateConsole();
    InitializeHooks();
    Sleep(500000);
    return 0;
    //Initialisation hook.
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    HANDLE hThread;
    DWORD code_error;
    char bufferMessage[256];
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        hThread = CreateThread(NULL, 0, MainFunction, NULL, 0, NULL);
        if ( hThread == NULL)
        {
            code_error = GetLastError();
            sprintf_s(bufferMessage, sizeof(bufferMessage),
                "Echec CreateThread. Code erreur: %lu\n", code_error);
            OutputDebugStringA(bufferMessage);
            
        }
        else
        {
            OutputDebugStringA("[MON_EDR] Thread cree avec succes.\n");
            CloseHandle(hThread);   
        }
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

