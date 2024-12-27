#include <iostream>
#include<string>
#include <vector>
#include <queue>
#include <cmath>
#include <limits>
#include<algorithm>
#include<iomanip>
using namespace std;

struct Node {
    int id;
    string name;
    double longitude;
    double latitude;
};

struct Graph {
    vector<Node> nodes;
    vector<vector<pair<int, double>>> adjacencyList;
};

Node createNode(int id, const string& name, double longitude, double latitude) {
    return {id, name, longitude, latitude};
}

void addNode(Graph &graph, Node node) {
    graph.nodes.push_back(node);
    graph.adjacencyList.emplace_back();
}

void addEdge(Graph &graph, int src, int dest, double weight) {
    graph.adjacencyList[src].emplace_back(dest, weight);
    graph.adjacencyList[dest].emplace_back(src, weight); // For undirected graph
}

double calculateDistance(const Node &node1, const Node &node2) {
    const double earthRadius = 6371.0; // km
    const double pi = acos(-1.0);

    double lat1 = node1.latitude * pi / 180.0;
    double lon1 = node1.longitude * pi / 180.0;
    double lat2 = node2.latitude * pi / 180.0;
    double lon2 = node2.longitude * pi / 180.0;

    double dLat = lat2 - lat1;
    double dLon = lon2 - lon1;

    double a = sin(dLat / 2) * sin(dLat / 2) +
               cos(lat1) * cos(lat2) *
               sin(dLon / 2) * sin(dLon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));

    return earthRadius * c;
}

void displayMap(const Graph &graph) {
    cout << "\n\t\t\t\t\t\033[1;36m=================== Campus Map ===================\033[0m\n\n";
    for (const Node &node : graph.nodes) {
        cout << "\t\t\t\t\t\033[1;33mNode ID: " << node.id << ", Name: " << node.name << "\033[0m\n";
        cout << "\t\t\t\t\t\033[1;32mConnections:\033[0m\n";
        for (const auto &edge : graph.adjacencyList[node.id]) {
            int neighborId = edge.first;
            double distance = edge.second;
            cout << "\t\t\t\t\t  --> " << graph.nodes[neighborId].name 
                 << " \033[1;35m(Distance: " << fixed << setprecision(3) << distance << " km)\033[0m\n";
        }
        cout << "\n";
    }
    cout << "\t\t\t\t\t\033[1;36m================ End of Map ==================\033[0m\n\n";
}

vector<int> dijkstra(const Graph& graph, int start, int end) {
    int n = graph.nodes.size();
    vector<double> dist(n, numeric_limits<double>::infinity());
    vector<int> prev(n, -1);
    
    // Using priority queue for better performance
    priority_queue<pair<double, int>, vector<pair<double, int>>, greater<pair<double, int>>> pq;
    
    dist[start] = 0;
    pq.push({0, start});
    
    while (!pq.empty()) {
        int u = pq.top().second;
        double d = pq.top().first;
        pq.pop();
        
        if (d > dist[u]) continue;
        
        for (const auto& edge : graph.adjacencyList[u]) {
            int v = edge.first;
            double weight = edge.second;
            
            if (dist[u] + weight < dist[v]) {
                dist[v] = dist[u] + weight;
                prev[v] = u;
                pq.push({dist[v], v});
            }
        }
    }
    
    vector<int> path;
    if (dist[end] != numeric_limits<double>::infinity()) {
        for (int curr = end; curr != -1; curr = prev[curr]) {
            path.push_back(curr);
        }
        reverse(path.begin(), path.end());
    }
    
    return path;
}

bool isValidPath(const vector<int>& path, const Graph& graph) {
    if (path.size() < 2) return false;
    
    for (size_t i = 0; i < path.size() - 1; i++) {
        bool hasEdge = false;
        for (const auto& edge : graph.adjacencyList[path[i]]) {
            if (edge.first == path[i + 1]) {
                hasEdge = true;
                break;
            }
        }
        if (!hasEdge) return false;
    }
    return true;
}

int findNodeByName(const Graph& graph, const string& name) {
    for (int i = 0; i < graph.nodes.size(); i++) {
        if (graph.nodes[i].name == name) {
            return i;
        }
    }
    return -1;
}

vector<int> bellmanFord(const Graph& graph, int start, int end) {
    int n = graph.nodes.size();
    vector<double> dist(n, numeric_limits<double>::infinity());
    vector<int> prev(n, -1);
    
    dist[start] = 0;
    
    // Relax all edges V-1 times
    for (int i = 0; i < n - 1; i++) {
        for (int u = 0; u < n; u++) {
            for (const auto& edge : graph.adjacencyList[u]) {
                int v = edge.first;
                double weight = edge.second;
                
                if (dist[u] != numeric_limits<double>::infinity() && 
                    dist[u] + weight < dist[v]) {
                    dist[v] = dist[u] + weight;
                    prev[v] = u;
                }
            }
        }
    }
    
    // Check for negative weight cycles
    for (int u = 0; u < n; u++) {
        for (const auto& edge : graph.adjacencyList[u]) {
            int v = edge.first;
            double weight = edge.second;
            
            if (dist[u] != numeric_limits<double>::infinity() && 
                dist[u] + weight < dist[v]) {
                cout << "\t\t\t\t\t\033[1;31mWarning: Graph contains negative weight cycle!\033[0m\n";
                return vector<int>();
            }
        }
    }
    
    // Reconstruct path
    vector<int> path;
    if (prev[end] != -1 || start == end) {
        for (int curr = end; curr != -1; curr = prev[curr]) {
            path.push_back(curr);
        }
        reverse(path.begin(), path.end());
    }
    
    return path;
}




int main() {
    Graph graph;
    // nodes
    addNode(graph, createNode(0, "Main Gate", 72.644462, 34.06817076));
    addNode(graph, createNode(1, "Clock Tower", 72.64518665, 34.06853819));
    addNode(graph, createNode(2, "Admin", 72.64384096, 34.06822645));
    addNode(graph, createNode(3, "AHA Auditorium", 72.64423934, 34.06974959));
    addNode(graph, createNode(4, "FCSE_FEE", 72.64348974, 34.06952355));
    addNode(graph, createNode(5, "FEE", 72.64305999, 34.06876414));
    addNode(graph, createNode(6, "Tennis Court", 72.64238185, 34.06836558));
    addNode(graph, createNode(7, "Sports Ground", 72.64152529, 34.06894772));
    addNode(graph, createNode(8, "FES", 72.64206225, 34.06936555));
    addNode(graph, createNode(9, "FES", 72.64262561, 34.06970212));
    addNode(graph, createNode(10, "Academic Block", 72.6430675, 34.0702205));
    addNode(graph, createNode(11, "ACB", 72.64316676, 34.07053452));
    addNode(graph, createNode(12, "Library", 72.6436385, 34.07111185));
    addNode(graph, createNode(13, "FME", 72.64460331, 34.06955101));
    addNode(graph, createNode(14, "FMCE", 72.6452253, 34.07014423));
    addNode(graph, createNode(15, "Mosque", 72.6414004, 34.0694484));
    addNode(graph, createNode(16, "Guest House", 72.645999, 34.06869548));
    addNode(graph, createNode(17, "HBL Bank", 72.64629011, 34.06945418));
    addNode(graph, createNode(18, "Transport office", 72.6471283, 34.0694546));
    addNode(graph, createNode(19, "Tuc area", 72.6471184, 34.0700648));
    addNode(graph, createNode(20, "Tuc", 72.64723833, 34.07076408));
    addNode(graph, createNode(21, "Cafe", 72.64372605, 34.06963965));
    addNode(graph, createNode(22, "Brabers Building", 72.64495761, 34.07150647));
    addNode(graph, createNode(23, "Incubation center", 72.64575473, 34.07166708));
    addNode(graph, createNode(24, "Graduate Hostel", 72.64489218, 34.07222031));
    addNode(graph, createNode(25, "New GH", 72.64571309, 34.07232143));
    addNode(graph, createNode(26, "New GH", 72.64603432, 34.07204423));
    addNode(graph, createNode(27, "Old GH", 72.64636269, 34.07176345));
    addNode(graph, createNode(28, "Old GH", 72.6468481, 34.07201091));
    addNode(graph, createNode(29, "Medical Center", 72.64665298, 34.0713875));
    addNode(graph, createNode(30, "Medical Center", 72.64681478, 34.07165876));
    addNode(graph, createNode(31, "Helipad", 72.64406114, 34.07268256));
    addNode(graph, createNode(32, "Faculty Club", 72.64268343, 34.07262808));
    addNode(graph, createNode(33, "C types Houses", 72.64767139, 34.07262006));
    addNode(graph, createNode(34, "Bechelor Hostel", 72.6502388, 34.0740146));
    addNode(graph, createNode(35, "D types Houses", 72.65119049, 34.07375826));
    addNode(graph, createNode(36, "Sports Complex", 72.6400504, 34.0681848));
    addNode(graph, createNode(37, "BasketBall Court", 72.6408476, 34.068232));
    addNode(graph, createNode(38, "Hostel 9", 72.64033551, 34.0687106));
    addNode(graph, createNode(39, "Hostel 10", 72.6395146, 34.06868086));
    addNode(graph, createNode(40, "Hostel 1", 72.64033551, 34.06907942));
    addNode(graph, createNode(41, "Hostel 2", 72.6395265, 34.06897234));
    addNode(graph, createNode(42, "Hostel 3", 72.64024033, 34.0698349));
    addNode(graph, createNode(43, "Hostel 4", 72.63936588, 34.0698111));
    addNode(graph, createNode(44, "Hostel 5", 72.64024033, 34.07010854));
    addNode(graph, createNode(45, "Hostel 6", 72.63936588, 34.07009069));
    addNode(graph, createNode(46, "Hostel 8", 72.64113625, 34.0702228));
    addNode(graph, createNode(47, "Central Mess", 72.64099581, 34.06996577));
    //edges
    addEdge(graph, 0, 1, calculateDistance(graph.nodes[0], graph.nodes[1]));
    addEdge(graph, 0, 2, calculateDistance(graph.nodes[0], graph.nodes[2]));
    addEdge(graph, 0, 3, calculateDistance(graph.nodes[0], graph.nodes[3]));
    addEdge(graph, 0, 4, calculateDistance(graph.nodes[0], graph.nodes[4]));
    addEdge(graph, 0, 5, calculateDistance(graph.nodes[0], graph.nodes[5]));
    addEdge(graph, 0, 6, calculateDistance(graph.nodes[0], graph.nodes[6]));
    addEdge(graph, 0, 7, calculateDistance(graph.nodes[0], graph.nodes[7]));
    addEdge(graph, 0, 8, calculateDistance(graph.nodes[0], graph.nodes[8]));
    addEdge(graph, 0, 9, calculateDistance(graph.nodes[0], graph.nodes[9]));
    addEdge(graph, 0, 10, calculateDistance(graph.nodes[0], graph.nodes[10]));
    addEdge(graph, 0, 11, calculateDistance(graph.nodes[0], graph.nodes[11]));
    addEdge(graph, 0, 13, calculateDistance(graph.nodes[0], graph.nodes[13]));
    addEdge(graph, 0, 14, calculateDistance(graph.nodes[0], graph.nodes[14]));
    addEdge(graph, 0, 16, calculateDistance(graph.nodes[0], graph.nodes[16]));
    addEdge(graph, 0, 17, calculateDistance(graph.nodes[0], graph.nodes[17]));
    addEdge(graph, 0, 19, calculateDistance(graph.nodes[0], graph.nodes[19]));
    addEdge(graph, 0, 26, calculateDistance(graph.nodes[0], graph.nodes[26]));
    addEdge(graph, 3, 13, calculateDistance(graph.nodes[3], graph.nodes[13]));
    addEdge(graph, 3, 4, calculateDistance(graph.nodes[3], graph.nodes[4]));
    addEdge(graph, 3, 5, calculateDistance(graph.nodes[3], graph.nodes[5]));
    addEdge(graph, 3, 8, calculateDistance(graph.nodes[3], graph.nodes[8]));
    addEdge(graph, 3, 14, calculateDistance(graph.nodes[3], graph.nodes[14]));
    addEdge(graph, 3, 10, calculateDistance(graph.nodes[3], graph.nodes[10]));
    addEdge(graph, 3, 11, calculateDistance(graph.nodes[3], graph.nodes[11]));
    addEdge(graph, 39,41, calculateDistance(graph.nodes[39], graph.nodes[41]));
    addEdge(graph, 38,40, calculateDistance(graph.nodes[38], graph.nodes[40]));
    addEdge(graph, 42,44, calculateDistance(graph.nodes[42], graph.nodes[44]));
    addEdge(graph, 43,45, calculateDistance(graph.nodes[43], graph.nodes[45]));
    addEdge(graph, 40,42, calculateDistance(graph.nodes[40], graph.nodes[42]));
    addEdge(graph, 44,46, calculateDistance(graph.nodes[44], graph.nodes[46]));
    addEdge(graph, 40,46, calculateDistance(graph.nodes[40], graph.nodes[46]));
    addEdge(graph, 46,47, calculateDistance(graph.nodes[46], graph.nodes[43]));
    addEdge(graph, 47, 10, calculateDistance(graph.nodes[47], graph.nodes[10]));
    addEdge(graph, 3, 12, calculateDistance(graph.nodes[3], graph.nodes[12]));
    addEdge(graph, 38, 7, calculateDistance(graph.nodes[38], graph.nodes[7]));
    addEdge(graph, 8, 7, calculateDistance(graph.nodes[8], graph.nodes[7]));
    addEdge(graph, 12, 22, calculateDistance(graph.nodes[12], graph.nodes[22]));
    addEdge(graph, 22, 23, calculateDistance(graph.nodes[22], graph.nodes[23]));
    addEdge(graph, 23, 25, calculateDistance(graph.nodes[23], graph.nodes[25]));
    addEdge(graph, 25, 26, calculateDistance(graph.nodes[25], graph.nodes[26]));
    addEdge(graph, 25, 27, calculateDistance(graph.nodes[25], graph.nodes[27]));
    addEdge(graph, 27, 28, calculateDistance(graph.nodes[27], graph.nodes[28]));
    addEdge(graph, 25, 19, calculateDistance(graph.nodes[15], graph.nodes[29]));
    addEdge(graph, 26, 20, calculateDistance(graph.nodes[26], graph.nodes[20]));
    addEdge(graph, 27, 19, calculateDistance(graph.nodes[27], graph.nodes[19]));
    addEdge(graph, 28, 20, calculateDistance(graph.nodes[28], graph.nodes[20]));
    addEdge(graph, 27, 29, calculateDistance(graph.nodes[27], graph.nodes[29]));
    addEdge(graph, 27, 33, calculateDistance(graph.nodes[27], graph.nodes[33]));
    addEdge(graph, 33, 34, calculateDistance(graph.nodes[33], graph.nodes[34]));
    addEdge(graph, 33, 35, calculateDistance(graph.nodes[33], graph.nodes[35]));
    addEdge(graph, 34, 35, calculateDistance(graph.nodes[34], graph.nodes[35]));
    addEdge(graph, 33, 19, calculateDistance(graph.nodes[33], graph.nodes[19]));
    //Extra
    addEdge(graph, 4, 5, calculateDistance(graph.nodes[4], graph.nodes[5]));
    addEdge(graph, 5, 6, calculateDistance(graph.nodes[5], graph.nodes[6]));
    addEdge(graph, 6, 7, calculateDistance(graph.nodes[6], graph.nodes[7]));
    addEdge(graph, 7, 8, calculateDistance(graph.nodes[7], graph.nodes[8]));
    addEdge(graph, 8, 9, calculateDistance(graph.nodes[8], graph.nodes[9]));
    addEdge(graph, 9, 10, calculateDistance(graph.nodes[9], graph.nodes[10]));
    addEdge(graph, 10, 11, calculateDistance(graph.nodes[10], graph.nodes[11]));
    addEdge(graph, 11, 12, calculateDistance(graph.nodes[11], graph.nodes[12]));


    cout << "\t\t\t\t\t\033[1;34m--------------------------------------------------------------\033[0m" << endl;
    cout << "\t\t\t\t\t\033[1;34m-                                                            -\033[0m" << endl;
    cout << "\t\t\t\t\t\033[1;34m-   GHULAM ISHAQ KHAN INSTITUTE OF ENGINEERING SCIENCES      -\033[0m" << endl;
    cout << "\t\t\t\t\t\033[1;34m-                      AND TECHNOLOGY                        -\033[0m" << endl;
    cout << "\t\t\t\t\t\033[1;34m-                                                            -\033[0m" << endl;
    cout << "\t\t\t\t\t\033[1;34m-       WELCOME TO THE CAMPUS NAVIGATION SYSTEM              -\033[0m" << endl;
    cout << "\t\t\t\t\t\033[1;34m-                                                            -\033[0m" << endl;
    cout << "\t\t\t\t\t\033[1;34m--------------------------------------------------------------\033[0m" << endl << endl;
    int choice;
    do{
        cout << "\t\t\t\t\t\033[1;32mPlease select the following options to proceed:\033[0m" << endl;
        cout << "\t\t\t\t\t\033[1;33m1. Display the map of the campus\033[0m" << endl;
        cout << "\t\t\t\t\t\033[1;33m2. Find the shortest path among the buildings\033[0m" << endl;
        cout << "\t\t\t\t\t\033[1;33m3. Add a new location\033[0m" << endl;
        cout << "\t\t\t\t\t\033[1;33m0. Exit\033[0m" << endl;

        cin>>choice;
        string startLoc, destLoc, name;
        double longitude, latitude;
        int startIdx, destIdx, newId;
        Node newNode;
        vector<int> path;
        double totalDistance;

        switch (choice)
        {
        case 1:
            displayMap(graph);
            break;
        case 2:
            cout << "\n\t\t\t\t\t\033[1;32mChoose algorithm for path finding:\033[0m\n";
            cout << "\t\t\t\t\t\033[1;33m1. Dijkstra's Algorithm\033[0m\n";
            cout << "\t\t\t\t\t\033[1;33m2. Bellman-Ford Algorithm\033[0m\n";
            
            int algo_choice;
            cin >> algo_choice;
            
            cout << "\n\t\t\t\t\t\033[1;32mEnter start location: \033[0m";
            cin.ignore();
            getline(cin, startLoc);
            cout << "\t\t\t\t\t\033[1;32mEnter destination: \033[0m";
            getline(cin, destLoc);
            
            startIdx = findNodeByName(graph, startLoc);
            destIdx = findNodeByName(graph, destLoc);
            
            if (startIdx == -1 || destIdx == -1) {
                cout << "\n\t\t\t\t\t\033[1;31mLocation not found!\033[0m\n\n";
                break;
            }
            
            if (algo_choice == 1) {
                path = dijkstra(graph, startIdx, destIdx);
            } else if (algo_choice == 2) {
                path = bellmanFord(graph, startIdx, destIdx);
            } else {
                cout << "\n\t\t\t\t\t\033[1;31mInvalid algorithm choice!\033[0m\n\n";
                break;
            }
            
            if (path.empty()) {
                cout << "\n\t\t\t\t\t\033[1;31mNo path exists between these locations!\033[0m\n\n";
            } else {
                cout << "\n\t\t\t\t\t\033[1;36mShortest path using " 
                    << (algo_choice == 1 ? "Dijkstra's" : "Bellman-Ford") 
                    << " algorithm: \033[0m";
                totalDistance = 0;
                
                for (int i = 0; i < path.size(); i++) {
                    cout << "\033[1;33m" << graph.nodes[path[i]].name << "\033[0m";
                    if (i < path.size() - 1) {
                        double segmentDist = calculateDistance(graph.nodes[path[i]], graph.nodes[path[i + 1]]);
                        totalDistance += segmentDist;
                        cout << " --> ";
                    }
                }
                cout << "\n\t\t\t\t\t\033[1;32mTotal distance: \033[1;35m" << fixed << setprecision(3) 
                    << totalDistance << " km\033[0m\n\n";
            }
            break;
        case 3:
            cout << "\n\t\t\t\t\t\033[1;32mEnter location name: \033[0m";
            cin.ignore();
            getline(cin, name);
            
            cout << "\t\t\t\t\t\033[1;32mEnter longitude (in decimal degrees): \033[0m";
            cin >> longitude;
            
            cout << "\t\t\t\t\t\033[1;32mEnter latitude (in decimal degrees): \033[0m";
            cin >> latitude;
            
            newId = graph.nodes.size();
            newNode = createNode(newId, name, longitude, latitude);
            addNode(graph, newNode);
            
            for (int i = 0; i < newId; i++) {
                double distance = calculateDistance(graph.nodes[i], newNode);
                if (distance <= 0.2) {
                    addEdge(graph, newId, i, distance);
                }
            }
            
            cout << "\n\t\t\t\t\t\033[1;32mNew location '\033[1;33m" << name 
                << "\033[1;32m' added successfully!\033[0m\n";
            cout << "\t\t\t\t\t\033[1;32mConnected to \033[1;33m" << graph.adjacencyList[newId].size() 
         << "\033[1;32m nearby locations.\033[0m\n\n";
            break;
        case 0:
            cout<<"Thank you for using the campus navigation system"<<endl;
            break;

        default:
            cout<<"Invalid choice"<<endl;
            break;
        }

    }while(choice!=0);
    return 0;
}
