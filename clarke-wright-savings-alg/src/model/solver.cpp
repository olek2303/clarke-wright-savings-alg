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
#include "model/clark_wright_alg.h"
#include <queue>

void dijkstra(int source, const std::vector<std::vector<std::pair<int, double>>>& adj,
    std::vector<double>& dist, std::vector<int>& parent) {
    int n = (int)adj.size();
    dist.assign(n, std::numeric_limits<double>::infinity());
    parent.assign(n, -1);
    dist[source] = 0.0;

    using PD = std::pair<double, int>;
    std::priority_queue<PD, std::vector<PD>, std::greater<PD>> pq;
    pq.emplace(0.0, source);

    while (!pq.empty()) {
        auto [curr_dist, u] = pq.top();
        pq.pop();
        if (curr_dist > dist[u]) continue;

        for (const auto& [v, cost] : adj[u]) {
            double nd = curr_dist + cost;
            if (nd < dist[v]) {
                dist[v] = nd;
                parent[v] = u;
                pq.emplace(nd, v);
            }
        }
    }
}

std::vector<int> reconstruct_path(int from, int to, const std::vector<std::vector<int>>& parent_matrix) {
    std::vector<int> path;
    if (parent_matrix[from][to] == -1 && from != to) {
        return {};
    }
    int current = to;
    while (current != from) {
        path.push_back(current);
        current = parent_matrix[from][current];
        if (current == -1) return {};
    }
    path.push_back(from);
    std::reverse(path.begin(), path.end());
    return path;
}


std::vector<std::vector<std::pair<int, int>>> solve_clarke_savings(const std::vector<Point>& vertices, const std::vector<Edge>& edges, int n_of_roads)
{
    std::cout << "Solving with Clarke-Wright based heuristic...\n";
    std::vector<std::vector<std::pair<int, int>>> all_output_edges;

    IloEnv env;

    try {
        int num_points = static_cast<int>(vertices.size());
        if (num_points < 3) {
            std::cerr << "You need to have at least 3 points!.\n";
            return {};
        }

        const int start_depot_idx = 0;
        const int end_depot_idx = num_points - 1;
        const int num_waypoints = num_points - 2;

        std::cout << "Start depot idx: " << start_depot_idx << "\n";
        std::cout << "End depot idx: " << end_depot_idx << "\n";
        std::cout << "Number of waypoints: " << num_waypoints << "\n";

        std::vector<std::vector<std::pair<int, double>>> adj(num_points);
        for (const auto& edge : edges) {
            adj[edge.u].emplace_back(edge.v, edge.cost);
            adj[edge.v].emplace_back(edge.u, edge.cost);
        }

        std::vector<std::vector<double>> dist_matrix(num_points, std::vector<double>(num_points, std::numeric_limits<double>::infinity()));
        std::vector<std::vector<int>> parent_matrix(num_points, std::vector<int>(num_points, -1));

        for (int i = 0; i < num_points; ++i) {
            std::vector<double> dist;
            std::vector<int> parent;
            dijkstra(i, adj, dist, parent);
            dist_matrix[i] = dist;
            parent_matrix[i] = parent;
        }

        std::vector<CustomerNode> nodes;
        for (int i = 1; i <= num_waypoints; ++i) {
            nodes.push_back({ i, 1.0 });
        }

        std::vector<Route> routes = generate_routes_with_variants(
            start_depot_idx, end_depot_idx, nodes, dist_matrix, num_waypoints * 10);

        if (routes.empty()) {
            std::cerr << "No routes found!.\n";
            return {};
        }


        std::vector<Route> sorted_routes = routes;
        std::sort(sorted_routes.begin(), sorted_routes.end(),
            [](const Route& a, const Route& b) {
                if (a.point_indices.size() != b.point_indices.size()) {
                    return a.point_indices.size() > b.point_indices.size();
                }
                else {
                    return a.total_distance < b.total_distance;
                }
            });


        int top_k = n_of_roads;
        int count = std::min(top_k, (int)sorted_routes.size());

        for (int i = 0; i < count; ++i) {
            const Route& best_route = sorted_routes[i];
            std::vector<std::pair<int, int>> output_edges;

            if (!best_route.point_indices.empty()) {
                std::cout << "Best route #" << (i + 1) << ": " << start_depot_idx;
                for (int curr : best_route.point_indices) {
                    std::cout << " -> " << curr;
                }
                std::cout << " -> " << end_depot_idx << "\n";
                std::cout << "Total distance of best route #" << (i + 1) << ": " << best_route.total_distance << "\n";

                int prev = start_depot_idx;

                for (int curr : best_route.point_indices) {
                    std::vector<int> path = reconstruct_path(prev, curr, parent_matrix);
                    if (path.empty()) {
                        std::cerr << "No path between: " << prev << " a " << curr << "\n";
                        continue;
                    }
                    for (size_t j = 0; j + 1 < path.size(); ++j) {
                        output_edges.emplace_back(path[j], path[j + 1]);
                    }
                    prev = curr;
                }

                std::vector<int> path = reconstruct_path(prev, end_depot_idx, parent_matrix);
                if (path.empty()) {
                    std::cerr << "No path between: " << prev << " a " << end_depot_idx << "\n";
                }
                for (size_t j = 0; j + 1 < path.size(); ++j) {
                    output_edges.emplace_back(path[j], path[j + 1]);
                }

                all_output_edges.push_back(std::move(output_edges));
            }
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
    return all_output_edges;
}

