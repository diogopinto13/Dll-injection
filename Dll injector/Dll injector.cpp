#include <Windows.h>
#include <Tlhelp32.h>
#include <Tchar.h>


/*
uintptr_t GetModuleBaseAddress(DWORD pid, const char* modName) {
		HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
		if (hSnap != INVALID_HANDLE_VALUE) {
			MODULEENTRY32 modEntry;
			modEntry.dwSize = sizeof(modEntry);
			if (Module32First(hSnap, &modEntry)) {
				do {
					if (!strcmp(modEntry.szModule, modName)) {
						CloseHandle(hSnap);
						return (uintptr_t)modEntry.modBaseAddr;
					}
				} while (Module32Next(hSnap, &modEntry));
			}
		}
	}
*/
DWORD findProcessID(){
	HANDLE hProcessSnap;
	HANDLE hProcess;
	PROCESSENTRY32 pe32;
	DWORD dwPriorityClass;

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pe32.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(hProcessSnap, &pe32))
	{
		CloseHandle(hProcessSnap);
		return(FALSE);
	}

	do {
		if (!wcscmp(pe32.szExeFile, L"witcher3.exe")) {
			//_tprintf(_T("%s"), pe32.szExeFile);
			//_tprintf(_T("%d"), pe32.th32ProcessID);
			return pe32.th32ProcessID;
		}

	} while (Process32Next(hProcessSnap, &pe32));
	return 0;
}


int main(int argc, char** argv) {
	DWORD pID = findProcessID();

	char dll[] = "evil.dll"; //name of the dll to be injected
	char dllPath[MAX_PATH] = { 0 }; //to store the full path of the dll
	//gets the full path
	if (!GetFullPathNameA(dll, MAX_PATH, dllPath, NULL)) {
		_tprintf(TEXT("Error in getting full path of the dll"));
		return 0;
	}

	//get an handle of the game
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, pID);
	if (hProcess == NULL) {
		_tprintf(TEXT("Error in getting the handler of the process"));
		_tprintf(_T("%d"), GetLastError());
		return 0;
	}
	//alloc the memory 
	LPVOID pszLibFileRemote = VirtualAllocEx(hProcess, NULL, strlen(dllPath) + 1, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (pszLibFileRemote == NULL) {
		_tprintf(TEXT("Error in allocating the virtual memory"));
		return 0;
	}

	if (WriteProcessMemory(hProcess, pszLibFileRemote, dllPath, strlen(dllPath) + 1, NULL) == 0) {
		_tprintf(TEXT("Error in writing the virtual memory"));
		return 0;
	}
	HANDLE handleThread = CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibraryA, pszLibFileRemote, NULL, NULL);
	if (handleThread == NULL) {
		_tprintf(TEXT("Error in creating a remote thread"));
		return 0;
	}
	else {
		_tprintf(TEXT("Injecting..."));
	}

	WaitForSingleObject(handleThread, INFINITE);
	CloseHandle(handleThread);
	VirtualFreeEx(hProcess, dllPath, 0, MEM_RELEASE);
	CloseHandle(hProcess);

	return 0;
}

