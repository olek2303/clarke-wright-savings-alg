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

std::vector<Route> generate_routes_with_variants(
    int start_depot_idx,
    int end_depot_idx,
    const std::vector<CustomerNode>& nodes,
    const std::vector<std::vector<double>>& dist_matrix,
    int num_variants = 3);
