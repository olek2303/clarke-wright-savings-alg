#pragma once

#pragma once

#include <vector>
#include <utility>

#include "common/types.h"

/**
 * @brief Template solver function - add your documentation here
 *
 * @param vertices A vector of 2D vertices (nodes), each represented by a Point struct containing x and y coordinates.
 * @param edges  A vector of edges connecting pairs of vertices, each represented by an Edge struct with
 *               endpoints u, v and a cost (typically Euclidean distance).
 *
 * @return std::vector<std::pair<int, int>>
 *         A list of pairs (u, v) representing the edges selected by the algorithm.
 */
std::vector<std::pair<int, int>> solveProblem(const std::vector<Point>& vertices, const std::vector<Edge>& edges);
