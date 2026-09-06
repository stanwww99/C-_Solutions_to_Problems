#include <algorithm>
#include <vector>

using namespace std;

// Global sparse table + log table
static vector<vector<vector<vector<int>>>> st;
static vector<int> logTable;

// Build the 2D sparse table
void init(const vector<vector<int>> grid) {
    int rows = grid.size();
    int cols = grid[0].size();

    // Precompute logs up to max dimension
    int maxDim = max(rows, cols);
    logTable.assign(maxDim + 1, 0);
    for (int i = 2; i <= maxDim; i++)
        logTable[i] = logTable[i / 2] + 1;

    int maxRowPow = logTable[rows] + 1;
    int maxColPow = logTable[cols] + 1;

    // Allocate sparse table:
    // st[rowPow][colPow][row][col]
    st.assign(
        maxRowPow,
        vector<vector<vector<int>>>(
            maxColPow,
            vector<vector<int>>(rows, vector<int>(cols))
        )
    );

    // Base layer: 1×1 blocks
    for (int r = 0; r < rows; r++)
        for (int c = 0; c < cols; c++)
            st[0][0][r][c] = grid[r][c];

    // Build along columns (increase width)
    for (int colPow = 1; colPow < maxColPow; colPow++) {
        int half = 1 << (colPow - 1);
        for (int r = 0; r < rows; r++) {
            for (int c = 0; c + (1 << colPow) <= cols; c++) {
                st[0][colPow][r][c] = min(
                    st[0][colPow - 1][r][c],
                    st[0][colPow - 1][r][c + half]
                );
            }
        }
    }

    // Build along rows (increase height)
    for (int rowPow = 1; rowPow < maxRowPow; rowPow++) {
        int half = 1 << (rowPow - 1);
        for (int colPow = 0; colPow < maxColPow; colPow++) {
            for (int r = 0; r + (1 << rowPow) <= rows; r++) {
                for (int c = 0; c + (1 << colPow) <= cols; c++) {
                    st[rowPow][colPow][r][c] = min(
                        st[rowPow - 1][colPow][r][c],
                        st[rowPow - 1][colPow][r + half][c]
                    );
                }
            }
        }
    }
}

// Query rectangle [a..b] × [c..d]
int query(int a, int b, int c, int d) {
    int rowLen = b - a + 1;
    int colLen = d - c + 1;

    int rowPow = logTable[rowLen];
    int colPow = logTable[colLen];

    int rowOffset = 1 << rowPow;
    int colOffset = 1 << colPow;

    return min({
        st[rowPow][colPow][a][c],
        st[rowPow][colPow][b - rowOffset + 1][c],
        st[rowPow][colPow][a][d - colOffset + 1],
        st[rowPow][colPow][b - rowOffset + 1][d - colOffset + 1]
    });
}