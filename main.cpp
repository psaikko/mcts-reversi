#include <iostream>
#include <algorithm>
#include <vector>
#include <functional>

#include "board.h"
#include "util.h"
#include "minimax.h"

#define PLY_DEPTH 5
#define RANDOM_GAMES 10

using namespace std;

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

  move_func minimax_sampling = bind(minimax_move, _1, eval_sampling_10, PLY_DEPTH);
  move_func minimax_pieces = bind(minimax_move, _1, eval_pieces, PLY_DEPTH);

  for (int i = 0; i < 10; ++i) {

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