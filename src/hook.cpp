#define _CRT_SECURE_NO_WARNINGS
#include "../useless.hpp"
#include <windows.h>
#include <string>
#include <vector>
#include "MinHook.h"
#include "imgui.h"
#include "imgui_impl_win32.h"

#ifdef USE_DX8
#include <d3d8.h>
#include "imgui_impl_dx8.h"

typedef IDirect3DDevice8 GM_D3DDEVICE;
typedef D3DPRESENT_PARAMETERS GM_PRESENT_PARAMETERS;
#define HOOK_ENDSCENE 35
#define HOOK_RESET 14
#else
#include <d3d9.h>
#include "imgui_impl_dx9.h"

typedef IDirect3DDevice9 GM_D3DDEVICE;
typedef D3DPRESENT_PARAMETERS GM_PRESENT_PARAMETERS;
#define HOOK_ENDSCENE 42
#define HOOK_RESET 16
#endif

GM_D3DDEVICE** d3d_device = (GM_D3DDEVICE**)0x6886a8;
static bool is_hooked = false;
bool show_menu = true;
static GM_D3DDEVICE* g_pd3dDevice = nullptr;
static HWND hwnd = nullptr;

typedef HRESULT(APIENTRY* EndScene_t)(GM_D3DDEVICE*);
static EndScene_t oEndScene = nullptr;

typedef HRESULT(APIENTRY* Reset_t)(GM_D3DDEVICE*, GM_PRESENT_PARAMETERS*);
static Reset_t oReset = nullptr;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

static WNDPROC oWndProc = nullptr;
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (show_menu) {
        if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
            return true;
    }
    
    return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

bool imgui_initialized = false;
extern bool g_rebuild_fonts;

HRESULT APIENTRY hkEndScene(GM_D3DDEVICE* pDevice) {
    if (!pDevice) return oEndScene(pDevice);

    if (!imgui_initialized) {
        g_pd3dDevice = pDevice;
        
        D3DDEVICE_CREATION_PARAMETERS params;
        if (SUCCEEDED(pDevice->GetCreationParameters(&params))) {
            hwnd = params.hFocusWindow;
        }
        
        if (hwnd) {
            oWndProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)WndProc);
            
            ImGui_ImplWin32_Init(hwnd);
#ifdef USE_DX8
            ImGui_ImplDX8_Init(pDevice);
            ImGui_ImplDX8_NewFrame();
#else
            ImGui_ImplDX9_Init(pDevice);
            ImGui_ImplDX9_NewFrame();
#endif
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();
            
            imgui_initialized = true;
        }
    }
    
    if (imgui_initialized) {
        if (g_rebuild_fonts) {
#ifdef USE_DX8
            ImGui_ImplDX8_InvalidateDeviceObjects();
            ImGui_ImplDX8_CreateDeviceObjects();
#else
            ImGui_ImplDX9_InvalidateDeviceObjects();
            ImGui_ImplDX9_CreateDeviceObjects();
#endif
            g_rebuild_fonts = false;
        }

        if (show_menu) {
#ifdef USE_DX8
            DWORD zEnable, alphaBlend, alphaTest;
            pDevice->GetRenderState(D3DRS_ZENABLE, &zEnable);
            pDevice->GetRenderState(D3DRS_ALPHABLENDENABLE, &alphaBlend);
            pDevice->GetRenderState(D3DRS_ALPHATESTENABLE, &alphaTest);

            ImGui::Render();
            ImGui_ImplDX8_RenderDrawData(ImGui::GetDrawData());
            
            pDevice->SetRenderState(D3DRS_ZENABLE, zEnable);
            pDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, alphaBlend);
            pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, alphaTest);
#else
            DWORD colorWrite, srgbWrite;
            pDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &colorWrite);
            pDevice->GetRenderState(D3DRS_SRGBWRITEENABLE, &srgbWrite);
            
            pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
            pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, false);
            
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            
            pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, colorWrite);
            pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, srgbWrite);
#endif
        } else {
            ImGui::EndFrame();
        }
        
#ifdef USE_DX8
        ImGui_ImplDX8_NewFrame();
#else
        ImGui_ImplDX9_NewFrame();
#endif
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
    }
    
    return oEndScene(pDevice);
}

HRESULT APIENTRY hkReset(GM_D3DDEVICE* pDevice, GM_PRESENT_PARAMETERS* pPresentationParameters) {
    if (imgui_initialized) {
#ifdef USE_DX8
        ImGui_ImplDX8_InvalidateDeviceObjects();
#else
        ImGui_ImplDX9_InvalidateDeviceObjects();
#endif
    }
    HRESULT hr = oReset(pDevice, pPresentationParameters);
    if (SUCCEEDED(hr) && imgui_initialized) {
#ifdef USE_DX8
        ImGui_ImplDX8_CreateDeviceObjects();
#else
        ImGui_ImplDX9_CreateDeviceObjects();
#endif
    }
    return hr;
}

GMREAL im_init() {
    if (is_hooked) return 1.0;
    if (!d3d_device || !*d3d_device) return 0.0;
    
    void** vTable = *reinterpret_cast<void***>(*d3d_device);
    
    if (MH_Initialize() != MH_OK) return 0.0;
    
    // Hook EndScene and Reset
    if (MH_CreateHook(vTable[HOOK_ENDSCENE], (LPVOID)hkEndScene, (LPVOID*)&oEndScene) != MH_OK) return 0.0;
    if (MH_CreateHook(vTable[HOOK_RESET], (LPVOID)hkReset, (LPVOID*)&oReset) != MH_OK) return 0.0;
    
    if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK) return 0.0;
    
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    
    is_hooked = true;
    return 1.0;
}
