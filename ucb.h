#pragma once
#include <vector>
#include <cmath>
#include <cassert>

#include "board.h"
#include "util.h"

int ucb1_move(BoardState *state, int n_trials) {

  auto valid_moves = state->moves();

  if (valid_moves.size() == 0) {
    state->apply(PASS);
    return false;
  }

  int player = state->active_player;

  vector<double> T(valid_moves.size());
  vector<double> N(valid_moves.size());

  for (int trial = 0; trial < n_trials*valid_moves.size(); ++trial) {
    // choose j with max x_j + sqrt((2 * ln n) / n_j)
    int max_j = -1;
    double max_val = -1;

    for (int j = 0; j < valid_moves.size(); ++j) {
      if (N[j] == 0) {
        max_j = j;
        break;
      }
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
