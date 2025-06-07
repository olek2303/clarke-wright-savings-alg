// main.cpp : Defines the entry point for the application.
//

#include <vector>
#include "model/subsets.h"
#include "model/solver.h"
#include "geometry/visualization.h"
#include "common/types.h"

int main()
{
	std::vector<Point> vertices = generateUniquePoints(15);
    std::vector<Edge> edges;
    int n = vertices.size();
    for (int i = 0; i < n; ++i)
    {
        for (int j = i + 1; j < n; ++j)
        {
            double distance = euclidean(vertices[i], vertices[j]);
            edges.push_back({ i, j, distance });
        }
    }
    auto outputEdges = solveProblem(vertices, edges);
    drawSVG(vertices, outputEdges);
    return 0;
}
