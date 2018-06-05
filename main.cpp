#include <iostream>
#include <algorithm>
#include <vector>
#include <functional>

#define EMPTY 0
#define BLACK 1
#define WHITE 2

#define PLY_DEPTH 2
#define RANDOM_GAMES 10

#define OTHER(p) ((p == WHITE) ? BLACK : WHITE)

#define BOUNDS(y, x) ((y >= 0 && y <= 7) && ((x >= 0 && x <= 7)))

using namespace std;

struct BoardState;

typedef pair<int,int> Point;
//typedef int eval_func(BoardState*, int);
typedef std::function<int(BoardState*, int)> eval_func;
//typedef bool move_func(BoardState*);
typedef std::function<bool(BoardState*)> move_func;

vector<Point> adjacent(int y, int x) {
  vector<Point> adj;

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

  void apply(Point move) {
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

  vector<Point> moves() {
    vector<Point> m;
  
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
          Point x(i, j);

          if (find(valid_moves.begin(), valid_moves.end(), x) != valid_moves.end())
            printf(" _");
          else 
            printf(" .");
        }
      }
      printf("\n\n");
    }
  }
};

int eval_pieces(BoardState *state, int player) {
  int s = 0;
  for (int i = 0; i < 8; ++i)
    for (int j = 0; j < 8; ++j)
      if (state->board[i][j] == player)
        ++s;
  return s;
}

int simulate_random_game(BoardState *start_state);

int eval_sampling(BoardState *state, int player, int samples) {
  int s = 0;
  for (int i = 0; i < samples; ++i) {
    if (simulate_random_game(state) == player) {
      s++;
    }
  }
  return s;
}

int min_move(BoardState *state, int d, int player, int alpha, eval_func eval);

int max_move(BoardState *state, int d, int player, int beta, eval_func eval) {
  auto valid_moves = state->moves();

  if (d == 0) {
    return eval(state, player);
  }

  if (valid_moves.size() == 0) {
    BoardState next_state(*state);
    next_state.pass();
    return min_move(&next_state, d - 1, player, numeric_limits<int>::min(), eval);
  }

  int best_score = 0;
  Point best_move;

  for (auto move : valid_moves) {
    BoardState next_state(*state);
    next_state.apply(move);

    int score = min_move(&next_state, d - 1, player, best_score, eval);

    if (score > beta) {
      best_score = beta;
      break;
    }

    if (score > best_score) {
      best_move = move;
      best_score = score;
    }
  }

  return best_score;
}

int min_move(BoardState *state, int d, int player, int alpha, eval_func eval) {
  auto valid_moves = state->moves();

  if (d == 0) {
    return eval(state, player);
  }

  if (valid_moves.size() == 0) {
    BoardState next_state(*state);
    next_state.pass();
    return max_move(&next_state, d - 1, player, numeric_limits<int>::max(), eval);
  }

  int best_score = numeric_limits<int>::max();
  Point best_move;

  for (auto move : valid_moves) {
    BoardState next_state(*state);
    next_state.apply(move);

    int score = max_move(&next_state, d - 1, player, best_score, eval);

    if (score < alpha) {
      best_score = alpha;
      break;
    }

    if (score < best_score) {
      best_move = move;
      best_score = score;
    }
  }

  return best_score;
}

bool minimax_move(BoardState *state, eval_func eval) {
  auto valid_moves = state->moves();

  if (valid_moves.size() == 0) {
    //printf("pass\n");
    state->pass();
    return false;
  }

  int player = state->next_move;
  int best_score = 0;
  Point best_move;

  for (auto move : valid_moves) {
    BoardState next_state(*state);
    next_state.apply(move);

    int score = min_move(&next_state, PLY_DEPTH, player, best_score, eval);

    if (score > best_score) {
      best_move = move;
      best_score = score;
    }
  }

  state->apply(best_move);

  return true;
}

bool greedy_move(BoardState *state, eval_func eval) {
  auto valid_moves = state->moves();

  if (valid_moves.size() == 0) {
    state->pass();
    return false;
  }

  int player = state->next_move;

  Point best_move;
  int best_score = -1;

  for (auto move : valid_moves) {
    BoardState next_state(*state);
    next_state.apply(move);
    int score = eval(&next_state, player);

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

    Point move(row, col);

    if (find(valid_moves.begin(), valid_moves.end(), move) == valid_moves.end()) {
      cout << "invalid move" << endl;
      continue;
    }

    state->apply(move);

    state->print();

    return true;
  }
}

int simulate_random_game(BoardState *start_state) {

  BoardState state(*start_state);

  bool passed = false;

  while (true) {
    bool pass = !random_move(&state);

    if (pass && passed) break;
    passed = pass;
  }

  int w_score = eval_pieces(&state, WHITE);
  int b_score = eval_pieces(&state, BLACK);

  // printf("simulated %d-%d\n", w_score, b_score);

  if (w_score > b_score) return WHITE;
  if (w_score < b_score) return BLACK;
  return EMPTY;
}

bool sampling_move(BoardState *state) {
  auto valid_moves = state->moves();

  if (valid_moves.size() == 0) {
    state->pass();
    return false;
  }

  int player = state->next_move;
  int best_score = -1;
  Point best_move;

  for (auto move : valid_moves) {
    BoardState next_state(*state);
    next_state.apply(move);

    int score = eval_sampling(&next_state, player, RANDOM_GAMES);

    if (score > best_score) {
      best_move = move;
      best_score = score;
    }
  }

  state->apply(best_move);

  return true;
}

int main(int argc, char ** argv) {

  using namespace std::placeholders;

  srand(time(0));

  int w_wins = 0;
  int b_wins = 0;

  eval_func eval_sampling_10 = bind(eval_sampling, _1, _2, 10);

  move_func greedy_sampling = bind(greedy_move, _1, eval_sampling_10);
  move_func greedy_pieces = bind(greedy_move, _1, eval_pieces);

  move_func minimax_sampling = bind(minimax_move, _1, eval_sampling_10);
  move_func minimax_pieces = bind(minimax_move, _1, eval_pieces);

  for (int i = 0; i < 1000; ++i) {

    BoardState state;

    move_func player_a = minimax_pieces;   // black
    move_func player_b = random_move;

    bool passed = false;

    while (true) {

      bool pass = !player_a(&state);

      if (pass && passed) break;
      passed = pass;
      
      swap(player_a, player_b);
    }

    int w_score = eval_pieces(&state, WHITE);
    int b_score = eval_pieces(&state, BLACK);

    if (w_score > b_score) w_wins++;
    if (w_score < b_score) b_wins++;

    cout << w_score << " " << b_score << endl;
  }

  cout << "W " << w_wins << endl;
  cout << "B " << b_wins << endl;
}