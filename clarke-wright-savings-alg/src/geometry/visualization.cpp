#include "geometry/visualization.h"

#include <CDT.h>
#include <iostream>
#include <fstream>
#include <array>
#include <map>
#include <set>
#include <cmath>

const std::vector<std::string> route_colors = { "red", "blue", "green", "orange", "purple" };

void offset_line(double& x1, double& y1, double& x2, double& y2, double offset) {
    double dx = x2 - x1;
    double dy = y2 - y1;
    double len = std::sqrt(dx * dx + dy * dy);
    if (len == 0) return;

    double nx = -dy / len;
    double ny = dx / len;
    x1 += nx * offset;
    y1 += ny * offset;
    x2 += nx * offset;
    y2 += ny * offset;
}

void drawSVG(const std::vector<Point>& points,
    const std::vector<std::vector<std::pair<int, int>>>& all_routes_edges,
    const std::vector<std::array<int, 3>>& triangles,
    const std::string& filename)
{
    if (points.empty()) return;

    // Compute bounding box
    double minX = points[0].x, maxX = points[0].x;
    double minY = points[0].y, maxY = points[0].y;
    for (const auto& p : points) {
        minX = std::min(minX, p.x);
        maxX = std::max(maxX, p.x);
        minY = std::min(minY, p.y);
        maxY = std::max(maxY, p.y);
    }

    double margin = 50;
    double scale = 25;

    double legend_width = 250;

    double width = (maxX - minX) * scale + 2 * margin + legend_width;
    double height = (maxY - minY) * scale + 2 * margin;

    auto transformX = [&](double x) { return (x - minX) * scale + margin; };
    auto transformY = [&](double y) { return height - ((y - minY) * scale + margin); };

    std::ofstream svg(filename);
    svg << "<svg xmlns='http://www.w3.org/2000/svg' width='" << width << "' height='" << height << "'>\n";

    // Delaunay edges in gray
    svg << "<g stroke='#cccccc' stroke-width='1'>\n";
    for (const auto& tri : triangles) {
        for (int i = 0; i < 3; ++i) {
            int a = tri[i], b = tri[(i + 1) % 3];
            svg << "<line x1='" << transformX(points[a].x) << "' y1='" << transformY(points[a].y)
                << "' x2='" << transformX(points[b].x) << "' y2='" << transformY(points[b].y) << "' />\n";
        }
    }
    svg << "</g>\n";


    std::map<std::pair<int, int>, std::vector<size_t>> edge_to_routes;
    for (size_t route_idx = 0; route_idx < all_routes_edges.size(); ++route_idx) {
        for (const auto& e : all_routes_edges[route_idx]) {
            int a = std::min(e.first, e.second);
            int b = std::max(e.first, e.second);
            edge_to_routes[{a, b}].push_back(route_idx);
        }
    }


    double offset_step = 3.0;
    for (const auto& [edge, routes] : edge_to_routes) {
        int a = edge.first;
        int b = edge.second;
        double x1 = transformX(points[a].x);
        double y1 = transformY(points[a].y);
        double x2 = transformX(points[b].x);
        double y2 = transformY(points[b].y);

        int n = (int)routes.size();

        for (int i = 0; i < n; ++i) {
            double offset = 0;
            if (n % 2 == 1) {

                offset = (i - n / 2) * offset_step;
            }
            else {

                offset = (i - n / 2 + 0.5) * offset_step;
            }

            double ox1 = x1, oy1 = y1, ox2 = x2, oy2 = y2;
            offset_line(ox1, oy1, ox2, oy2, offset);

            const std::string& color = route_colors[routes[i] % route_colors.size()];
            svg << "<line stroke='" << color << "' stroke-width='3' "
                << "x1='" << ox1 << "' y1='" << oy1
                << "' x2='" << ox2 << "' y2='" << oy2 << "' />\n";
        }
    }

    // Vertices in blue with labels
    svg << "<g fill='blue' font-family='Arial' font-size='14' text-anchor='middle' dominant-baseline='middle'>\n";
    for (size_t i = 0; i < points.size(); ++i) {
        double cx = transformX(points[i].x);
        double cy = transformY(points[i].y);

        // Draw the vertex circle
        svg << "<circle cx='" << cx << "' cy='" << cy << "' r='6' />\n";

        // Draw the vertex number (offset a bit above)
        svg << "<text x='" << cx << "' y='" << (cy - 12) << "' fill='black'>" << i << "</text>\n";

        // For first and last vertex add labels "start" and "meta"
        if (i == 0) {
            svg << "<text x='" << cx << "' y='" << (cy + 20) << "' fill='green' font-weight='bold'>start</text>\n";
        }
        else if (i == points.size() - 1) {
            svg << "<text x='" << cx << "' y='" << (cy + 20) << "' fill='red' font-weight='bold'>meta</text>\n";
        }
    }
    svg << "</g>\n";


    svg << "<g font-family='Arial' font-size='14'>\n";
    double legend_x = width - legend_width + 10;
    double legend_y = 30;
    svg << "<text x='" << legend_x << "' y='" << legend_y - 20 << "' font-weight='bold'>Legenda tras:</text>\n";

    for (size_t i = 0; i < all_routes_edges.size(); ++i) {
        const std::string& color = route_colors[i % route_colors.size()];
        svg << "<rect x='" << legend_x << "' y='" << legend_y - 12 << "' width='20' height='12' fill='" << color << "' />\n";
        svg << "<text x='" << (legend_x + 30) << "' y='" << legend_y << "' fill='black'>Trasa #" << (i + 1)
            << "</text>\n";
        legend_y += 25;
    }
    svg << "</g>\n";

    svg << "</svg>\n";
    svg.close();
    std::cout << "SVG visualization written to " << filename << std::endl;
}
