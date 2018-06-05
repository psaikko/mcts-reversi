#include <iostream>
#include <algorithm>
#include <vector>
#include <functional>

#include "board.h"
#include "util.h"
#include "minimax.h"
#include "basic.h"

#define PLY_DEPTH 3
#define RANDOM_GAMES 10

using namespace std;

int simulate_random_game(BoardState *start_state) {

  BoardState state(*start_state);

  bool passed = false;

  while (true) {
    bool pass = !random_move(&state);

    if (pass && passed) break;
    passed = pass;
  }

  int w_score = 0;
  int b_score = 0;

  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8; ++j) {
      if (state.board[i][j] == WHITE)
        w_score++;
      else if (state.board[i][j] == BLACK)
        b_score++;
    }
  }

  // printf("simulated %d-%d\n", w_score, b_score);

  if (w_score > b_score) return WHITE;
  if (w_score < b_score) return BLACK;
  return EMPTY;
}

int eval_pieces(BoardState *state, int player) {
  int s = 0;
  for (int i = 0; i < 8; ++i)
    for (int j = 0; j < 8; ++j)
      if (state->board[i][j] == player)
        ++s;
  return s;
}

int eval_sampling(BoardState *state, int player, int samples) {
  int s = 0;
  for (int i = 0; i < samples; ++i) {
    if (simulate_random_game(state) == player) {
      s++;
    }
  }
  return s;
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

  for (int i = 0; i < 3; ++i) {

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