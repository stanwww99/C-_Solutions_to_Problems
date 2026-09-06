#include <iostream>
#include <vector>
#include <deque>

using namespace std;

const long long INF = 4e18; // Safe infinity value

struct Edge {
    int to;
    long long weight;
};

int main() {
    // Fast I/O
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int n, m;
    if (!(cin >> n >> m)) return 0;

    vector<vector<Edge>> graph(n + 1);
    for (int i = 0; i < m; i++) {
        int u, v;
        long long w;
        cin >> u >> v >> w;
        graph[u].push_back({v, w});
    }

    vector<long long> dist(n + 1);
    vector<int> count(n + 1);
    vector<bool> inQueue(n + 1);
    vector<bool> negInf(n + 1);
    
    // BFS queue for negative cycles
    vector<int> bfsQueue(n + 1);

    for (int i = 1; i <= n; i++) {
        // Reset state for each source
        fill(dist.begin(), dist.end(), INF);
        fill(count.begin(), count.end(), 0);
        fill(inQueue.begin(), inQueue.end(), false);
        fill(negInf.begin(), negInf.end(), false);

        deque<int> q;
        q.push_back(i);
        dist[i] = 0;
        inQueue[i] = true;
        count[i] = 1;

        while (!q.empty()) {
            int u = q.front();
            q.pop_front();
            inQueue[u] = false;

            if (negInf[u]) continue;

            for (const auto& edge : graph[u]) {
                int v = edge.to;
                long long w = edge.weight;

                if (negInf[v]) continue;

                if (dist[u] + w < dist[v]) {
                    dist[v] = dist[u] + w;
                    
                    if (!inQueue[v]) {
                        count[v]++;
                        
                        // Negative cycle detected
                        if (count[v] > n) {
                            negInf[v] = true;
                            
                            // BFS to mark all nodes reachable from the negative cycle
                            int head = 0, tail = 0;
                            bfsQueue[tail++] = v;
                            
                            while (head < tail) {
                                int curr = bfsQueue[head++];
                                for (const auto& nextEdge : graph[curr]) {
                                    if (!negInf[nextEdge.to]) {
                                        negInf[nextEdge.to] = true;
                                        bfsQueue[tail++] = nextEdge.to;
                                    }
                                }
                            }
                            continue;
                        }

                        inQueue[v] = true;
                        
                        // SLF (Small Label First) Optimization
                        if (!q.empty() && dist[v] < dist[q.front()]) {
                            q.push_front(v);
                        } else {
                            q.push_back(v);
                        }
                    }
                }
            }
        }

        // Output formatting
        for (int j = 1; j <= n; j++) {
            if (negInf[j]) {
                cout << "-INF";
            } else if (dist[j] > INF / 2) { 
                cout << "INF";
            } else {
                cout << dist[j];
            }

            if (j < n) cout << " ";
        }
        cout << "\n";
    }

    return 0;
}