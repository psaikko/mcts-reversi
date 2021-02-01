#pragma once

#include <limits>

#include "util.h"
#include "board.h"

int min_move(BoardState *state, int d, int player, int alpha, eval_func eval);

int max_move(BoardState *state, int d, int player, int beta, eval_func eval) {
  auto valid_moves = state->moves();

  if (d == 0) {
    return eval(state, player);
  }

  if (valid_moves.size() == 0) {
    BoardState next_state(*state);
    next_state.apply(PASS);
    return min_move(&next_state, d - 1, player, std::numeric_limits<int>::min(), eval);
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
    next_state.apply(PASS);
    return max_move(&next_state, d - 1, player, std::numeric_limits<int>::max(), eval);
  }

  int best_score = std::numeric_limits<int>::max();
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

bool minimax_move(BoardState *state, eval_func eval, int max_depth) {
  auto valid_moves = state->moves();

  if (valid_moves.size() == 0) {
    printf("pass\n");
    state->apply(PASS);
    return false;
  }

  int player = state->active_player;
  int best_score = -1;
  Point best_move;

  for (auto move : valid_moves) {
    BoardState next_state(*state);
    next_state.apply(move);

    int score = min_move(&next_state, max_depth, player, best_score, eval);
    if (score > best_score) {
      best_move = move;
      best_score = score;
    }
  }
  assert(best_score != -1);
  state->apply(best_move);

  return true;
}