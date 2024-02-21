#include "../Backend/Backend.h"
#include "../../Cheat/Functions/Functions.h"

Backend::presentVariable originalPresent;
Backend::presentVariable hookedPresent;

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static bool init = false;

Backend RunBackend;

bool Backend::DirectXPresentHook()
{
	ZeroMemory(&m_gSwapChainDescription, sizeof(m_gSwapChainDescription));

	m_gSwapChainDescription.BufferCount = 2;
	m_gSwapChainDescription.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	m_gSwapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	m_gSwapChainDescription.OutputWindow = GetForegroundWindow();
	m_gSwapChainDescription.SampleDesc.Count = 1;
	m_gSwapChainDescription.Windowed = TRUE;
	m_gSwapChainDescription.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	HRESULT createDevice = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, m_gFeatureLevels, 2, D3D11_SDK_VERSION, &m_gSwapChainDescription, &m_gSwapChain, &m_gDevice, nullptr, nullptr);
		
	if (FAILED(createDevice)) 
		return false; // dont return false make an endless cycle (only if u wanna go cpu boom) 

	void** DX11Vtable = *reinterpret_cast<void***>(m_gSwapChain);

	UnloadDevices(false); // don't need to reset mainrendertargetview
	hookedPresent = (Backend::presentVariable)DX11Vtable[8]; // 8. virtual table is present

	return true;
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	if (RunBackend.m_bOpenMenu && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam)) // if menu open then handle imgui events
		return true;

	return CallWindowProc(RunBackend.m_goriginalWndProc, hWnd, uMsg, wParam, lParam);
}

void Backend::LoadImGui(HWND window, ID3D11Device* device, ID3D11DeviceContext* context)
{
	ImGui::CreateContext(); // creating the context cus we need imgui
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange; // dont change cursors
	ImGui_ImplWin32_Init(window); // which window u wanna draw your imgui huh???
	ImGui_ImplDX11_Init(device, context); // u need the device's context since u can't draw with only device, thanx dx11
} // loading the imgui

void Backend::DrawImGui(ID3D11DeviceContext* context, ID3D11RenderTargetView* targetview)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	if (m_bOpenMenu)
	{
		ImGui::Begin("DirectX11 Hook", &m_bOpenMenu);
		{ // not needed, but better visually
			ImGui::Text("https://github.com/zzzmate - DX11 Hook");
			ImGui::Checkbox("Camera FOV", &Variables::EnableCamera);
			if(Variables::EnableCamera)
				ImGui::SliderFloat("##CameraFOV", &Variables::CameraFov, 20, 180, "Camera FOV: %.0f");
			ImGui::Checkbox("Circle FOV", &Variables::EnableCircleFov);
			if (Variables::EnableCircleFov)
			{
				ImGui::SliderFloat("##CircleFOV", &Variables::CircleFov, 20, 180, "Circle FOV: %.0f");
				ImGui::Checkbox("Rainbow?", &Variables::EnableRainbow);
			}
			ImGui::Checkbox("Watermark", &Variables::EnableWatermark);
			if (Variables::EnableWatermark)
				ImGui::Checkbox("FPS?", &Variables::EnableFPS);
			ImGui::Checkbox("Enable Snaplines", &Variables::EnableSnaplines);
			if (Variables::EnableSnaplines)
			{
				ImGui::SameLine();
				ImGui::ColorEdit3("##SnaplineColor", (float*)&Variables::PlayerSnaplineColor, ImGuiColorEditFlags_NoDragDrop | ImGuiColorEditFlags_NoInputs);
				ImGui::Checkbox("Rainbow", &Variables::EnableRainbowSnaplines);
				ImGui::SliderInt("##LineType", &Variables::LineTypes, 1, 2, "Line Type: %i");
			}
		}
		ImGui::End();
	}

	RunBackend.RenderCheat();
	ImGui::EndFrame();
	ImGui::Render();
	context->OMSetRenderTargets(1, &targetview, NULL);  // 1 render target, render it to our monitor, no dsv
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData()); // drawing the imgui menu
}

void Backend::UnloadImGui()
{
	MH_DisableHook(hookedPresent); 
	MH_RemoveHook(hookedPresent);
	MH_Uninitialize();

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}

void Backend::UnloadDevices(bool renderTargetViewReset)
{
	if(renderTargetViewReset) if (m_gMainRenderTargetView) { m_gMainRenderTargetView->Release(); m_gMainRenderTargetView = nullptr; }
	if (m_gPointerContext) { m_gPointerContext->Release(); m_gPointerContext = nullptr; }
	if (m_gDevice) { m_gDevice->Release(); m_gDevice = nullptr; }
	SetWindowLongPtr(m_gWindow, GWLP_WNDPROC, (LONG_PTR)(m_goriginalWndProc));
}

void Backend::RenderCheat()
{
	if (Variables::EnableCamera)
	{
		//Unity::CCamera* CameraMain = Unity::Camera::GetMain();
		auto CameraMain = GameFunctions::GetUnityCamera();

		if (CameraMain != nullptr)
			CameraMain->CallMethodSafe<void*>("set_fieldOfView", Variables::CameraFov);
	}

	if (Variables::EnableCircleFov && Variables::EnableRainbow)
		Utils::UseFov(true);
	else if (Variables::EnableCircleFov && !Variables::EnableRainbow)
		Utils::UseFov(false);

	if (Variables::EnableWatermark && Variables::EnableFPS)
		Utils::Watermark("@zzzmate", true); // text to set the watermark, and true/false to show fps
	else if(Variables::EnableWatermark && !Variables::EnableFPS)
		Utils::Watermark("@zzzmate", false); // text to set the watermark, and true/false to show fps
	
	if (Utils::PlayerList.size() > 0) // this is a player list loop, so anything that related to multiple players put here
	{
		for (int i = 0; i < Utils::PlayerList.size(); i++)
		{
			if (!Utils::PlayerList[i]) continue;

			auto PlayerPosition = Utils::PlayerList[i]->GetTransform()->GetPosition();

			if (Variables::EnableSnaplines)
			{
				Unity::Vector3 RealPlayerPos = PlayerPosition;
				RealPlayerPos.y -= 0.2f; // not needed to convert it again
				Vector2 ScreenPosition;
				if (Utils::World2Screen(RealPlayerPos, ScreenPosition)) {
					ImColor SnapLineColor;
					if (Variables::EnableRainbowSnaplines)
						SnapLineColor = ImColor(Variables::RainbowColor.x, Variables::RainbowColor.y, Variables::RainbowColor.z, Variables::RainbowColor.w);
					else
						SnapLineColor = Variables::PlayerSnaplineColor;

					switch (Variables::LineTypes)
					{
					case 1:
						ImGui::GetBackgroundDrawList()->AddLine(ImVec2(Variables::ScreenCenter.x, Variables::ScreenSize.y), ImVec2(ScreenPosition.x, ScreenPosition.y), SnapLineColor, 1.5f);
						break;
					case 2:
						ImGui::GetBackgroundDrawList()->AddLine(ImVec2(Variables::ScreenCenter.x, Variables::ScreenCenter.y), ImVec2(ScreenPosition.x, ScreenPosition.y), SnapLineColor, 1.5f);
						break;
					}
				}
			}
		}
	}
}	

static long __stdcall PresentHook(IDXGISwapChain* pointerSwapChain, UINT sync, UINT flags)
{
	if (!init) {
		if (SUCCEEDED(pointerSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&RunBackend.m_gDevice))) // check if device working 
		{
			RunBackend.m_gDevice->GetImmediateContext(&RunBackend.m_gPointerContext); // need context immediately!!
			pointerSwapChain->GetDesc(&RunBackend.m_gPresentHookSwapChain); // welp we need the presenthook's outputwindow so it's actually ours o_o
			RunBackend.m_gWindow = RunBackend.m_gPresentHookSwapChain.OutputWindow;

			pointerSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&RunBackend.m_gPointerBackBuffer); // getting back buffer
			if (RunBackend.m_gPointerBackBuffer != nullptr) RunBackend.m_gDevice->CreateRenderTargetView(RunBackend.m_gPointerBackBuffer, NULL, &RunBackend.m_gMainRenderTargetView); // from backbuffer to our monitor
			RunBackend.m_gPointerBackBuffer->Release(); // don't need this shit anymore, but please comeback the next injection

			RunBackend.LoadImGui(RunBackend.m_gWindow, RunBackend.m_gDevice, RunBackend.m_gPointerContext); // load imgui!!!
			RunBackend.m_goriginalWndProc = (WNDPROC)SetWindowLongPtr(RunBackend.m_gWindow, GWLP_WNDPROC, (LONG_PTR)WndProc); // i think u need this

			RunBackend.m_gPointerContext->RSGetViewports(&RunBackend.m_gVps, &RunBackend.m_gViewport);
			Variables::ScreenSize = { RunBackend.m_gViewport.Width, RunBackend.m_gViewport.Height };
			Variables::ScreenCenter = { RunBackend.m_gViewport.Width / 2.0f, RunBackend.m_gViewport.Height / 2.0f };

			ImGui::GetIO().Fonts->AddFontDefault();
			RunBackend.g_mFontConfig.GlyphExtraSpacing.x = 1.2;
			BaseFonts::GameFont = ImGui::GetIO().Fonts->AddFontFromMemoryTTF(BaseFonts::TTSquaresCondensedBold, 14, 14, &RunBackend.g_mFontConfig);
			ImGui::GetIO().Fonts->AddFontDefault();

			init = true;
		}
		else
			return originalPresent(pointerSwapChain, sync, flags); // returning original too
	}

	if (Utils::KeyPressed(VK_INSERT))
		RunBackend.m_bOpenMenu = !RunBackend.m_bOpenMenu;

	static float isRed = 0.0f, isGreen = 0.01f, isBlue = 0.0f;
	int FrameCount = ImGui::GetFrameCount();

	if (isGreen == 0.01f && isBlue == 0.0f) isRed += 0.01f;
	if (isRed > 0.99f && isBlue == 0.0f) {isRed = 1.0f; isGreen += 0.01f; }
	if (isGreen > 0.99f && isBlue == 0.0f) { isGreen = 1.0f; isRed -= 0.01f; }
	if (isRed < 0.01f && isGreen == 1.0f){ isRed = 0.0f; isBlue += 0.01f; }
	if (isBlue > 0.99f && isRed == 0.0f) { isBlue = 1.0f; isGreen -= 0.01f; } // ugliest function ive ever seen
	if (isGreen < 0.01f && isBlue == 1.0f) { isGreen = 0.0f; isRed += 0.01f; }
	if (isRed > 0.99f && isGreen == 0.0f) { isRed = 1.0f; isBlue -= 0.01f; }
	if (isBlue < 0.01f && isGreen == 0.0f) { isBlue = 0.0f; isRed -= 0.01f;
		if (isRed < 0.01f) isGreen = 0.01f; }

	Variables::RainbowColor = ImVec4(isRed, isGreen, isBlue, 1.0f);

	RunBackend.DrawImGui(RunBackend.m_gPointerContext, RunBackend.m_gMainRenderTargetView); // draw imgui every time
	return originalPresent(pointerSwapChain, sync, flags); // return the original so no stack corruption
}

bool Backend::Load()
{
	RunBackend.DirectXPresentHook(); // this always okay if game directx11
	MH_Initialize(); // aint no error checking cuz if minhook bad then its your problem 

	MH_CreateHook(reinterpret_cast<void**>(hookedPresent), &PresentHook, reinterpret_cast<void**>(&originalPresent)); 
	MH_EnableHook(hookedPresent); // hooking present

	return true;
}

void Backend::Unload()
{
	UnloadImGui(); // imgui unload
	UnloadDevices(true); // unloading all devices
}