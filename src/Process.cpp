#include "../include/Process.h"

// Definition of global weight mapping
const int NICE_TO_WEIGHT[40] = {
    88761,     71755,     56483,     46273,     36291, 
    29154,     23254,     18705,     14949,     11916, 
    9548,      7620,      6100,      4904,      3906,  
    3121,      2501,      1991,      1586,      1277,  
    1024,      820,       655,       526,       423,   
    335,       272,       215,       172,       137,   
    110,       87,        70,        56,        45,    
    36,        29,        23,        18,        15     
};

// Constructor Definition
Process::Process(int id, int arr, int burst, int n){
    pid = id;
    arrival_time = arr;
    burst_time = burst;
    remaining_time = burst;
    nice = n;
    weight = NICE_TO_WEIGHT[n + 20]; 
    vruntime = 0.0;
}

// Getters Definitions
int Process::get_pid() const { 
    return pid; 
}

int Process::get_arrival_time() const { 
    return arrival_time; 
}

int Process::get_burst_time() const { 
    return burst_time; 
}

int Process::get_remaining_time() const { 
    return remaining_time; 
}

int Process::get_nice() const { 
    return nice; 
}

int Process::get_weight() const { 
    return weight; 
}

double Process::get_vruntime() const { 
    return vruntime; 
}

// Setters Definitions
void Process::set_remaining_time(int t){ 
    remaining_time = t; 
}

void Process::set_vruntime(double vr){ 
    vruntime = vr; 
}

// Comparator Operator Definition
bool CompareVRuntime::operator()(const Process& a, const Process& b) const {
    if (a.get_vruntime() != b.get_vruntime()) {
        return a.get_vruntime() < b.get_vruntime();
    }
    return a.get_pid() < b.get_pid();
}