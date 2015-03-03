//The MIT License (MIT)
//
//Copyright (c) 2015
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#include "ProcessHandler.h"

sProcessInfo gSupportedProcessInfo[] = 
{
	{L"iw3sp.exe",		(BYTE*)0x00C8155C},
	{L"iw3mp.exe",		(BYTE*)0x00C5FA9C},
	{L"CoDWaW.exe",		(BYTE*)0x0176C8B8},
	{L"CoDWaWmp.exe",	(BYTE*)0x0098FE7C},
	{L"BlackOps.exe",	(BYTE*)0x01A7987C},
	{L"BlackOpsMP.exe",	(BYTE*)0x00E67E6C},
	{L"t6sp.exe",		(BYTE*)0x01FAB220},
	{L"t6mp.exe",		(BYTE*)0x021EF968},
	{L"t6zm.exe",		(BYTE*)0x021C5868},
#if _WIN64 || __amd64__
	{L"s1_sp64_ship.exe",	(BYTE*)0x14102FDD0},
	{L"s1_mp64_ship.exe",	(BYTE*)0x143FFFBF0},
#endif
};

DWORD gSupportedProcessCount = sizeof(gSupportedProcessInfo) / sizeof(sProcessInfo);


HANDLE OpenFirstSupportedProcess(BYTE** pHealthAddressPtr)
{
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry) == TRUE)
	{
		while (Process32Next(snapshot, &entry) == TRUE)
		{
			for(DWORD i = 0; i < gSupportedProcessCount; i++)
			{
				if (_wcsicmp(entry.szExeFile, gSupportedProcessInfo[i].string) == 0)
				{  
					HANDLE hProcess = OpenProcess(PROCESS_VM_READ, FALSE, entry.th32ProcessID);
					if(hProcess)
					{
						(*pHealthAddressPtr) = gSupportedProcessInfo[i].healthAddress;
						CloseHandle(snapshot);
						return hProcess;
					}
				}
			}
		}
	}

	return NULL;
}