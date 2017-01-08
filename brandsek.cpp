#include <bits/stdc++.h>
using namespace std;

int get_node_number(int x, map<int,int>& node_numbers, vector<int>& nodes) {
	static int n = 0;
	int y = node_numbers[x];
	if (y > 0) {
		return y;
	}
	nodes.push_back(x);
	node_numbers[x] = ++n;
	assert(nodes.size() - 1 == n);
	return n;
}

vector<vector<int>> read_input(map<int,int>& node_numbers, vector<int>& nodes) {
	vector<vector<int>> G;
	int a, b;
	while (scanf("%d%d", &a, &b) == 2) {
		int an = get_node_number(a, node_numbers, nodes);
		int bn = get_node_number(b, node_numbers, nodes);
		if (G.size() <= an) {
			G.resize(an + 1);
		}
		G[an].push_back(bn);
	}
	return G;
}

void brandes_step(int s, const vector<vector<int>>& G, vector<int>& sigma, vector<int>& d, vector<long double>& delta, vector<vector<int>>& P, vector<long double>& BC) {
	vector<int> S;
	sigma[s] = 1;
	d[s] = 0;
	queue<int> Q;
	Q.push(s);
	while (!Q.empty()) {
		int v = Q.front();
		Q.pop();
		S.push_back(v);
		for (int w: G[v]) {
//			fprintf(stderr, "w %d -- d %d\n", w, d.size());
			if (d[w] < 0) {
				Q.push(w);
				d[w] = d[v] + 1;
			}
			if (d[w] == d[v] + 1) {
				sigma[w] += sigma[v];
				P[w].push_back(v);
			}
		}
	}
	for (int i = S.size() - 1; i >= 0; i --) {
		int w = S[i];
		for (int v: P[w]) 
			delta[v] += (sigma[v] / (long double) sigma[w]) * (1 + delta[w]);
		if (w != s)
			BC[w] += delta[w];
	}
	for (int w: S) {
		P[w].clear();
		sigma[w] = 0;
		d[w] = -1;
		delta[w] = 0;
	}
}

int get_next_node(mutex& get_access) {
	static int last_node = 0;
	lock_guard<mutex> lock(get_access);
	return ++last_node;
}

vector<long double> run(const vector<vector<int>>& G, mutex& get_access) {
	vector<int> sigma, d;
	vector<long double> delta, BC;
	vector<vector<int>> P;
	int n = G.size();
	sigma.resize(n, 0);
	d.resize(n, -1);
	delta.resize(n, 0);
	BC.resize(n, 0);
	P.resize(n);
	
	bool still_going = true;
	while (still_going) {
		int s = get_next_node(get_access);
		if (s < n) {
			brandes_step(s, G, sigma, d, delta, P, BC);
		} else {
			still_going = false;
		}
	}
	return BC;
}

int main(int argc, char* argv[]) {
	int n_threads = atoi(argv[1]);
	freopen(argv[2], "r", stdin);
	freopen(argv[3], "w", stdout);
	
	map<int,int> node_numbers;
	vector<int> nodes(1);
	vector<vector<int> > G = read_input(node_numbers, nodes);
	
	int n = nodes.size();
	G.resize(n);
	
	mutex get_access;
	
	vector<long double> BC = run(G, get_access);
	
	for (auto p: node_numbers) {
		int a = p.first;
		int b = p.second;
		if (not G[b].empty()) {
			printf("%d %Lf\n", a, BC[b]);
		}
	}
}
