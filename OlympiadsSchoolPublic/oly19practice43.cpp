#include <iostream>
#include <vector>
#include <algorithm>

const int MM = 1000001;  // Segment tree covers indices 1 to MM-1 (1 to 1000000)

// Segment tree node structure
struct Node {
    int val;  // Current value
    int lp;   // Lazy propagation flag
    Node() : val(0), lp(0) {}
};

// Query structure with comparison for sorting
struct St {
    int l, r, x, upl, upr;
    // Define < operator for descending order by x (equivalent to Java's compareTo)
    bool operator<(const St& o) const {
        return x > o.x;
    }
};

// Global segment tree and query vectors
std::vector<Node> tree(3 * MM);
std::vector<St> q, s;

// Push up: Combine values from child nodes
void pushUp(int rt) {
    tree[rt].val = tree[rt * 2].val & tree[rt * 2 + 1].val;
}

// Push down: Propagate lazy updates to children
void pushDown(int rt, int nl, int nr) {
    if (tree[rt].lp != 0 && nl != nr) {
        int lc = rt * 2;
        int rc = rt * 2 + 1;
        tree[lc].lp = tree[lc].val = 1;
        tree[rc].lp = tree[rc].val = 1;
    }
    tree[rt].lp = 0;
}

// Update the segment tree in range [l, r] with value val
void update(int l, int r, int val, int nl, int nr, int rt) {
    if (r < nl || l > nr) {
        return;
    }
    if (l <= nl && nr <= r) {
        tree[rt].val = val;
        tree[rt].lp = val;
        return;
    }
    int mid = (nl + nr) / 2;
    pushDown(rt, nl, nr);
    update(l, r, val, nl, mid, rt * 2);
    update(l, r, val, mid + 1, nr, rt * 2 + 1);
    pushUp(rt);
}

// Query the bitwise AND in range [l, r]
int query(int l, int r, int nl, int nr, int rt) {
    if (r < nl || l > nr) {
        return 1;  // Identity for AND when out of range
    }
    if (l <= nl && nr <= r) {
        return tree[rt].val;
    }
    int mid = (nl + nr) / 2;
    pushDown(rt, nl, nr);
    return query(l, r, nl, mid, rt * 2) & query(l, r, mid + 1, nr, rt * 2 + 1);
}

// Helper functions for update and query over the full range
void update(int l, int r, int val) {
    update(l, r, val, 1, MM - 1, 1);
}

int query(int l, int r) {
    return query(l, r, 1, MM - 1, 1);
}

// Process the first 'mid' queries to check if they satisfy the condition
bool go(int mid) {
    // Copy first 'mid' queries from q to s
    for (int i = 0; i < mid; i++) {
        s[i] = q[i];
    }
    // Sort the first 'mid' elements in descending order by x
    std::sort(s.begin(), s.begin() + mid);

    // Reset the segment tree
    for (auto& node : tree) {
        node.val = 0;
        node.lp = 0;
    }

    // Process each query
    for (int i = 0; i < mid; i++) {
        if (s[i].l > s[i].r) {
            return false;
        }
        // Merge consecutive queries with the same x
        if (i < mid - 1 && s[i].x == s[i + 1].x) {
            s[i + 1].l = std::max(s[i].l, s[i + 1].l);
            s[i + 1].r = std::min(s[i].r, s[i + 1].r);
            s[i + 1].upl = std::min(s[i].upl, s[i + 1].upl);
            s[i + 1].upr = std::max(s[i].upr, s[i + 1].upr);
            continue;
        }
        // Check if the range has been updated; if so, condition fails
        if (query(s[i].l, s[i].r) == 1) {
            return false;
        }
        update(s[i].upl, s[i].upr, 1);
    }
    return true;
}

int main() {
    // Optimize input/output
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int n, m;
    std::cin >> n >> m;

    // Resize vectors to hold m queries
    q.resize(m);
    s.resize(m);

    // Read m queries
    for (int i = 0; i < m; i++) {
        std::cin >> q[i].l >> q[i].r >> q[i].x;
        q[i].upl = q[i].l;
        q[i].upr = q[i].r;
    }

    // Binary search to find the largest mid where go(mid) is true
    int l = 1, r = m;
    while (l <= r) {
        int mid = (l + r) / 2;
        if (go(mid)) {
            l = mid + 1;
        } else {
            r = mid - 1;
        }
    }

    // Output the result
    std::cout << (l % (m + 1)) << std::endl;

    return 0;
}