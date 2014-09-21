#include <cli/program.h>

#include <iostream>

namespace global {
    double g_total_time_spent_by_heuristics = 0;
    double g_total_time_spent_separating_cuts = 0;
    long g_total_number_of_cuts_added = 0;
    long g_search_for_cuts_every_n_nodes = 1;
}

int main(int argc, char* argv[]) {    
    std::vector<std::string> args(argv + 1, argv + argc);
    
    Program p;
    p.autorun(args);
    
    return 0;
}