#include "model/solver.h"

#include <ilcplex/ilocplex.h>
#include <iostream>
#include <set>
#include <thread>

#include <random> // to be removed

#include "model/subsets.h"

std::vector<std::pair<int, int>> solveProblem(const std::vector<Point>& vertices, const std::vector<Edge>& edges)
{
    std::cout << "Solving...\n";

    std::vector<std::pair<int, int>> output_edges;
    IloEnv env;
    try
    {
        /*
        Remove template code below and uncomment the rest of try block.
        */
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, edges.size() - 1);

        int num_edges_to_select = std::min(3, static_cast<int>(edges.size())); // select up to 3 random edges
        std::set<int> selected_indices;
        while (selected_indices.size() < num_edges_to_select) {
            selected_indices.insert(dis(gen));
        }
        for (int idx : selected_indices) {
            output_edges.emplace_back(edges[idx].u, edges[idx].v);
        }
        /*
        Remove template code above and uncomment the rest of try block.
        */


        // int n = vertices.size();
        // int m = edges.size();
        // IloModel model(env);
        // IloBoolVarArray x(env, m);

        // // Implement objective and constraint here

        // // Solve
        // IloCplex cplex(model);
        // cplex.setOut(env.getNullStream());
        // cplex.setParam(IloCplex::ParallelMode, 1); // opportunistic
        // cplex.setParam(IloCplex::Threads, std::thread::hardware_concurrency());
        // if (!cplex.solve())
        // {
        //     std::cerr << "No solution found." << std::endl;
        //     return {};
        // }

        // std::cout << "Total calculation cost: " << cplex.getObjValue() << std::endl;

        // // Collect edges
        // for (int e = 0; e < m; ++e)
        //     if (cplex.getValue(x[e]) > 0.5)
        //         output_edges.emplace_back(edges[e].u, edges[e].v);
    }
    catch (IloException& e)
    {
        std::cerr << "CPLEX exception: " << e.getMessage() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Unknown error solving MST." << std::endl;
    }
    env.end();
    return output_edges;
}
