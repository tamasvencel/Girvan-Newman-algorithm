#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <queue>
#include <cmath>

using namespace std;

const int N = 100;

vector<vector<int>> adjList(N, vector<int>(N, 0));
vector<int> dist(N, -1);
vector<int> numShortestPaths(N, 0);  // Number of shortest paths to each node
vector<pair<pair<int, int>, double>> flows;

int n, m;

void bfs(int start);
bool cmpFunc(const pair<pair<int, int>, double>& a, const pair<pair<int, int>, double>& b);

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
        adjList[x][y] = 1;
        adjList[y][x] = 1;
    }

    int startingNode = atoi(argv[2]);

    bfs(startingNode - 1);

    inputFile.close();

    return 0;
}

void bfs(int start) {
    queue<int> q;
    q.push(start);
    dist[start] = 0;
    numShortestPaths[start] = 1;  // There's one path to the starting node
    int k = 0;

    vector<pair<int, int>> levels(n, make_pair(0, 0));
    vector<int> bfsOrder(n);

    while (!q.empty()) {
        int u = q.front();
        q.pop();

        levels[u].first = u;
        levels[u].second = dist[u];
        bfsOrder[k] = u;

        for (int j = 0; j < n; ++j) {
            if (adjList[u][j] == 1 && dist[j] == -1) {
                dist[j] = dist[u] + 1;
                numShortestPaths[j] = numShortestPaths[u];  // Increment the number of shortest paths
                q.push(j);
            } else if (adjList[u][j] == 1 && dist[j] == dist[u] + 1) {
                numShortestPaths[j] += numShortestPaths[u];  // Accumulate paths
            }
        }
        ++k;
    }

    cout << "\n\nLevels:\n";
    for (pair<int, int> i: levels) {
        cout << i.first + 1 << " - " << i.second << "\n";
    }

    cout << "\nBFS order:\n\n";
    for (int i = 0; i < k; ++i) {
        cout << bfsOrder[i] + 1 << " ";
    }

    cout << "\nNumber of shortest paths to each node from the starting node:\n";
    for (int i = 0; i < n; ++i) {
        cout << i + 1 << " -> " << start + 1 << ": " << numShortestPaths[i] << "\n";
    }

    int e = 0;
    for (int i = bfsOrder.size() - 1; i >= 0; --i) {
        int level = levels[bfsOrder[i]].second;
        int currentNode = bfsOrder[i];
        double sumOfPreviousFlows = 0.0;

        for (int neighbour = 0; neighbour < n; ++neighbour) {
            int neighbourLevel1 = levels[neighbour].second;
            if (adjList[currentNode][neighbour] == 1 && neighbourLevel1 > level) {
                for (pair<pair<int, int>, double> &flow: flows) {
                    if (flow.first.second == currentNode + 1) {
                        sumOfPreviousFlows += flow.second;
                    }
                }
            }
        }

        for (int neighbour = 0; neighbour < n; ++neighbour) {
            int neighbourLevel2 = levels[neighbour].second;
            if (adjList[currentNode][neighbour] == 1 && neighbourLevel2 < level) {
                flows.push_back({{currentNode + 1, neighbour + 1}, 0.0}); // Initialize new edge in flows
                flows[e].second +=
                        ((sumOfPreviousFlows + 1) / (double)(numShortestPaths[currentNode])) *
                        numShortestPaths[neighbour];
                ++e;
            }
        }
    }


    cout << "\n\nFlows:\n";
    for (pair<pair<int, int>, double> flow: flows) {
        cout << "(" << flow.first.first << "," << flow.first.second << "): " << flow.second << "\n";
    }

    sort(flows.begin(), flows.end(), cmpFunc);

    cout << "\n\nFlows after sorting:\n";
    for (pair<pair<int, int>, double> flow: flows) {
        cout << "(" << flow.first.first << "," << flow.first.second << "): " << flow.second << "\n";
    }

    cout << "\nThe highest flow is on edge: (" << flows[flows.size()-1].first.first << "," << flows[flows.size()-1].first.second << "): " << flows[flows.size()-1].second << "\n";
}

bool cmpFunc(const pair<pair<int, int>, double>& a, const pair<pair<int, int>, double>& b) {
    return a.second < b.second;
}
