#include <iostream>
#include <algorithm>
#include <vector>

#define EMPTY 0
#define BLACK 1
#define WHITE 2

#define OTHER(p) ((p == WHITE) ? BLACK : WHITE)

#define BOUNDS(y, x) ((y >= 0 && y <= 7) && ((x >= 0 && x <= 7)))

using namespace std;

vector<pair<int, int>> adjacent(int y, int x) {
  vector<pair<int, int>> adj;

  if (y > 1) {
    adj.emplace_back(y - 1, x);
    if (x > 1)
      adj.emplace_back(y - 1, x - 1);
    if (x < 7)
      adj.emplace_back(y - 1, x + 1);
  } 

  if (y < 7) {
    adj.emplace_back(y + 1, x);
    if (x > 1)
      adj.emplace_back(y + 1, x - 1);
    if (x < 7)
      adj.emplace_back(y + 1, x + 1);
  }

  if (x > 0)
    adj.emplace_back(y, x - 1);

  if (x < 7)
    adj.emplace_back(y, x + 1);

  return move(adj);
}

struct BoardState {

  int board[8][8];
  int next_move;

  BoardState() : next_move(BLACK) {
    fill(*board, board[7]+8, 0);

    board[3][3] = WHITE;
    board[3][4] = BLACK;
    board[4][3] = BLACK;
    board[4][4] = WHITE;
  }

  void apply(pair<int,int> move) {
    board[move.first][move.second] = next_move;

    for (auto adj_p : adjacent(move.first, move.second)) {
      int x = adj_p.second;
      int y = adj_p.first;

      if (board[y][x] == OTHER(next_move)) {

        int dx = x - move.second;
        int dy = y - move.first;

        while (true) {
          x += dx;
          y += dy;

          if (!BOUNDS(y, x)) {
            break;
          }

          if (board[y][x] == next_move) {
            while (true) {
              x -= dx;
              y -= dy;

              if (board[y][x] == next_move)
                break;

              board[y][x] = next_move;
            }
            break;
          }

          if (board[y][x] == EMPTY) {
            break;
          }
        }
      }
    }

    next_move = OTHER(next_move);
  }

  void pass() {
    next_move = OTHER(next_move);
  }

  vector<pair<int, int>> moves() {
    vector<pair<int, int>> m;
  
    for (int i = 0; i < 8; ++i) {
      for (int j = 0; j < 8; ++j) {
        if (board[i][j] == next_move) {
          for (auto adj_p : adjacent(i, j)) {
            int x = adj_p.second;
            int y = adj_p.first;

            if (board[y][x] == OTHER(next_move)) {

              int dx = x - j;
              int dy = y - i;

              while (true) {
                x += dx;
                y += dy;

                if (!BOUNDS(y, x)) {
                  break;
                }

                if (board[y][x] == next_move) {
                  break;
                }

                if (board[y][x] == EMPTY) {
                  m.emplace_back(y, x);
                  break;
                }
              }
            }
          }
        }
      }
    }

    return move(m);
  }

  void print() {

    auto valid_moves = moves();

    printf(" ");
    for (int i = 0; i < 8; ++i)
      printf("%c", 'a' + i);
    printf("\n");

    for (int i = 0; i < 8; ++i) {
      printf("%d", i+1);
      for (int j = 0; j < 8; ++j) {
        if (board[i][j] == WHITE)
          printf("#");
        else if (board[i][j] == BLACK)
          printf("O");
        else {
          pair<int,int> x(i, j);

          if (find(valid_moves.begin(), valid_moves.end(), x) != valid_moves.end())
            printf("_");
          else 
            printf(" ");
        }
      }
      printf("\n");
    }
  }
};

bool random_move(BoardState *state) {
  auto valid_moves = state->moves();

  if (valid_moves.size() == 0) {
    state->pass();
    return false;
  }

  auto move = valid_moves[rand() % valid_moves.size()];

  state->apply(move);

  return true;
}

bool io_move(BoardState *state) {
  auto valid_moves = state->moves();

  if (valid_moves.size() == 0) {
    state->pass();
    return false;
  }

  while (true) {
    string cmd;
    cin >> cmd;

    if (cmd.size() != 2) {
      cout << "bad command" << endl;
      continue;
    }        

    int row = int(cmd[1] - '1');
    int col = int(cmd[0] - 'a');

    if (!BOUNDS(row, col)) {
      cout << "out of bounds" << endl;
      continue;
    }

    pair<int,int> move(row, col);

    if (find(valid_moves.begin(), valid_moves.end(), move) == valid_moves.end()) {
      cout << "invalid move" << endl;
      continue;
    }

    state->apply(move);
    return true;
  }
}

int main(int argc, char ** argv) {

  BoardState state;

  bool (*player_a)(BoardState *state) = *io_move;
  bool (*player_b)(BoardState *state) = *random_move;

  bool passed = false;

  while (true) {

    state.print();

    bool pass = !(*player_a)(&state);

    if (pass && passed) break;
    passed = pass;
    
    swap(player_a, player_b);
  }
}