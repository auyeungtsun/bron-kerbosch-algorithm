#include <iostream>
#include <vector>
#include <set>
#include <algorithm>
#include <cassert>
#include <stdexcept>

using namespace std;

class Graph {
public:
    int num_vertices;
    vector<vector<bool>> adj_matrix;

    /**
     * @brief Constructor for the Graph class.
     * @param n The number of vertices in the graph.
     */
    Graph(int n) : num_vertices(n), adj_matrix(n, vector<bool>(n, false)) {}

    /**
     * @brief Adds an undirected edge between vertices u and v.
     * @param u The first vertex.
     * @param v The second vertex.
     */
    void add_edge(int u, int v) {
        if (u >= 0 && u < num_vertices && v >= 0 && v < num_vertices) {
            adj_matrix[u][v] = true;
            adj_matrix[v][u] = true;
        }
    }
    
    /**
     * @brief Finds all maximal cliques in the graph using the Bron-Kerbosch algorithm.
     * @brief A clique is a subset of vertices in a graph where every two distinct vertices are connected by an edge.
     *        In other words, a clique is a complete subgraph of the original graph.
     *        A maximal clique is a clique that cannot be extended by including one more adjacent vertex, meaning
     *        it is not a subset of a larger clique.
     * @return A vector of sets, where each set represents a maximal clique.
     * @note Time Complexity: The worst-case time complexity of the Bron-Kerbosch algorithm
     *       is O(3^(n/3)), where n is the number of vertices.
     * @note Space Complexity: The space complexity is O(n + m), where n is the number of vertices, and m is the number of edges.
     */
    vector<set<int>> find_max_cliques() {
        // 'cliques' stores all maximal cliques found.
        // 'R' is the current clique being built.
        // 'P' is the set of candidate vertices that could be added to the clique.
        // 'X' is the set of vertices that have already been processed and cannot be added to the clique.
        vector<set<int>> cliques;
        set<int> R, P, X;
        for (int i = 0; i < num_vertices; ++i) {
            P.insert(i);
        }
        if (num_vertices > 0) {
            bron_kerbosch(R, P, X, cliques);
        }
        return cliques;
    }

private:
    void bron_kerbosch(set<int>& R, set<int>& P, set<int>& X, vector<set<int>>& cliques) {
        if (P.empty() && X.empty()) {
            cliques.push_back(R);
            return;
        }
        if (P.empty()) {
             return;
        }
        int u = *P.begin();
        for (int v : P) {
            if(X.count(v)) continue;
            if (degree(v) > degree(u))
              u = v;
        }
        set<int> P_minus_N;
        for (int v : P) {
          if(!is_neighbor(v,u))
            P_minus_N.insert(v);
        }

        for (int v : P_minus_N) {
            set<int> new_R = R;
            new_R.insert(v);
            set<int> new_P, new_X;
            for (int neighbor : get_neighbors(v)) {
                if (P.count(neighbor)) {
                    new_P.insert(neighbor);
                }
                if (X.count(neighbor)) {
                    new_X.insert(neighbor);
                }
            }

            bron_kerbosch(new_R, new_P, new_X, cliques);
            P.erase(v);
            X.insert(v);
        }
    }

    vector<int> get_neighbors(int v) {
        vector<int> neighbors;
        for (int i = 0; i < num_vertices; ++i) {
            if (adj_matrix[v][i]) {
                neighbors.push_back(i);
            }
        }
        return neighbors;
    }

     bool is_neighbor(int u, int v) {
        return adj_matrix[u][v];
    }

    int degree(int u){
        int count = 0;
        for(int i = 0; i < num_vertices; ++i){
            if(adj_matrix[u][i])
            count++;
        }
        return count;
    }
};

void test_find_max_cliques() {
    cout << "Running tests for find_max_cliques..." << endl;

    // Helper lambda for comparing results
    auto run_test = [](const string& test_name, Graph& g, vector<set<int>>& expected_cliques) {
        cout << "--- Test Case: " << test_name << " ---" << endl;
        vector<set<int>> actual_cliques = g.find_max_cliques();

        sort(expected_cliques.begin(), expected_cliques.end());
        sort(actual_cliques.begin(), actual_cliques.end());

        assert(actual_cliques == expected_cliques);
        cout << test_name << ": Passed!" << endl;
    };

    // Test Case 1: Empty Graph (0 vertices)
    {
        Graph g(0);
        vector<set<int>> expected = {};
        run_test("Empty Graph (0 Vertices)", g, expected);
    }

    // Test Case 2: 1 Vertex
    {
        Graph g(1);
        vector<set<int>> expected = {{0}};
        run_test("1 Vertex", g, expected);
    }

    // Test Case 3: 2 Vertices, No Edge
    {
        Graph g(2);
        vector<set<int>> expected = {{0}, {1}};
        run_test("2 Vertices, No Edge", g, expected);
    }

    // Test Case 4: 2 Vertices, 1 Edge
    {
        Graph g(2);
        g.add_edge(0, 1);
        vector<set<int>> expected = {{0, 1}};
        run_test("2 Vertices, 1 Edge", g, expected);
    }

    // Test Case 5: Triangle (K3)
    {
        Graph g(3);
        g.add_edge(0, 1);
        g.add_edge(1, 2);
        g.add_edge(2, 0);
        vector<set<int>> expected = {{0, 1, 2}};
        run_test("Triangle (K3)", g, expected);
    }

     // Test Case 6: Line Graph (3 vertices)
     {
        Graph g(3);
        g.add_edge(0, 1);
        g.add_edge(1, 2);
        vector<set<int>> expected = {{0, 1}, {1, 2}};
        run_test("Line Graph (3 Vertices)", g, expected);
     }

     // Test Case 7: Square Graph (C4)
     {
        Graph g(4);
        g.add_edge(0, 1);
        g.add_edge(1, 2);
        g.add_edge(2, 3);
        g.add_edge(3, 0);
        vector<set<int>> expected = {{0, 1}, {1, 2}, {2, 3}, {0, 3}};
        run_test("Square Graph (C4)", g, expected);
     }

    // Test Case 8: Complete Graph (K4)
    {
        Graph g(4);
        g.add_edge(0, 1); g.add_edge(0, 2); g.add_edge(0, 3);
        g.add_edge(1, 2); g.add_edge(1, 3);
        g.add_edge(2, 3);
        vector<set<int>> expected = {{0, 1, 2, 3}};
        run_test("Complete Graph (K4)", g, expected);
    }


    // Test Case 9: Square with one diagonal (forms two triangles)
    {
        Graph g(4);
        g.add_edge(0, 1); g.add_edge(1, 2); g.add_edge(2, 3); g.add_edge(3, 0);
        g.add_edge(0, 2);
        vector<set<int>> expected = {{0, 1, 2}, {0, 2, 3}};
        run_test("Square + 1 Diagonal (0-2)", g, expected);
    }

    // Test Case 10: Disconnected Components (Two Triangles)
    {
        Graph g(6);
        g.add_edge(0, 1); g.add_edge(1, 2); g.add_edge(2, 0);
        g.add_edge(3, 4); g.add_edge(4, 5); g.add_edge(5, 3);
        vector<set<int>> expected = {{0, 1, 2}, {3, 4, 5}};
        run_test("Disconnected (2 Triangles)", g, expected);
    }

    // Test Case 11: Pentagon (C5)
    {
        Graph g(5);
        g.add_edge(0, 1); g.add_edge(1, 2); g.add_edge(2, 3); g.add_edge(3, 4); g.add_edge(4, 0);
        vector<set<int>> expected = {{0, 1}, {1, 2}, {2, 3}, {3, 4}, {0, 4}};
        run_test("Pentagon (C5)", g, expected);
    }


    // Test Case 12: House Graph (Square base + triangle roof)
    {
        Graph g(5);
        g.add_edge(0, 1); g.add_edge(1, 2); g.add_edge(2, 3); g.add_edge(3, 0);
        g.add_edge(0, 4); g.add_edge(1, 4);
        vector<set<int>> expected = {{0, 1, 4}, {1, 2}, {2, 3}, {0, 3}};
        run_test("House Graph", g, expected);
    }

    // Test Case 13: Bron-Kerbosch Example Graph (from Wikipedia/common examples)
    {
        Graph g(6);
        g.add_edge(0, 1); g.add_edge(0, 4);
        g.add_edge(1, 2); g.add_edge(1, 4);
        g.add_edge(2, 3);
        g.add_edge(3, 4); g.add_edge(3, 5);
        // Expected Maximal Cliques: {0, 1, 4}, {1, 2}, {2, 3}, {3, 4}, {3, 5}
        vector<set<int>> expected = {{0, 1, 4}, {1, 2}, {2, 3}, {3, 4}, {3, 5}};
        run_test("Bron-Kerbosch Example", g, expected);
    }

    // Test Case 14: Graph with an isolated vertex
    {
        Graph g(4);
        g.add_edge(0, 1); g.add_edge(1, 2); g.add_edge(0, 2);
        vector<set<int>> expected = {{0, 1, 2}, {3}};
        run_test("Triangle + Isolated Vertex", g, expected);
    }

     // Test Case 15: Complete Bipartite Graph K_{3,3} (should have only edges as max cliques)
    {
        Graph g(6);
        for(int i = 0; i < 3; ++i) {
            for(int j = 3; j < 6; ++j) {
                g.add_edge(i, j);
            }
        }
        vector<set<int>> expected = {
            {0, 3}, {0, 4}, {0, 5},
            {1, 3}, {1, 4}, {1, 5},
            {2, 3}, {2, 4}, {2, 5}
        };
         run_test("Complete Bipartite K(3,3)", g, expected);
    }


    cout << "\nAll tests passed!" << endl;
}

void run_find_max_cliques_sample() {
    Graph g(5);
    g.add_edge(0, 1);
    g.add_edge(1, 2);
    g.add_edge(2, 3);
    g.add_edge(3, 4);
    g.add_edge(4, 0);
    vector<set<int>> cliques = g.find_max_cliques();
    cout << "Maximal cliques found: " << endl;
    for (const auto& clique : cliques) {
        cout << "{ ";
        for (int node : clique) {
            cout << node << " ";
        }
        cout << "}" << endl;
    }
}

int main() {
    test_find_max_cliques();
    run_find_max_cliques_sample();
    return 0;
}