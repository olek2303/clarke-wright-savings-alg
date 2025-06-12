#pragma once

#include <vector>
#include <utility>

#include "common/types.h"

/**
 * @brief Computes the shortest paths from a source node to all other nodes using Dijkstra's algorithm.
 *
 * @param source The index of the source node.
 * @param adj The adjacency list of the graph, where adj[u] contains pairs (v, cost) for each edge u-v.
 * @param dist Output vector of distances from the source to each node.
 * @param parent Output vector of parent indices for path reconstruction.
 */
void dijkstra(int source, const std::vector<std::vector<std::pair<int, double>>>& adj,
    std::vector<double>& dist, std::vector<int>& parent);

/**
 * @brief Reconstructs the shortest path from node 'from' to node 'to' using a parent matrix.
 *
 * @param from The starting node index.
 * @param to The destination node index.
 * @param parent_matrix Matrix where parent_matrix[i][j] is the parent of j on the shortest path from i.
 * @return std::vector<int> The sequence of node indices representing the path, or empty if no path exists.
 */
std::vector<int> reconstruct_path(int from, int to, const std::vector<std::vector<int>>& parent_matrix);

/**
 * @brief Solves the vehicle routing problem using a Clarke-Wright based heuristic.
 *
 * @param vertices The list of points (nodes) in the graph.
 * @param edges The list of edges in the graph.
 * @param n_of_roads The number of top routes to return.
 * @return std::vector<std::vector<std::pair<int, int>>> A list of routes, each represented as a list of edges (pairs of node indices).
 */
std::vector<std::vector<std::pair<int, int>>> solve_clarke_savings(const std::vector<Point>& vertices,
    const std::vector<Edge>& edges,
    int n_of_roads);
