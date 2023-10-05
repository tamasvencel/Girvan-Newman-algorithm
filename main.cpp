#include <iostream>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

const int N = 100;

vector<vector<int>> adjList(N, vector<int>(N, 0));
vector<int> dist(N, -1);
vector<int> numShortestPaths(N, 0);  // Number of shortest paths to each node
vector<pair<pair<int, int>, double>> flows;

int n, m;

void bfs(int start, vector<pair<int, int>> &bfsOrder);

int main(int args, char *argv[]) {
    // store argv[1] in a string variable
    string filename = argv[1];

    ifstream inputFile(filename);

    if (!inputFile.is_open()) {
        cerr << "Could not open the file " << filename << endl;
        return 1;
    }

    inputFile >> n >> m;

    vector<pair<int, int>> bfsOrder(n, make_pair(0, 0));

    int x, y;
    for (int i = 0; i < m; ++i) {
        inputFile >> x >> y;
        --x;
        --y;
        adjList[x][y] = 1;
        adjList[y][x] = 1;
    }

    int startingNode = atoi(argv[2]);

    bfs(startingNode - 1, bfsOrder);

    inputFile.close();

    return 0;
}

void bfs(int start, vector<pair<int, int>> &bfsOrder) {
    vector<int> q(N);
    q[0] = start;
    dist[start] = 0;
    numShortestPaths[start] = 1;  // There's one path to the starting node
    int head = 0, rear = 0, k = 0;
    while (head <= rear) {
        int u = q[head];
        cout << u + 1 << " ";

        bfsOrder[k].first = u;
        bfsOrder[k].second = dist[u];

        for (int j = 0; j < n; ++j) {
            if (adjList[u][j] == 1 && dist[j] == -1) {
                dist[j] = dist[u] + 1;
                numShortestPaths[j] = numShortestPaths[u];  // Increment the number of shortest paths
                ++rear;
                q[rear] = j;
            } else if (adjList[u][j] == 1 && dist[j] == dist[u] + 1) {
                numShortestPaths[j] += numShortestPaths[u];  // Accumulate paths
            }
        }
        ++head;
        ++k;
    }

    cout << "\n\nBfs Order:\n";
    for (int i = 0; i < bfsOrder.size(); ++i) {
        cout << bfsOrder[i].first + 1 << " " << bfsOrder[i].second << " \n";
    }

    cout << "\nNumber of shortest paths to each node from the starting node:\n";
    for (int i = 0; i < n; ++i) {
        cout << i + 1 << " -> " << start + 1 << ": " << numShortestPaths[i] << "\n";
    }

    for (int i = bfsOrder.size() - 1; i >= 0; ++i) {
        int level = bfsOrder[i].second;
        int currentNode = bfsOrder[i].first;
        int numOfCurrentNodeNeighbours = adjList[currentNode].size();

//        for (neighbour : adjList[currentNode]) {
//                flows[currentNode] += numShortestPaths[neighbour] / level;
//        }
    }
}
