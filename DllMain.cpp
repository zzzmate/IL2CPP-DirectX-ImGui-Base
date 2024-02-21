#include "Utils/Utils.h"
#include "Utils/Backend/Backend.h"
#include "Cheat/Hooks/Hooks.h"

#define QWORD int64_t
FILE* f;

// run it in Release | x64, already set up everything

QWORD WINAPI PlayerCacheThread(LPVOID param)
{
	while (!Utils::KeyPressed(VK_END))
	{
		void* IL2CPPPlayerCacheThread = IL2CPP::Thread::Attach(IL2CPP::Domain::Get());
		Utils::PlayerList.clear();

		auto RunningList = Unity::Object::FindObjectsOfType<Unity::CComponent>("UnityEngine.Rigidbody");

		for (int i = 0; i < RunningList->m_uMaxLength; i++)
		{
			if (!RunningList->operator[](i)) continue;
			Utils::PlayerList.push_back(RunningList->operator[](i)->GetGameObject());
		}

		IL2CPP::Thread::Detach(IL2CPPPlayerCacheThread);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}
	return 0;
}

QWORD WINAPI MainThread(LPVOID param)
{
	RunBackend.Load(); // load everything
	RunHooks.Load(f);

	HANDLE PlayerCache = CreateThread(nullptr, 0, LPTHREAD_START_ROUTINE(PlayerCacheThread), (HMODULE)param, 0, nullptr);

	if (PlayerCache)
		CloseHandle(PlayerCache);

	while (!Utils::KeyPressed(VK_END)) // uninject
		std::this_thread::sleep_for(std::chrono::milliseconds(420)); // no cpu frying in my city

	RunHooks.Unload(f);
	RunBackend.Unload(); // unload everything
	return 0;
}

BOOL __stdcall DllMain(HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved)
{
	switch (dwReason)
	{
	case 1:
		HANDLE hMainThread = CreateThread(nullptr, 0, LPTHREAD_START_ROUTINE(MainThread), hModule, 0, nullptr);

		if (hMainThread)
			CloseHandle(hMainThread);
		break;
	}
	return TRUE;
}