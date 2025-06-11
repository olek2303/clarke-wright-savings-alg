#include "model/clark_wright_alg.h"

double calculate_route_distance_separate_depots(
    const Route& route,
    int start_depot_idx,
    int end_depot_idx,
    const std::vector<std::vector<double>>& dist_matrix) {

    if (route.point_indices.empty()) return 0.0;
    double distance = 0.0;

    int from = start_depot_idx;
    for (size_t i = 0; i < route.point_indices.size(); ++i) {
        int to = route.point_indices[i];
        double d = dist_matrix[from][to];
        if (std::isinf(d)) return std::numeric_limits<double>::infinity();  // trasa nieosiągalna
        distance += d;
        from = to;
    }

    double d = dist_matrix[route.point_indices.back()][end_depot_idx];
    if (std::isinf(d)) return std::numeric_limits<double>::infinity();

    distance += d;
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
    int num_variants) {

    std::vector<Route> all_valid_routes;

    std::vector<DirectedSaving> base_savings;
    for (const auto& cust_i : nodes) {
        for (const auto& cust_j : nodes) {
            if (cust_i.id == cust_j.id) continue;
            double cij = dist_matrix[cust_i.id][cust_j.id];
            double sid = dist_matrix[start_depot_idx][cust_j.id];
            double die = dist_matrix[cust_i.id][end_depot_idx];

            if (std::isinf(cij) || std::isinf(sid) || std::isinf(die)) continue;

            double saving_value = die + sid - cij;
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

        for (const auto& route : routes_variant) {
            if (!std::isinf(route.total_distance) && !route.point_indices.empty()) {
                all_valid_routes.push_back(route);
            }
        }
    }

    return all_valid_routes;
}
