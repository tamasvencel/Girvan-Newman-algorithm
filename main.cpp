#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <queue>
#include <cmath>

using namespace std;

void bfs(int start, int &n, int &m, vector<vector<int>> &adjacencyList, vector<pair<pair<int, int>, double>> &flows);
void printAdjList(const vector<vector<int>> &adjacencyList, const int &n, const int &m);
bool cmpFunc(const pair<pair<int, int>, double> &a, const pair<pair<int, int>, double> &b);
void addFlowsToFinalFlows(vector<pair<pair<int, int>, double>> &flows, vector<pair<pair<int, int>, double>> &finalFlows);

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <filename>" << endl;
        return 1;
    }

    string filename = argv[1];

    ifstream inputFile(filename);

    if (!inputFile.is_open()) {
        cerr << "Couldn't open the file " << filename << endl;
        return 1;
    }

    int n, m;
    inputFile >> n >> m;

    vector<vector<int>> adjacencyList(n);
    vector<pair<pair<int, int>, double>> flows;
    vector<pair<pair<int, int>, double>> finalFlows;
    vector<pair<int, int>> edgeList;

    int start, end;
    for (int i = 0; i < m; ++i) {
        inputFile >> start >> end;
        --start;
        --end;

        adjacencyList[start].push_back(end);
        adjacencyList[end].push_back(start);

        finalFlows.push_back({{start, end}, 0});

        edgeList.emplace_back(start,end);
    }

    printAdjList(adjacencyList, n, m);

//    int startingNode = atoi(argv[2]);
//    bfs(startingNode - 1);

    while (!edgeList.empty()) {
        for (int i = 0; i < n; ++i) {
            bfs(i, n, m, adjacencyList, flows);
            addFlowsToFinalFlows(flows, finalFlows);
            flows.clear();
        }

        // print finalFlows
//        cout << "\nFinal flows:\n";
//        for (auto &finalFlow: finalFlows) {
//            cout << "(" << finalFlow.first.first + 1 << "," << finalFlow.first.second + 1 << "): " << finalFlow.second << endl;
//        }

        auto maxFlow = max_element(finalFlows.begin(), finalFlows.end(), cmpFunc);
        auto max = maxFlow->second;

        for (auto &flow: finalFlows) {
            if (flow.second == max) {
                edgeList.erase(find(edgeList.begin(), edgeList.end(), flow.first));

                adjacencyList[flow.first.first].erase(find(adjacencyList[flow.first.first].begin(),
                                                               adjacencyList[flow.first.first].end(),
                                                               flow.first.second));
                adjacencyList[flow.first.second].erase(find(adjacencyList[flow.first.second].begin(),
                                                                adjacencyList[flow.first.second].end(),
                                                                flow.first.first));

                cout << endl << "edge (" << flow.first.first + 1 << "," << flow.first.second + 1 << "): "
                     << flow.second << " - has been deleted!";
            }
        }
        cout << "\n---------------------------------------" << endl;

        finalFlows.clear();
    }

    if (edgeList.empty()) {
        cout << "\nAll the edges have been successfully deleted!" << endl;
    }
//    printAdjList(adjacencyList, n, m);

    inputFile.close();
    return 0;
}

void bfs(int start, int &n, int &m, vector<vector<int>> &adjacencyList, vector<pair<pair<int, int>, double>> &flows) {
    vector<int> dist(n, -1);
    vector<int> numShortestPaths(n, 0);
    vector<int> levels(n);
    vector<int> bfsOrder;

    queue<int> q;

    q.push(start);
    dist[start] = 0;
    numShortestPaths[start] = 1;

    while (!q.empty()) {
        int current = q.front();
        q.pop();

        levels[current] = dist[current];
        bfsOrder.push_back(current);

        for (auto &neighbour : adjacencyList[current]) {
            if (dist[neighbour] == -1) {
                dist[neighbour] = dist[current] + 1;
                numShortestPaths[neighbour] = numShortestPaths[current];  // Increment the number of shortest paths
                q.push(neighbour);
            } else if (dist[neighbour] == dist[current] + 1) {
                numShortestPaths[neighbour] += numShortestPaths[current];  // Accumulate paths
            }
        }
    }

//    cout << "\nLevels:\n";
//    for (int i = 0; i < n; ++i) {
//        cout << i + 1 << "-" << levels[i] << endl;
//    }
//
//    cout << "\nBFS order:\n";
//    for (int i = 0; i < bfsOrder.size() - 1; ++i) {
//        cout << bfsOrder[i] + 1 << " ";
//    }
//
//    cout << "\n\nNumber of shortest paths to each node from the starting node:\n";
//    for (int i = 0; i < n; ++i) {
//        cout << i + 1 << " -> " << start + 1 << ": " << numShortestPaths[i] << "\n";
//    }

    int e = 0;
    for (int i = bfsOrder.size() - 1; i >= 0; --i) {
        int levelOfCurrentNode = levels[bfsOrder[i]];
        int currentNode = bfsOrder[i];
        double sumOfPreviousFlows = 0.0;

        for (pair<pair<int, int>, double> &flow: flows) {
            if (flow.first.second == currentNode) {
                sumOfPreviousFlows += flow.second;
            }
        }

        for (auto &neighbour: adjacencyList[currentNode]) {
            if (levels[neighbour] < levelOfCurrentNode) {
                flows.push_back({{currentNode, neighbour}, 0.0}); // Initialize new edge in flows
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

void printAdjList(const vector<vector<int>> &adjacencyList, const int &n, const int &m) {
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
}

bool cmpFunc(const pair<pair<int, int>, double> &a, const pair<pair<int, int>, double> &b) {
    return a.second < b.second;
}

void addFlowsToFinalFlows(vector<pair<pair<int, int>, double>> &flows, vector<pair<pair<int, int>, double>> &finalFlows) {
    for (auto &flow : flows) {
        bool found = false;
        for (auto &finalFlow : finalFlows) {
            if (finalFlow.first == flow.first || finalFlow.first == make_pair(flow.first.second, flow.first.first)) {
                finalFlow.second += flow.second;
                found = true;
                break;
            }
        }
        if (!found) {
            finalFlows.push_back(flow);
        }
    }
}
