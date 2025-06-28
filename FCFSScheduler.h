#ifndef FCFS_SCHEDULER_H
#define FCFS_SCHEDULER_H

#include <vector>
#include <sstream>
#include <algorithm>
#include "Process.h"

class FCFSScheduler {
private:
    std::vector<Process> processes;
    const int MAX_PROCESSES = 10;

public:
    FCFSScheduler() = default;

    bool addProcess(const std::string& name, int arrival, int burst) {
        if (processes.size() >= MAX_PROCESSES) return false;
        processes.emplace_back(name, arrival, burst);
        return true;
    }

    void clearProcesses() {
        processes.clear();
    }

    const std::vector<Process>& getProcesses() const {
        return processes;
    }

    std::vector<Process>& getProcessesRef() {
        return processes;
    }

    int getMaxProcesses() const {
        return MAX_PROCESSES;
    }

    void calculateMetrics() {
        if (processes.empty()) return;

        // Sort processes by arrival time (Bubble Sort)
        for(size_t i = 0; i < processes.size() - 1; i++) {
            for(size_t j = i + 1; j < processes.size(); j++) {
                if(processes[j].arrivalTime < processes[i].arrivalTime) {
                    std::swap(processes[j], processes[i]);
                }
            }
        }

        // Initialize currentTime with the first process's arrival time
        int currentTime = processes[0].arrivalTime;

        for (auto& process : processes) {
            if (currentTime < process.arrivalTime) {
                currentTime = process.arrivalTime;
            }

            process.responseTime = currentTime - process.arrivalTime;
            process.waitingTime = process.responseTime;
            process.completionTime = currentTime + process.burstTime;
            process.turnAroundTime = process.completionTime - process.arrivalTime;

            currentTime += process.burstTime;
        }
    }

    std::string getResultSummary() const {
        if (processes.empty()) return "No processes to display.";

        float avgWaitingTime = 0;
        float avgTurnaroundTime = 0;
        float avgResponseTime = 0;

        for (const auto& process : processes) {
            avgWaitingTime += process.waitingTime;
            avgTurnaroundTime += process.turnAroundTime;
            avgResponseTime += process.responseTime;
        }

        std::stringstream ss;
        ss << "Results:\r\n\r\n";
        ss << "Average Waiting Time: " << avgWaitingTime/processes.size() << "\r\n";
        ss << "Average Turnaround Time: " << avgTurnaroundTime/processes.size() << "\r\n";
        ss << "Average Response Time: " << avgResponseTime/processes.size() << "\r\n\r\n";
        
        ss << "Process Details:\r\n";
        for (const auto& process : processes) {
            ss << "Process " << process.processName << ":\r\n";
            ss << "Arrival Time: " << process.arrivalTime << "\r\n";
            ss << "Burst Time: " << process.burstTime << "\r\n";
            ss << "Completion Time: " << process.completionTime << "\r\n";
            ss << "Waiting Time: " << process.waitingTime << "\r\n";
            ss << "Turnaround Time: " << process.turnAroundTime << "\r\n";
            ss << "Response Time: " << process.responseTime << "\r\n\r\n";
        }

        return ss.str();
    }
};

#endif // FCFS_SCHEDULER_H 