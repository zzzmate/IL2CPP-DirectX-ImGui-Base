#include "Hooks.h"
#include "../Functions/Functions.h"

Hooks RunHooks;
Signatures RunSignatures;

void Hooks::Load(FILE* f)
{
	Hooks::LoadConsole(f); // need to put this first in case of melonloader junking your console
	Hooks::LoadModules();
	// this function calls when ur injecting ur dll
	RunSignatures.SearchSignatures(true); // true, so you getting also the addresses to your console
	//Hooks::LoadDetourHooks(); // if u using signatures like in the example, first let run the searchsignatures function, only uncomment if u wanna run all the functions when injecting dll
	//Hooks::LoadMinHookHooks(); // @TODO: UNCOMMENT ME IF U WANNA USE ME 
}

void Hooks::LoadModules()
{
	if (IL2CPP::Initialize(true))
	{
		UnitySDK::UnityGameBase = (uintptr_t)GetModuleHandleA(NULL);
		printf("[ @zzzmate ] Base Address: 0x%llX\n", UnitySDK::UnityGameBase);
		UnitySDK::UnityGameAssembly = (uintptr_t)GetModuleHandleA("GameAssembly.dll");
		printf("[ @zzzmate ] GameAssembly Base Address: 0x%llX\n", UnitySDK::UnityGameAssembly);
		UnitySDK::UnityPlayer = (uintptr_t)GetModuleHandleA("UnityPlayer.dll");
		printf("[ @zzzmate ] UnityPlayer Base Address: 0x%llX\n", UnitySDK::UnityPlayer);
	}

	IL2CPP::Callback::Initialize();
}

void Hooks::LoadConsole(FILE* f)
{
	AllocConsole();
	AttachConsole(GetCurrentProcessId());
	SetConsoleTitle(L"@zzzmate");
	freopen_s(&f, "CONOUT$", "w", stdout);
	system("cls"); // if you using melonloader you gon need this
}

void Hooks::LoadDetourHooks()
{
	GameFunctions::GetUnityCamera();
}

void Hooks::LoadMinHookHooks()
{
	// THE EXAMPLE IS FROM THE ORIGINAL IDOL'S BASE, AND IT'S NOT GOING TO WORK.
	// IF YOU WANNA MAKE HOOKS LIKE THIS ITS JUST AN EXAMPLE, THE FUNCTION NOT GOING TO RUN UNLESS YOU OUT COMMENT IT.
	// BUT IF U DONT CHANGE THIS FUNCTION THEN YOUR GAME IS GOING TO CRASH!
	// YOU NEED TO FIND UR OWN FUNCTIONS AND SET IT IN THE SIGNATURES ALSO.
	if (MH_CreateHook(reinterpret_cast<LPVOID*>(Offsets::SetTargetRecoil), &GameFunctions::TargetRecoilHook, (LPVOID*)&GameFunctions::TargetRecoilTemplate) == MH_OK)
		MH_EnableHook(reinterpret_cast<LPVOID*>(Offsets::SetTargetRecoil));
}

void Hooks::UnloadMinHookHooks()
{
	MH_DisableHook(reinterpret_cast<LPVOID*>(Offsets::SetTargetRecoil));
	MH_RemoveHook(reinterpret_cast<LPVOID*>(Offsets::SetTargetRecoil));
	// don't uninitaliaze, backend going to do that!!!
}

void Hooks::UnloadConsole(FILE* f)
{
	FreeConsole();
	if (f) fclose(f);
}

void Hooks::Unload(FILE* f)
{
	//Hooks::UnloadMinHookHooks(); // @TODO: UNCOMMENT ME IF U USING ME!

	IL2CPP::Callback::Uninitialize();
	Hooks::UnloadConsole(f);
	// don't uninitaliaze, backend going to do that!!!
}