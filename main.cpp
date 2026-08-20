#include <windows.h>
#include <tlhelp32.h>
#include <iostream>

using namespace std;

DWORD access = PROCESS_ALL_ACCESS;
DWORD pid;

uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName) {
    uintptr_t modBaseAddr = 0;

    /*  
        Tirar snapshots de processos especificos.
        O primeiro parametro representa todos os módulos do processo especificado, ou seja, a funcao
        ira tirar um snapshot de todos os módulos.
        O segundo parametro representa o ID do processo. 
    */
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
    if(hSnap != INVALID_HANDLE_VALUE) {
        MODULEENTRY32W modEntry;
        modEntry.dwSize = sizeof(modEntry);

        /*
            Recupera informacoes sobre o primeiro modulo do processo especificado.
            O primeiro parametro é o ID do retorno da funcao CreateToolhelp32Snapshot.
            O segundo parametro é um ponteiro para a estrutura MODULEENTRY32W.
        */
        if(Module32FirstW(hSnap, &modEntry)) {
            do
            {       
                if (!_wcsicmp(modEntry.szModule, modName)) // Executa uma comparacao.
                {
                    modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
                    break;
                }
            } while (Module32NextW(hSnap, &modEntry)); // Recupera informacoes do proximo modulo.
        }
    }
    CloseHandle(hSnap);
    return modBaseAddr;
}

int main() {

    /* Get window name */
    HWND hWnd = FindWindowA("Notepad", NULL);
    if(!hWnd) {
        cout << "Window not found!" << endl;
    }

    /* Get pid from thread */
    DWORD wndPid = GetWindowThreadProcessId(hWnd, &pid);
    if(!wndPid) {
        cout << "PID not found!" << endl;
    }

    /* Open process */
    HANDLE pHandle = OpenProcess(access, 0, pid);
    if(!pHandle) {
        cout << "Process not open!" << endl;
    }

    /* Get base address */
    uintptr_t modBase = GetModuleBaseAddress(pid, L"Notepad.exe");
    if(!modBase) {
        cout << "Not found base address" << endl;
    }
}