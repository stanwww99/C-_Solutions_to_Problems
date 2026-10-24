#include <bits/stdc++.h>
using namespace std;

class FastScanner {
    static const int BUF_SIZE = 1 << 16;
    char buffer[BUF_SIZE];
    int ptr = 0, len = 0;

    inline int read() {
        if (ptr >= len) {
            len = fread(buffer, 1, BUF_SIZE, stdin);
            ptr = 0;
            if (len <= 0) return -1;
        }
        return buffer[ptr++];
    }

public:
    int nextInt() {
        int c;
        while ((c = read()) <= 32);

        int x = 0;
        do {
            x = x * 10 + (c - '0');
            c = read();
        } while (c > 32);

        return x;
    }
};

int main() {
    FastScanner fs;

    int R = fs.nextInt();
    int C = fs.nextInt();
    int M = fs.nextInt();

    vector<int> ur(M);
    vector<int> vc(M);

    vector<vector<bool>> alive(R + 2, vector<bool>(C + 2, false));

    for (int i = 1; i <= R; i++) {
        fill(alive[i].begin(), alive[i].end(), true);
    }

    for (int i = 0; i < M; i++) {
        ur[i] = fs.nextInt();
        vc[i] = fs.nextInt();
        alive[ur[i]][vc[i]] = false;
    }

    vector<vector<short>> dp(R + 2, vector<short>(C + 2, 0));

    int mx = 0;

    for (int i = R; i >= 1; i--) {
        for (int j = C; j >= 1; j--) {
            if (alive[i][j]) {
                dp[i][j] = (short)(1 + min(
                    dp[i + 1][j],
                    min(dp[i][j + 1], dp[i + 1][j + 1])
                ));
                mx = max(mx, (int)dp[i][j]);
            }
        }
    }

    vector<int> ans(M);

    for (int t = M - 1; t >= 0; t--) {

        ans[t] = mx;

        int r = ur[t];
        int c = vc[t];

        alive[r][c] = true;

        for (int i = r; i >= 1; i--) {
            bool changed = false;

            for (int j = c; j >= 1; j--) {

                if (!alive[i][j]) continue;

                short old = dp[i][j];

                short nw = (short)(1 + min(
                    dp[i + 1][j],
                    min(dp[i][j + 1], dp[i + 1][j + 1])
                ));

                if (nw != old) {
                    dp[i][j] = nw;
                    changed = true;
                    if (nw > mx) mx = nw;
                }
            }

            if (!changed) break;
        }
    }

    for (int x : ans) {
        printf("%d\n", x);
    }

    return 0;
}