#pragma once
#include "../../IL2CPPResolver/il2cpp_resolver.hpp"
#include "../../IL2CPPResolver/Unity/Structures/Engine.hpp"
#include "../Utils/Offsets.h"

// Put "inline" keyword to every function, regards

namespace GameFunctions {
	inline Unity::CCamera* GetUnityCamera()
	{
		Unity::CCamera* (UNITY_CALLING_CONVENTION GetCameraTemplate)();
		return reinterpret_cast<decltype(GetCameraTemplate)>(Offsets::GetCameraMainOffset)(); // ugly ass original func returning ngl
	}

	inline void (UNITY_CALLING_CONVENTION TargetRecoilTemplate)(CombatMaster_Battle_Gameplay_Player_PlayerMovement_o*, float);
	inline void TargetRecoilHook(CombatMaster_Battle_Gameplay_Player_PlayerMovement_o* movement, float recoil)
	{
		if (Variables::EnableRecoil)
			recoil = Variables::RecoilEdit;

		return TargetRecoilTemplate(movement, recoil);
	}
}