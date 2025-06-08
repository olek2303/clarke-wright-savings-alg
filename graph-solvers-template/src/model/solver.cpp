#include "model/solver.h"

#include <ilcplex/ilocplex.h>
#include <iostream>
#include <set>
#include <thread>

#include <random> // to be removed

#include "model/subsets.h"
#include "model/clarke_wright.h"

std::vector<std::pair<int, int>> solveProblem(const std::vector<Point>& vertices, const std::vector<Edge>& edges)
{
    std::cout << "Solving with Clarke-Wright based heuristic...\n";

    std::vector<std::pair<int, int>> output_edges;
    IloEnv env;
    try
    {
        int num_points = static_cast<int>(vertices.size());
        if (num_points < 3) {
            std::cerr << "Za ma³o punktów (min. 3: start, przynajmniej 1 klient, meta).\n";
            return {};
        }

        const int start_depot_idx = 0;
        const int end_depot_idx = num_points - 1;
        const int num_waypoints = num_points - 2;

        std::vector<std::vector<double>> dist_matrix(num_points, std::vector<double>(num_points, std::numeric_limits<double>::infinity()));
        for (const auto& edge : edges) {
            dist_matrix[edge.u][edge.v] = edge.cost;
            dist_matrix[edge.v][edge.u] = edge.cost;
        }

        std::vector<CustomerNode> nodes;
        for (int i = 1; i <= num_waypoints; ++i) {
            nodes.push_back({ i, 1.0 });
        }

        std::cout << "\nUruchamianie algorytmu oszczednosci..." << std::endl;
        std::cout << "------------------------------------------------------------------" << std::endl;
        std::cout << "Indeks Startu: " << start_depot_idx << ", Indeks Mety: " << end_depot_idx << std::endl;
        std::cout << "Liczba Waypointow: " << num_waypoints << " (indeksy 1.." << num_waypoints << ")" << std::endl;

//        std::vector<Route> routes = generate_routes_with_variants(
//                start_depot_idx, end_depot_idx, nodes, dist_matrix, 10);
        std::vector<Route> routes = solve_for_single_tour(
                start_depot_idx, end_depot_idx, nodes, dist_matrix);
//        std::vector<Route> routes = find_k_different_tours(
//                start_depot_idx, end_depot_idx, nodes, dist_matrix, 10);

        std::cout << "Wygenerowano " <<  routes.size() <<" trasy:" << std::endl;

        for (size_t i = 0; i < routes.size(); ++i) {
            const auto& route = routes[i];
            std::cout << "Trasa " << i + 1 << ": ";
            std::cout << "Start(" << start_depot_idx << ") -> ";
            for (size_t j = 0; j < route.point_indices.size(); ++j) {
                std::cout << route.point_indices[j] << (j == route.point_indices.size() - 1 ? "" : " -> ");
            }
            std::cout << " -> Meta(" << end_depot_idx << ")";
            std::cout << " (Dystans: " << std::fixed << std::setprecision(2) << route.total_distance << ")" << std::endl;

        }

        if (routes.empty()) {
            std::cerr << "Nie wygenerowano ¿adnej trasy.\n";
            return {};
        }

        // Wybieramy trasê odwiedzaj¹c¹ najwiêcej punktów
        const auto& best_route = *std::max_element(
                routes.begin(), routes.end(),
                [](const Route& a, const Route& b) {
                    return a.point_indices.size() < b.point_indices.size();
                });

        if (!best_route.point_indices.empty()) {
            output_edges.emplace_back(start_depot_idx, best_route.point_indices.front());
            for (size_t i = 0; i + 1 < best_route.point_indices.size(); ++i) {
                output_edges.emplace_back(best_route.point_indices[i], best_route.point_indices[i + 1]);
            }
            output_edges.emplace_back(best_route.point_indices.back(), end_depot_idx);
        }
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
