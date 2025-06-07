#pragma once

#include <vector>
#include <string>

#include "common/types.h"

/**
 * @brief Generates an SVG file that visualizes the input point set, the Delaunay triangulation
 *        (via CDT library), and the resulting output edges.
 *
 * The visualization includes:
 *  - Blue points representing the nodes.
 *  - Gray lines for the Delaunay triangulation edges.
 *  - Red lines for the output edges obtained from the solver.
 *
 * Coordinates are scaled and flipped vertically to display properly in SVG coordinate space.
 * This function provides a geometric interpretation of the solver solution in the context of
 * the point layout and Delaunay structure.
 *
 * @param points     A vector of Point structs representing node positions in 2D space.
 * @param output_edges  A vector of edges (pairs of vertex indices) selected by the solver.
 * @param filename   The name of the SVG file to write (default: "visualization.svg").
 */
void drawSVG(const std::vector<Point>& points, const std::vector<std::pair<int, int>>& output_edges, const std::string& filename = "visualization.svg");
