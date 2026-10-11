#include <iostream>
#include <vector>

using namespace std;

const int MOD = 998244353;
const int MAX_N = 4000005;

long long fact[MAX_N];
long long invFact[MAX_N];

// Computes (base^exp) % MOD
long long power(long long base, long long exp) {
    long long res = 1;
    base %= MOD;
    while (exp > 0) {
        if (exp % 2 == 1) res = (res * base) % MOD;
        base = (base * base) % MOD;
        exp /= 2;
    }
    return res;
}

// Computes modular inverse
long long modInverse(long long n) {
    return power(n, MOD - 2);
}

// Precomputes factorials and inverse factorials for O(1) combinations
void precompute() {
    fact[0] = 1;
    invFact[0] = 1;
    for (int i = 1; i < MAX_N; i++) {
        fact[i] = (fact[i - 1] * i) % MOD;
    }
    invFact[MAX_N - 1] = modInverse(fact[MAX_N - 1]);
    for (int i = MAX_N - 2; i >= 1; i--) {
        invFact[i] = (invFact[i + 1] * (i + 1)) % MOD;
    }
}

// Standard nCr Combinations wrapper
long long choose(int n, int k) {
    if (k < 0 || k > n || n < 0) return 0;
    return fact[n] * invFact[k] % MOD * invFact[n - k] % MOD;
}

// Reflection principle path evaluation for specific assignment < y
long long f(int i, int y, int n) {
    long long res = (choose(2LL * n - i - y, n - y) - choose(2LL * n - i - y, n - y - 1) + MOD) % MOD;
    return res;
}

// Evaluates the nested sum of completion paths for all possible assignments >= L
long long g(int i, int L, int n) {
    long long res = (choose(2LL * n - i - L + 1, n - L) - choose(2LL * n - i - L + 1, n - L - 1) + MOD) % MOD;
    return res;
}

void solve() {
    int n;
    if (!(cin >> n)) return;
    vector<int> q(n + 1);
    for (int i = 1; i <= n; i++) {
        cin >> q[i];
    }

    long long ans = 0;
    int y = 0;        // Keeping track of maximum element placed
    int m = 1;        // Tracking the minimum unplaced element
    vector<bool> used(n + 2, false);

    for (int i = 1; i <= n; i++) {
        int qi = q[i];

        // Case 1: we place exactly `m` provided it's functionally capable of generating substitutions > q[i]
        if (m < y && m > qi) {
            ans = (ans + f(i, y, n)) % MOD;
        }

        // Case 2: placing anything strictly greater than max(y, q[i]) avoiding 321 occurrences
        int L = max(y, qi) + 1;
        if (L <= n) {
            ans = (ans + g(i, L, n)) % MOD;
        }

        // Step forward assuming prefix matches q entirely on i-th point.
        // Break checking if continuation natively forms a 321 sequence violating future properties.
        if (qi < y && qi != m) {
            break;
        }

        used[qi] = true;
        while (m <= n && used[m]) {
            m++;
        }
        y = max(y, qi);
    }

    cout << ans << "\n";
}

int main() {
    // Optimizing fast I/O operations
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    precompute();

    int t;
    if (cin >> t) {
        while (t--) solve();
    }

    return 0;
}
