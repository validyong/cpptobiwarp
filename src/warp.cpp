#include "tobii_gameintegration.h"
#include <iostream>
#include "windows.h"
#include <thread>

using namespace TobiiGameIntegration;

void GazeSample()
{
    // ITobiiGameIntegrationApi *api = GetApi("Gaze Sample");
    // IStreamsProvider *streamsProvider = api->GetStreamsProvider();
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

    while (true)
    {
        api->Update();

        GazePoint gazePoint;
        if (streamsProvider->GetLatestGazePoint(gazePoint))
        {

            // check your button
            // for (int vkCode = 0x01; vkCode <= 0xFF; ++vkCode)
            // {
            //     if (GetAsyncKeyState(vkCode) & 0x8000)
            //     {
            //         std::cout << "Button pressed: VK_CODE = " << std::hex << vkCode << std::endl;
            //     }
            // }

            if (GetAsyncKeyState(0x24) & 0x8000)
            {
                // std::cout << "Gaze point: [" << gazePoint.X << ", " << gazePoint.Y << "]" << std::endl;
                // Converts line-of-sight coordinates to pixel coordinates
                int cursorX = static_cast<int>((gazePoint.X + 1) / 2 * screenWidth) + offsetX;
                int cursorY = static_cast<int>((1 - gazePoint.Y) / 2 * screenHeight) + offsetY; // Y座標の変換を修正
                SetCursorPos(cursorX, cursorY);
                // std::cout << "Cursor moved to: [" << cursorX << ", " << cursorY << "]" << std::endl;
            }
        }

        // Alt + F4 to exit the programme
        if ((GetAsyncKeyState(VK_MENU) & 0x8000) && (GetAsyncKeyState(VK_F4) & 0x8000))
        {
            std::cout << "Alt + F4 detected. Exiting program." << std::endl;
            break;
        }

        Sleep(1000 / 144);
    }

    // api->Shutdown();
}

int main()
{
    GazeSample();
    return 0;
}