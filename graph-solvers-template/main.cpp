#include <vector>
#include "model/subsets.h"
#include "model/solver.h"
#include "geometry/visualization.h"
#include "common/types.h"
#include <CDT.h>

std::pair<std::vector<std::array<int, 3>>, std::vector<Edge>>
computeTriangulationAndEdges(const std::vector<Point>& vertices)
{
    std::vector<CDT::V2d<double>> cdt_points;
    for (const auto& p : vertices)
        cdt_points.emplace_back(p.x, p.y);

    CDT::Triangulation<double> cdt;
    cdt.insertVertices(cdt_points);
    cdt.eraseSuperTriangle();

    std::vector<Edge> edges;
    for (const auto& tri : cdt.triangles) {
        for (int i = 0; i < 3; ++i) {
            int a = tri.vertices[i], b = tri.vertices[(i + 1) % 3];
            if (a > b) std::swap(a, b);
            edges.push_back({ a, b, euclidean(vertices[a], vertices[b]) });
        }
    }

    std::sort(edges.begin(), edges.end(), [](const Edge& e1, const Edge& e2) {
        return std::tie(e1.u, e1.v) < std::tie(e2.u, e2.v);
        });
    edges.erase(std::unique(edges.begin(), edges.end(), [](const Edge& e1, const Edge& e2) {
        return e1.u == e2.u && e1.v == e2.v;
        }), edges.end());

    std::vector<std::array<int, 3>> triangles;
    for (const auto& tri : cdt.triangles) {
        std::array<int, 3> triangle = { tri.vertices[0], tri.vertices[1], tri.vertices[2] };
        triangles.push_back(triangle);
    }

    return { triangles, edges };
}


int main()
{
    std::vector<Point> vertices = generateUniquePoints(50);

    auto [triangles, edges] = computeTriangulationAndEdges(vertices);

    auto outputEdges = solveProblem(vertices, edges, 20);
    std::vector<std::vector<std::pair<int, int>>> clearGraph;

    // Pętla do rysowania każdej trasy w osobnym pliku
    for (int i = 0; i < outputEdges.size(); ++i) {
        // Stwórz dynamiczną nazwę pliku, np. "graph_with_route_0.svg", "graph_with_route_1.svg" itd.
        std::string filename = "graph_with_route_" + std::to_string(i) + ".svg";

        // Stwórz wektor zawierający tylko jedną, bieżącą trasę
        // Zakładam, że drawSVG oczekuje formatu std::vector<std::vector<...>>
        std::vector<std::vector<std::pair<int, int>>> singleRoute = { outputEdges[i] };

        // Narysuj SVG dla pojedynczej trasy
        drawSVG(vertices, singleRoute, triangles, filename);
    }
    drawSVG(vertices, clearGraph, triangles, "graph_clear.svg");

    return 0;
}
