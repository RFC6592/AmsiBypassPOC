#include <windows.h>
#include <tlhelp32.h>
#include <stdio.h>
#include <stdexcept>
#include <sstream>
#include "Memory.h"



//00007FFAE957C650 | 48:85D2 | test rdx, rdx | // Verification de RDX 
//00007FFAE957C653 | 74 3F | je amsi.7FFAE957C694 | // Si RDX == RDX jump 7FFAE957C694
//00007FFAE957C655 | 48 : 85C9 | test rcx, rcx | // Verification de RDX 
//00007FFAE957C658 | 74 3A | je amsi.7FFAE957C694 | // Si RDX == RDX jump 7FFAE957C694
//00007FFAE957C65A | 48 : 8379 08 00 | cmp qword ptr ds : [rcx + 8] , 0 |
//00007FFAE957C65F | 74 33 | je amsi.7FFAE957C694 |




int wmain() {

    Memory ourMemory;
    ourMemory.GetProcess("powershell.exe");
	system("pause");

	return EXIT_SUCCESS;
}
