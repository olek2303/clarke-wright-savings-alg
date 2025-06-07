#include "geometry/visualization.h"

#include <CDT.h>
#include <iostream>
#include <fstream>

void drawSVG(const std::vector<Point>& points, const std::vector<std::pair<int, int>>& output_edges, const std::string& filename)
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
    double scale = 25; // correlated to vertices' domain
    double width = (maxX - minX) * scale + 2 * margin;
    double height = (maxY - minY) * scale + 2 * margin;

    auto transformX = [&](double x) { return (x - minX) * scale + margin; };
    auto transformY = [&](double y) { return height - ((y - minY) * scale + margin); };

    // Vertices as CDT points
    std::vector<CDT::V2d<double>> cdt_points;
    for (const auto& p : points)
        cdt_points.emplace_back(p.x, p.y);

    CDT::Triangulation<double> cdt;
    cdt.insertVertices(cdt_points);
    cdt.eraseSuperTriangle();

    // SVG stream
    std::ofstream svg(filename);
    svg << "<svg xmlns='http://www.w3.org/2000/svg' width='" << width << "' height='" << height << "'>\n";

    // Delaunay edges in gray
    svg << "<g stroke='#cccccc' stroke-width='1'>\n";
    for (const auto& tri : cdt.triangles) {
        for (int i = 0; i < 3; ++i) {
            int a = tri.vertices[i], b = tri.vertices[(i + 1) % 3];
            svg << "<line x1='" << transformX(points[a].x) << "' y1='" << transformY(points[a].y)
                << "' x2='" << transformX(points[b].x) << "' y2='" << transformY(points[b].y) << "' />\n";
        }
    }
    svg << "</g>\n";

    // Output edges in red
    svg << "<g stroke='red' stroke-width='3'>\n";
    for (auto& e : output_edges) {
        svg << "<line x1='" << transformX(points[e.first].x) << "' y1='" << transformY(points[e.first].y)
            << "' x2='" << transformX(points[e.second].x) << "' y2='" << transformY(points[e.second].y) << "' />\n";
    }
    svg << "</g>\n";

    // Vertices in blue
    svg << "<g fill='blue'>\n";
    for (const auto& p : points)
        svg << "<circle cx='" << transformX(p.x) << "' cy='" << transformY(p.y) << "' r='6' />\n";
    svg << "</g>\n";

    svg << "</svg>\n";
    svg.close();
    std::cout << "SVG visualization written to " << filename << std::endl;
}
