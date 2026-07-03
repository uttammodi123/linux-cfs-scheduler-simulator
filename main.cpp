#include <iostream>
#include "include/Process.h"
#include "include/Scheduler.h"

int main() {
    Scheduler cfs_engine;

    // Create a few sample processes: Process(pid, arrival_time, burst_time, nice)
    // Remember: nice ranges from -20 to 19. 0 is default behavior.
    cfs_engine.add_process(Process(101, 0, 6, 0));  // Arrives at t=0
    cfs_engine.add_process(Process(102, 3, 4, -5)); // Arrives at t=3 (Higher priority!)
    cfs_engine.add_process(Process(103, 5, 2, 5));  // Arrives at t=5 (Lower priority)

    // Fire up the engine execution loop!
    cfs_engine.run_simulation();

    return 0;
}