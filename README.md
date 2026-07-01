# Linux Completely Fair Scheduler (CFS) Simulator

A discrete event simulation engine implemented in C++ that models the modern Linux kernel's CPU scheduling architecture. 

> 🚧 **Status:** Under active development. Core architecture design in progress. Estimated completion: Late July 2026.

---

## 🎯 Project Overview
This project simulates how the Linux kernel manages CPU time allocation using a Red-Black Tree to achieve fair scheduling. It models core OS concepts including dynamic time-slice calculation using the Linux Nice-to-Weight matrix, virtual runtime (`vruntime`) tracking, preemption, and context-switching overheads.

## 🛠️ Planned Architecture & Performance Targets
* **Process Selection:** Implementation of a Red-Black Tree (`std::set`) to track and extract minimum `vruntime` tasks.
* **Latency & Fairness:** Mathematical calculation of dynamic time slices using priorities spanning the `-20` to `19` nice value spectrum to prevent task starvation.
* **Metrics Engine:** Real-time CLI performance dashboard tracking system execution metrics:
  * Throughput
  * Turnaround Time
  * Wait Time

## 📅 Execution Roadmap (20-Day Plan)
- [ ] **Phase 1 (Days 1-5):** Define Task control blocks, `vruntime` tracking math, and Red-Black tree integration.
- [ ] **Phase 2 (Days 6-10):** Implement Nice-to-Weight matrix math and dynamic time-slice allocation logic.
- [ ] **Phase 3 (Days 11-15):** Build the discrete event loop to simulate clock ticks, preemption, and context switches.
- [ ] **Phase 4 (Days 16-20):** Create CLI performance visualizer, log generator, and complete benchmarking.

## ⚙️ How to Run (Coming Soon)
```bash
# Clone the repository
git clone [https://github.com/uttammodi123/linux-cfs-scheduler-simulator.git](https://github.com/uttammodi123/linux-cfs-scheduler-simulator.git)
