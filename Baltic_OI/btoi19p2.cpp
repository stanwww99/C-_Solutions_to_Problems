#include <bits/stdc++.h>
using namespace std;

struct DynBitset {
    int n; // number of bits
    vector<uint64_t> a;
    uint64_t lastMask;

    DynBitset() : n(0), lastMask(~0ULL) {}
    DynBitset(int bits) { init(bits); }

    void init(int bits) {
        n = bits;
        int blocks = (n + 63) >> 6;
        a.assign(blocks, 0);
        if (n % 64 == 0) lastMask = ~0ULL;
        else lastMask = ((1ULL << (n % 64)) - 1ULL);
    }

    void setBit(int i) { a[i >> 6] |= (1ULL << (i & 63)); }
    bool test(int i) const { return (a[i >> 6] >> (i & 63)) & 1ULL; }
    bool empty() const {
        for (auto v : a) if (v) return false;
        return true;
    }

    void orWith(const DynBitset &other) {
        int m = (int)a.size();
        for (int i = 0; i < m; ++i) a[i] |= other.a[i];
    }

    void andWith(const DynBitset &other) {
        int m = (int)a.size();
        for (int i = 0; i < m; ++i) a[i] &= other.a[i];
    }

    uint64_t count() const {
        uint64_t s = 0;
        for (auto v : a) s += __builtin_popcountll(v);
        return s;
    }

    // shift left by 1 (bit i -> i+1). Bits that overflow beyond n are dropped.
    DynBitset shiftLeft1() const {
        DynBitset res;
        res.init(n);
        int m = (int)a.size();
        uint64_t carry = 0;
        for (int i = 0; i < m; ++i) {
            uint64_t cur = a[i];
            uint64_t newCarry = (cur >> 63) & 1ULL;
            res.a[i] = (cur << 1) | carry;
            carry = newCarry;
        }
        // mask last block to clear bits beyond n
        if (!res.a.empty()) res.a.back() &= res.lastMask;
        return res;
    }

    // shift right by 1 (bit i -> i-1). Bits that go negative are dropped.
    DynBitset shiftRight1() const {
        DynBitset res;
        res.init(n);
        int m = (int)a.size();
        uint64_t carry = 0;
        for (int i = m - 1; i >= 0; --i) {
            uint64_t cur = a[i];
            uint64_t newCarry = (cur & 1ULL) << 63;
            res.a[i] = (cur >> 1) | carry;
            carry = newCarry;
        }
        // last block mask not needed for right shift, but keep consistent
        if (!res.a.empty()) res.a.back() &= res.lastMask;
        return res;
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int R, C, M;
    if (!(cin >> R >> C >> M)) return 0;
    vector<string> grid(R);
    for (int r = 0; r < R; ++r) cin >> grid[r];
    string s; cin >> s;

    vector<DynBitset> water(R), curr(R);
    for (int r = 0; r < R; ++r) {
        water[r].init(C);
        for (int c = 0; c < C; ++c) {
            if (grid[r][c] == '.') water[r].setBit(c);
        }
        curr[r] = water[r];
    }

    for (int step = 0; step < M; ++step) {
        char ch = s[step];
        vector<DynBitset> next(R);
        for (int r = 0; r < R; ++r) next[r].init(C);

        if (ch == 'N' || ch == '?') {
            for (int r = 0; r < R; ++r) {
                if (curr[r].empty()) continue;
                int nr = r - 1;
                if (nr >= 0) next[nr].orWith(curr[r]);
            }
        }
        if (ch == 'S' || ch == '?') {
            for (int r = 0; r < R; ++r) {
                if (curr[r].empty()) continue;
                int nr = r + 1;
                if (nr < R) next[nr].orWith(curr[r]);
            }
        }
        if (ch == 'E' || ch == '?') {
            for (int r = 0; r < R; ++r) {
                if (curr[r].empty()) continue;
                DynBitset shifted = curr[r].shiftLeft1();
                next[r].orWith(shifted);
            }
        }
        if (ch == 'W' || ch == '?') {
            for (int r = 0; r < R; ++r) {
                if (curr[r].empty()) continue;
                DynBitset shifted = curr[r].shiftRight1();
                next[r].orWith(shifted);
            }
        }
        
        for (int r = 0; r < R; ++r) {
            next[r].andWith(water[r]);
        }
        curr.swap(next);
    }

    unsigned long long ans = 0;
    for (int r = 0; r < R; ++r) ans += curr[r].count();
    cout << ans << '\n';
    return 0;
}
