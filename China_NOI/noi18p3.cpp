#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstring>

using namespace std;

const int MAXS = 500005;
const int MAXT = 1000005;

// Segment Tree to store end_pos of Suffix Automaton
struct SegTree {
    int ls, rs;
} tr[MAXS * 40];
int root[MAXS * 2], totSeg;

void insertSeg(int &p, int l, int r, int pos) {
    if (!p) p = ++totSeg;
    if (l == r) return;
    int mid = (l + r) / 2;
    if (pos <= mid) insertSeg(tr[p].ls, l, mid, pos);
    else insertSeg(tr[p].rs, mid + 1, r, pos);
}

int mergeSeg(int p, int q, int l, int r) {
    if (!p || !q) return p + q;
    int cur = ++totSeg;
    if (l == r) return cur;
    int mid = (l + r) / 2;
    tr[cur].ls = mergeSeg(tr[p].ls, tr[q].ls, l, mid);
    tr[cur].rs = mergeSeg(tr[p].rs, tr[q].rs, mid + 1, r);
    return cur;
}

bool querySeg(int p, int l, int r, int ql, int qr) {
    if (!p || ql > r || qr < l) return false;
    if (ql <= l && r <= qr) return true;
    int mid = (l + r) / 2;
    return querySeg(tr[p].ls, l, mid, ql, qr) || querySeg(tr[p].rs, mid + 1, r, ql, qr);
}

// Suffix Automaton for string S
struct SAM_S {
    int nxt[MAXS * 2][26], link[MAXS * 2], len[MAXS * 2];
    int last, sz;
    
    void init() {
        last = sz = 1;
    }
    
    void extend(int c, int pos, int n) {
        int cur = ++sz, p = last;
        len[cur] = len[p] + 1;
        insertSeg(root[cur], 1, n, pos);
        for (; p && !nxt[p][c]; p = link[p]) nxt[p][c] = cur;
        if (!p) {
            link[cur] = 1;
        } else {
            int q = nxt[p][c];
            if (len[q] == len[p] + 1) {
                link[cur] = q;
            } else {
                int clone = ++sz;
                len[clone] = len[p] + 1;
                memcpy(nxt[clone], nxt[q], sizeof(nxt[q]));
                link[clone] = link[q];
                for (; p && nxt[p][c] == q; p = link[p]) nxt[p][c] = clone;
                link[q] = link[cur] = clone;
            }
        }
        last = cur;
    }
    
    int c[MAXS * 2], a[MAXS * 2];
    void build(int n) {
        for (int i = 1; i <= sz; i++) c[len[i]]++;
        for (int i = 1; i <= n; i++) c[i] += c[i - 1];
        for (int i = sz; i >= 1; i--) a[c[len[i]]--] = i;
        for (int i = sz; i > 1; i--) {
            int u = a[i];
            root[link[u]] = mergeSeg(root[link[u]], root[u], 1, n);
        }
    }
} samS;

int match[MAXT];

// Suffix Automaton for query string T
struct SAM_T {
    int nxt[MAXT * 2][26], link[MAXT * 2], len[MAXT * 2];
    int val[MAXT * 2];
    int last, sz;
    int posNode[MAXT];
    
    void init() {
        for(int i = 1; i <= sz; ++i) {
            memset(nxt[i], 0, sizeof(nxt[i]));
            link[i] = len[i] = val[i] = 0;
        }
        last = sz = 1;
    }
    
    void extend(int c, int i) {
        int cur = ++sz, p = last;
        len[cur] = len[p] + 1;
        for (; p && !nxt[p][c]; p = link[p]) nxt[p][c] = cur;
        if (!p) {
            link[cur] = 1;
        } else {
            int q = nxt[p][c];
            if (len[q] == len[p] + 1) {
                link[cur] = q;
            } else {
                int clone = ++sz;
                len[clone] = len[p] + 1;
                memcpy(nxt[clone], nxt[q], sizeof(nxt[q]));
                link[clone] = link[q];
                for (; p && nxt[p][c] == q; p = link[p]) nxt[p][c] = clone;
                link[q] = link[cur] = clone;
            }
        }
        last = cur;
        posNode[i] = cur;
    }
    
    int c[MAXT * 2], a[MAXT * 2];
    long long solve(int Tlen) {
        for(int i = 1; i <= Tlen; i++) c[i] = 0;
        for (int i = 1; i <= sz; i++) c[len[i]]++;
        for (int i = 1; i <= Tlen; i++) c[i] += c[i - 1];
        for (int i = sz; i >= 1; i--) a[c[len[i]]--] = i;
        
        for (int i = 1; i <= Tlen; i++) {
            val[posNode[i]] = max(val[posNode[i]], match[i]);
        }
        
        long long ans = 0;
        for (int i = sz; i > 1; i--) {
            int u = a[i];
            val[link[u]] = max(val[link[u]], val[u]); // propagate max matched down fail tree
            ans += max(0, len[u] - max(len[link[u]], val[u]));
        }
        return ans;
    }
} samT;

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    
    string S;
    if (!(cin >> S)) return 0;
    int n = S.length();
    
    samS.init();
    for (int i = 0; i < n; i++) {
        samS.extend(S[i] - 'a', i + 1, n);
    }
    samS.build(n);

    int q;
    if (!(cin >> q)) return 0;
    
    while (q--) {
        string T;
        int l, r;
        cin >> T >> l >> r;
        int Tlen = T.length();
        int u = 1, l_match = 0;
        
        // Match string T on Suffix Automaton S within range [l, r]
        for (int i = 0; i < Tlen; i++) {
            int c = T[i] - 'a';
            while (true) {
                if (u == 1 && l_match == 0) {
                    int v = samS.nxt[u][c];
                    if (v && querySeg(root[v], 1, n, l, r)) {
                        u = v; l_match = 1;
                    }
                    break;
                }
                int v = samS.nxt[u][c];
                // Can we extend the match within the target segment of S?
                if (v && querySeg(root[v], 1, n, l + l_match, r)) {
                    u = v; l_match++;
                    break;
                }
                // Reduce size step-by-step 
                l_match--;
                if (l_match == samS.len[samS.link[u]]) {
                    u = samS.link[u];
                }
            }
            match[i + 1] = l_match; // Maximum match ending at pos i+1 of T
        }

        // Build temporary SAM for T to avoid duplicate counting
        samT.init();
        for (int i = 0; i < Tlen; i++) {
            samT.extend(T[i] - 'a', i + 1);
        }
        
        cout << samT.solve(Tlen) << "\n";
    }
    return 0;
}