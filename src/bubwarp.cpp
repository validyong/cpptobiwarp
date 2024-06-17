#include "tobii_gameintegration.h"
#include <iostream>
#include "windows.h"
#include <thread>
#include <gdiplus.h>

#pragma comment(lib, "Gdiplus.lib")

using namespace TobiiGameIntegration;
using namespace Gdiplus;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void CreateOverlayWindow(HINSTANCE hInstance, HWND &hwnd)
{
    const wchar_t CLASS_NAME[] = L"OverlayWindowClass";

    WNDCLASSW wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    RegisterClassW(&wc);

    hwnd = CreateWindowExW(
        WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_NOACTIVATE,
        CLASS_NAME,
        L"Overlay",
        WS_POPUP,
        0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
        NULL,
        NULL,
        hInstance,
        NULL);

    // ウィンドウの背景を完全に透明に設定
    SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 0, LWA_COLORKEY);
    ShowWindow(hwnd, SW_SHOW);
}

void DrawGazeBubble(HDC hdc, int x, int y)
{
    // 背景をクリア
    PatBlt(hdc, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), BLACKNESS);

    // GDI+の初期化
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    {
        Graphics graphics(hdc);
        graphics.SetSmoothingMode(SmoothingModeAntiAlias);

        // グラデーションペンを作成
        GraphicsPath path;
        int radius = 30; // 円の半径を30に設定
        path.AddEllipse(x - radius, y - radius, radius * 2, radius * 2);

        PathGradientBrush brush(&path);
        Color centerColor(0, 0, 0, 0);     // 中心は透明
        Color surroundColor(20, 0, 0, 60); // 縁は薄い半透明の青
        brush.SetCenterColor(centerColor);
        int count = 1;
        brush.SetSurroundColors(&surroundColor, &count);

        // 円の縁を描画
        Pen pen(&brush, 10); // ペンの幅を10に設定
        graphics.DrawEllipse(&pen, x - radius, y - radius, radius * 2, radius * 2);
    }

    // GDI+のクリーンアップ
    GdiplusShutdown(gdiplusToken);
}

void GazeSample()
{
    ITobiiGameIntegrationApi *api = GetApi("Gaze Sample");
    IStreamsProvider *streamsProvider = api->GetStreamsProvider();

    // メインモニタの解像度とオフセットを取得
    HMONITOR hMonitor = MonitorFromWindow(GetDesktopWindow(), MONITOR_DEFAULTTOPRIMARY);
    MONITORINFO monitorInfo;
    monitorInfo.cbSize = sizeof(MONITORINFO);
    GetMonitorInfo(hMonitor, &monitorInfo);
    int screenWidth = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
    int screenHeight = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;
    int offsetX = monitorInfo.rcMonitor.left;
    int offsetY = monitorInfo.rcMonitor.top;

    std::cout << "Main monitor resolution: [" << screenWidth << ", " << screenHeight << "]" << std::endl;
    std::cout << "Main monitor offset: [" << offsetX << ", " << offsetY << "]" << std::endl;

    // トラッキング範囲をメインモニタの解像度に設定
    api->GetTrackerController()->TrackRectangle({0, 0, screenWidth, screenHeight});

    HWND hwnd;
    CreateOverlayWindow(GetModuleHandle(NULL), hwnd);
    HDC hdc = GetDC(hwnd);

    while (true)
    {
        api->Update();

        GazePoint gazePoint;
        if (streamsProvider->GetLatestGazePoint(gazePoint))
        {
            // 視線座標をピクセル座標に変換
            int cursorX = static_cast<int>((gazePoint.X + 1) / 2 * screenWidth) + offsetX;
            int cursorY = static_cast<int>((1 - gazePoint.Y) / 2 * screenHeight) + offsetY;
            if (GetAsyncKeyState(VK_XBUTTON2) & 0x8000) // スペースキーが押されたら
            {
                std::cout << "Gaze point: [" << gazePoint.X << ", " << gazePoint.Y << "]" << std::endl;
                // 視線座標をピクセル座標に変換
                // Y座標の変換を修正
                SetCursorPos(cursorX, cursorY);
                std::cout << "Cursor moved to: [" << cursorX << ", " << cursorY << "]" << std::endl;
            }

            // 視線位置にバブルを描画
            DrawGazeBubble(hdc, cursorX, cursorY);
        }

        Sleep(1000 / 144); // 144Hzに対応
    }

    ReleaseDC(hwnd, hdc);
    api->Shutdown();
}

int main()
{
    GazeSample();
    return 0;
}