#pragma once
#include <iostream>
#include <vector>

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
        const std::vector<std::vector<double>>& dist_matrix);

std::vector<Route> clarke_wright_separate_depots(
        int start_depot_idx,
        int end_depot_idx,
        const std::vector<CustomerNode>& nodes,
        const std::vector<std::vector<double>>& dist_matrix,
        std::vector<DirectedSaving>& savings_list);

std::vector<Route> generate_routes_with_variants(
        int start_depot_idx,
        int end_depot_idx,
        const std::vector<CustomerNode>& nodes,
        const std::vector<std::vector<double>>& dist_matrix,
        int num_variants);

std::vector<Route> find_k_different_tours(
        int start_depot_idx,
        int end_depot_idx,
        const std::vector<CustomerNode>& nodes,
        const std::vector<std::vector<double>>& dist_matrix,
        int num_tours_to_find);

std::vector<Route> solve_for_single_tour(
        int start_depot_idx,
        int end_depot_idx,
        const std::vector<CustomerNode>& nodes,
        const std::vector<std::vector<double>>& dist_matrix);
