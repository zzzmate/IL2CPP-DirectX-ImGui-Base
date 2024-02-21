#pragma once
#include "../../Utils/Utils.h"
#include "../../Utils/Math/Vectors/Vector2.h"

// put 'inline' keyword to every variable or u getting errors

namespace Variables {
	inline Vector2 ScreenSize = { 0, 0 };
	inline Vector2 ScreenCenter = { 0, 0 };

	inline bool EnableCamera = false;
	inline float CameraFov = 90.f;

	inline bool EnableRecoil = false;
	inline float RecoilEdit = 0.0f;

	inline bool EnableCircleFov = false;
	inline float CircleFov = 15.f;

	inline bool EnableRainbow = false;
	inline ImVec4 RainbowColor = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

	inline bool EnableWatermark = false;
	inline bool EnableFPS = false;

	inline bool EnableSnaplines = false;
	inline bool EnableRainbowSnaplines = false;
	inline ImColor PlayerSnaplineColor = ImColor(255.0f / 255, 255.0f / 255, 255.0f / 255);

	inline int LineTypes = 1;
}