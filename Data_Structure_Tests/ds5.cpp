#include <bits/stdc++.h>
using namespace std;
using ll = long long;

/*
  Link-Cut Tree (splay-based) with lazy propagation for:
    - path assign, path add, path min/max/sum
    - subtree assign, subtree add, subtree min/max/sum (via re-root trick)
    - change root, change parent (cut + link with cycle check)

  Key invariants and notes:
    - Each Node represents a vertex of the original tree.
    - The splay tree structure stores preferred-path segments.
    - isRoot(x) checks whether x is the root of its splay (not the represented tree).
    - access(x) makes x the rightmost node on the path from the represented tree root to x,
      and after access(x) the preferred path from the represented root to x is represented
      by a single splay tree whose root is x.
    - makeRoot(x) re-roots the represented tree at x by access(x) then toggling a reverse flag.
    - Subtree of x (when tree is rooted at R) can be isolated by: makeRoot(R); access(x);
      then x and x->r (right child) together represent the subtree of x in that rooting.
    - Lazy propagation supports two types of updates: assign (set all values) and add (increment).
      Assign has higher priority: when assign is present, add is applied on top of assign.
    - All aggregate fields (sum, min, max, size) are maintained in pull(x).
*/

struct Node {
    Node *l = nullptr, *r = nullptr, *p = nullptr; // splay children and parent
    bool rev = false;                              // subtree reverse flag for re-rooting

    // value and aggregates for the vertex and its splay-subtree
    ll val = 0;    // current value at this vertex (after applying lazy)
    ll sum = 0;    // sum of values in splay-subtree
    ll mn = LLONG_MAX; // minimum value in splay-subtree
    ll mx = LLONG_MIN; // maximum value in splay-subtree
    int sz = 1;    // size of splay-subtree (number of vertices)

    // lazy propagation
    bool hasAssign = false; // whether an assign is pending
    ll assignVal = 0;       // value to assign
    ll addVal = 0;          // pending addition (applied after assign if assign exists)

    Node(ll v = 0) {
        val = v;
        sum = v;
        mn = v;
        mx = v;
        sz = 1;
    }
};

// --- utility: is x a root of its splay tree (i.e., not a child of its parent in splay sense) ---
inline bool isRoot(Node* x) {
    return !x->p || (x->p->l != x && x->p->r != x);
}

// --- pull: recompute aggregates from children and own value ---
// Must be called after children pointers change or node's own val changes.
inline void pull(Node* x) {
    x->sz = 1;
    x->sum = x->val;
    x->mn = x->val;
    x->mx = x->val;
    if (x->l) {
        x->sz += x->l->sz;
        x->sum += x->l->sum;
        x->mn = min(x->mn, x->l->mn);
        x->mx = max(x->mx, x->l->mx);
    }
    if (x->r) {
        x->sz += x->r->sz;
        x->sum += x->r->sum;
        x->mn = min(x->mn, x->r->mn);
        x->mx = max(x->mx, x->r->mx);
    }
}

// --- applyAssign: set entire splay-subtree to value v ---
// This overwrites previous add lazy; assign has priority.
inline void applyAssign(Node* x, ll v) {
    x->hasAssign = true;
    x->assignVal = v;
    x->addVal = 0;        // clear pending adds because assign overrides
    x->val = v;
    x->sum = v * x->sz;
    x->mn = v;
    x->mx = v;
}

// --- applyAdd: add v to entire splay-subtree ---
// If an assign is pending, we adjust assignVal instead of stacking adds separately.
inline void applyAdd(Node* x, ll v) {
    if (x->hasAssign) {
        // add on top of pending assign: increase assignVal
        x->assignVal += v;
        x->val += v;
        x->sum += v * x->sz;
        x->mn += v;
        x->mx += v;
    } else {
        x->addVal += v;
        x->val += v;
        x->sum += v * x->sz;
        x->mn += v;
        x->mx += v;
    }
}

// --- push: propagate lazy flags (rev, assign, add) to children ---
// Must be called before accessing children or rotating.
inline void push(Node* x) {
    if (!x) return;
    if (x->rev) {
        // reverse children pointers and propagate flag
        swap(x->l, x->r);
        if (x->l) x->l->rev ^= true;
        if (x->r) x->r->rev ^= true;
        x->rev = false;
    }
    if (x->hasAssign) {
        if (x->l) applyAssign(x->l, x->assignVal);
        if (x->r) applyAssign(x->r, x->assignVal);
        x->hasAssign = false;
    }
    if (x->addVal != 0) {
        if (x->l) applyAdd(x->l, x->addVal);
        if (x->r) applyAdd(x->r, x->addVal);
        x->addVal = 0;
    }
}

// --- rotate: single rotation in splay tree ---
// Maintains splay-parent pointers and child pointers; calls pull on affected nodes.
void rotate(Node* x) {
    Node* p = x->p;
    Node* g = p->p;
    // connect x to g
    if (!isRoot(p)) {
        if (g->l == p) g->l = x; else g->r = x;
    }
    x->p = g;

    // perform rotation depending on whether x is left or right child
    if (p->l == x) {
        p->l = x->r;
        if (x->r) x->r->p = p;
        x->r = p;
        p->p = x;
    } else {
        p->r = x->l;
        if (x->l) x->l->p = p;
        x->l = p;
        p->p = x;
    }
    // after rotation, update aggregates bottom-up
    pull(p);
    pull(x);
}

// --- pushAll: push lazy flags from top of splay path down to x ---
// Ensures correct lazy state before splaying x.
void pushAll(Node* x) {
    if (!isRoot(x)) pushAll(x->p);
    push(x);
}

// --- splay: bring x to root of its splay tree ---
// After splay, x->p may still be non-null if x is connected to another splay root.
void splay(Node* x) {
    pushAll(x);
    while (!isRoot(x)) {
        Node* p = x->p;
        Node* g = p->p;
        if (!isRoot(p)) {
            // zig-zig or zig-zag
            if ((p->l == x) ^ (g->l == p)) rotate(x);
            else rotate(p);
        }
        rotate(x);
    }
}

// --- access: expose path from represented-tree root to x ---
// After access(x), x is at the root of the splay representing the preferred path,
// and x->r is null (preferred path to x ends at x). Returns last accessed node.
Node* access(Node* x) {
    Node* last = nullptr;
    for (Node* y = x; y; y = y->p) {
        splay(y);
        // detach previous preferred right child and attach last
        y->r = last;
        pull(y);
        last = y;
    }
    splay(x);
    return last;
}

// --- makeRoot: make x the root of the represented tree ---
// Implementation: access(x); toggle reverse flag on x.
void makeRoot(Node* x) {
    access(x);
    x->rev ^= true;
    push(x); // push so that immediate operations see correct children
}

// --- findRoot: find the root of the represented tree containing x ---
// After access(x), go to the leftmost node in the splay (which is the root).
Node* findRoot(Node* x) {
    access(x);
    // go to leftmost node
    while (true) {
        push(x);
        if (!x->l) break;
        x = x->l;
    }
    splay(x);
    return x;
}

// --- connected: check if two nodes are in the same represented tree ---
// We can compare roots after access.
bool connected(Node* a, Node* b) {
    if (a == b) return true;
    access(a);
    access(b);
    // after access(b), if a has a parent then they are connected
    return a->p != nullptr;
}

// --- link: connect a as a child of b (add edge a-b) ---
// Precondition: a and b are in different trees (no cycle).
void link(Node* a, Node* b) {
    makeRoot(a);            // ensure a is root of its tree
    if (findRoot(b) != a) { // only link if different trees
        a->p = b;           // attach a's splay root under b (preferred edge)
    }
}

// --- cut: remove edge a-b if it exists ---
// Implementation: makeRoot(a); access(b); then if b->l == a and a->r == null, cut.
void cut(Node* a, Node* b) {
    makeRoot(a);
    access(b);
    // after access(b), a should be in b->l if edge exists and a->r must be null
    if (b->l == a && a->r == nullptr) {
        b->l->p = nullptr;
        b->l = nullptr;
        pull(b);
    }
}

// ---------------- Path operations ----------------
// All path operations are implemented by re-rooting one endpoint and accessing the other.
// After makeRoot(u); access(v); the splay rooted at v contains exactly the path u..v
// in its nodes (in-order). So we can apply lazy updates or read aggregates on v.

void pathAssign(Node* u, Node* v, ll val) {
    makeRoot(u);
    access(v);
    applyAssign(v, val);
}

void pathAdd(Node* u, Node* v, ll val) {
    makeRoot(u);
    access(v);
    applyAdd(v, val);
}

ll pathSum(Node* u, Node* v) {
    makeRoot(u);
    access(v);
    return v->sum;
}

ll pathMin(Node* u, Node* v) {
    makeRoot(u);
    access(v);
    return v->mn;
}

ll pathMax(Node* u, Node* v) {
    makeRoot(u);
    access(v);
    return v->mx;
}

// ---------------- Subtree operations ----------------
// Trick: to operate on the subtree of x when the tree is rooted at R:
//   makeRoot(R); access(x);
// After that, x and x->r together represent the subtree of x (in this implementation).
// We apply updates to x (the splay root) and its right child.

void subtreeAssign(Node* currentRoot, Node* x, ll val) {
    makeRoot(currentRoot); // ensure tree is rooted at currentRoot
    access(x);             // isolate subtree of x in x and x->r
    applyAssign(x, val);
    if (x->r) applyAssign(x->r, val);
    pull(x);
}

void subtreeAdd(Node* currentRoot, Node* x, ll val) {
    makeRoot(currentRoot);
    access(x);
    applyAdd(x, val);
    if (x->r) applyAdd(x->r, val);
    pull(x);
}

ll subtreeSum(Node* currentRoot, Node* x) {
    makeRoot(currentRoot);
    access(x);
    return x->sum; // x includes its right subtree after access
}

ll subtreeMin(Node* currentRoot, Node* x) {
    makeRoot(currentRoot);
    access(x);
    return x->mn;
}

ll subtreeMax(Node* currentRoot, Node* x) {
    makeRoot(currentRoot);
    access(x);
    return x->mx;
}

// ---------------- changeParent ----------------
// Set parent of x to y in the rooted tree at currentRoot, if valid.
// If y is inside x's subtree (wrt currentRoot), do nothing to avoid cycles.
// Implementation steps:
//  1) makeRoot(currentRoot); access(x); // isolate subtree of x
//  2) makeRoot(x); if findRoot(y) == x then y is in x's subtree -> abort
//  3) Otherwise cut x from its current parent (if any) and link x under y.
void changeParent(Node* currentRoot, Node* x, Node* y) {
    makeRoot(currentRoot);
    access(x);
    // check if y is in subtree of x by re-rooting at x and checking root of y
    makeRoot(x);
    if (findRoot(y) == x) {
        // y is in x's subtree; linking would create a cycle -> do nothing
        makeRoot(currentRoot); // restore optional
        return;
    }
    // cut x from its parent in the currentRoot-rooted tree
    access(x);
    if (x->l) {
        x->l->p = nullptr;
        x->l = nullptr;
        pull(x);
    }
    // link x under y
    link(x, y);
    makeRoot(currentRoot); // restore optional
}

// ---------------- I/O and main driver ----------------
// The main loop reads the problem-style commands and executes them.
// Commands K correspond to operations described in the problem statement.

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int N, M;
    if (!(cin >> N >> M)) return 0;

    // allocate nodes 1..N
    vector<Node*> nodes(N+1);
    for (int i = 1; i <= N; ++i) nodes[i] = new Node(0);

    // read initial edges and link them (tree)
    for (int i = 0; i < N-1; ++i) {
        int u, v; cin >> u >> v;
        // link only if not already connected (defensive)
        if (!connected(nodes[u], nodes[v])) link(nodes[u], nodes[v]);
    }

    // initial values
    for (int i = 1; i <= N; ++i) {
        ll w; cin >> w;
        nodes[i]->val = w;
        nodes[i]->sum = w;
        nodes[i]->mn = w;
        nodes[i]->mx = w;
        nodes[i]->sz = 1;
    }

    int rootIdx; cin >> rootIdx;
    Node* currentRoot = nodes[rootIdx];

    ostringstream output;
    for (int qi = 0; qi < M; ++qi) {
        int K; cin >> K;
        if (K == 0) {
            int x; ll y; cin >> x >> y;
            subtreeAssign(currentRoot, nodes[x], y);
        } else if (K == 1) {
            int x; cin >> x;
            currentRoot = nodes[x];
        } else if (K == 2) {
            int x,y; ll z; cin >> x >> y >> z;
            pathAssign(nodes[x], nodes[y], z);
        } else if (K == 3) {
            int x; cin >> x;
            output << subtreeMin(currentRoot, nodes[x]) << '\n';
        } else if (K == 4) {
            int x; cin >> x;
            output << subtreeMax(currentRoot, nodes[x]) << '\n';
        } else if (K == 5) {
            int x; ll y; cin >> x >> y;
            subtreeAdd(currentRoot, nodes[x], y);
        } else if (K == 6) {
            int x,y; ll z; cin >> x >> y >> z;
            pathAdd(nodes[x], nodes[y], z);
        } else if (K == 7) {
            int x,y; cin >> x >> y;
            output << pathMin(nodes[x], nodes[y]) << '\n';
        } else if (K == 8) {
            int x,y; cin >> x >> y;
            output << pathMax(nodes[x], nodes[y]) << '\n';
        } else if (K == 9) {
            int x,y; cin >> x >> y;
            changeParent(currentRoot, nodes[x], nodes[y]);
        } else if (K == 10) {
            int x,y; cin >> x >> y;
            output << pathSum(nodes[x], nodes[y]) << '\n';
        } else if (K == 11) {
            int x; cin >> x;
            output << subtreeSum(currentRoot, nodes[x]) << '\n';
        } else {
            // unknown command: ignore or handle gracefully
        }
    }

    cout << output.str();
    return 0;
}

//WA 0/1