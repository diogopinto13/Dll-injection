// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <Windows.h>

// Offset to the money variable (adjust this based on your game's memory layout)
DWORD moneyOffset = 40;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        MessageBoxA(NULL, "DLL injected!", "DLL injected!", MB_OK);
        // Get the base address of the current process's executable module
        HMODULE moduleHandle = GetModuleHandle(NULL);
        if (moduleHandle == NULL)
        {
            // Handle error, e.g., unable to get module handle
            return FALSE;
        }

        // Calculate the address of the money variable
        DWORD_PTR moneyAddress = (DWORD_PTR)moduleHandle + moneyOffset;

        // Check for F1 key press and manipulate money if detected
        while (true)
        {
            if (GetAsyncKeyState(VK_F1) & 1) // F1 key pressed
            {
                // Read current money value
                MessageBoxA(NULL, "adding 1000 coins!", "DLL injected!", MB_OK);
                int currentMoney;
                ReadProcessMemory(GetCurrentProcess(), (LPVOID)moneyAddress, &currentMoney, sizeof(int), NULL);

                WCHAR message[100];
                wsprintfW(message, L"Current Money: %d", currentMoney);
                MessageBoxW(NULL, message, L"Money Information", MB_OK);

                // Modify the money value (add 1000 coins)
                int newMoney = currentMoney + 1000;
                WriteProcessMemory(GetCurrentProcess(), (LPVOID)moneyAddress, &newMoney, sizeof(int), NULL);
            }
        }
    }
    return TRUE;
}