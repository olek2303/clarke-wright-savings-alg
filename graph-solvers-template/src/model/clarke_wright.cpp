#include "model/clarke_wright.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <cmath>


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

/*
 * Below previous solution with best route from multiple variants.
 */

std::vector<Route> generate_routes_with_variants_OLD(
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



std::vector<Route> solve_for_single_tour(
        int start_depot_idx,
        int end_depot_idx,
        const std::vector<CustomerNode>& nodes,
        const std::vector<std::vector<double>>& dist_matrix) {

    /*
     * Below solution with all savings from the graph.
     * Returns one route covering whole graph.
     */

    std::vector<DirectedSaving> all_savings;
    for (const auto& cust_i : nodes) {
        for (const auto& cust_j : nodes) {
            if (cust_i.id == cust_j.id) continue;

            double saving_value =
                    dist_matrix[cust_i.id][end_depot_idx] +
                    dist_matrix[start_depot_idx][cust_j.id] -
                    dist_matrix[cust_i.id][cust_j.id];

            all_savings.push_back({ cust_i.id, cust_j.id, saving_value });
        }
    }

    std::sort(all_savings.begin(), all_savings.end());

    std::vector<Route> final_solution = clarke_wright_separate_depots(
            start_depot_idx, end_depot_idx, nodes, dist_matrix, all_savings);

    return final_solution;
}

std::vector<Route> find_k_different_tours(
        int start_depot_idx,
        int end_depot_idx,
        const std::vector<CustomerNode>& nodes,
        const std::vector<std::vector<double>>& dist_matrix,
        int num_tours_to_find) {
    std::vector<Route> found_tours;
    std::vector<DirectedSaving> base_savings;
    for (const auto& cust_i : nodes) {
        for (const auto& cust_j : nodes) {
            if (cust_i.id == cust_j.id) continue;
            double saving_value =
                    dist_matrix[cust_i.id][end_depot_idx] +
                    dist_matrix[start_depot_idx][cust_j.id] -
                    dist_matrix[cust_i.id][cust_j.id];
            base_savings.push_back({ cust_i.id, cust_j.id, saving_value });
        }
    }
    std::sort(base_savings.begin(), base_savings.end());

    int seed = 42;
    for (int k = 0; k < num_tours_to_find; ++k) {
        std::vector<DirectedSaving> savings_variant = base_savings;

        size_t part_to_shuffle = savings_variant.size() / 5;
        if (part_to_shuffle > 1) {
            std::shuffle(savings_variant.begin(),
                         savings_variant.begin() + part_to_shuffle,
                         std::default_random_engine(seed + k));
        }

        std::vector<Route> result = clarke_wright_separate_depots(
                start_depot_idx, end_depot_idx, nodes, dist_matrix, savings_variant);

        if (result.size() == 1 && result[0].point_indices.size() == nodes.size()) {
            found_tours.push_back(result[0]);
        }
    }

    return found_tours;
}