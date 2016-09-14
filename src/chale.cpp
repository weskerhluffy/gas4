/**
http://www.programering.com/a/MDN5ATNwATY.html
*/
#include <cstdio>
#include <iostream>
#include <algorithm>
#include <cstring>
#include <cmath>
#define maxn 200005
#define lson num<<1,s,mid
#define rson num<<1|1,mid+1,e
typedef long long LL;
using namespace std;

LL cov[maxn<<2];

void pushup(int num)
{
    if(cov[num<<1]==cov[num<<1|1])cov[num]=cov[num<<1];
    else cov[num]=-1;
}
void pushdown(int num)
{
    if(cov[num]!=-1)
    {
        cov[num<<1]=cov[num<<1|1]=cov[num];
        cov[num]=-1;
    }
}
void build(int num,int s,int e)
{
    cov[num]=-1;
    if(s==e)
    {
        scanf("%lld",&cov[num]);
        return ;
    }
    int mid=(s+e)>>1;
    build(lson);
    build(rson);
    pushup(num);
}

void update(int num,int s,int e,int l,int r)
{
    if(l<=s && r>=e)
    {
        if(cov[num]!=-1)
        {
            cov[num]=(LL)sqrt(cov[num]*1.0);
            return;
        }
    }
    int mid=(s+e)>>1;
    pushdown(num);
    if(l<=mid)update(lson,l,r);
    if(r>mid)update(rson,l,r);
    pushup(num);
}
LL query(int num,int s,int e,int l,int r)
{
    if(cov[num]!=-1)return cov[num]*(r-l+1);
    if(l<=s && r>=e)
    {
        if(cov[num]!=-1)
        return cov[num]*(e-s+1);
    }
    int mid=(s+e)>>1;
    pushdown(num);
    if(r<=mid)return query(lson,l,r);
    else if(l>mid)return query(rson,l,r);
    else return query(lson,l,mid)+query(rson,mid+1,r);
}
int main()
{
    int CASE=1;
    int n;
    while(scanf("%d",&n)!=EOF)
    {

        build(1,1,n);
        int m;
        scanf("%d",&m);

        printf("Case #%d:\n",CASE++);
        while(m--)
        {
            int op,l,r;
            scanf("%d%d%d",&op,&l,&r);
            if(l>r)swap(l,r);
            if(op)
            {
                printf("%lld\n",query(1,1,n,l,r));
            }
            else update(1,1,n,l,r);
        }

        puts("");
    }
    return 0;
}
