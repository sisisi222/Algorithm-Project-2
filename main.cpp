/*
* COT-4400 Analysis of Algorithms
* Project 2
*
* Chau Nguyen
* Huong Duong
* Philip Lancaster
*/


#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <queue>

using namespace std;

// Add edge to adjacency list where directed edge (part, dep) has weight w is stored as adj[part] = (dep, w) 
void addEdge(vector<pair<uint64_t, uint64_t> > adj[], uint64_t part, uint64_t dep, uint64_t weight) {
    adj[part].push_back(make_pair(dep, weight));
}

// Memoized DP to calculate the total number of sprockets for omnidroid (postorder DFS) - store answer in sprocket[n-1]
void MemoOmni(vector<pair<uint64_t, uint64_t> > adj[], pair<uint64_t, uint64_t> node, vector<bool> discovered, uint64_t sprocket[]) {
    uint64_t part = node.first, weight = node.second;
    discovered[part] = true;
    for (auto it = adj[part].begin(); it != adj[part].end(); it++) {
        uint64_t dep = it->first, dep_weight = it->second;
        if (!discovered[dep])
            MemoOmni(adj, make_pair(dep, dep_weight), discovered, sprocket);
        sprocket[part] += sprocket[dep] * dep_weight;
    }
}

uint64_t BuildOmni(ifstream& file_in)
{
    uint64_t part_count, dep_count;

    file_in >> part_count >> dep_count;
    vector<pair<uint64_t, uint64_t> > adj[dep_count]; // Adjacency list
    uint64_t sprocket[part_count];                    // Sprocket array
    vector<bool> discovered(part_count, false);

    // Loop through m dependency parts 
    for (uint64_t m = 0; m < dep_count; m++) {
        uint64_t i, j;
        // Part i is used in constructing part j 
        file_in >> i >> j;

        // Add directed edge (j, i) to adjacency list with number of dependency time (weight) = 1 
        // If edge already exists, increment number of dependency times (weight)
        if (!adj[j].empty()) {
            bool flag = false;
            for (auto it = adj[j].begin(); it != adj[j].end(); it++) {
                uint64_t dep = it->first, weight = it->second;
                if (dep == i) {
                    it->second = ++weight;
                    flag = true;
                }
            }
            if (!flag) addEdge(adj, j, i, 1);
        }
        else addEdge(adj, j, i, 1);
    }

    // Loop through input number of sprockets used to construct each part
    for (uint64_t n = 0; n < part_count; n++) {
        file_in >> sprocket[n];
    }

    // Call memoized DP function, answer stored in sprocket[n-1]
    MemoOmni(adj, make_pair(part_count - 1, 1), discovered, sprocket);
    return sprocket[part_count - 1];
}

uint64_t BuildRobo(ifstream& file_in)
{
    uint64_t part_count;

    file_in >> part_count;
    uint64_t sprocket[part_count];                  // Sprocket array
    queue<pair<uint64_t, uint64_t> > edges;         // Queue of edges (dependency stages)

    // Loop through each stage, add s to sprocket[stage] and enqueue edge if stage is dependent on previous stages (if p > 0)
    for (uint64_t n = 0; n < part_count; n++) {
        uint64_t p;
        file_in >> sprocket[n] >> p;
        while (p > 0) {
            edges.push(make_pair(n, n - p));
            p--;
        }
    }

    // Iterate through the edges (starting from the first stages)
    while (!edges.empty()) {
        uint64_t stage = edges.front().first, prev_stage = edges.front().second;
        edges.pop();
        sprocket[stage] += sprocket[prev_stage];    // Update the dependent stage's total # of sprockets
    }

    return sprocket[part_count - 1];
}

int main() {
    // Run g++ -o robot -std=c++11 main.cpp 
    ifstream file_in("input.txt");
    ofstream file_out("output.txt"); // Write output to "output.txt"

    if (file_in) {
        uint64_t robot_num;
        file_in >> robot_num;
        for (uint64_t i = 0; i < robot_num; i++) {
            string robot_type;

            file_in >> robot_type;
            if (robot_type == "omnidroid") {
                file_out << BuildOmni(file_in) << endl;
            }
            else if (robot_type == "robotomaton") {
                file_out << BuildRobo(file_in) << endl;     // Write output to file "output.txt"
            }
        }
    }
    return 0;
}