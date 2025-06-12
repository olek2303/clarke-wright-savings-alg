#pragma once

#include <vector>
#include <string>

#include "common/types.h"

/**
 * @brief Offsets a line segment (x1, y1)-(x2, y2) by a given perpendicular distance.
 *
 * @param x1 Reference to the x-coordinate of the first endpoint (modified in place).
 * @param y1 Reference to the y-coordinate of the first endpoint (modified in place).
 * @param x2 Reference to the x-coordinate of the second endpoint (modified in place).
 * @param y2 Reference to the y-coordinate of the second endpoint (modified in place).
 * @param offset The perpendicular offset distance.
 */
void offset_line(double& x1, double& y1, double& x2, double& y2, double offset);

/**
 * @brief Draws an SVG visualization of the points, routes, and Delaunay triangles.
 *
 * @param points The list of points (vertices) to visualize.
 * @param all_routes_edges A list of routes, each represented as a list of edges (pairs of point indices).
 * @param triangles The list of triangles (each as an array of 3 point indices) for Delaunay triangulation.
 * @param filename The output SVG file name.
 */
void drawSVG(const std::vector<Point>& points,
    const std::vector<std::vector<std::pair<int, int>>>& all_routes_edges,
    const std::vector<std::array<int, 3>>& triangles,
    const std::string& filename);
