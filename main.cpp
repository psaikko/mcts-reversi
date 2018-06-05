#include <iostream>
#include <algorithm>
#include <vector>
#include <functional>
#include <cmath>
#include <cassert>

#include "board.h"
#include "util.h"
#include "minimax.h"
#include "basic.h"

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

int ucb1_move(BoardState *state, int n_trials) {

  auto valid_moves = state->moves();

  if (valid_moves.size() == 0) {
    state->pass();
    return false;
  }

  int player = state->next_move;

  vector<double> T(valid_moves.size());
  vector<double> N(valid_moves.size());


  for (int i = 0; i < valid_moves.size(); ++i) {
    BoardState next_state(*state);
    next_state.apply(valid_moves[i]);

    N[i] = 1;
    T[i] = simulate_random_game(&next_state) == player;
  }


  for (int trial = valid_moves.size(); trial < n_trials*valid_moves.size(); ++trial) {
    // choose j with max x_j + sqrt((2 * ln n) / n_j)
    int max_j = -1;
    double max_val = -1;

    for (int j = 0; j < valid_moves.size(); ++j) {
      double val = T[j] / N[j] + sqrt( ( 2 * log(trial) ) / N[j]);
      if (val > max_val) {
        max_val = val;
        max_j = j;
      }
    }

    assert(max_j != -1);

    BoardState next_state(*state);
    next_state.apply(valid_moves[max_j]);

    N[max_j] += 1;
    T[max_j] += simulate_random_game(&next_state) == player;
  }

  Point best_move;
  double best_score = -1;

  printf("UCB1 ");
  for (int i = 0; i < valid_moves.size(); ++i) {

    printf("%.2f ", N[i] ? T[i] / N[i] : 0.0);

    if (N[i] && T[i] / N[i] > best_score) {
      best_score = T[i] / N[i];
      best_move = valid_moves[i];
    }
  }  
  printf("\n");

  state->apply(best_move);

  return true;
}

int main(int argc, char ** argv) {

  using namespace std::placeholders;

  srand(time(0));

  int w_wins = 0;
  int b_wins = 0;

  eval_func eval_sampling_10 = bind(eval_sampling, _1, _2, 10);
  eval_func eval_sampling_100 = bind(eval_sampling, _1, _2, 100);
  eval_func eval_sampling_1000 = bind(eval_sampling, _1, _2, 1000);

  move_func greedy_sampling_10 = bind(greedy_move, _1, eval_sampling_10);
  move_func greedy_sampling_100 = bind(greedy_move, _1, eval_sampling_100);
  move_func greedy_sampling_1000 = bind(greedy_move, _1, eval_sampling_1000);

  move_func ucb1_move_10 = bind(ucb1_move, _1, 10);
  move_func ucb1_move_100 = bind(ucb1_move, _1, 100);
  move_func ucb1_move_1000 = bind(ucb1_move, _1, 1000);

  move_func greedy_pieces = bind(greedy_move, _1, eval_pieces);

  move_func minimax_sampling = bind(minimax_move, _1, eval_sampling_10, 3);
  move_func minimax_pieces = bind(minimax_move, _1, eval_pieces, 3);

  for (int i = 0; i < 100; ++i) {

    BoardState state;

    move_func player_a = ucb1_move_10;   // black
    move_func player_b = greedy_sampling_10;

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