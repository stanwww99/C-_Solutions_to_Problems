#include <bits/stdc++.h>
using namespace std;

#define ll long long

ll findSum(int L, int R, int rem, int rightQuery) {
    if (rem == 0 || L > R ) return 0LL;
    if (L == R) return ((ll)L)*rem;
    int mid = (L+R)>>1;
    //query mid + 1
    printf("? %d\n",mid+1);
    fflush(stdout);
    int right;
    scanf("%d",&right);
    right -= rightQuery;
    return findSum(L,mid,rem-right,rightQuery+right)+findSum(mid+1,R,right,rightQuery);
}


int main() {
    int N;
    scanf("%d",&N);
    printf("! %lld\n",findSum(0,10000000,N,0));

}