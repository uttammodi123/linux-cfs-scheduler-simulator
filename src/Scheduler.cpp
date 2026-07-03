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
void Scheduler::run_simulation() {
    std::cout << "Starting Linux CFS Simulator Engine...\n";
    std::cout << "--------------------------------------------\n";

    // Loop continues until BOTH the arrival pool AND the ready queue are completely empty
    while (!arrival_pool.empty() || !ready_queue.empty()) {
        
        // 1. Check if any new processes have arrived at this clock tick
        check_new_arrivals();

        // 2. Placeholder for CPU execution logic
        if (!ready_queue.empty()) {
            // For now, we will just print that the queue has tasks
            std::cout << "[Time " << current_time << "] Processing queue size: " 
                      << ready_queue.size() << "\n";
        } else {
            std::cout << "[Time " << current_time << "] CPU Idle. Waiting for tasks...\n";
        }

        // Advance the system clock by 1 tick
        current_time++;

        // Safety breaker to prevent infinite loops during initial testing
        if (current_time > 20) {
            std::cout << "Simulation safety limit reached. Stopping.\n";
            break;
        }
    }

    std::cout << "--------------------------------------------\n";
    std::cout << "Simulation Finished Successfully!\n";
}