#include "model/solver.h"
#include <ilcplex/ilocplex.h>

#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <random>
#include <limits>
#include <cmath>
#include <stdexcept>
#include <thread>
#include <queue>
#include <iomanip>

#include <unordered_map>

// Funkcja do obliczania podobieństwa między trasami (Jaccard similarity)
double routeSimilarity(const std::vector<std::pair<int, int>>& route1,
    const std::vector<std::pair<int, int>>& route2) {
    std::set<std::pair<int, int>> edges1(route1.begin(), route1.end());
    std::set<std::pair<int, int>> edges2(route2.begin(), route2.end());

    std::set<std::pair<int, int>> intersection;
    std::set_intersection(edges1.begin(), edges1.end(),
        edges2.begin(), edges2.end(),
        std::inserter(intersection, intersection.begin()));

    std::set<std::pair<int, int>> union_set;
    std::set_union(edges1.begin(), edges1.end(),
        edges2.begin(), edges2.end(),
        std::inserter(union_set, union_set.begin()));

    if (union_set.empty()) return 0.0;
    return static_cast<double>(intersection.size()) / union_set.size();
}

// Funkcja do sprawdzenia czy trasa jest wystarczająco różna od istniejących
bool isRouteDifferent(const std::vector<std::pair<int, int>>& newRoute,
    const std::vector<std::vector<std::pair<int, int>>>& existingRoutes,
    double minDifferenceThreshold = 0.3) {
    for (const auto& existingRoute : existingRoutes) {
        if (routeSimilarity(newRoute, existingRoute) > (1.0 - minDifferenceThreshold)) {
            return false;
        }
    }
    return true;
}

// Funkcja do znajdowania alternatywnej trasy używając k-shortest paths
std::vector<std::pair<int, int>> findAlternativePath(
    const std::vector<Point>& vertices,
    const std::vector<Edge>& edges,
    const std::set<int>& avoidNodes,
    double costMultiplier = 1.0) {

    int start = 0;
    int end = (int)vertices.size() - 1;

    // Buduj graf z modyfikowanymi kosztami
    std::unordered_map<int, std::vector<std::pair<int, double>>> adj;
    std::set<std::pair<int, int>> edge_set;

    for (const auto& edge : edges) {
        if (avoidNodes.count(edge.u) || avoidNodes.count(edge.v)) continue;

        double modifiedCost = edge.cost;
        // Zwiększ koszt krawędzi prowadzących do unikanych węzłów
        if (avoidNodes.count(edge.u) || avoidNodes.count(edge.v)) {
            modifiedCost *= costMultiplier;
        }

        adj[edge.u].emplace_back(edge.v, modifiedCost);
        adj[edge.v].emplace_back(edge.u, modifiedCost);
        edge_set.insert({ edge.u, edge.v });
        edge_set.insert({ edge.v, edge.u });
    }

    // Dijkstra do znajdowania najkrótszej ścieżki
    std::vector<double> dist(vertices.size(), std::numeric_limits<double>::infinity());
    std::vector<int> parent(vertices.size(), -1);
    std::priority_queue<std::pair<double, int>,
        std::vector<std::pair<double, int>>,
        std::greater<std::pair<double, int>>> pq;

    dist[start] = 0;
    pq.push({ 0, start });

    while (!pq.empty()) {
        auto [d, u] = pq.top();
        pq.pop();

        if (d > dist[u]) continue;

        for (auto [v, cost] : adj[u]) {
            if (dist[u] + cost < dist[v]) {
                dist[v] = dist[u] + cost;
                parent[v] = u;
                pq.push({ dist[v], v });
            }
        }
    }

    // Rekonstruuj ścieżkę
    std::vector<std::pair<int, int>> path;
    if (dist[end] == std::numeric_limits<double>::infinity()) {
        return path; // Brak ścieżki
    }

    int current = end;
    while (parent[current] != -1) {
        path.push_back({ parent[current], current });
        current = parent[current];
    }

    std::reverse(path.begin(), path.end());
    return path;
}

std::vector<std::vector<std::pair<int, int>>> solveProblem(
    const std::vector<Point>& vertices,
    const std::vector<Edge>& edges,
    int n_of_roads) {

    int start = 0;
    int end = (int)vertices.size() - 1;

    // === Adjacency and Edge set for validation ===
    std::unordered_map<int, std::vector<std::pair<int, double>>> adj;
    std::set<std::pair<int, int>> edge_set;
    for (const auto& edge : edges) {
        adj[edge.u].emplace_back(edge.v, edge.cost);
        adj[edge.v].emplace_back(edge.u, edge.cost);
        edge_set.insert({ edge.u, edge.v });
        edge_set.insert({ edge.v, edge.u });
    }

    // === Initial routes ===
    std::unordered_map<int, int> nodeToRoute;
    std::vector<std::vector<int>> routes;
    for (int i = 0; i < (int)vertices.size(); ++i) {
        if (i == start || i == end) continue;
        routes.push_back({ start, i, end });
        nodeToRoute[i] = (int)routes.size() - 1;
    }

    // === Compute Savings ===
    struct Saving {
        int i, j;
        double value;
    };
    std::vector<Saving> savings;
    for (const auto& edge : edges) {
        int i = edge.u;
        int j = edge.v;
        if (i == start || j == start || i == end || j == end) continue;

        const auto& p0 = vertices[start];
        const auto& pi = vertices[i];
        const auto& pj = vertices[j];

        double c0i = std::hypot(p0.x - pi.x, p0.y - pi.y);
        double c0j = std::hypot(p0.x - pj.x, p0.y - pj.y);

        savings.push_back({ i, j, c0i + c0j - edge.cost });
    }

    std::sort(savings.begin(), savings.end(), [](const Saving& a, const Saving& b) {
        return a.value > b.value;
        });

    // === Route merging ===
    for (const auto& s : savings) {
        if (nodeToRoute.count(s.i) == 0 || nodeToRoute.count(s.j) == 0) continue;
        int ri = nodeToRoute[s.i];
        int rj = nodeToRoute[s.j];
        if (ri == rj || routes[ri].empty() || routes[rj].empty()) continue;

        auto& route_i = routes[ri];
        auto& route_j = routes[rj];

        // Merge if i at end-1 of route_i and j at 1 of route_j
        int back_i = route_i[route_i.size() - 2];
        int front_j = route_j[1];

        if (back_i == s.i && front_j == s.j &&
            edge_set.count({ s.i, s.j })) {

            // valid merge
            route_i.pop_back(); // remove end
            route_j.erase(route_j.begin()); // remove start
            route_i.insert(route_i.end(), route_j.begin(), route_j.end());

            for (int k = 1; k < (int)route_j.size() - 1; ++k) {
                nodeToRoute[route_j[k]] = ri;
            }

            route_j.clear();
        }
    }

    // === Include all non-empty routes ===
    std::vector<std::vector<int>> candidates;
    for (const auto& route : routes) {
        if (!route.empty() && route.front() == start && route.back() == end) {
            candidates.push_back(route);
        }
    }

    // Sort by number of interior points (more = better)
    std::sort(candidates.begin(), candidates.end(), [](const auto& a, const auto& b) {
        return a.size() > b.size();
        });

    // Take top N and convert to edge pairs
    std::vector<std::vector<std::pair<int, int>>> finalRoutes;
    for (int i = 0; i < (int)candidates.size(); ++i) {
        const auto& route = candidates[i];
        bool valid = true;

        // Must start and end at correct nodes
        if (route.front() != start || route.back() != end) {
            continue;
        }

        std::vector<std::pair<int, int>> path;
        for (size_t j = 0; j + 1 < route.size(); ++j) {
            int u = route[j];
            int v = route[j + 1];
            if (edge_set.count({ u, v }) == 0) {
                valid = false;
                break;
            }
            path.emplace_back(u, v);
        }

        if (valid) {
            finalRoutes.push_back(path);
        }

        if ((int)finalRoutes.size() >= n_of_roads) break;
    }

    return finalRoutes;
}

std::vector<std::vector<std::pair<int, int>>> solveMultipleRoutes(
    const std::vector<Point>& vertices,
    const std::vector<Edge>& edges,
    int n_of_roads) {

    std::vector<std::vector<std::pair<int, int>>> allRoutes;
    std::set<int> usedNodes; // Węzły używane w już znalezionych trasach

    std::default_random_engine rng(std::random_device{}());
    std::uniform_real_distribution<double> noiseDist(0.8, 1.2); // Szum multiplikatywny
    std::uniform_real_distribution<double> avoidDist(2.0, 5.0); // Mnożnik dla unikanych węzłów

    int maxAttempts = n_of_roads * 50;
    int attempts = 0;

    while ((int)allRoutes.size() < n_of_roads && attempts < maxAttempts) {
        ++attempts;

        // Strategia 1: Znajdź trasę używając Clark-Wright z szumem
        std::vector<Edge> noisyEdges;
        for (const auto& e : edges) {
            double noisyMultiplier = noiseDist(rng);
            // Zwiększ koszt krawędzi prowadzących do już używanych węzłów
            if (usedNodes.count(e.u) || usedNodes.count(e.v)) {
                noisyMultiplier *= avoidDist(rng);
            }
            noisyEdges.push_back({ e.u, e.v, e.cost * noisyMultiplier });
        }

        auto cwRoutes = solveProblem(vertices, noisyEdges, 1);

        // Strategia 2: Jeśli Clark-Wright nie dał dobrej trasy, użyj alternatywnej metody
        std::vector<std::pair<int, int>> newRoute;
        if (!cwRoutes.empty()) {
            newRoute = cwRoutes[0];
        }
        else {
            // Spróbuj znaleźć alternatywną ścieżkę
            newRoute = findAlternativePath(vertices, edges, usedNodes, avoidDist(rng));
        }

        if (newRoute.empty()) continue;

        // Sprawdź czy trasa jest wystarczająco różna
        if (!isRouteDifferent(newRoute, allRoutes, 0.4)) {
            // Jeśli za podobna, spróbuj z większym unikaniem używanych węzłów
            if (attempts % 10 == 0) {
                std::set<int> stronglyAvoidedNodes;
                for (const auto& route : allRoutes) {
                    for (const auto& edge : route) {
                        if (edge.first != 0 && edge.first != (int)vertices.size() - 1) {
                            stronglyAvoidedNodes.insert(edge.first);
                        }
                        if (edge.second != 0 && edge.second != (int)vertices.size() - 1) {
                            stronglyAvoidedNodes.insert(edge.second);
                        }
                    }
                }
                newRoute = findAlternativePath(vertices, edges, stronglyAvoidedNodes, 10.0);

                if (newRoute.empty() || !isRouteDifferent(newRoute, allRoutes, 0.3)) {
                    continue;
                }
            }
            else {
                continue;
            }
        }

        allRoutes.push_back(newRoute);

        // Aktualizuj zestaw używanych węzłów
        for (const auto& edge : newRoute) {
            if (edge.first != 0 && edge.first != (int)vertices.size() - 1) {
                usedNodes.insert(edge.first);
            }
            if (edge.second != 0 && edge.second != (int)vertices.size() - 1) {
                usedNodes.insert(edge.second);
            }
        }

        // Co jakiś czas resetuj część używanych węzłów, żeby nie zablokować wszystkich możliwości
        if (attempts % 20 == 0 && usedNodes.size() > vertices.size() / 3) {
            std::vector<int> nodesToRemove(usedNodes.begin(), usedNodes.end());
            std::shuffle(nodesToRemove.begin(), nodesToRemove.end(), rng);
            // Usuń połowę używanych węzłów
            for (size_t i = 0; i < nodesToRemove.size() / 2; ++i) {
                usedNodes.erase(nodesToRemove[i]);
            }
        }
    }

    // Jeśli nadal za mało tras, spróbuj z mniej restrykcyjnymi kryteriami
    if ((int)allRoutes.size() < n_of_roads) {
        int remainingRoutes = n_of_roads - (int)allRoutes.size();
        for (int i = 0; i < remainingRoutes && i < maxAttempts; ++i) {
            std::vector<Edge> veryNoisyEdges;
            for (const auto& e : edges) {
                double multiplier = noiseDist(rng) * noiseDist(rng); // Więcej szumu
                veryNoisyEdges.push_back({ e.u, e.v, e.cost * multiplier });
            }

            auto routes = solveProblem(vertices, veryNoisyEdges, 1);
            if (!routes.empty() && isRouteDifferent(routes[0], allRoutes, 0.2)) {
                allRoutes.push_back(routes[0]);
            }
        }
    }

    // Ostateczne wypełnienie (jeśli naprawdę nic nie działa)
    while ((int)allRoutes.size() < n_of_roads) {
        if (!allRoutes.empty()) {
            // Dodaj zmodyfikowaną wersję pierwszej trasy
            auto modifiedRoute = allRoutes[0];
            if (modifiedRoute.size() > 2) {
                // Usuń jeden węzeł środkowy
                modifiedRoute.erase(modifiedRoute.begin() + 1);
            }
            allRoutes.push_back(modifiedRoute);
        }
        else {
            break;
        }
    }

    // === Wypisywanie znalezionych tras ===
    std::cout << "\n=== ZNALEZIONE TRASY ===" << std::endl;
    for (int i = 0; i < (int)allRoutes.size(); ++i) {
        const auto& route = allRoutes[i];

        // Oblicz długość trasy
        double totalLength = 0.0;
        std::cout << "Trasa " << (i) << ": ";

        if (!route.empty()) {
            // Wypisz pierwszy węzeł
            std::cout << route[0].first;

            // Wypisz kolejne węzły i oblicz długość
            for (const auto& edge : route) {
                std::cout << " -> " << edge.second;

                // Znajdź koszt tej krawędzi w oryginalnych danych
                for (const auto& originalEdge : edges) {
                    if ((originalEdge.u == edge.first && originalEdge.v == edge.second) ||
                        (originalEdge.u == edge.second && originalEdge.v == edge.first)) {
                        totalLength += originalEdge.cost;
                        break;
                    }
                }
            }

            std::cout << " | Dlugosc: " << std::fixed << std::setprecision(2) << totalLength << std::endl;
        }
        else {
            std::cout << "PUSTA" << std::endl;
        }
    }
    std::cout << "========================\n" << std::endl;

    return allRoutes;
}