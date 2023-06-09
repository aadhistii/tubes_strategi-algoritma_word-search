#include <bits/stdc++.h>

using namespace std;

class BruteForce {
private:
  // true = use recursive approach, false = use iterative approach
  static const bool REC = true;
  const vector<pair<int, int>> DIRECTION = {
    {0, -1}, {-1, -1}, {-1, 0}, {-1, 1},
    {0, 1}, {1, 1}, {1, 0}, {1, -1}
  };

  struct Result {
    int row;
    int col;
    pair<int, int> dir;

    Result(int row, int col, pair<int, int> dir) {
      this->row = row;
      this->col = col;
      this->dir = dir;
    }

    string to_string() {
      stringstream ret;
      ret << "(row=" << row << ",col=" << col << ",dir={" << dir.first << "," << dir.second << "})";
      return ret.str();
    }
  };

  int n, m, w;
  double elapsed_time;

  bool traversable(int row, int col, int idx) {
    return (0 <= row && row < n) && (0 <= col && col < m) && (0 <= idx && idx < w);
  }

  string recurse(const vector<string>& grid, int row, int col, int d_row, int d_col, int idx) {
    if (!traversable(row, col, idx)) {
      return "";
    }

    string ret = string(1, grid[row][col]);

    return ret + recurse(grid, row + d_row, col + d_col, d_row, d_col, idx + 1);
  }

public:
  vector<Result> result;

  void print_time() {
    printf("time needed to find %d solution(s) is %.2fms (%.6fs)\n", int(result.size()), elapsed_time, elapsed_time / double(1000));
  }

  void print_result(const vector<string>& grid) {
    for (auto res : result) {
      printf("%s\n", res.to_string().c_str());

      int row = res.row;
      int col = res.col;
      int d_row = res.dir.first;
      int d_col = res.dir.second;

      printf("[");
      for (int i = 0; i < w; i += 1) {
        if (i > 0) printf(",");
        printf("{(%d,%d),%c}", row, col, grid[row][col]);

        row += d_row;
        col += d_col;
      }
      printf("]\n");
    }
  }

  void solve(const vector<string>& grid, const string& target) {
    auto begin_time = chrono::high_resolution_clock::now();

    n = grid.size();
    m = grid[0].size();
    w = target.size();

    for (int row = 0; row < n; row += 1) {
      for (int col = 0; col < m; col += 1) {
        char letter = grid[row][col];

        for (auto dir : DIRECTION) {
          int idx = 0;
          int c_row = row;
          int c_col = col;
          int d_row = dir.first;
          int d_col = dir.second;
          string words = "";

          if (REC) {
            words = recurse(grid, row, col, d_row, d_col, 0);
          } else {
            while (traversable(c_row, c_col, idx)) {
              words += grid[c_row][c_col];

              idx += 1;
              c_row += d_row;
              c_col += d_col;
            }
          }

          if (words == target) {
            result.push_back(Result{row, col, dir});
          }
        }
      }
    }

    auto end_time = chrono::high_resolution_clock::now();
    elapsed_time = chrono::duration<double, std::milli>(end_time - begin_time).count();
  }
};

// testing

int main() {
  // get input
  int n;
  cin >> n;

  vector<string> grid (n);
  for (int i = 0; i < n; i += 1) {
    cin >> grid[i];
  }

  string target;
  cin >> target;

  // solve with brute force
  BruteForce bf;
  bf.solve(grid, target);

  printf("\n----\n\n");

  printf("solution:\n");
  bf.print_result(grid);

  printf("\n----\n\n");

  bf.print_time();

  return 0;
}