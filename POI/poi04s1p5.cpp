#include <algorithm>
#include <cstdio>
#include <functional>
#include <vector>

using namespace std;

/*
  Problem summary (what this program computes)
  - Input: n queries of the form (a, b, d).
  - For each query we reduce a := a/d, b := b/d and compute the number of pairs
    (x, y) with 1 <= x <= a, 1 <= y <= b and gcd(x, y) == 1.
  - The program answers all queries efficiently by:
    1) Precomputing a small factor helper F for fast recursive coprime counting.
    2) Reordering queries and extracting long monotone subsequences so we can
       walk (a,b) incrementally (±1 steps) and update the current count cheaply.
*/

typedef unsigned int uint;
typedef unsigned long long ull;

// store a single query (reduced a,b and original index i)
struct question {
  uint i, a, b;
  // ordering by b (used for initial sort)
  bool operator<(question q) const { return b < q.b; }
  bool operator>(question q) const { return b > q.b; }
  bool operator==(question q) const { return i == q.i; }
};

// global containers used across functions
vector<question> Q;    // all queries to process
vector<ull> answer;    // answers indexed by original query id
uint m = 2;            // maximum value among reduced a and b (at least 2)

// helper structure: for each x, store one prime factor p and the remainder r
// such that x = p^k * r with p not dividing r. This enables a one-step factorization.
struct split {
  uint p, r;
};
vector<split> F;

/* -------------------- Input reading and preprocessing -------------------- */

void read_questions()
{
  uint n;
  scanf("%u", &n);
  Q.reserve(n);
  answer.resize(n);
  for (uint i = 0; i < n; i++) {
    question q;
    uint d;
    // read a, b, d
    scanf("%u%u%u", &q.a, &q.b, &d);
    q.i = i;
    // reduce by d immediately
    q.a /= d;
    q.b /= d;
    // ensure a >= b to simplify later processing (we'll keep pairs ordered)
    if (q.a < q.b)
      swap(q.a, q.b);
    Q.push_back(q);
    // track maximum value needed for sieving / factor helper
    m = max(m, max(q.a, q.b));
  }
}

/*
  Build F so that for any x (2..m) we can get:
    F[x].p = a prime p dividing x (specifically the base prime of the highest p-power dividing x)
    F[x].r = x / (p^k) where p^k is the maximal power of p dividing x
  Implementation detail:
    - We first sieve primes up to m (is_prime).
    - For each prime p we call sieve_F(p, p) which marks multiples of p^k.
    - For each multiple i of p^k we set F[i].p = p and F[i].r = i / p^k,
      but only if F[i].p was not set yet (we keep the first prime-power factorization).
*/
void sieve_F(uint p, uint q)
{
  if (q > m)
    return;
  // recursively process next power p^(k+1)
  sieve_F(p, q * p);
  // mark all numbers that are multiples of q = p^k
  for (uint i = q, r = 1; i <= m; i += q, r++)
    if (F[i].p == 1) {     // not set yet
      F[i].p = p;
      F[i].r = r;          // r = i / q
    }
}

void prepare_F()
{
  // simple sieve to find primes up to m
  vector<bool> is_prime(m + 1, true);
  // 0 and 1 are not prime
  if (m >= 0) is_prime[0] = false;
  if (m >= 1) is_prime[1] = false;

  // classic sieve of Eratosthenes
  for (uint p = 2; p * p <= m; ++p) {
    if (is_prime[p]) {
      for (uint j = p * p; j <= m; j += p)
        is_prime[j] = false;
    }
  }

  // initialize F with p = 1 meaning "unset"
  F = vector<split>(m + 1);
  for (uint i = 1; i <= m; i++)
    F[i].p = 1;

  // for each prime, mark multiples of p^k using sieve_F
  for (uint i = 2; i <= m; i++)
    if (is_prime[i])
      sieve_F(i, i);
}

/* -------------------- Fast coprime counting -------------------- */

/*
  Count numbers in {1..a} that are coprime with b.
  Uses recursion and the precomputed F table:
    - If b == 1 => all numbers 1..a are coprime => return a.
    - Otherwise let (p, r) = F[b], where b = p^k * r and p does not divide r.
      Then numbers coprime to b are numbers coprime to r but not divisible by p.
      So:
        coprime_line_count(a, b) = coprime_line_count(a, r) - coprime_line_count(a / p, r)
  This is a small recursion depth equal to the number of distinct prime factors of b.
*/
uint coprime_line_count(uint a, uint b)
{
  if (b == 0 || a == 0)
    return 0;
  else if (b == 1)
    return a;
  else {
    uint p = F[b].p;
    uint r = F[b].r;
    // inclusion-exclusion step removing multiples of p
    return coprime_line_count(a, r) - coprime_line_count(a / p, r);
  }
}

/* -------------------- Long monotone subsequence extraction -------------------- */

/*
  We want to find long subsequences of Q that are monotone in (a,b) so we can
  process many queries by walking (a,b) incrementally. The function below
  implements patience-sorting style LIS (Longest Increasing Subsequence) but
  returns the actual subsequence (not just length). We use it twice:
    - once with less<question>() to get an increasing subsequence
    - once with greater<question>() to get a decreasing subsequence
  Then we pick the longer of the two.
*/

// comparator that orders by (a, then b) ascending
struct question_ab_less : public binary_function<question, question, bool> {
  bool operator()(question x, question y)
  {
    if (x.a < y.a) return true;
    if (y.a < x.a) return false;
    return x.b < y.b;
  }
};

// generic patience-sorting LIS reconstruction using comparator Cmp
template <typename Cmp>
vector<question> find_increasing_subsequence(Cmp cmp)
{
  vector<int> prev(Q.size(), -1);      // previous index in subsequence
  vector<int> qidx;                    // indices of chosen elements for each pile
  vector<question> piles;              // top elements of piles

  for (uint i = 0; i < Q.size(); i++) {
    // find position to place Q[i] using upper_bound with comparator cmp
    auto it = upper_bound(piles.begin(), piles.end(), Q[i], cmp);
    if (it == piles.end()) {
      // new pile
      qidx.push_back((int)i);
      piles.push_back(Q[i]);
      it = piles.end() - 1;
    } else {
      // replace top of existing pile
      qidx[it - piles.begin()] = (int)i;
      *it = Q[i];
    }
    // set predecessor for reconstruction
    if (it != piles.begin())
      prev[i] = qidx[it - piles.begin() - 1];
  }

  // reconstruct the subsequence indices
  vector<question> result;
  if (!qidx.empty()) {
    int j = qidx.back();
    result.resize(piles.size());
    for (int k = (int)piles.size() - 1; k >= 0; --k) {
      result[k] = Q[j];
      j = prev[j];
    }
  }
  return result;
}

// choose the longer of increasing or decreasing subsequence
vector<question> get_monotonic_subsequence()
{
  vector<question> seq1 = find_increasing_subsequence(less<question>());
  vector<question> seq2 = find_increasing_subsequence(greater<question>());
  return seq1.size() > seq2.size() ? seq1 : seq2;
}

/* -------------------- Remove subsequence from Q -------------------- */

// Remove the elements of seq from Q while preserving order of remaining elements.
void strip_subsequence(vector<question> seq)
{
  uint i = 0, j = 0, k = 0;
  while (k < seq.size()) {
    if (Q[i] == seq[k]) {
      // skip this element (it belongs to the subsequence)
      k++; i++;
    } else {
      // keep this element
      Q[j++] = Q[i++];
    }
  }
  // copy any remaining elements after we've exhausted seq
  while (i < Q.size())
    Q[j++] = Q[i++];
  Q.resize(j);
}

/* -------------------- Answering one monotone subsequence -------------------- */

/*
  For a monotone subsequence seq we maintain a current point (a, b) and the
  current count c = number of coprime pairs for that (a, b). We then move
  a or b by ±1 steps until we reach each query's (a, b), updating c by adding
  or subtracting coprime_line_count for the changed coordinate.
*/
void find_answer_for(vector<question> seq)
{
  auto it = seq.begin();
  // initialize current (a,b) and count c to the first element's b and a = 0
  // (we start from a = 0, b = first.b so we can increment a up to the query's a)
  uint a = 0;
  uint b = it->b;
  ull c = 0;

  while (it != seq.end()) {
    if (a == it->a && b == it->b) {
      // reached the query point: record answer
      answer[it->i] = c;
      ++it;
    } else if (a < it->a) {
      // increase a by 1 and add numbers in [1..b] coprime with new a
      c += coprime_line_count(b, ++a);
    } else if (a > it->a) {
      // decrease a by 1 and remove contribution
      c -= coprime_line_count(b, a--);
    } else if (b < it->b) {
      // increase b by 1 and add numbers in [1..a] coprime with new b
      c += coprime_line_count(a, ++b);
    } else {
      // decrease b by 1 and remove contribution
      c -= coprime_line_count(a, b--);
    }
  }
}

/* -------------------- Main solve loop -------------------- */

void solve()
{
  // sort queries by b (primary) to group similar b values
  sort(Q.begin(), Q.end(), question_ab_less());

  // repeatedly extract a long monotone subsequence, process it, and remove it
  do {
    vector<question> mon = get_monotonic_subsequence();
    strip_subsequence(mon);
    find_answer_for(mon);
  } while (!Q.empty());

  // print answers in original input order
  for (uint i = 0; i < answer.size(); i++)
    printf("%llu\n", answer[i]);
}

/* -------------------- Program entry point -------------------- */

int main()
{
  read_questions();   // read and reduce queries
  prepare_F();        // build factor helper F up to max m
  solve();            // process queries and print answers
  return 0;
}
