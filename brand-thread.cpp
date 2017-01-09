#include <bits/stdc++.h>
using namespace std;

int get_node_number(int x, map<int,int>& node_numbers) {
	//Access to map
	static int n = 0;
	int y = node_numbers[x];
	if (y > 0) {
		return y;
	}
	node_numbers[x] = ++n;
	return n;
}

vector<vector<int>> read_input(map<int,int>& node_numbers) {
	vector<vector<int>> G;
	int a, b;
	while (scanf("%d%d", &a, &b) == 2) {
		int an = get_node_number(a, node_numbers);
		int bn = get_node_number(b, node_numbers);
		if (G.size() <= an) {
			G.resize(an + 1);
		}
		G[an].push_back(bn);
	}
	return G;
}

int get_next_node(mutex& get_access) {
	//Give next node number to proceed loop, thread-safe
	static int last_node = 0;
	lock_guard<mutex> lock(get_access);
	return ++last_node;
}

void run(const vector<vector<int>>& G, mutex& get_access, promise<vector<double>>& result) {
	vector<int> d;
	vector<double> sigma, delta, BC;
	vector<vector<int>> P;
	int n = G.size();
	sigma.resize(n, 0);
	d.resize(n, -1);
	delta.resize(n, 0);
	BC.resize(n, 0);
	P.resize(n);
	
	bool still_going = true;
	while (still_going) { // Brandes loop in pararell
		int s = get_next_node(get_access);
		if (s < n) {
			//One Brandes Algorithm loop
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
					delta[v] += (sigma[v] / sigma[w]) * (1 + delta[w]);
				if (w != s)
					BC[w] += delta[w];
			}
			for (int w: S) {
				P[w].clear();
				sigma[w] = 0;
				d[w] = -1;
				delta[w] = 0;
			}
		} else {
			still_going = false;
		}
	}
	result.set_value(BC);
}

int main(int argc, char* argv[]) {
	int n_threads = atoi(argv[1]);
	if (freopen(argv[2], "r", stdin) == NULL) {
		fprintf(stderr, "Unable to redirect %s to stdin\n", argv[2]);
	}
	if (freopen(argv[3], "w", stdout) == NULL) {
		fprintf(stderr, "Unable to redirect %s to stdout\n", argv[3]);
	}
	
	map<int,int> node_numbers;
	vector<vector<int> > G = read_input(node_numbers);
	
	int n = node_numbers.size() + 1;
	G.resize(n);
	
	mutex get_access;
	
	//Patrial results stored in promise
	vector<promise<vector<double>>> promise_v(n_threads);
	
	for (int i = 1; i < n_threads; i ++) { //we are thread no. 0
		thread t{[&G, &get_access, &promise_v, i]{run(G, get_access, promise_v[i]);}};
		t.detach();
	}
	run(G, get_access, promise_v[0]);
	
	vector<double> BC = promise_v[0].get_future().get();
	
	//Accumulating partial results
	for (int i = 1; i < n_threads; i++) {
		vector<double> BCpart = promise_v[i].get_future().get();
		assert(BC.size() == BCpart.size());
		for (int i = 0; i < BC.size(); i ++) {
			BC[i] += BCpart[i];
		}
	}
	
	for (auto p: node_numbers) {
		int a = p.first;
		int b = p.second;
		if (not G[b].empty()) {
			printf("%d %lf\n", a, BC[b]);
		}
	}
}
