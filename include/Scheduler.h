#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <vector>
#include <set>
#include "Process.h"

class Scheduler {
private:
    int current_time;
    std::vector<Process> arrival_pool;
    std::set<Process, CompareVRuntime> ready_queue;

    // Helper method to move newly arrived processes into the RBT ready queue
    void check_new_arrivals();

public:
    // Constructor initializes system clock to 0
    Scheduler();

    // Add a process to the simulation before it kicks off
    void add_process(const Process& p);

    // The main execution loop of the scheduler
    void run_simulation();
    
};

#endif