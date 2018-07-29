#include "GraphicsEngine.h"
#include "EngineGlobals.h"

#include <stdio.h>
#include <io.h>
#include <strsafe.h>
#include <fcntl.h>

#include "Core/MinWindows.h"
#include "Core/Engine.h"
#include "Core/Platform/PlatformCore.h"
#include <string>
#include <iostream>

#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "d3dcompiler.lib")
#define USE_SEP_CONSOLE 1

void ErrorExit(LPTSTR lpszFunction)
{
	// Retrieve the system error message for the last-error code

	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0, NULL);

	// Display the error message and exit the process

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
	ExitProcess(dw);
}

int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE ,
	LPSTR    lpCmdLine,
	int       nCmdShow)
{

	int exitcode = 0;
#if USE_SEP_CONSOLE
	//Allocate a console window
	//so that messages can be redirected to stdout
	if (!AllocConsole())
	{
		ErrorExit(TEXT("AllocConsole"));
	}

	FILE* pf_out;
	freopen_s(&pf_out, "CONOUT$", "w", stdout);
#endif
	
	Engine* engine = new Engine();
	engine->PreInit();

	PlatformWindow* myapp = PlatformWindow::CreateApplication(engine,hInstance, lpCmdLine, nCmdShow);
	exitcode = myapp->Run();

	myapp->DestroyApplication();
	engine->Destory();
	delete engine;
	
#if USE_SEP_CONSOLE
	fclose(pf_out);
	//Free the console window
	if (!FreeConsole())
	{
		ErrorExit(TEXT("FreeConsole"));
	}
#endif
	return exitcode;
}
