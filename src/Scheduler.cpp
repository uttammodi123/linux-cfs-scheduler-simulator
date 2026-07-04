#include "../include/Scheduler.h"
#include <iostream>
#include <algorithm> // Required for std::erase_if
#include <iomanip> // For setprecision

// Constructor: Initialize the system clock to 0
Scheduler::Scheduler() {
    current_time = 0;
    ready_queue_total_weight = 0;
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
            ready_queue_total_weight += p.get_weight();
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

int Scheduler::calculate_time_slice(const Process& current_proc) const {
    // Total system weight is the tree weight plus our active process weight 
    // (since it will be popped off right before this is called)
    int total_system_weight = ready_queue_total_weight + current_proc.get_weight();

    double share = (double)current_proc.get_weight() / total_system_weight;
    int time_slice = (int)(TARGET_LATENCY * share);

    return std::max(1, time_slice);
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
            ready_queue_total_weight -= current_job.get_weight();
            
            // ERASE: Remove it from the tree so we can safely update its values
            ready_queue.erase(ready_queue.begin());

            // Calculate its dynamic time slice allocation
            int allocated_slice = calculate_time_slice(current_job);

            
            std::cout << "[Time " << current_time << "] CPU Selected Process " << current_job.get_pid() 
                      << " (vruntime: " << std::setprecision(4) << current_job.get_vruntime() 
                      << "). Calculated Time Slice: " << allocated_slice << " Ticks.\n";

            
            // Start the Inner Execution Loop up to allocated_slice ticks
            for (int tick = 0; tick < allocated_slice; ++tick) {
                // Check if anyone arrived *during* this step execution
                check_new_arrivals();

                // PREEMPTION CHECK
                // See if an incoming or waiting process now has a lower vruntime than our active process
                if (!ready_queue.empty() && ready_queue.begin()->get_vruntime() < current_job.get_vruntime()) {
                    std::cout << "   >>> [PREEMPTION EVENT] Process " << ready_queue.begin()->get_pid() 
                              << " (vruntime: " << ready_queue.begin()->get_vruntime() 
                              << ") preempts current Process " << current_job.get_pid() 
                              << " (vruntime: " << current_job.get_vruntime() << ") <<<\n";
                    break; // Terminate inner loop early
                }

                std::cout << "[Tick " << current_time << "] CPU running Process " << current_job.get_pid() 
                          << " (Slice Progress: " << tick + 1 << "/" << allocated_slice << ")\n";

                // Execute 1 tick of work
                current_job.execute_one_tick();
                current_job.update_vruntime(1);

                // Check if the process completed its total work early
                if (current_job.get_remaining_time() == 0) {
                    std::cout << ">>> [Time " << current_time + 1 << "] Process " 
                              << current_job.get_pid() << " has finished executing! <<<\n";
                    break; // Terminate inner loop early
                }

                current_time++; // Push the system clock forward 1 unit
                std::cout << "------------------------------------------------------------------------\n";
            }

            // RE-INSERT: If the process still needs execution time, put it back in the RBT
            if (current_job.get_remaining_time() > 0) {
                ready_queue.insert(current_job);
                ready_queue_total_weight += current_job.get_weight();
            } else {
                current_time++; // Catch up the master clock if it broke out on completion
            }
        } else {
            std::cout << "[Time " << current_time << "] CPU Idle. Waiting for tasks...\n";
            current_time++;
            std::cout << "------------------------------------------------------------------------\n";
        }
    }

    std::cout << "Simulation Finished Successfully!\n";
}







/*
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
*/