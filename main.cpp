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

  BoardState(const BoardState & other) {
    next_move = other.next_move;
    copy(*other.board, other.board[7]+8, *board);
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
      printf(" %c", 'a' + i);
    printf("\n\n");

    for (int i = 0; i < 8; ++i) {
      printf("%d", i+1);
      for (int j = 0; j < 8; ++j) {
        if (board[i][j] == WHITE)
          printf(" \u25CB");
        else if (board[i][j] == BLACK)
          printf(" \u25CF");
        else {
          pair<int,int> x(i, j);

          if (find(valid_moves.begin(), valid_moves.end(), x) != valid_moves.end())
            printf(" _");
          else 
            printf("  ");
        }
      }
      printf("\n\n");
    }
  }

  int score(int player) {
    int s = 0;
    for (int i = 0; i < 8; ++i)
      for (int j = 0; j < 8; ++j)
        if (board[i][j] == player)
          ++s;
    return s;
  }
};

int min_move(BoardState *state, int d, int player);

int max_move(BoardState *state, int d, int player) {
  auto valid_moves = state->moves();

  if (d == 0) {
    return state->score(player);
  }

  if (valid_moves.size() == 0) {
    BoardState next_state(*state);
    next_state.pass();
    return max_move(&next_state, d - 1, player);
  }

  int best_score = 0;
  pair<int,int> best_move;

  for (auto move : valid_moves) {
    BoardState next_state(*state);
    next_state.apply(move);

    int score = min_move(&next_state, d - 1, player);

    if (score > best_score) {
      best_move = move;
      best_score = score;
    }
  }

  return best_score;
}

int min_move(BoardState *state, int d, int player) {
  auto valid_moves = state->moves();

  if (d == 0) {
    return state->score(player);
  }

  if (valid_moves.size() == 0) {
    BoardState next_state(*state);
    next_state.pass();
    return max_move(&next_state, d - 1, player);
  }

  int best_score = numeric_limits<int>::max();
  pair<int,int> best_move;

  for (auto move : valid_moves) {
    BoardState next_state(*state);
    next_state.apply(move);

    int score = max_move(&next_state, d - 1, player);

    if (score < best_score) {
      best_move = move;
      best_score = score;
    }
  }

  return best_score;
}

#define PLY_DEPTH 5

bool minimax_move(BoardState *state) {
  auto valid_moves = state->moves();

  if (valid_moves.size() == 0) {
    //printf("pass\n");
    state->pass();
    return false;
  }

  int player = state->next_move;
  int best_score = 0;
  pair<int,int> best_move;

  for (auto move : valid_moves) {
    BoardState next_state(*state);
    next_state.apply(move);

    int score = min_move(&next_state, PLY_DEPTH, player);

    if (score > best_score) {
      best_move = move;
      best_score = score;
    }
  }

  state->apply(best_move);

  return true;
}

bool greedy_move(BoardState *state) {
  auto valid_moves = state->moves();

  if (valid_moves.size() == 0) {
    state->pass();
    return false;
  }

  int player = state->next_move;

  pair<int,int> best_move;
  int best_score = -1;

  for (auto move : valid_moves) {
    BoardState next_state(*state);
    next_state.apply(move);
    int score = next_state.score(player);

    if (score > best_score) {
      best_move = move;
      best_score = score;
    }
  }

  state->apply(best_move);

  return true;
}

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

  state->print();

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

  srand(time(0));

  int w_wins = 0;
  int b_wins = 0;

  for (int i = 0; i < 100; ++i) {

    BoardState state;

    bool (*player_a)(BoardState *state) = *io_move; // black
    bool (*player_b)(BoardState *state) = *minimax_move;

    bool passed = false;

    while (true) {

      bool pass = !(*player_a)(&state);

      if (pass && passed) break;
      passed = pass;
      
      swap(player_a, player_b);
    }

    int w_score = state.score(WHITE);
    int b_score = state.score(BLACK);

    if (w_score > b_score) w_wins++;
    if (w_score < b_score) b_wins++;

    cout << w_score << " " << b_score << endl;
  }

  cout << "W " << w_wins << endl;
  cout << "B " << b_wins << endl;
}