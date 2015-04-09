#include <parser/parser.h>
#include <program/program.h>
#include <solver/heuristics/heuristic_solver.h>
#include <solver/bc/bc_solver.h>
#include <solver/metaheuristics/tabu/tabu_solver.h>
#include <solver/subgradient/subgradient_solver.h>

#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

program::program(const std::vector<std::string>& args) {
    if(args.size() != 3) {
        print_usage();
        return;
    }
    
    load(args[1], args[0]);
    
    std::vector<std::string> possible_parameters = {
        "branch_and_cut",
        "tabu_and_branch_and_cut",
        "tabu_tuning",
        "subgradient",
        "tabu",
        "heuristics"
    };
    
    if(std::find(possible_parameters.begin(), possible_parameters.end(), args[2]) == possible_parameters.end()) {
        print_usage();
        return;
    }
    
    auto heuristic_solutions = std::vector<path>();
    auto hsolv = heuristic_solver(g, params, data);
    
    if(args[2] == "heuristics") {
        heuristic_solutions = hsolv.run_constructive_heuristics();
    } else {
        if(params.bc.use_initial_solutions) {
            heuristic_solutions = hsolv.run_all_heuristics();
        }
    }

    if(args[2] == "branch_and_cut") {
        auto bsolv = bc_solver(g, params, data, heuristic_solutions);
        bsolv.solve_with_branch_and_cut();
    } else if(args[2] == "subgradient") {        
        auto ssolv = subgradient_solver(g, params, heuristic_solutions);
        ssolv.solve();
    } else if(args[2] == "tabu" || args[2] == "tabu_and_branch_and_cut") {
        auto tsolv = tabu_solver(g, params, data, heuristic_solutions);
        auto sols = tsolv.solve_sequential();
        
        if(args[2] == "tabu_and_branch_and_cut") {
            heuristic_solutions.insert(heuristic_solutions.end(), sols.begin(), sols.end());
            auto bsolv = bc_solver(g, params, data, heuristic_solutions);
            bsolv.solve_with_branch_and_cut();
        }
    } else if(args[2] == "tabu_tuning") {
        auto tsolv = tabu_solver(g, params, data, heuristic_solutions);
        tsolv.solve_parameter_tuning();
    }
}

void program::load(const std::string& params_filename, const std::string& instance_filename) {
    auto par = parser(params_filename, instance_filename);
    g = std::move(par.generate_tsp_graph());
    params = std::move(par.read_program_params());
    data = program_data();
}

void program::print_usage() {
    std::cout << "Usage: " << std::endl;
    std::cout << "\t./tsppddl <instance> <params> [branch_and_cut | subgradient | tabu | heuristics | tabu_and_branch_and_cut]" << std::endl;
    std::cout << "\t\t instance: path to the json file with the instance data" << std::endl;
    std::cout << "\t\t params: path to the json file with the program params" << std::endl;
    std::cout << "\t\t branch_and_cut: to start solving the problem with branch and cut (warmstarted with heuristic solutions)" << std::endl;
    std::cout << "\t\t subgradient: to start solving the problem with lagrangian relaxation and the subgradient method" << std::endl;
    std::cout << "\t\t tabu: to start solving the problem with the tabu search metaheuristic algorithm" << std::endl;
    std::cout << "\t\t tabu_tuning: to start the parameter tuning for the tabu search metaheuristic algorithm" << std::endl;
    std::cout << "\t\t heuristics: to run the constructive heuristics" << std::endl;
    std::cout << "\t\t tabu_and_branch_and_cut: to start solving the problem with branch and cut (warmstarted with tabu metaheuristics solutions)" << std::endl;
}