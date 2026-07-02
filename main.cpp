#include <iostream>
#include <set>
#include "include/Process.h"

int main() {
    // Instantiate our Red-Black Tree using our encapsulated Process class and Custom Comparator
    std::set<Process, CompareVRuntime> rb_tree;
    
    std::cout << "CFS Simulator Multi-File Architecture Compiled Successfully!" << std::endl;
    return 0;
}