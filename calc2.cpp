#include<bits/stdc++.h>
#define LD long double
using namespace std;
int main()
{
int ile = 50;

LD time1;

for(int i = 1; i <= 8; i++)
	{
	LD mi = 1e9;
	LD mx = -1e9;
	LD sum = 0;
	string s;
	getline(cin, s);
	for(int j = 1; j <= ile; j++)
		{
		string s;
		scanf(" ");
		char buf[100];
		long double x;
		scanf("%s 0m%Lfs ", buf, &x);
//		cerr<<x<<endl;
		mi = min(x, mi);
		mx = max(x, mx);
		sum += x;

		getline(cin, s);
		scanf(" ");
		getline(cin, s);
		}
	if(i == 1)time1 = sum / ile;
	sum /= ile;
	cout<<i<<" "<<mi<<" "<<mx<<" "<<sum<<" "<<time1/sum<<endl;
	}
}
