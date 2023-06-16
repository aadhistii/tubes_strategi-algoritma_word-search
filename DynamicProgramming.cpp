#include <bits/stdc++.h>

using namespace std;

class DynamicProgramming {
private:
  // true = print memo table for debugging
  static const bool DEBUG = false;
  // the number of available direction
  static const int DIR = 8;
  // delta for each direction
  const vector<pair<int, int>> DIRECTION = {
    {0, -1}, {-1, -1}, {-1, 0}, {-1, 1},
    {0, 1}, {1, 1}, {1, 0}, {1, -1}
  };

  // memo is for memorizing things (used as dp table)
  // using double linked-list to memorize neighborhood
  struct Memo {
    // meta is for recording mutual head & tail for each direction
    struct Meta {
      vector<Memo*> head;
      vector<Memo*> tail;
      vector<int> size;
      bool taken;

      Meta() {
        this->head.resize(DIR);
        this->head.assign(DIR, nullptr);
        this->tail.resize(DIR);
        this->tail.assign(DIR, nullptr);
        this->size.resize(DIR);
        this->size.assign(DIR, 0);
        this->taken = false;
      }
    };

    int row;
    int col;
    char letter;
    int WORD; // length of target
    vector<vector<Memo*>> next;
    vector<vector<Memo*>> prev;
    vector<vector<Meta*>> meta;

    Memo(int row, int col, char letter, int w) {
      this->row = row;
      this->col = col;
      this->letter = letter;
      this->WORD = w;

      this->next.resize(DIR);
      this->prev.resize(DIR);
      this->meta.resize(DIR);
    }

    bool empty(int dir) {
      return meta[dir].empty();
    }

    bool empty(int dir, int idx) {
      return empty(dir) || meta[dir][idx] == nullptr;
    }

    // chain neighborhood memo and compute mutual head & tail for each direction
    // for example, we have "ABA" as our target words and we have "ABABA" as our test
    // #1 iteration:
    //    for (0,1) direction: this will make A (row=0,col=0) -> B (row=0,col=1) (A chained to B, going forward)
    // #2 iteration:
    //    for (0,1) direction: this will make B (row=0,col=1) -> A (row=0,col=2) (B chained to A, going forward)
    //    for (0,-1) direction: this will make A (row=0,col=0) <- B (row=0,col=1) (B chained to A, going backward)
    //    at this point, we have one solution from (0,1) direction:
    //        A (row=0,col=0) -> B (row=0,col=1) -> A (row=0,col=2)
    // #3 iteration:
    //    for (0,1) direction: this wil make A (row=0,col=2) -> B (row=0,col=3) (A chained to B, going forward)
    //    for (0,-1) direction: this will make B (row=0,col=1) <- A (row=0,col=2) (A chained to B, going backward)
    //    at this point, we have another solution from (0,-1) direction:
    //        A (row=0,col=0) <- B (row=0,col=1) <- A (row=0,col=2)
    // thus, so far we have two solution:
    //    A -> B -> A and A <- B <- A (two exact oposite directions are handled seamlessly)
    // and so on...
    void make_chain(int dir, int idx, Memo* memo) {
      if (empty(dir)) {
        this->next[dir].resize(WORD);
        this->next[dir].assign(WORD, nullptr);
        this->prev[dir].resize(WORD);
        this->prev[dir].assign(WORD, nullptr);
        this->meta[dir].resize(WORD);
        this->meta[dir].assign(WORD, nullptr);
      }

      if (memo->empty(dir)) {
        memo->next[dir].resize(WORD);
        memo->next[dir].assign(WORD, nullptr);
        memo->prev[dir].resize(WORD);
        memo->prev[dir].assign(WORD, nullptr);
        memo->meta[dir].resize(WORD);
        memo->meta[dir].assign(WORD, nullptr);
      }

      this->next[dir][idx] = memo;
      memo->prev[dir][idx + 1] = this;

      if (empty(dir, idx) && memo->empty(dir, idx + 1)) {
        Meta* meta = new Meta();
        meta->head[dir] = this;
        meta->tail[dir] = memo;
        meta->size[dir] = 2;

        this->prev[dir][idx] = nullptr;

        this->meta[dir][idx] = meta; 
        memo->meta[dir][idx + 1] = meta; 
      } else if (empty(dir, idx)) { 
        memo->meta[dir][idx + 1]->head[dir] = this;
        memo->meta[dir][idx + 1]->size[dir] += 1;

        this->prev[dir][idx] = nullptr;

        this->meta[dir][idx] = memo->meta[dir][idx + 1];
      } else if (memo->empty(dir, idx + 1)) {
        this->meta[dir][idx]->tail[dir] = memo;
        this->meta[dir][idx]->size[dir] += 1;

        memo->meta[dir][idx + 1] = this->meta[dir][idx];
      }
    }

    string to_string() {
      stringstream ret;
      ret << "(row=" << row << ",col=" << col << ",lett=" << letter << ")";
      return ret.str();
    }
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
  unordered_map<char, set<int>> indexes;
  vector<vector<Memo*>> memos;

  bool indexable(int idx) {
    return (0 <= idx && idx < w);
  }

  bool traversable(int row, int col) {
    return (0 <= row && row < n) && (0 <= col && col < m);
  }

  void print_memos() {
    for (int row = 0; row < int(memos.size()); row += 1) {
      for (int col = 0; col < int(memos[row].size()); col += 1) {
        if (memos[row][col] == nullptr) {
          continue;
        }

        stringstream ss;
        ss << "(memo=" << memos[row][col]->to_string() << ")\n";

        for (int i = 0; i < DIR; i += 1) {
          pair<int, int> dir = DIRECTION[i];

          if (memos[row][col]->empty(i)) {
            continue;
          }

          ss << "    (dir={" << dir.first << "," << dir.second << "})\n";

          for (int j = 0; j < w; j += 1) {
            if (memos[row][col]->empty(i, j)) {
              continue;
            }

            auto meta = memos[row][col]->meta[i][j];
            int length = meta->size[i];

            ss << "        (";
            ss << "idx=" << j << ",";
            ss << "head=" << meta->head[i]->to_string().c_str() << ",";
            ss << "tail=" << meta->tail[i]->to_string().c_str() << ",";
            ss << "len=" << length << ")\n";

            int k = j;
            Memo* memo = memos[row][col];

            while (memo != nullptr && k < w) {
              ss << "            (";
              ss << "idx=" << k << ",";
              ss << "memo=" << memo->to_string() << ")\n";

              k += 1;
              memo = memo->next[i][k];
            }
          }
        }

        printf("%s", ss.str().c_str());
      }
    }
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

    for (int i = 0; i < w; i += 1) {
      indexes[target[i]].insert(i);
    }

    memos.resize(n);
    memos.assign(n, vector<Memo*>(m, nullptr));

    for (int row = 0; row < n; row += 1) {
      for (int col = 0; col < m; col += 1) {
        char letter = grid[row][col];

        if (indexes.find(letter) == indexes.end()) {
          continue;
        }

        if (memos[row][col] == nullptr) {
          memos[row][col] = new Memo(row, col, letter, w);
        }

        for (int i = 0; i < DIR; i += 1) {
          pair<int, int> dir = DIRECTION[i];
          int d_row = dir.first;
          int d_col = dir.second;
          int n_row = row + d_row;
          int n_col = col + d_col;

          if (!traversable(n_row, n_col)) {
            continue;
          }

          for (int idx : indexes[letter]) {
            if (indexable(idx + 1) && grid[n_row][n_col] == target[idx + 1]) {
              if (memos[n_row][n_col] == nullptr) {
                memos[n_row][n_col] = new Memo(n_row, n_col, grid[n_row][n_col], w);
              }

              memos[row][col]->make_chain(i, idx, memos[n_row][n_col]);

              // collecting result if the list length is equal to `w` and is not taken yet
              if (memos[row][col]->meta[i][idx]->size[i] == w) {
                if (!memos[row][col]->meta[i][idx]->taken) {
                  result.push_back(Result{memos[row][col]->meta[i][idx]->head[i]->row, memos[row][col]->meta[i][idx]->head[i]->col, dir});
                  memos[row][col]->meta[i][idx]->taken = true;
                }
              }
            }
          }
        }
      }
    }

    auto end_time = chrono::high_resolution_clock::now();
    elapsed_time = chrono::duration<double, std::milli>(end_time - begin_time).count();

    if (DEBUG) {
      print_memos();
    }
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

  // solve with dynamic programming
  DynamicProgramming dp;
  dp.solve(grid, target);

  printf("\n----\n\n");

  printf("solution:\n");
  dp.print_result(grid);

  printf("\n----\n\n");

  dp.print_time();

  return 0;
}