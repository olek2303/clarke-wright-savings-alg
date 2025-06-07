#include "common/types.h"

#include <set>
#include <random>
#include <iostream>

bool Point::operator<(const Point& other) const
{
    return (x < other.x) || (x == other.x && y < other.y);
}

double euclidean(const Point& a, const Point& b)
{
    return std::sqrt((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

std::vector<Point> generateUniquePoints(int n, double minX, double minY, double maxX, double maxY)
{
    std::set<Point> uniquePoints;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> distX(minX, maxX);
    std::uniform_real_distribution<> distY(minY, maxY);

    const int maxAttempts = n * 10;
    int attempts = 0;

    while (uniquePoints.size() < static_cast<size_t>(n) && attempts < maxAttempts)
    {
        Point p = { distX(gen), distY(gen) };
        uniquePoints.insert(p);
        ++attempts;
    }

    if (uniquePoints.size() < static_cast<size_t>(n))
        std::cout << "Warning: Only " << uniquePoints.size() << " unique points generated after " << attempts << " attempts.\n";

    return std::vector<Point>(uniquePoints.begin(), uniquePoints.end());
}
