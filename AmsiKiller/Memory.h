#pragma once

#include <iostream>
#include <Windows.h>
#include <vector>
#include "Xor.h"

using namespace std;



class Memory {
public:
	//Memory();
	//~Memory();


	int SearchPattern(BYTE* startAddress, DWORD searchSize, BYTE* pattern, DWORD patternSize);
	int Patching(DWORD tpid);
	int UpdateAmsi(HANDLE processHandle, PVOID functionAddr, HMODULE hm);

	int openProcessAndLoadLibrary(DWORD tpid, const char* dllName, const char* functionName);

	void GetProcess(const char*);
	
	//HANDLE ourHandle;

};