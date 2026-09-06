#include <bits/stdc++.h>
using namespace std;

const int MAXN = 100000 + 1;

struct Node {
    Node *c[2];    // left and right splay children
    Node *p;       // parent in auxiliary tree
    int val;       // node's own value
    int tot;       // sum of subtree
    int mn, mx;    // min and max in subtree
    int sz;        // subtree size
    int set;       // lazy set value
    int inc;       // lazy increment value
    int id;        // original index
    bool rev;      // lazy reverse flag
    bool setl;     // whether set is active

    Node(int x = 0, int idx = 0) {
        c[0] = c[1] = nullptr;
        p = nullptr;
        val = tot = mn = mx = x;
        sz = 1;
        set = inc = 0;
        id = idx;
        rev = setl = false;
    }

    // Push lazy tags down to children and apply to this node
    void push() {
        if (rev) {
            swap(c[0], c[1]);
            if (c[0]) c[0]->rev ^= 1;
            if (c[1]) c[1]->rev ^= 1;
            rev = false;
        }
        if (setl) {
            // assign this subtree to 'set'
            inc = 0;
            val = mn = mx = set;
            tot = sz * val;
            if (c[0]) { c[0]->setl = true; c[0]->set = set; c[0]->inc = 0; }
            if (c[1]) { c[1]->setl = true; c[1]->set = set; c[1]->inc = 0; }
            setl = false;
            set = 0;
        }
        if (inc) {
            // add inc to this subtree
            val += inc;
            mn += inc;
            mx += inc;
            tot += sz * inc;
            if (c[0]) {
                if (c[0]->setl) c[0]->set += inc;
                else c[0]->inc += inc;
            }
            if (c[1]) {
                if (c[1]->setl) c[1]->set += inc;
                else c[1]->inc += inc;
            }
            inc = 0;
        }
    }

    // apply lazy increment to this node/subtree
    void increment(int v) {
        if (setl) set += v;
        else inc += v;
    }

    // apply lazy set to this node/subtree
    void modify(int v) {
        setl = true;
        set = v;
    }
};

Node* lct[MAXN];

// safe getters for null
int sz(Node* t) { return t ? t->sz : 0; }
int val(Node* t) { return t ? t->tot : 0; }
int mn(Node* t)  { return t ? t->mn : INT_MAX; }
int mx(Node* t)  { return t ? t->mx : INT_MIN; }

// recompute aggregates from children; ensure children pushed first
void upd(Node* t) {
    if (!t) return;
    t->push();
    if (t->c[0]) t->c[0]->push();
    if (t->c[1]) t->c[1]->push();
    t->sz = sz(t->c[0]) + sz(t->c[1]) + 1;
    t->tot = val(t->c[0]) + val(t->c[1]) + t->val;
    t->mn = min({ t->val, mn(t->c[0]), mn(t->c[1]) });
    t->mx = max({ t->val, mx(t->c[0]), mx(t->c[1]) });
}

// true if t is root of its splay (i.e., not a child of its parent)
bool isrt(Node* t) {
    return (!t->p) || (t->p->c[0] != t && t->p->c[1] != t);
}

// rotate around parent; dir indicates which child of tmp becomes t's child
void rot(Node* t, bool dir) {
    // dir is used to pick which child of tmp moves
    assert(t);
    assert(t->c[!dir]);
    Node* tmp = t->c[!dir];
    t->c[!dir] = tmp->c[dir];
    tmp->c[dir] = t;
    if (t->c[!dir]) t->c[!dir]->p = t;
    if (!isrt(t)) t->p->c[t->p->c[1] == t] = tmp;
    tmp->p = t->p;
    t->p = tmp;
    upd(t);
    upd(tmp);
}

// splay t to root of its auxiliary tree
void splay(Node* t) {
    assert(t);
    while (!isrt(t)) {
        if (!isrt(t->p)) t->p->p->push();
        t->p->push();
        t->push();
        if (!isrt(t->p)) {
            bool dir = t->p->p->c[1] == t->p;
            if (t->p->c[dir] == t) rot(t->p->p, !dir);
            else rot(t->p, dir);
            rot(t->p, !dir);
        } else {
            rot(t->p, t->p->c[0] == t);
        }
    }
    t->push();
    upd(t);
}

// access: expose path from t to root of represented tree
Node* access(Node* t) {
    Node* v = t;
    Node* last = nullptr;
    while (v) {
        splay(v);
        v->c[1] = last;   // detach previous preferred path and attach last as right child
        last = v;
        v = v->p;
    }
    splay(t);
    return last;
}

// make t the root of the represented tree
void evert(Node* t) {
    access(t);
    t->rev ^= 1;
}

// expose path u..v and return v as splay root representing that path
Node* path(Node* u, Node* v) {
    evert(u);
    access(v);
    return v;
}

// lca via two accesses
Node* lca(Node* u, Node* v) {
    access(u);
    return access(v);
}

// link u as child of v (connect edge u-v)
void link(Node* u, Node* v) {
    evert(u);
    u->p = v;
}

// cut the connection between t and its parent
void cut(Node* t) {
    access(t);
    if (t->c[0]) {
        t->c[0]->p = nullptr;
        t->c[0] = nullptr;
        upd(t);
    }
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int n, q;
    cin >> n >> q;

    // read initial values and create nodes
    for (int i = 1; i <= n; ++i) {
        int a; cin >> a;
        lct[i] = new Node(a, i);
    }

    // build initial tree by linking edges
    for (int i = 1; i <= n - 1; ++i) {
        int a, b; cin >> a >> b;
        link(lct[a], lct[b]);
    }

    int rt; // current root
    cin >> rt;

    while (q--) {
        int k; cin >> k;
        if (k == 0) {
            cin >> rt; // change root
        }
        else if (k == 1) {
            int x, y, z; cin >> x >> y >> z;
            path(lct[x], lct[y])->modify(z); // set path to z
        }
        else if (k == 2) {
            int x, y, z; cin >> x >> y >> z;
            path(lct[x], lct[y])->increment(z); // add z to path
        }
        else if (k == 3) {
            int x, y; cin >> x >> y;
            cout << path(lct[x], lct[y])->mn << '\n';
        }
        else if (k == 4) {
            int x, y; cin >> x >> y;
            cout << path(lct[x], lct[y])->mx << '\n';
        }
        else if (k == 5) {
            int x, y; cin >> x >> y;
            cout << path(lct[x], lct[y])->tot << '\n';
        }
        else if (k == 6) {
            // move x under y if it doesn't create a cycle relative to current root
            evert(lct[rt]);
            int x, y; cin >> x >> y;
            if (lca(lct[x], lct[y]) == lct[x]) continue; // invalid, skip
            cut(lct[x]);
            link(lct[x], lct[y]);
        }
        else if (k == 7) {
            evert(lct[rt]);
            int x, y; cin >> x >> y;
            cout << lca(lct[x], lct[y])->id << '\n';
        }
    }
    return 0;
}
