#include <bits/stdc++.h>
using namespace std;
using ll = long long;
using str = string;
using pii = pair<int, int>;
using pil = pair<int, ll>;
using pll = pair<ll, ll>;
using ull = unsigned long long;
#define FOR(i, l, r) for (int i=l; i<=r; i++)
#define FOR2(i, l, r) for (int i=l; i>=r; i--)
#define ALL(v) v.begin(), v.end()
#define fi first
#define endl '\n'
#define se second
#define ce cout << endl;
#define pb push_back
#define eb emplace_back
#define ep emplace
#define ins insert
const ll MOD = 998244353;
const ll INF = 2e18; 
const int N = 100005;

struct line {
    ll m, c;
    int node;
    line(ll _m = 0, ll _c = 0, int _node = 0) : m(_m), c(_c), node(_node) {}
};

int n;
vector<pii> adj[N];
ll V[N], S[N], d[N], dp[N];
line hull[N];
int top = 0;

void dfs_dist(int u, int p) {
    for (auto& [to, w] : adj[u]) if (to != p) {
        d[to] = d[u] + w;
        dfs_dist(to, u);
    }
}

long double intersect(const line& A, const line& B){
    return (long double)(B.c - A.c) / (A.m - B.m);
}
void dfs_dp(int u, int p) {
    int l = 1, r = top;
    while (l < r) {
        int mid = l + r>> 1;
        if (intersect(hull[mid], hull[mid + 1]) >= V[u]) {
            r = mid;
        } 
        else {
            l = mid + 1;
        }
    }
    dp[u] = hull[l].m * V[u] + hull[l].c + S[u] + V[u] * d[u];

    line cur(-d[u], dp[u], u);

    l = 2, r = top;
    int pos = top + 1;
    while (l <= r) {
        int mid = l + r >> 1;
        if (intersect(hull[mid - 1], hull[mid]) >=
                intersect(hull[mid - 1], cur)) {
            pos = mid;
            r = mid - 1;
        } 
        else {
            l = mid + 1;
        }
    }

    line old_line = hull[pos];
    int old_top = top;

    hull[pos] = cur;
    top = pos;

    for (auto& [v, w] : adj[u]) if (v != p) {
        dfs_dp(v, u);
    }

    hull[pos] = old_line;
    top = old_top;
}

void solve() {
    cin >> n;
    FOR(i, 1, n - 1) {
        int u, v, w;
        cin >> u >> v >> w;
        adj[u].pb({v, w});
        adj[v].pb({u, w});
    }
    FOR(i, 2, n) {
        cin >> S[i] >> V[i];
    }

    dfs_dist(1, 0);

    dp[1] = 0;
    hull[1] = line(0, 0, 1);
    top = 1;

    for (auto& [v, w] : adj[1]) {
        dfs_dp(v, 1);
    }

    FOR(i, 2, n) {
        cout << dp[i] << " ";
    }
}
int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(0); cout.tie(0);
    int t = 1;
    while (t--) {
        solve();
    }

    return 0;
}