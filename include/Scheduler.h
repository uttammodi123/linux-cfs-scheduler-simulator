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

    //Helper to compute dynamic execution window (Delta t)
    int calculate_time_slice(const Process& current_proc) const;

    int ready_queue_total_weight; //optimized running weight tracker!

public:
    // Constructor initializes system clock to 0
    Scheduler();

    // Add a process to the simulation before it kicks off
    void add_process(const Process& p);

    // The main execution loop of the scheduler
    void run_simulation();
    
};

#endif