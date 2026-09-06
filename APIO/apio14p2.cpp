#include <bits/stdc++.h>
using namespace std;
using ll = long long;
const ll INF = LLONG_MAX / 4;

struct FastScanner {
    static const int BUFSIZE = 1 << 16;
    int idx, size;
    char buf[BUFSIZE];
    FastScanner() : idx(0), size(0) {}
    inline char read() {
        if (idx >= size) {
            size = (int)fread(buf, 1, BUFSIZE, stdin);
            idx = 0;
            if (size == 0) return 0;
        }
        return buf[idx++];
    }
    ll nextLong() {
        char c;
        while ((c = read()) && c <= ' ') if (!c) return LLONG_MIN;
        int sign = 1;
        if (c == '-') { sign = -1; c = read(); }
        ll x = 0;
        while (c > ' ') {
            x = x * 10 + (c - '0');
            c = read();
        }
        return x * sign;
    }
    int nextInt() { return (int)nextLong(); }
};

struct Line {
    ll m, b;
    int id;
    Line() {}
    Line(ll _m, ll _b, int _id) : m(_m), b(_b), id(_id) {}
    inline ll val(ll x) const { return m * x + b; }
};

// check if middle line b is unnecessary between a and c
inline bool isBad(const Line &a, const Line &b, const Line &c) {
    // use __int128 to avoid overflow:
    // (c.b - a.b) * (a.m - b.m) <= (b.b - a.b) * (a.m - c.m)
    __int128 left  = (__int128)(c.b - a.b) * (__int128)(a.m - b.m);
    __int128 right = (__int128)(b.b - a.b) * (__int128)(a.m - c.m);
    return left <= right;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    FastScanner fs;
    int n = fs.nextInt();
    int k = fs.nextInt();

    vector<ll> S(n + 1, 0);
    for (int i = 1; i <= n; ++i) {
        ll x = fs.nextLong();
        S[i] = S[i - 1] + x;
    }

    int parts = k + 1;
    vector<ll> prev(n + 1, INF), cur(n + 1, INF);
    prev[0] = 0;

    // choice[t][i] : best previous split j for dp with t parts ending at i
    vector<vector<int>> choice(parts + 1, vector<int>(n + 1, -1));

    for (int t = 1; t <= parts; ++t) {
        fill(cur.begin(), cur.end(), INF);
        deque<Line> dq;

        for (int i = t; i <= n; ++i) {
            int jAdd = i - 1;
            if (prev[jAdd] != INF) {
                ll m = -2LL * S[jAdd];
                ll b = prev[jAdd] + S[jAdd] * S[jAdd];
                Line nl(m, b, jAdd);
                while (dq.size() >= 2) {
                    Line l2 = dq.back(); dq.pop_back();
                    Line l1 = dq.back();
                    if (isBad(l1, l2, nl)) {
                        // l2 is bad, continue (already removed)
                        continue;
                    } else {
                        dq.push_back(l2);
                        break;
                    }
                }
                dq.push_back(nl);
            }

            if (!dq.empty()) {
                ll x = S[i];
                while (dq.size() >= 2) {
                    Line f1 = dq[0];
                    Line f2 = dq[1];
                    if (f1.val(x) >= f2.val(x)) dq.pop_front();
                    else break;
                }
                Line best = dq.front();
                cur[i] = x * x + best.val(x);
                choice[t][i] = best.id;
            } else {
                cur[i] = INF;
                choice[t][i] = -1;
            }
        }

        prev.swap(cur);
    }

    ll minSumSq = prev[n];
    ll total = S[n];
    ll maxPoints = (total * total - minSumSq) / 2;

    // output
    // Use fast output via string builder
    string out;
    out.reserve(64 + k * 12);
    out += to_string(maxPoints);
    out.push_back('\n');

    vector<int> splits(k);
    int idx = k - 1;
    int t = parts;
    int pos = n;
    while (t > 1) {
        int prevPos = choice[t][pos];
        splits[idx--] = prevPos;
        pos = prevPos;
        --t;
    }
    for (int i = 0; i < k; ++i) {
        if (i) out.push_back(' ');
        out += to_string(splits[i]);
    }
    out.push_back('\n');

    fwrite(out.data(), 1, out.size(), stdout);
    return 0;
}
