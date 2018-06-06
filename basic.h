#pragma once

#include <iostream>
#include <algorithm>

#include "util.h"
#include "board.h"

bool greedy_move(BoardState *state, eval_func eval) {
  auto valid_moves = state->moves();

  if (valid_moves.size() == 0) {
    state->pass();
    return false;
  }

  int player = state->active_player;

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

    using namespace std;

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

    if (std::find(valid_moves.begin(), valid_moves.end(), move) == valid_moves.end()) {
      cout << "invalid move" << endl;
      continue;
    }

    state->apply(move);

    state->print();

    return true;
  }
}