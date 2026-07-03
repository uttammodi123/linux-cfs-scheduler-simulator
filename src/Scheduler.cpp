#include "../include/Scheduler.h"
#include <iostream>
#include <algorithm> // Required for std::erase_if

// Constructor: Initialize the system clock to 0
Scheduler::Scheduler() {
    current_time = 0;
}

// Add a process to our arrival pool before execution begins
void Scheduler::add_process(const Process& p) {
    arrival_pool.push_back(p);
}

void Scheduler::check_new_arrivals() {
    // 1. Move arriving processes to the ready queue first
    for (const auto& p : arrival_pool) {
        if (p.get_arrival_time() == current_time) {
            ready_queue.insert(p);
            std::cout << "[Time " << current_time << "] Process " 
                      << p.get_pid() << " arrived. Added to Ready Queue.\n";
        }
    }

    // 2. Erase them cleanly from the vector using standard remove_if
    arrival_pool.erase(
        std::remove_if(arrival_pool.begin(), arrival_pool.end(), [this](const Process& p) {
            return p.get_arrival_time() == current_time;
        }),
        arrival_pool.end()
    );
}

// The core engine clock loop
void Scheduler::run_simulation(){
    std::cout << "Starting Linux CFS Simulator Engine...\n";
    std::cout << "--------------------------------------------\n";

    while (!arrival_pool.empty() || !ready_queue.empty()) {
        
        // 1. Check and migrate newly arrived tasks at this clock tick
        check_new_arrivals();

        // 2. CPU Execution Step
        if (!ready_queue.empty()) {
            // EXTRACT: Fetch the process with the absolute lowest vruntime (front of the set)
            Process current_job = *ready_queue.begin();
            
            // ERASE: Remove it from the tree so we can safely update its values
            ready_queue.erase(ready_queue.begin());

            std::cout << "[Time " << current_time << "] Running Process " << current_job.get_pid() 
                      << " (vruntime: " << current_job.get_vruntime() 
                      << ", RemTime: " << current_job.get_remaining_time() << ")\n";

            // MODIFY: Execute the task for 1 time unit and recalculate its virtual runtime
            current_job.execute_one_tick();
            current_job.update_vruntime(1); // delta_exec = 1 tick

            // RE-INSERT: If the process still needs execution time, put it back in the RBT
            if (current_job.get_remaining_time() > 0) {
                ready_queue.insert(current_job);
            } else {
                std::cout << ">>> [Time " << (current_time + 1) << "] Process " 
                          << current_job.get_pid() << " has finished executing! <<<\n";
            }
        } else {
            std::cout << "[Time " << current_time << "] CPU Idle. Waiting for tasks...\n";
        }

        // Advance system clock
        current_time++;
        
        std::cout << "--------------------------------------------\n";
    }

    std::cout << "Simulation Finished Successfully!\n";
}