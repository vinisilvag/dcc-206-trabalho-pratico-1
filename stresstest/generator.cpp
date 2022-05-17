#include <bits/stdc++.h>
using namespace std;

#define fast_io ios_base::sync_with_stdio(0);cin.tie(0);
#define endl '\n'

typedef long long ll;

const int INF = 0x3f3f3f3f;
const ll LINF = 0x3f3f3f3f3f3f3f3fLL;

int rand(int a, int b) {
  return rand() % (b - a + 1) + a;
}

int main(int argc, char** argv) { 
  srand(atoi(argv[1]));
  
  const int MX = 450;
  int n = MX, m = MX, v = rand(1, min({n, m, 10}));
  cout << v << endl;
  cout << n << " " << m << endl;

  set<pair<int, int>> used;
  vector<pair<int, int>> positions;
  for (int i = 0; i < 2*v; i++) {
    pair<int, int> position;
    do {
      position.first  = rand(0, n - 1);
      position.second = rand(0, m - 1);
    } while (used.count(position));
    used.insert(position);
    positions.push_back(position);
  }

  vector<string> grid(n, string(m, '*'));

  for (int i = 0; i < v; i++) {
    auto [a, b] = positions[i];
    grid[a][b] = char('a' + i);
  }
  for (int i = 0; i < v; i++) {
    auto [a, b] = positions[v + i];
    grid[a][b] = char('0' + i);
  }

  vector<vector<bool>> vis(n, vector<bool>(m, false));

  function<void(int, int)> flood_fill = [&] (int i, int j) {
    if (i < 0 || i >= n) return;
    if (j < 0 || j >= m) return;
    if (vis[i][j]) return;
    if (grid[i][j] == '-') return;
    vis[i][j] = true;
    flood_fill(i - 1, j);
    flood_fill(i + 1, j);
    flood_fill(i, j - 1);
    flood_fill(i, j + 1);
  };

  auto backup = grid;
  while (true) {
    grid = backup;
    for (int i = 0; i < n; i++) {
      for (int j = 0; j < m; j++) {
        if (grid[i][j] != '*') continue;
        if (rand(0, 100) < 30) {
          grid[i][j] = '-';
        }
      }
    }

    vis.assign(n, vector<bool>(m, false));
    flood_fill(positions[0].first, positions[0].second);
    bool everyone_ok = true;
    for (auto [a, b] : positions) {
      everyone_ok &= vis[a][b] == true;
    }
    if (everyone_ok) {
      break;
    }
  }

  for (auto s : grid)
    cout << s << endl;

  for (int i = 0; i < v; i++) {
    for (int j = 0; j < v; j++) {
      cout << rand(1, 10) << " \n"[j == v - 1];
    }
  }
  
  return 0;
}
