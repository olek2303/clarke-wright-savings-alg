#pragma once

#include <cmath>
#include <vector>

/**
 * @brief Represents a 2D point in Cartesian coordinates.
 *
 * Used to define the spatial location of a node in the plane.
 * These points serve as the vertices for graph construction and
 * visualization (e.g., in Delaunay triangulation or MST layout).
 */
struct Point
{
    double x;  ///< X-coordinate of the point.
    double y;  ///< Y-coordinate of the point.

    /**
    * @brief Lexicographical comparison operator for Point.
    *
    * Defines a strict weak ordering based on (x, y) coordinates,
    * enabling use of Point in ordered containers like std::set or std::map.
    *
    * A point is considered less than another if its x-coordinate is smaller,
    * or if the x-coordinates are equal and its y-coordinate is smaller.
    *
    * @param other The point to compare with.
    * @return true  If this point is lexicographically less than the other.
    * @return false Otherwise.
    */
    bool operator<(const Point& other) const;
};

/**
 * @brief Represents an undirected edge between two vertices with an associated cost.
 *
 * Each edge connects two distinct vertex indices (u and v) and carries a weight
 * (typically the Euclidean distance between the two vertices it connects).
 * Used as the fundamental input to the MST solver and CPLEX model.
 */
struct Edge
{
    int u;        ///< Index of the first vertex (one endpoint of the edge).
    int v;        ///< Index of the second vertex (the other endpoint).
    double cost;  ///< Cost or weight of the edge (e.g., Euclidean distance).
};

/**
 * @brief Computes the Euclidean distance between two 2D points.
 *
 * Uses the standard Euclidean formula:
 *   distance = sqrt((x1 - x2)² + (y1 - y2)²)
 *
 * This function is typically used to assign edge weights based on the
 * geometric distance between nodes in the plane.
 *
 * @param a The first point (with x and y coordinates).
 * @param b The second point (with x and y coordinates).
 *
 * @return double
 *         The Euclidean distance between point a and point b.
 */
double euclidean(const Point& a, const Point& b);

/**
 * @brief Generates a specified number of unique 2D points within a given rectangular range.
 *
 * Points are randomly sampled with uniform distribution over the specified [minX, maxX] × [minY, maxY] region.
 * Duplicate points (within floating-point precision) are avoided using a set and `Point::operator<`.
 *
 * The function attempts up to n * 10 random samples to achieve the desired number of unique points.
 * If it fails to reach `n` due to collisions, a warning is printed and fewer points are returned.
 *
 * @param n      The number of unique points to generate.
 * @param minX   The minimum X-coordinate (inclusive). Default is 0.0.
 * @param minY   The minimum Y-coordinate (inclusive). Default is 0.0.
 * @param maxX   The maximum X-coordinate (inclusive). Default is 40.0.
 * @param maxY   The maximum Y-coordinate (inclusive). Default is 40.0.
 *
 * @return std::vector<Point>
 *         A vector of unique points in ascending order (as defined by Point::operator<).
 */
std::vector<Point> generateUniquePoints(int n, double minX = 0.0, double minY = 0.0, double maxX = 40.0, double maxY = 40.0);
