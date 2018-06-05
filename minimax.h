#pragma once

#include <limits>

#include "util.h"
#include "board.h"

int minimax_nodes;
int minimax_leaves;
int minimax_pruned;

int min_move(BoardState *state, int d, int player, int alpha, eval_func eval);

int max_move(BoardState *state, int d, int player, int beta, eval_func eval) {
  auto valid_moves = state->moves();

  if (d == 0) {
    minimax_leaves++;
    return eval(state, player);
  }

  minimax_nodes++;

  if (valid_moves.size() == 0) {
    BoardState next_state(*state);
    next_state.pass();
    return min_move(&next_state, d - 1, player, std::numeric_limits<int>::min(), eval);
  }

  int best_score = 0;
  Point best_move;

  for (auto move : valid_moves) {
    BoardState next_state(*state);
    next_state.apply(move);

    int score = min_move(&next_state, d - 1, player, best_score, eval);

    if (score > beta) {
      ++minimax_pruned;
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
    minimax_leaves++;
    return eval(state, player);
  }

  minimax_nodes++;

  if (valid_moves.size() == 0) {
    BoardState next_state(*state);
    next_state.pass();
    return max_move(&next_state, d - 1, player, std::numeric_limits<int>::max(), eval);
  }

  int best_score = std::numeric_limits<int>::max();
  Point best_move;

  for (auto move : valid_moves) {
    BoardState next_state(*state);
    next_state.apply(move);

    int score = max_move(&next_state, d - 1, player, best_score, eval);

    if (score < alpha) {
      ++minimax_pruned;
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

  minimax_nodes = 0;
  minimax_leaves = 0;
  minimax_pruned = 0;

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

    int score = min_move(&next_state, max_depth, player, best_score, eval);

    if (score > best_score) {
      best_move = move;
      best_score = score;
    }
  }

  printf("Minimax %d nodes (%d pruned), %d leaves evaluated\n", minimax_nodes, minimax_pruned, minimax_leaves);

  state->apply(best_move);

  return true;
}