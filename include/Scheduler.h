#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <vector>
#include <set>
#include "Process.h"

class Scheduler {
private:
    int current_time;

    const int TARGET_LATENCY = 20; // Every process gets a turn within a 20-tick window

    std::vector<Process> arrival_pool;
    std::set<Process, CompareVRuntime> ready_queue;

    // Helper method to move newly arrived processes into the RBT ready queue
    void check_new_arrivals();

    int last_running_pid;                         // Remembers who used the CPU last tick
    int total_context_switches;                   // Counts total context switch occurrences
    std::vector<Process> completed_processes;     // Storage container for retired tasks

    //Helper to compute dynamic execution window (Delta t)
    int calculate_time_slice(const Process& current_proc) const;

    int ready_queue_total_weight; //optimized running weight tracker!

    void print_final_metrics() const;

public:
    // Constructor initializes system clock to 0
    Scheduler();

    // Add a process to the simulation before it kicks off
    void add_process(const Process& p);

    // The main execution loop of the scheduler
    void run_simulation();
    
};

#endif