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

        // Create a vector of valid processes for calculation
        std::vector<Process*> validProcesses;
        for (auto& process : processes) {
            if (process.arrivalTime >= 0 && process.burstTime > 0) {
                validProcesses.push_back(&process);
            }
        }

        if (validProcesses.empty()) return;

        // Sort valid processes by arrival time
        std::sort(validProcesses.begin(), validProcesses.end(), 
            [](const Process* a, const Process* b) {
                return a->arrivalTime < b->arrivalTime;
            });

        int currentTime = validProcesses[0]->arrivalTime;

        for (auto* process : validProcesses) {
            // If there's a gap between current time and next process arrival,
            // move current time to the next process arrival
            if (currentTime < process->arrivalTime) {
                currentTime = process->arrivalTime;
            }

            process->responseTime = currentTime - process->arrivalTime;
            process->waitingTime = process->responseTime;
            process->completionTime = currentTime + process->burstTime;
            process->turnAroundTime = process->completionTime - process->arrivalTime;

            currentTime += process->burstTime;
        }

        // Reset metrics for invalid processes
        for (auto& process : processes) {
            if (process.arrivalTime < 0 || process.burstTime <= 0) {
                process.responseTime = 0;
                process.waitingTime = 0;
                process.completionTime = 0;
                process.turnAroundTime = 0;
            }
        }
    }

    std::string getResultSummary() const {
        if (processes.empty()) return "No processes to display.";

        float avgWaitingTime = 0;
        float avgTurnaroundTime = 0;
        float avgResponseTime = 0;
        int validProcesses = 0;

        // Only calculate averages for valid processes
        for (const auto& process : processes) {
            if (process.arrivalTime >= 0 && process.burstTime > 0) {
                avgWaitingTime += process.waitingTime;
                avgTurnaroundTime += process.turnAroundTime;
                avgResponseTime += process.responseTime;
                validProcesses++;
            }
        }

        std::stringstream ss;
        ss << "Results:\r\n\r\n";
        
        if (validProcesses > 0) {
            ss << "Average Waiting Time: " << avgWaitingTime/validProcesses << "\r\n";
            ss << "Average Turnaround Time: " << avgTurnaroundTime/validProcesses << "\r\n";
            ss << "Average Response Time: " << avgResponseTime/validProcesses << "\r\n\r\n";
        } else {
            ss << "No valid processes for calculation.\r\n\r\n";
        }
        
        ss << "Process Details:\r\n";
        for (const auto& process : processes) {
            ss << "Process " << process.processName << ":\r\n";
            
            // Check if process has invalid values
            if (process.arrivalTime < 0 || process.burstTime <= 0) {
                ss << "Status: INVALID INPUT\r\n";
                ss << "Arrival Time: " << process.arrivalTime;
                if (process.arrivalTime < 0) ss << " (cannot be negative)";
                ss << "\r\n";
                ss << "Burst Time: " << process.burstTime;
                if (process.burstTime <= 0) ss << " (must be positive)";
                ss << "\r\n";
                ss << "Cannot calculate metrics for invalid input.\r\n\r\n";
            } else {
                ss << "Status: VALID\r\n";
                ss << "Arrival Time: " << process.arrivalTime << "\r\n";
                ss << "Burst Time: " << process.burstTime << "\r\n";
                ss << "Completion Time: " << process.completionTime << "\r\n";
                ss << "Waiting Time: " << process.waitingTime << "\r\n";
                ss << "Turnaround Time: " << process.turnAroundTime << "\r\n";
                ss << "Response Time: " << process.responseTime << "\r\n\r\n";
            }
        }

        return ss.str();
    }
};

#endif // FCFS_SCHEDULER_H 