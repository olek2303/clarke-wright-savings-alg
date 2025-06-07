#include "model/solver.h"
#include <ilcplex/ilocplex.h>

#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <random>
#include <limits>
#include <cmath>
#include <stdexcept>
#include <thread>

struct CustomerNode {
    int id;
    double demand;
};

struct Route {
    std::vector<int> point_indices;
    double total_distance;
    Route() : total_distance(0) {}
};

struct DirectedSaving {
    int from_idx;
    int to_idx;
    double value;
    bool operator<(const DirectedSaving& other) const { return value > other.value; }
};

double calculate_route_distance_separate_depots(
    const Route& route,
    int start_depot_idx,
    int end_depot_idx,
    const std::vector<std::vector<double>>& dist_matrix) {

    if (route.point_indices.empty()) return 0.0;
    double distance = 0.0;
    distance += dist_matrix[start_depot_idx][route.point_indices.front()];
    for (size_t i = 0; i < route.point_indices.size() - 1; ++i) {
        distance += dist_matrix[route.point_indices[i]][route.point_indices[i + 1]];
    }
    distance += dist_matrix[route.point_indices.back()][end_depot_idx];
    return distance;
}

std::vector<Route> clarke_wright_separate_depots(
    int start_depot_idx,
    int end_depot_idx,
    const std::vector<CustomerNode>& nodes,
    const std::vector<std::vector<double>>& dist_matrix,
    std::vector<DirectedSaving>& savings_list) {

    if (nodes.empty()) return {};

    std::vector<Route> current_routes;
    for (const auto& customer : nodes) {
        Route initial_route;
        initial_route.point_indices.push_back(customer.id);
        current_routes.push_back(initial_route);
    }

    std::sort(savings_list.begin(), savings_list.end());

    for (const auto& saving : savings_list) {
        int from_idx = saving.from_idx;
        int to_idx = saving.to_idx;

        int route1_idx = -1;
        int route2_idx = -1;

        for (size_t i = 0; i < current_routes.size(); ++i) {
            const auto& pts = current_routes[i].point_indices;
            if (pts.empty()) continue;
            if (pts.back() == from_idx) route1_idx = static_cast<int>(i);
            if (pts.front() == to_idx) route2_idx = static_cast<int>(i);
        }

        if (route1_idx != -1 && route2_idx != -1 && route1_idx != route2_idx) {
            auto& r1 = current_routes[route1_idx];
            auto& r2 = current_routes[route2_idx];

            bool has_overlap = false;
            for (int pid : r2.point_indices) {
                if (std::find(r1.point_indices.begin(), r1.point_indices.end(), pid) != r1.point_indices.end()) {
                    has_overlap = true;
                    break;
                }
            }

            if (!has_overlap) {
                r1.point_indices.insert(
                    r1.point_indices.end(),
                    r2.point_indices.begin(),
                    r2.point_indices.end()
                );
                r2.point_indices.clear();
            }
        }
    }

    std::vector<Route> final_routes;
    for (const auto& route : current_routes) {
        if (!route.point_indices.empty()) {
            Route final_route = route;
            final_route.total_distance = calculate_route_distance_separate_depots(
                route, start_depot_idx, end_depot_idx, dist_matrix);
            final_routes.push_back(final_route);
        }
    }

    return final_routes;
}

std::vector<Route> generate_routes_with_variants(
    int start_depot_idx,
    int end_depot_idx,
    const std::vector<CustomerNode>& nodes,
    const std::vector<std::vector<double>>& dist_matrix,
    int num_variants = 3) {

    std::vector<Route> all_valid_routes;

    std::vector<DirectedSaving> base_savings;
    for (const auto& cust_i : nodes) {
        for (const auto& cust_j : nodes) {
            if (cust_i.id == cust_j.id) continue;
            double saving_value =
                dist_matrix[cust_i.id][end_depot_idx] +
                dist_matrix[start_depot_idx][cust_j.id] -
                dist_matrix[cust_i.id][cust_j.id];
            if (saving_value > 0) {
                base_savings.push_back({ cust_i.id, cust_j.id, saving_value });
            }
        }
    }

    std::sort(base_savings.begin(), base_savings.end());

    int seed = 42;
    for (int k = 0; k < num_variants; ++k) {
        std::vector<DirectedSaving> savings_variant = base_savings;
        std::shuffle(savings_variant.begin(), savings_variant.end(), std::default_random_engine(seed + k));

        std::vector<Route> routes_variant = clarke_wright_separate_depots(
            start_depot_idx, end_depot_idx, nodes, dist_matrix, savings_variant);

        if (!routes_variant.empty()) {
            all_valid_routes.push_back(routes_variant[0]);
        }

        std::sort(all_valid_routes.begin(), all_valid_routes.end(),
            [](const Route& a, const Route& b) {
                return a.point_indices.size() > b.point_indices.size();
            });

        if ((int)all_valid_routes.size() >= num_variants)
            break;
    }

    return all_valid_routes;
}

std::vector<std::pair<int, int>> solveProblem(const std::vector<Point>& vertices, const std::vector<Edge>& edges)
{
    std::cout << "Solving with Clarke-Wright based heuristic...\n";
    std::vector<std::pair<int, int>> output_edges;
    IloEnv env;

    try {
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

        std::vector<Route> routes = generate_routes_with_variants(
            start_depot_idx, end_depot_idx, nodes, dist_matrix, 10);  // próbujemy wiêcej wariantów

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
    catch (const IloException& e) {
        std::cerr << "CPLEX exception: " << e.getMessage() << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Standard exception: " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "Unknown exception occurred.\n";
    }

    env.end();
    return output_edges;
}
