#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <queue>
#include <cmath>

using namespace std;

const int N = 100;

vector<vector<int>> adjMatrix(N, vector<int>(N, 0));
vector<vector<int>> adjacencyList(N);
vector<int> dist(N, -1);
vector<int> numShortestPaths(N, 0);
vector<pair<pair<int, int>, double>> flows;
vector<pair<pair<int, int>, double>> finalFlows;

int n, m;

void bfs(int start);

bool cmpFunc(const pair<pair<int, int>, double> &a, const pair<pair<int, int>, double> &b);

int main(int argc, char *argv[]) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <filename> <starting_node>" << endl;
        return 1;
    }

    // store argv[1] in a string variable
    string filename = argv[1];

    ifstream inputFile(filename);

    if (!inputFile.is_open()) {
        cerr << "Could not open the file " << filename << endl;
        return 1;
    }

    inputFile >> n >> m;

    int x, y;
    for (int i = 0; i < m; ++i) {
        inputFile >> x >> y;
        --x;
        --y;

        adjMatrix[x][y] = 1;
        adjMatrix[y][x] = 1;

        adjacencyList[x].push_back(y);
        adjacencyList[y].push_back(x);
    }

    cout << "Adjacency List:" << endl;
    for (int i = 0; i < n; ++i) {
        cout << i + 1 << ": ";
        for (int j = 0; j < adjacencyList[i].size(); ++j) {
            cout << adjacencyList[i][j] + 1; // Adding 1 to convert from 0-based to 1-based indexing
            if (j < adjacencyList[i].size() - 1) {
                cout << ", ";
            }
        }
        cout << endl;
    }

    int startingNode = atoi(argv[2]);

    bfs(startingNode - 1);

    for (int i = 0; i < flows.size(); ++i) {
        finalFlows.push_back({{flows[i].first.first, flows[i].first.second}, flows[i].second});
    }

    flows.clear();

    for (int i = 1; i < n; ++i) {
        bfs(i);

        for (pair<pair<int, int>, double> flow: flows) {
            for (pair<pair<int, int>, double> &finalFlow: finalFlows) {
                if (flow.first.first == finalFlow.first.first && flow.first.second == finalFlow.first.second) {
                    finalFlow.second += flow.second;
                }
            }
        }

        flows.clear();
    }

    sort(finalFlows.begin(), finalFlows.end(), cmpFunc);

    cout << "\nfinal flows:\n";
    for (pair<pair<int, int>, double> finalFlow: finalFlows) {
        cout << "(" << finalFlow.first.first + 1 << "," << finalFlow.first.second + 1 << "): " << finalFlow.second
             << endl;
    }

    inputFile.close();

    return 0;
}

void bfs(int start) {
    queue<int> q;
    q.push(start);
    dist[start] = 0;
    numShortestPaths[start] = 1;
    int k = 0;

    vector<int> levels(n);
    vector<int> bfsOrder(n);

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        levels[u] = dist[u];
        bfsOrder[k] = u;

        for (int j = 0; j < n; ++j) {
            if (adjMatrix[u][j] == 1 && dist[j] == -1) {
                dist[j] = dist[u] + 1;
                numShortestPaths[j] = numShortestPaths[u];  // Increment the number of shortest paths
                q.push(j);
            } else if (adjMatrix[u][j] == 1 && dist[j] == dist[u] + 1) {
                numShortestPaths[j] += numShortestPaths[u];  // Accumulate paths
            }
        }
        ++k;
    }

//    cout << "\nLevels:\n";
//    for (int i = 0; i < n; ++i) {
//        cout << i + 1 << "-" << levels[i] << endl;
//    }
//
//    cout << "\nBFS order:\n";
//    for (int i = 0; i < k; ++i) {
//        cout << bfsOrder[i] + 1 << " ";
//    }
//
//    cout << "\n\nNumber of shortest paths to each node from the starting node:\n";
//    for (int i = 0; i < n; ++i) {
//        cout << i + 1 << " -> " << start + 1 << ": " << numShortestPaths[i] << "\n";
//    }

    int e = 0;
    for (int i = bfsOrder.size() - 1; i >= 0; --i) {
        int level = levels[bfsOrder[i]];
        int currentNode = bfsOrder[i];
        double sumOfPreviousFlows = 0.0;

        for (pair<pair<int, int>, double> &flow: flows) {
            if (flow.first.second == currentNode + 1) {
                sumOfPreviousFlows += flow.second;
            }
        }

        for (auto &neighbour: adjacencyList[currentNode]) {
            if (levels[neighbour] < level) {
                flows.push_back({{currentNode + 1, neighbour + 1}, 0.0}); // Initialize new edge in flows
                flows[e].second += (((sumOfPreviousFlows + 1) / (double) (numShortestPaths[currentNode])) *
                                    numShortestPaths[neighbour]);
                ++e;
            }
        }
    }


//    cout << "\nFlows:\n";
//    for (pair<pair<int, int>, double> flow: flows) {
//        cout << "(" << flow.first.first << "," << flow.first.second << "): " << flow.second << "\n";
//    }
//
//    sort(flows.begin(), flows.end(), cmpFunc);
//
//    cout << "\nFlows after sorting:\n";
//    for (pair<pair<int, int>, double> flow: flows) {
//        cout << "(" << flow.first.first << "," << flow.first.second << "): " << flow.second << "\n";
//    }
//
//    cout << "\nThe highest flow is on edge: (" << flows[flows.size() - 1].first.first << ","
//         << flows[flows.size() - 1].first.second << "): " << flows[flows.size() - 1].second << "\n";
}

bool cmpFunc(const pair<pair<int, int>, double> &a, const pair<pair<int, int>, double> &b) {
    return a.second < b.second;
}
