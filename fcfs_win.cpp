#include <windows.h>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>  // Added for std::max
#include <cmath>
#include "FCFSScheduler.h"

// Global variables for GUI
HWND hwndName = NULL;
HWND hwndArrival = NULL;
HWND hwndBurst = NULL;
HWND hwndAdd = NULL;
HWND hwndCalculate = NULL;
HWND hwndClear = NULL;
HWND hwndResult = NULL;
HWND hwndGantt = NULL;
UINT_PTR animationTimer = 0;
FCFSScheduler scheduler;

// Helper function to create enhanced color for hover effect
COLORREF GetEnhancedColor(COLORREF baseColor, bool isHovered) {
    if (!isHovered) return baseColor;
    
    int r = std::min(255, GetRValue(baseColor) + 40);
    int g = std::min(255, GetGValue(baseColor) + 40);
    int b = std::min(255, GetBValue(baseColor) + 40);
    return RGB(r, g, b);
}

void DrawGanttChart(HWND hwnd) {
    const auto& processes = scheduler.getProcesses();
    if (processes.empty()) return;

    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    
    // Get client area dimensions
    RECT rect;
    GetClientRect(hwnd, &rect);
    
    // Gantt chart settings
    int chartTop = 30;
    int chartHeight = 60;
    int blockHeight = 40;
    
    int currentTime = 0;
    int maxTime = 0;
    int firstArrivalTime = processes[0].arrivalTime;

    // Calculate total time and find max burst time
    int maxBurst = 0;
    for (const auto& process : processes) {
        maxTime += process.burstTime;
        maxBurst = std::max(maxBurst, process.burstTime);
    }
    maxTime += firstArrivalTime;

    // Calculate time unit scale based on available width and total time
    int availableWidth = rect.right - rect.left - 100;
    float timeUnit = (maxTime > availableWidth / 40) ? (float)availableWidth / maxTime : 40;

    // Draw time axis with shadow effect
    HPEN shadowPen = CreatePen(PS_SOLID, 1, RGB(180, 180, 180));
    SelectObject(hdc, shadowPen);
    MoveToEx(hdc, 51, chartTop + chartHeight + 1, NULL);
    LineTo(hdc, (int)(timeUnit * maxTime) + 51, chartTop + chartHeight + 1);
    DeleteObject(shadowPen);

    HPEN axisPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));
    SelectObject(hdc, axisPen);
    MoveToEx(hdc, 50, chartTop + chartHeight, NULL);
    LineTo(hdc, (int)(timeUnit * maxTime) + 50, chartTop + chartHeight);
    DeleteObject(axisPen);

    // Create font for labels
    HFONT hFont = CreateFontW(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                            DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                            CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_DONTCARE,
                            L"Arial");
    HFONT hOldFont = (HFONT)SelectObject(hdc, hFont);
    
    // If first process doesn't start at 0, draw idle block
    if (firstArrivalTime > 0) {
        RECT idleRect = {
            50,
            chartTop + 5,
            (int)(firstArrivalTime * timeUnit) + 50,
            chartTop + 5 + blockHeight
        };
        
        HBRUSH idleBrush = CreateSolidBrush(RGB(200, 200, 200));
        FillRect(hdc, &idleRect, idleBrush);
        DeleteObject(idleBrush);

        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, RGB(100, 100, 100));
        TextOutW(hdc, 55, chartTop + 15, L"IDLE", 4);
    }

    // Draw processes
    currentTime = firstArrivalTime;
    int yPos = chartTop + 5;
    
    for (const auto& process : processes) {
        float animWidth = process.burstTime * timeUnit * process.animProgress;
        
        RECT processRect = {
            (int)(currentTime * timeUnit) + 50,
            yPos,
            (int)(currentTime * timeUnit + animWidth) + 50,
            yPos + blockHeight
        };

        COLORREF enhancedColor = GetEnhancedColor(process.color, process.isHovered);
        HBRUSH hBrush = CreateSolidBrush(enhancedColor);
        FillRect(hdc, &processRect, hBrush);
        DeleteObject(hBrush);

        SetBkMode(hdc, TRANSPARENT);
        std::wstring wtext(process.processName.begin(), process.processName.end());
        
        SetTextColor(hdc, RGB(100, 100, 100));
        TextOutW(hdc, (int)(currentTime * timeUnit) + 56, yPos + 6, 
                wtext.c_str(), wtext.length());
        
        SetTextColor(hdc, RGB(0, 0, 0));
        TextOutW(hdc, (int)(currentTime * timeUnit) + 55, yPos + 5, 
                wtext.c_str(), wtext.length());

        std::wstring timeStr = std::to_wstring(currentTime);
        TextOutW(hdc, (int)(currentTime * timeUnit) + 45, chartTop + chartHeight + 5,
                timeStr.c_str(), timeStr.length());

        currentTime += process.burstTime;
    }

    // Draw final time marker
    std::wstring finalTime = std::to_wstring(currentTime);
    TextOutW(hdc, (int)(currentTime * timeUnit) + 45, chartTop + chartHeight + 5,
            finalTime.c_str(), finalTime.length());

    // Draw scale information
    if (timeUnit != 40) {
        std::wstring scaleInfo = L"Scale: 1 unit = " + std::to_wstring((int)(1000/timeUnit)/1000.0) + L" pixels";
        SetTextColor(hdc, RGB(100, 100, 100));
        TextOutW(hdc, 51, 11, scaleInfo.c_str(), scaleInfo.length());
        SetTextColor(hdc, RGB(0, 0, 0));
        TextOutW(hdc, 50, 10, scaleInfo.c_str(), scaleInfo.length());
    }

    SelectObject(hdc, hOldFont);
    DeleteObject(hFont);
    EndPaint(hwnd, &ps);
}

void CALLBACK AnimationTimerProc(HWND hwnd, UINT msg, UINT_PTR id, DWORD time) {
    bool needsUpdate = false;
    auto& processes = scheduler.getProcessesRef();
    
    for (auto& process : processes) {
        if (process.animProgress < 1.0f) {
            process.animProgress = std::min(1.0f, process.animProgress + 0.1f);
            needsUpdate = true;
        }
    }
    
    if (needsUpdate) {
        InvalidateRect(hwndGantt, NULL, TRUE);
    } else {
        KillTimer(hwnd, animationTimer);
        animationTimer = 0;
    }
}

LRESULT CALLBACK GanttWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch(msg) {
        case WM_PAINT:
            DrawGanttChart(hwnd);
            break;
            
        case WM_MOUSEMOVE: {
            POINT pt = {LOWORD(lParam), HIWORD(lParam)};
            bool updateNeeded = false;
            
            int currentTime = 0;
            float timeUnit = 40;
            auto& processes = scheduler.getProcessesRef();
            
            for (auto& process : processes) {
                RECT processRect = {
                    (int)(currentTime * timeUnit) + 50,
                    35,
                    (int)((currentTime + process.burstTime) * timeUnit) + 50,
                    75
                };
                
                bool newHoverState = PtInRect(&processRect, pt);
                if (newHoverState != process.isHovered) {
                    process.isHovered = newHoverState;
                    updateNeeded = true;
                }
                
                currentTime += process.burstTime;
            }
            
            if (updateNeeded) {
                InvalidateRect(hwnd, NULL, TRUE);
            }
            break;
        }
            
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch(msg) {
        case WM_CREATE: {
            // Create input fields
            CreateWindowW(L"STATIC", L"Process Name:", 
                WS_VISIBLE | WS_CHILD,
                10, 10, 100, 20, hwnd, NULL, NULL, NULL);
            
            hwndName = CreateWindowW(L"EDIT", L"", 
                WS_VISIBLE | WS_CHILD | WS_BORDER,
                120, 10, 100, 20, hwnd, NULL, NULL, NULL);

            CreateWindowW(L"STATIC", L"Arrival Time:", 
                WS_VISIBLE | WS_CHILD,
                10, 40, 100, 20, hwnd, NULL, NULL, NULL);
            
            hwndArrival = CreateWindowW(L"EDIT", L"", 
                WS_VISIBLE | WS_CHILD | WS_BORDER,
                120, 40, 100, 20, hwnd, NULL, NULL, NULL);

            CreateWindowW(L"STATIC", L"Burst Time:", 
                WS_VISIBLE | WS_CHILD,
                10, 70, 100, 20, hwnd, NULL, NULL, NULL);
            
            hwndBurst = CreateWindowW(L"EDIT", L"", 
                WS_VISIBLE | WS_CHILD | WS_BORDER,
                120, 70, 100, 20, hwnd, NULL, NULL, NULL);

            hwndAdd = CreateWindowW(L"BUTTON", L"Add Process", 
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                10, 100, 100, 30, hwnd, (HMENU)1, NULL, NULL);

            hwndCalculate = CreateWindowW(L"BUTTON", L"Calculate", 
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                120, 100, 100, 30, hwnd, (HMENU)2, NULL, NULL);

            // Add Clear All button
            hwndClear = CreateWindowW(L"BUTTON", L"Clear All", 
                WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON,
                10, 140, 210, 30, hwnd, (HMENU)3, NULL, NULL);

            hwndResult = CreateWindowW(L"EDIT", L"", 
                WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_READONLY | WS_VSCROLL,
                10, 180, 210, 260, hwnd, NULL, NULL, NULL);

            // Register Gantt chart window class
            WNDCLASSEXW wcGantt = {0};
            wcGantt.cbSize = sizeof(WNDCLASSEXW);
            wcGantt.lpfnWndProc = GanttWndProc;
            wcGantt.hInstance = GetModuleHandle(NULL);
            wcGantt.hCursor = LoadCursor(NULL, IDC_ARROW);
            wcGantt.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
            wcGantt.lpszClassName = L"GanttChartClass";
            RegisterClassExW(&wcGantt);

            hwndGantt = CreateWindowW(L"GanttChartClass", NULL,
                WS_VISIBLE | WS_CHILD | WS_BORDER,
                230, 10, 550, 150,
                hwnd, NULL, NULL, NULL);

            break;
        }
        
        case WM_COMMAND: {
            if (LOWORD(wParam) == 1) {  // Add Process button
                char name[256], arrival[256], burst[256];
                GetWindowText(hwndName, name, 256);
                GetWindowText(hwndArrival, arrival, 256);
                GetWindowText(hwndBurst, burst, 256);

                if (strlen(name) > 0 && strlen(arrival) > 0 && strlen(burst) > 0) {
                    int arrivalTime = atoi(arrival);
                    int burstTime = atoi(burst);
                    
                    // Always add the process, even with invalid values
                    if (scheduler.addProcess(name, arrivalTime, burstTime)) {
                        // Clear input fields
                        SetWindowText(hwndName, "");
                        SetWindowText(hwndArrival, "");
                        SetWindowText(hwndBurst, "");
                        
                        // Update process count display
                        std::string status = "Processes added: " + 
                            std::to_string(scheduler.getProcesses().size()) + 
                            "/" + std::to_string(scheduler.getMaxProcesses());
                        SetWindowText(hwndResult, status.c_str());
                        
                        // Redraw Gantt chart
                        InvalidateRect(hwndGantt, NULL, TRUE);
                    }
                }
            }
            else if (LOWORD(wParam) == 2) {  // Calculate button
                if (!scheduler.getProcesses().empty()) {
                    scheduler.calculateMetrics();
                    if (!animationTimer) {
                        animationTimer = SetTimer(hwnd, 1, 50, AnimationTimerProc);
                    }
                    SetWindowText(hwndResult, scheduler.getResultSummary().c_str());
                    InvalidateRect(hwndGantt, NULL, TRUE);
                }
            }
            else if (LOWORD(wParam) == 3) {  // Clear All button
                if (!scheduler.getProcesses().empty()) {
                    if (MessageBoxA(hwnd, "Are you sure you want to clear all processes?", 
                        "Confirm", MB_YESNO | MB_ICONQUESTION) == IDYES) {
                        scheduler.clearProcesses();
                        SetWindowText(hwndResult, "All processes cleared.");
                        InvalidateRect(hwndGantt, NULL, TRUE);
                    }
                } else {
                    MessageBoxA(hwnd, "No processes to clear!", "Info", MB_OK | MB_ICONINFORMATION);
                }
            }
            break;
        }
        
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
            
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
            
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszClassName = L"FCFSSchedulerClass";
    RegisterClassExW(&wc);

    HWND hwnd = CreateWindowW(
        L"FCFSSchedulerClass", L"FCFS CPU Scheduler",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 350,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) return 0;

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
} 