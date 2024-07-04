#include "tobii_gameintegration.h"
#include <iostream>
#include "windows.h"
#include <thread>
#include <memory>

using namespace TobiiGameIntegration;

void GazeSample()
{
    std::unique_ptr<ITobiiGameIntegrationApi, void (*)(ITobiiGameIntegrationApi *)> api(GetApi("Gaze Sample"), [](ITobiiGameIntegrationApi *p)
                                                                                        { p->Shutdown(); });
    IStreamsProvider *streamsProvider = api->GetStreamsProvider();

    // Get the resolution and offset of the main monitor
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

    // Set the tracking range to the resolution of the main monitor
    api->GetTrackerController()->TrackRectangle({0, 0, screenWidth, screenHeight});

    POINT lastCursorPos = {0, 0};

    while (true)
    {
        api->Update();

        GazePoint gazePoint;
        if (streamsProvider->GetLatestGazePoint(gazePoint))
        {
            int cursorX = static_cast<int>((gazePoint.X + 1) / 2 * screenWidth) + offsetX;
            int cursorY = static_cast<int>((1 - gazePoint.Y) / 2 * screenHeight) + offsetY;
            lastCursorPos = {cursorX, cursorY};
        }

        // Windowsメッセージループを使用してキーイベントをキャプチャ
        MSG msg = {0};
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_KEYDOWN && msg.wParam == 0x24) // 0x24 is VK_HOME
            {
                SetCursorPos(lastCursorPos.x, lastCursorPos.y);
            }
            if (msg.message == WM_KEYDOWN && msg.wParam == VK_F4 && (GetAsyncKeyState(VK_MENU) & 0x8000))
            {
                std::cout << "Alt + F4 detected. Exiting program." << std::endl;
                return;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Check Home button state using GetAsyncKeyState
        if (GetAsyncKeyState(0x24) & 0x8000) // 0x24 is VK_HOME
        {
            SetCursorPos(lastCursorPos.x, lastCursorPos.y);
        }

        Sleep(1000 / 144);
    }
}

int main()
{
    GazeSample();
    return 0;
}