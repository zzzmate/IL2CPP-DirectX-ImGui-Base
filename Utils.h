#pragma once

#define WIN32_LEAN_AND_MEAN
#define UNITY_CALLING_CONVENTION __fastcall*

#include <windows.h>
#include <thread>
#include <stdlib.h> 
#include <math.h>
#include <algorithm>
#include <string>
#include <sstream>
#include <MinHook.h> // @IF ERROR: install please vcpkg then run this: "vcpkg install minhook:x64-windows-static"

#include "./ImGui/imgui.h"
#include "./ImGui/imgui_impl_win32.h"
#include "./ImGui/imgui_impl_dx11.h"
#include "./ImGui/imgui_internal.h"

#include "../Cheat/Utils/SDK.h"
#include "../Cheat/Utils/Offsets.h"
#include "../Cheat/Utils/Variables.h"
#include "../Cheat/Signatures/Signatures.h"
#include "../Cheat/Hooks/Hooks.h"
#include "../IL2CPPResolver/il2cpp_resolver.hpp"
#include "../IL2CPPResolver/Unity/Structures/Engine.hpp"
#include "../Utils/IL2CPPHeader/il2cpp.h"

#include "../Utils/Math/Vectors/Vector2.h"
#include "../Utils/Math/Vectors/Vector3.h"

#include <d3d11.h>
#pragma comment(lib, "d3d11.lib")

namespace Utils {
	// all of the functions that are in Idol's base are here too without any change, just the names
	inline std::vector<Unity::CGameObject*> PlayerList(NULL);

	inline bool KeyPressed(int vKey)
	{
		return (GetAsyncKeyState(vKey) & 1) != 0;
	}

	inline void AddressLog(uintptr_t address, const char* name) {
		printf("[ LOG ] %s: 0x%llX\n", name, address);
	}

	inline bool World2Screen(Unity::Vector3 world, Vector2& screen)
	{
		Unity::CCamera* CameraMain = Unity::Camera::GetMain(); // GetMain is the actual camera like in banana shooter, no need to change it
		if (!CameraMain) {
			return false;
		}

		Unity::Vector3 buffer = CameraMain->CallMethodSafe<Unity::Vector3>("WorldToScreenPoint", world, Unity::eye::mono); // Call the worldtoscren function using monoeye (2)

		if (buffer.x > Variables::ScreenSize.x || buffer.y > Variables::ScreenSize.y || buffer.x < 0 || buffer.y < 0 || buffer.z < 0) // check if point is on screen
		{
			return false;
		}

		if (buffer.z > 0.0f) // Check if point is in view
		{
			screen = Vector2(buffer.x, Variables::ScreenSize.y - buffer.y);
		}

		if (screen.x > 0 || screen.y > 0) // Check if point is in view
		{
			return true;
		}
	}

	inline auto RectFilled(float x0, float y0, float x1, float y1, ImColor color, float rounding, int rounding_corners_flags) -> VOID
	{
		auto vList = ImGui::GetBackgroundDrawList();
		vList->AddRectFilled(ImVec2(x0, y0), ImVec2(x1, y1), color, rounding, rounding_corners_flags);
	}

	inline auto HealthBar(float x, float y, float w, float h, int phealth, ImColor col) -> VOID
	{
		auto vList = ImGui::GetBackgroundDrawList();

		int healthValue = max(0, min(phealth, 100));

		int barColor = ImColor
		(min(510 * (100 - healthValue) / 100, 255), min(510 * healthValue / 100, 255), 25, 255);
		vList->AddRect(ImVec2(x - 1, y - 1), ImVec2(x + w + 1, y + h + 1), col);
		RectFilled(x, y, x + w, y + (((float)h / 100.0f) * (float)phealth), barColor, 0.0f, 0);
	}

	inline void UseFov(static bool IsRainbow)
	{
		if(IsRainbow)
			ImGui::GetForegroundDrawList()->AddCircle(ImVec2(Variables::ScreenCenter.x, Variables::ScreenCenter.y), Variables::CircleFov, ImColor(Variables::RainbowColor.x, Variables::RainbowColor.y, Variables::RainbowColor.z, Variables::RainbowColor.w), 360);
		else
			ImGui::GetForegroundDrawList()->AddCircle(ImVec2(Variables::ScreenCenter.x, Variables::ScreenCenter.y), Variables::CircleFov, ImColor(255, 255, 255), 360);
	}

	inline auto AddText(ImVec2 pos, char* text, ImColor color) -> void
	{
		auto DrawList = ImGui::GetForegroundDrawList();
		auto wText = text;

		auto Size = ImGui::CalcTextSize(wText);
		pos.x -= Size.x / 2.f;
		pos.y -= Size.y / 2.f;

		//	ImGui::PushFont(m_font);

		DrawList->AddText(ImVec2(pos.x + 1, pos.y + 1), ImColor(0, 0, 0, 255), wText);
		DrawList->AddText(ImVec2(pos.x, pos.y), color, wText);

		//	ImGui::PopFont();
	}

	inline float DrawOutlinedText(ImFont* pFont, const ImVec2& pos, float size, ImU32 color, bool center, const char* text, ...)
	{
		va_list(args);
		va_start(args, text);

		CHAR wbuffer[256] = { };
		vsprintf_s(wbuffer, text, args);

		va_end(args);

		auto DrawList = ImGui::GetBackgroundDrawList();
		std::stringstream stream(text);
		std::string line;

		float y = 0.0f;
		int i = 0;
		while (std::getline(stream, line))
		{
			ImVec2 textSize = pFont->CalcTextSizeA(size, FLT_MAX, 0.0f, wbuffer);

			if (center)
			{
				DrawList->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) + 1, (pos.y + textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), wbuffer);
				DrawList->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) - 1, (pos.y + textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), wbuffer);
				DrawList->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) + 1, (pos.y + textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), wbuffer);
				DrawList->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) - 1, (pos.y + textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), wbuffer);

				DrawList->AddText(pFont, size, ImVec2(pos.x - textSize.x / 2.0f, pos.y + textSize.y * i), ImGui::GetColorU32(color), wbuffer);
			}
			else
			{
				DrawList->AddText(pFont, size, ImVec2((pos.x) + 1, (pos.y + textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), wbuffer);
				DrawList->AddText(pFont, size, ImVec2((pos.x) - 1, (pos.y + textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), wbuffer);
				DrawList->AddText(pFont, size, ImVec2((pos.x) + 1, (pos.y + textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), wbuffer);
				DrawList->AddText(pFont, size, ImVec2((pos.x) - 1, (pos.y + textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), wbuffer);

				DrawList->AddText(pFont, size, ImVec2(pos.x, pos.y + textSize.y * i), ImGui::GetColorU32(color), wbuffer);
			}

			y = pos.y + textSize.y * (i + 1);
			i++;
		}
		return y;
	}

	inline float DrawOutlinedTextForeground(ImFont* pFont, const ImVec2& pos, float size, ImU32 color, bool center, const char* text, ...)
	{
		va_list(args);
		va_start(args, text);

		CHAR wbuffer[256] = { };
		vsprintf_s(wbuffer, text, args);

		va_end(args);

		auto DrawList = ImGui::GetForegroundDrawList();

		std::stringstream stream(text);
		std::string line;

		float y = 0.0f;
		int i = 0;

		while (std::getline(stream, line))
		{
			ImVec2 textSize = pFont->CalcTextSizeA(size, FLT_MAX, 0.0f, wbuffer);

			if (center)
			{
				DrawList->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) + 1, (pos.y + textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), wbuffer);
				DrawList->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) - 1, (pos.y + textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), wbuffer);
				DrawList->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) + 1, (pos.y + textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), wbuffer);
				DrawList->AddText(pFont, size, ImVec2((pos.x - textSize.x / 2.0f) - 1, (pos.y + textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), wbuffer);

				DrawList->AddText(pFont, size, ImVec2(pos.x - textSize.x / 2.0f, pos.y + textSize.y * i), ImGui::GetColorU32(color), wbuffer);
			}
			else
			{
				DrawList->AddText(pFont, size, ImVec2((pos.x) + 1, (pos.y + textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), wbuffer);
				DrawList->AddText(pFont, size, ImVec2((pos.x) - 1, (pos.y + textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), wbuffer);
				DrawList->AddText(pFont, size, ImVec2((pos.x) + 1, (pos.y + textSize.y * i) - 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), wbuffer);
				DrawList->AddText(pFont, size, ImVec2((pos.x) - 1, (pos.y + textSize.y * i) + 1), ImGui::GetColorU32(ImVec4(0, 0, 0, 255)), wbuffer);

				DrawList->AddText(pFont, size, ImVec2(pos.x, pos.y + textSize.y * i), ImGui::GetColorU32(color), wbuffer);
			}

			y = pos.y + textSize.y * (i + 1);
			i++;
		}
		return y;
	}

	inline void Watermark(std::string Text, bool ShowFps)
	{
		Utils::DrawOutlinedText(BaseFonts::GameFont, ImVec2(Variables::ScreenCenter.x, Variables::ScreenSize.y - 20), 15.0f, ImColor(Variables::RainbowColor.x, Variables::RainbowColor.y, Variables::RainbowColor.z), true, Text.c_str());
		if(ShowFps)
			Utils::DrawOutlinedText(BaseFonts::GameFont, ImVec2(Variables::ScreenCenter.x, 5), 13.0f, ImColor(Variables::RainbowColor.x, Variables::RainbowColor.y, Variables::RainbowColor.z), true, "[ %.1f FPS ]", ImGui::GetIO().Framerate);
	}

	inline void MouseMove(float tarx, float tary, float X, float Y, int smooth)
	{
		float ScreenCenterX = (X / 2);
		float ScreenCenterY = (Y / 2);
		float TargetX = 0;
		float TargetY = 0;

		smooth = smooth + 3;

		if (tarx != 0)
		{
			if (tarx > ScreenCenterX)
			{
				TargetX = -(ScreenCenterX - tarx);
				TargetX /= smooth;
				if (TargetX + ScreenCenterX > ScreenCenterX * 2) TargetX = 0;
			}

			if (tarx < ScreenCenterX)
			{
				TargetX = tarx - ScreenCenterX;
				TargetX /= smooth;
				if (TargetX + ScreenCenterX < 0) TargetX = 0;
			}
		}

		if (tary != 0)
		{
			if (tary > ScreenCenterY)
			{
				TargetY = -(ScreenCenterY - tary);
				TargetY /= smooth;
				if (TargetY + ScreenCenterY > ScreenCenterY * 2) TargetY = 0;
			}

			if (tary < ScreenCenterY)
			{
				TargetY = tary - ScreenCenterY;
				TargetY /= smooth;
				if (TargetY + ScreenCenterY < 0) TargetY = 0;
			}
		}
		mouse_event(MOUSEEVENTF_MOVE, static_cast<DWORD>(TargetX), static_cast<DWORD>(TargetY), NULL, NULL);
	}

	inline void DrawOutlineBox(const ImVec2& min, const ImVec2& max, ImU32 color, float thickness = 1.0f) {
		ImGuiWindow* window = ImGui::GetCurrentWindow();
		if (window == nullptr)
			return;

		ImVec2 tl = ImVec2(min.x, min.y);
		ImVec2 tr = ImVec2(max.x, min.y);
		ImVec2 bl = ImVec2(min.x, max.y);
		ImVec2 br = ImVec2(max.x, max.y);

		window->DrawList->AddLine(tl, tr, color, thickness);
		window->DrawList->AddLine(tr, br, color, thickness);
		window->DrawList->AddLine(br, bl, color, thickness);
		window->DrawList->AddLine(bl, tl, color, thickness);
	}

}
