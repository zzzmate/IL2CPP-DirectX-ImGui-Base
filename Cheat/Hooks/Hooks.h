#pragma once
#include "../../Utils/Utils.h"
// TODO: Make a ReturnOriginal function

class Hooks
{
public:
	void Load(FILE* f);
public:
	void LoadModules();
	void LoadConsole(FILE* f);
public:
	void LoadDetourHooks();
	void LoadMinHookHooks();
public:
	void UnloadMinHookHooks();
	void UnloadConsole(FILE* f);
	void Unload(FILE* f);
};

extern Hooks RunHooks;