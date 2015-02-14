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

#include "LFX2.h" //Can be found as part of the AlienFX SDK
#include <TlHelp32.h>
#include <iostream>

int _tmain(int argc, _TCHAR* argv[])
{
	HINSTANCE hLibrary = LoadLibrary(_T("LightFX.dll"));
	if(hLibrary)
	{
		LFX2INITIALIZE LFX_Initialize = (LFX2INITIALIZE)GetProcAddress(hLibrary, LFX_DLL_INITIALIZE);
		LFX2RELEASE LFX_Release = (LFX2RELEASE)GetProcAddress(hLibrary, LFX_DLL_RELEASE);
		LFX2RESET LFX_Reset = (LFX2RESET)GetProcAddress(hLibrary,LFX_DLL_RESET);
		LFX2UPDATE LFX_Update = (LFX2UPDATE)GetProcAddress(hLibrary, LFX_DLL_UPDATE);
		LFX2LIGHT LFX_Light = (LFX2LIGHT)GetProcAddress(hLibrary,LFX_DLL_LIGHT);

		LFX_Initialize();

		PROCESSENTRY32 entry;
		entry.dwSize = sizeof(PROCESSENTRY32);
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

		if (Process32First(snapshot, &entry) == TRUE)
		{
			bool done = false;
			while (Process32Next(snapshot, &entry) == TRUE && !done)
			{
				if (_wcsicmp(entry.szExeFile, L"BlackOps.exe") == 0)
				{  
					printf("Found Process\n\n");
					HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
					DWORD ProcessID = GetProcessId(hProcess);

					BYTE* HealthAddress = (BYTE*)0x01A7987C;

					bool running = true;
					int health = 0, max_health = 0, buf = 0;
					while(running == true)
					{
						SIZE_T numofbytesread = 0;
						if(!ReadProcessMemory(hProcess,HealthAddress,&health,sizeof(int),&numofbytesread))
						{
							running = false;
							break;
						}
						if(health != buf)
						{
							health = buf;

							if(health > max_health)
								max_health = health;
							else if(health <= 0)
								max_health = 100;
						
							LFX_Reset();
							unsigned int lightSettings = 0;
							unsigned int brightness = (max_health-health)*0x1000000;
							lightSettings = LFX_RED | brightness;
							LFX_Light(LFX_ALL,lightSettings);
							LFX_Update();
								
							Sleep(10);
						}
					}

					CloseHandle(hProcess);
					done = true;
				}
			}
		}
		LFX_Release();
		CloseHandle(snapshot);	
	}
	else
	{
		printf("Couldnt load LightFX.dll\n");
	}
	return 0;
}