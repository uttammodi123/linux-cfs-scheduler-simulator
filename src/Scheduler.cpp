#include "../include/Scheduler.h"
#include <iostream>
#include <algorithm> // Required for std::erase_if
#include <iomanip> // For setprecision

// Constructor: Initialize the system clock to 0
Scheduler::Scheduler() {
    current_time = 0;
    ready_queue_total_weight = 0;
    last_running_pid = -1;       // -1 means the CPU is currently empty
    total_context_switches = 0;  // Start the penalty counter at 0
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

void Scheduler::print_final_metrics() const {
    std::cout << "\n========================================================================\n";
    std::cout << "                 FINAL CFS SIMULATION PERFORMANCE REPORT                \n";
    std::cout << "========================================================================\n";
    
    std::cout << std::left << std::setw(8) << "PID" 
              << std::setw(15) << "Arrival Time" 
              << std::setw(12) << "Burst Time" 
              << std::setw(18) << "Completion Time" 
              << std::setw(15) << "Turnaround" 
              << "Waiting Time\n";
    std::cout << "------------------------------------------------------------------------\n";

    double total_turnaround = 0;
    double total_waiting = 0;

    for (const auto& p : completed_processes) {
        int turnaround = p.get_completion_time() - p.get_arrival_time();
        int waiting = turnaround - p.get_burst_time();
        
        total_turnaround += turnaround;
        total_waiting += waiting;

        std::cout << std::left << std::setw(8) << p.get_pid()
                  << std::setw(15) << p.get_arrival_time()
                  << std::setw(12) << p.get_burst_time()
                  << std::setw(18) << p.get_completion_time()
                  << std::setw(15) << turnaround
                  << waiting << "\n";
    }
    
    std::cout << "------------------------------------------------------------------------\n";
    double avg_turnaround = total_turnaround / completed_processes.size();
    double avg_waiting = total_waiting / completed_processes.size();
    double throughput = (double)completed_processes.size() / current_time;
    double overhead_pct = ((double)total_context_switches / current_time) * 100;

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Average Turnaround Time : " << avg_turnaround << " Ticks\n";
    std::cout << "Average Waiting Time    : " << avg_waiting << " Ticks\n";
    std::cout << "System Core Throughput  : " << throughput << " processes/tick\n";
    std::cout << "Total Context Switches  : " << total_context_switches << "\n";
    std::cout << "CPU Overhead Loss Cost  : " << overhead_pct << "%\n";
    std::cout << "========================================================================\n";
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

            // ------------------------------------------------------------------------
            // ADDITION: CONTEXT SWITCH OVERHEAD PENALTY
            // ------------------------------------------------------------------------
            if (last_running_pid != -1 && last_running_pid != current_job.get_pid()) {
                std::cout << "[Time " << current_time << "] [CONTEXT SWITCH OVERHEAD] Swapping PID " 
                          << last_running_pid << " out -> Loading PID " << current_job.get_pid() << " in.\n";
                
                total_context_switches++;
                current_time++;       // Pay the 1-tick system clock penalty
                check_new_arrivals(); // Check for arrivals during this overhead tick
                std::cout << "------------------------------------------------------------------------\n";
            }

            last_running_pid = current_job.get_pid();

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
                    current_job.set_completion_time(current_time + 1);
                    completed_processes.push_back(current_job);
                    last_running_pid = -1; // Reset since CPU is now empty
                    
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
            last_running_pid = -1;
            current_time++;
            std::cout << "------------------------------------------------------------------------\n";
        }
    }

    print_final_metrics();

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