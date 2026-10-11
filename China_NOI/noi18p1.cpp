#include <bits/stdc++.h>
using namespace std;

struct Edge {
    int u, v;
    long long l, a;
};

struct DNode {
    int v;
    long long dist;
    bool operator<(DNode const& other) const {
        return dist > other.dist;
    }
};

static const int LOG = 20;

int dsu[400005];
int up[400005][LOG];
long long minDist[400005];
long long weightNode[400005];

int find_set(int v) {
    if (dsu[v] == v) return v;
    return dsu[v] = find_set(dsu[v]);
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T;
    cin >> T;

    while (T--) {
        int n, m;
        cin >> n >> m;

        vector<vector<Edge>> adj(n + 1);
        vector<Edge> edges(m);

        for (int i = 0; i < m; i++) {
            cin >> edges[i].u >> edges[i].v >> edges[i].l >> edges[i].a;
            adj[edges[i].u].push_back(edges[i]);
            adj[edges[i].v].push_back({edges[i].v, edges[i].u, edges[i].l, edges[i].a});
        }

        // Dijkstra from 1
        vector<long long> dist(n + 1, LLONG_MAX);
        dist[1] = 0;
        priority_queue<DNode> pq;
        pq.push({1, 0});

        while (!pq.empty()) {
            auto [v, d] = pq.top();
            pq.pop();
            if (d != dist[v]) continue;
            for (auto &e : adj[v]) {
                if (dist[e.v] > dist[v] + e.l) {
                    dist[e.v] = dist[v] + e.l;
                    pq.push({e.v, dist[e.v]});
                }
            }
        }

        // Sort edges by decreasing a
        sort(edges.begin(), edges.end(), [](const Edge &e1, const Edge &e2) {
            return e1.a > e2.a;
        });

        int maxNodes = 2 * n;
        int nodeCount = n;

        for (int i = 1; i <= maxNodes; i++) {
            dsu[i] = i;
            weightNode[i] = 0;
            minDist[i] = LLONG_MAX;
            for (int j = 0; j < LOG; j++) up[i][j] = 0;
        }

        for (int i = 1; i <= n; i++) {
            minDist[i] = dist[i];
            weightNode[i] = LLONG_MAX;
        }

        // Kruskal reconstruction tree
        for (auto &e : edges) {
            int ru = find_set(e.u);
            int rv = find_set(e.v);
            if (ru != rv) {
                ++nodeCount;
                dsu[ru] = nodeCount;
                dsu[rv] = nodeCount;
                dsu[nodeCount] = nodeCount;

                weightNode[nodeCount] = e.a;
                minDist[nodeCount] = min(minDist[ru], minDist[rv]);

                up[ru][0] = nodeCount;
                up[rv][0] = nodeCount;
            }
        }

        up[nodeCount][0] = nodeCount;

        // Binary lifting table
        for (int j = 1; j < LOG; j++) {
            for (int i = 1; i <= nodeCount; i++) {
                up[i][j] = up[ up[i][j-1] ][j-1];
            }
        }

        int Q;
        long long K, S;
        cin >> Q >> K >> S;

        long long lastans = 0;

        while (Q--) {
            long long v0, p0;
            cin >> v0 >> p0;

            int v = (int)((v0 + K * lastans - 1) % n + 1);
            long long p = (p0 + K * lastans) % (S + 1);

            int curr = v;
            for (int j = LOG - 1; j >= 0; j--) {
                int nxt = up[curr][j];
                if (nxt != 0 && weightNode[nxt] > p) {
                    curr = nxt;
                }
            }

            lastans = minDist[curr];
            cout << lastans << "\n";
        }
    }

    return 0;
}