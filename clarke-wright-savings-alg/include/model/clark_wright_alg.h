#pragma once

#include <vector>
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <random>
#include <limits>
#include <cmath>
#include <stdexcept>
#include <thread>
#include <queue>

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

/**
 * @brief Calculates the total distance of a route with separate start and end depots.
 *
 * @param route The route to evaluate.
 * @param start_depot_idx Index of the starting depot.
 * @param end_depot_idx Index of the ending depot.
 * @param dist_matrix Matrix of distances between all points.
 * @return double The total distance of the route, or infinity if the route is not feasible.
 */
double calculate_route_distance_separate_depots(
    const Route& route,
    int start_depot_idx,
    int end_depot_idx,
    const std::vector<std::vector<double>>& dist_matrix);

/**
 * @brief Applies the Clarke-Wright savings algorithm for separate depots to generate routes.
 *
 * @param start_depot_idx Index of the starting depot.
 * @param end_depot_idx Index of the ending depot.
 * @param nodes List of customer nodes.
 * @param dist_matrix Matrix of distances between all points.
 * @param savings_list List of directed savings (will be sorted and used in-place).
 * @return std::vector<Route> The list of generated routes.
 */
std::vector<Route> clarke_wright_separate_depots(
    int start_depot_idx,
    int end_depot_idx,
    const std::vector<CustomerNode>& nodes,
    const std::vector<std::vector<double>>& dist_matrix,
    std::vector<DirectedSaving>& savings_list);

/**
 * @brief Generates multiple route variants using randomized Clarke-Wright savings orderings.
 *
 * @param start_depot_idx Index of the starting depot.
 * @param end_depot_idx Index of the ending depot.
 * @param nodes List of customer nodes.
 * @param dist_matrix Matrix of distances between all points.
 * @param num_variants Number of randomized variants to generate.
 * @return std::vector<Route> All valid routes found across all variants.
 */
std::vector<Route> generate_routes_with_variants(
    int start_depot_idx,
    int end_depot_idx,
    const std::vector<CustomerNode>& nodes,
    const std::vector<std::vector<double>>& dist_matrix,
    int num_variants = 3);
