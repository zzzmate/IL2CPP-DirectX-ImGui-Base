#pragma once
#include "../Utils.h"

class Backend
{
public:
	bool Load();
	void Unload();
public:
	bool DirectXPresentHook();
	void UnloadDevices(bool renderTargetViewReset);
public:
	void RenderCheat();
public:
	void LoadImGui(HWND window, ID3D11Device* device, ID3D11DeviceContext* context);
	void DrawImGui(ID3D11DeviceContext* context, ID3D11RenderTargetView* targetview);
	void UnloadImGui();
public:
	typedef long(__stdcall* presentVariable)(IDXGISwapChain*, UINT, UINT);
public:
	DXGI_SWAP_CHAIN_DESC m_gSwapChainDescription{};
	IDXGISwapChain* m_gSwapChain = nullptr;
	ID3D11Device* m_gDevice = nullptr;
	const D3D_FEATURE_LEVEL m_gFeatureLevels[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };
	HWND m_gWindow = NULL;
	ID3D11DeviceContext* m_gPointerContext = nullptr;
	ID3D11RenderTargetView* m_gMainRenderTargetView = nullptr;
	ID3D11Texture2D* m_gPointerBackBuffer = nullptr;
public:
	WNDPROC m_goriginalWndProc;
	DXGI_SWAP_CHAIN_DESC m_gPresentHookSwapChain;
	D3D11_VIEWPORT m_gViewport;
	inline static UINT m_gVps = 1;
public:
	ImFontConfig g_mFontConfig;
public:
	bool m_bOpenMenu = true; // no need to extern it, only used in Backend.cpp
};

extern Backend RunBackend;