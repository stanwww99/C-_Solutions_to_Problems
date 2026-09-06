#include <bits/stdc++.h>
using namespace std;

using ll = long long;

// Fast input using fread
static const int BUFSIZE = 1 << 20;
char buf[BUFSIZE], *p = buf, *e = buf;
inline char getChar() {
    if (p == e) {
        e = buf + fread(buf, 1, BUFSIZE, stdin);
        p = buf;
    }
    return p == e ? '\0' : *p++;
}
inline ll readLL() {
    char c = getChar();
    while (c < '0' || c > '9') c = getChar();
    ll x = 0;
    while (c >= '0' && c <= '9') {
        x = x * 10 + (c - '0');
        c = getChar();
    }
    return x;
}

// Custom binary heap (min-heap)
vector<ll> heapDist;
vector<int> heapNode;

void heapPush(ll d, int v) {
    heapDist.push_back(d);
    heapNode.push_back(v);
    int idx = (int)heapDist.size() - 1;
    while (idx > 0) {
        int par = (idx - 1) >> 1;
        if (heapDist[par] <= heapDist[idx]) break;
        swap(heapDist[par], heapDist[idx]);
        swap(heapNode[par], heapNode[idx]);
        idx = par;
    }
}

void heapPop() {
    int n = (int)heapDist.size() - 1;
    heapDist[0] = heapDist[n];
    heapNode[0] = heapNode[n];
    heapDist.pop_back();
    heapNode.pop_back();
    int idx = 0, sz = (int)heapDist.size();
    while (true) {
        int left = idx * 2 + 1;
        if (left >= sz) break;
        int right = left + 1;
        int child = left;
        if (right < sz && heapDist[right] < heapDist[left]) child = right;
        if (heapDist[child] >= heapDist[idx]) break;
        swap(heapDist[child], heapDist[idx]);
        swap(heapNode[child], heapNode[idx]);
        idx = child;
    }
}

int main() {
    int N = (int)readLL();
    vector<ll> a(N);
    for (int i = 0; i < N; i++) a[i] = readLL();

    int mod = (int)a[0];
    if (mod == 1) {
        puts("0");
        return 0;
    }

    // Keep only the smallest weight for each residue (excluding 0)
    const ll INFLL = (1LL << 62);
    vector<ll> best(mod, INFLL);
    for (int i = 1; i < N; i++) {
        int r = (int)(a[i] % mod);
        if (r == 0) continue;
        if (a[i] < best[r]) best[r] = a[i];
    }

    // Build edges: (residue, weight)
    vector<pair<int, ll>> edges;
    for (int r = 1; r < mod; r++) {
        if (best[r] != INFLL) {
            edges.emplace_back(r, best[r]);
        }
    }

    if (edges.empty()) {
        puts("0");
        return 0;
    }

    int M = (int)edges.size();

    // Dijkstra
    vector<ll> dist(mod, INFLL);
    vector<char> used(mod, 0);
    dist[0] = 0;

    heapDist.reserve(1 << 20);
    heapNode.reserve(1 << 20);
    heapPush(0, 0);

    int done = 0;
    while (!heapDist.empty()) {
        ll d = heapDist[0];
        int u = heapNode[0];
        heapPop();

        if (used[u] || d != dist[u]) continue;
        used[u] = 1;
        done++;
        if (done == mod) break;

        for (const auto &e : edges) {
            int r = e.first;
            ll w = e.second;
            int v = u + r;
            if (v >= mod) v -= mod;  // since r < mod, one subtraction is enough
            ll nd = d + w;
            if (nd < dist[v]) {
                dist[v] = nd;
                heapPush(nd, v);
            }
        }
    }

    ll ans = 0;
    for (int i = 0; i < mod; i++) {
        ans ^= dist[i];
    }
    printf("%lld\n", ans);

    return 0;
}