#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

const int MOD = 1000000007;

int dp[2][105][1005][3];

inline void add(int &a, int b) {
    a += b;
    if (a >= MOD) a -= MOD;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int N, L;
    if (!(cin >> N >> L)) return 0;

    vector<int> A(N + 1);
    for (int i = 1; i <= N; ++i) {
        cin >> A[i];
    }

    if (N == 1) {
        cout << 1 << "\n";
        return 0;
    }

    sort(A.begin() + 1, A.end());

    dp[1][1][0][0] = 1;
    dp[1][1][0][1] = 2;

    for (int i = 1; i < N; ++i) {
        int curr = i % 2;
        int next = (i + 1) % 2;

        for (int j = 0; j <= i + 1; ++j) {
            for (int k = 0; k <= L; ++k) {
                dp[next][j][k][0] = 0;
                dp[next][j][k][1] = 0;
                dp[next][j][k][2] = 0;
            }
        }

        int diff = A[i + 1] - A[i];

        for (int j = 1; j <= i; ++j) {
            for (int k = 0; k <= L; ++k) {
                for (int m = 0; m <= 2; ++m) {
                    int val = dp[curr][j][k][m];
                    if (!val) continue;

                    int nk = k + (2 * j - m) * diff;
                    if (nk > L) continue;

                    if (j + 1 - m > 0)
                        add(dp[next][j + 1][nk][m], (1LL * val * (j + 1 - m)) % MOD);

                    if (2 - m > 0)
                        add(dp[next][j + 1][nk][m + 1], (1LL * val * (2 - m)) % MOD);

                    if (2 * j - m > 0)
                        add(dp[next][j][nk][m], (1LL * val * (2 * j - m)) % MOD);

                    if (2 - m > 0 && 2 * j - m > 0)
                        add(dp[next][j][nk][m + 1], (1LL * val * (2 - m)) % MOD);

                    if (j - 1 > 0)
                        add(dp[next][j - 1][nk][m], (1LL * val * (j - 1)) % MOD);
                }
            }
        }
    }

    int ans = 0;
    for (int k = 0; k <= L; ++k) {
        add(ans, dp[N % 2][1][k][2]);
    }

    cout << ans << "\n";

    return 0;
}
