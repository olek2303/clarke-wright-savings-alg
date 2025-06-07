#pragma once

#include <vector>

/**
 * @brief Generates all non-trivial subsets of a vertex set {0, 1, ..., n-1},
 *        excluding subsets that are too small (size < 2) or the full set (size = n).
 * 
 * Each subset is represented as a vector of vertex indices.
 * These subsets are typically used in the MST Integer Programming model to enforce
 * subtour elimination constraints: For each subset S, the number of edges within S 
 * must be at most |S| - 1 to prevent cycles.
 * 
 * The subsets are generated using a bitmasking approach, resulting in 2^n total combinations,
 * of which trivial and full-set subsets are excluded.
 * 
 * @param n The number of vertices in the graph.
 * 
 * @return std::vector<std::vector<int>> 
 *         A list of subsets, where each subset is a vector of integers (vertex indices).
 */
std::vector<std::vector<int>> generateSubsets(int n);
