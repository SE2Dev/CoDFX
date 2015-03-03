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
#include "ProcessHandler.h"
#include <iostream>

bool gProcError = false;

FARPROC WINAPI GetProcAddress_s (__in HMODULE hModule, __in LPCSTR lpProcName)
{
	FARPROC out = GetProcAddress(hModule, lpProcName);
	if(out == NULL)
	{
		gProcError = true;
	}
	return out;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	HINSTANCE hLibrary = LoadLibrary(L"LightFX.dll");
	if(hLibrary)
	{
		LFX2INITIALIZE LFX_Initialize = (LFX2INITIALIZE)GetProcAddress_s(hLibrary, LFX_DLL_INITIALIZE);
		LFX2RELEASE LFX_Release = (LFX2RELEASE)GetProcAddress_s(hLibrary, LFX_DLL_RELEASE);
		LFX2RESET LFX_Reset = (LFX2RESET)GetProcAddress_s(hLibrary,LFX_DLL_RESET);
		LFX2UPDATE LFX_Update = (LFX2UPDATE)GetProcAddress_s(hLibrary, LFX_DLL_UPDATE);
		LFX2LIGHT LFX_Light = (LFX2LIGHT)GetProcAddress_s(hLibrary,LFX_DLL_LIGHT);

		if(gProcError)
		{
			MessageBox(0,L"ERROR: LightFX.dll does not contain a required function",0,0);
			return 2;
		}

		BYTE* pHealth = nullptr;
		HANDLE hProcess = OpenFirstSupportedProcess(&pHealth);

		if(hProcess && pHealth)
		{
			LFX_Initialize();
			
			int playerHealth = 0;
			int playerMaxHealth = 0;

			int readBuf = NULL;
			SIZE_T numofbytesread = NULL;

			while(ReadProcessMemory(hProcess,pHealth,&readBuf,sizeof(int),&numofbytesread))
			{
				if(playerHealth != readBuf) //prevents updating unnecessarily
				{
					playerHealth = readBuf;

					if(playerHealth > playerMaxHealth)
						playerMaxHealth = playerHealth;
						
					LFX_Reset();

					unsigned int brightness = (playerMaxHealth-playerHealth)*0x1000000;
					unsigned int lightSettings = LFX_RED | brightness;

					LFX_Light(LFX_ALL,lightSettings);
					LFX_Update();

					if(playerHealth <= 0) //Fix for lights on instant death from maximum health
						playerMaxHealth = 1;

					Sleep(10);
				}				
			}
			
			LFX_Release();
			CloseHandle(hProcess);	
			FreeLibrary(hLibrary);
		}
		else
		{
			MessageBox(0,L"ERROR: Couldn't find a supported process\nClosing health monitor",0,0);
			FreeLibrary(hLibrary);
			return 1;
		}
	}
	else
	{
		MessageBox(0,L"ERROR: Couldn't load LightFX.dll\n",0,0);
	}
	return 0;
}