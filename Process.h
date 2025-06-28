#ifndef PROCESS_H
#define PROCESS_H

#include <windows.h>
#include <string>

class Process {
public:
    std::string processName;
    int arrivalTime;
    int burstTime;
    int completionTime;
    int waitingTime;
    int turnAroundTime;
    int responseTime;
    COLORREF color;      // Base color for GUI
    float animProgress;  // Animation progress (0.0 to 1.0)
    bool isHovered;      // Hover state for GUI

    Process(const std::string& name, int arrival, int burst) 
        : processName(name), arrivalTime(arrival), burstTime(burst),
          completionTime(0), waitingTime(0), turnAroundTime(0), responseTime(0),
          animProgress(0.0f), isHovered(false) {
        // Assign random color for the process
        color = RGB(rand() % 156 + 100, rand() % 156 + 100, rand() % 156 + 100);
    }
};

#endif // PROCESS_H 