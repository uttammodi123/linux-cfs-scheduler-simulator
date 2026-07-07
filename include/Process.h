#ifndef PROCESS_H
#define PROCESS_H

// Linux kernel weight mapping for nice values from -20 to 19
extern const int NICE_TO_WEIGHT[40];

class Process {
private:
    int pid;
    int arrival_time;
    int burst_time;
    int remaining_time;
    int nice;
    int weight;
    double vruntime;
    int completion_time;

public:
    // Constructor Declaration
    Process(int id, int arr, int burst, int n);

    // Getters
    int get_pid() const;
    int get_arrival_time() const;
    int get_burst_time() const;
    int get_remaining_time() const;
    int get_nice() const;
    int get_weight() const;
    double get_vruntime() const;
    int get_completion_time() const;


    // Setters
    void set_remaining_time(int t);
    void set_vruntime(double vr);
    void set_completion_time(int t);

    void execute_one_tick();
    void update_vruntime(int execution_time);
};

// Custom Comparator Struct Declaration
struct CompareVRuntime {
    bool operator()(const Process& a, const Process& b) const;
};

#endif