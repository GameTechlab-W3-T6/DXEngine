#define _CRTDBG_MAP_ALLOC
// #define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#include <crtdbg.h>
#include "stdafx.h"
#include "UApplication.h"
#include "EditorApplication.h"
#include "IniFile.h"

#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

#ifndef DBG_NEW 
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ ) 
#define new DBG_NEW 
#endif

#endif

// Engine entry point
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
#ifdef _DEBUG
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	IniFile ini;
	if (!ini.load("editor.ini"))
	{
		assert(false && "Failed to open 'editor.ini'.");
	}

	bool bIsShaderReflectionEnabled = ini.getBool("Graphics", "ShaderReflection", false);

	// Create application instance
	EditorApplication app;

	// Initialize engine systems
	if (!app.Initialize(hInstance, L"Simple Engine", 1600, 900))
	{
		MessageBox(nullptr, L"Failed to initialize application", L"Error", MB_OK | MB_ICONERROR);
		return -1;
	}

	// Run main loop
	app.Run(bIsShaderReflectionEnabled);

	app.Shutdown();
	_CrtDumpMemoryLeaks();
	// Cleanup handled automatically by destructor
	return 0;
}