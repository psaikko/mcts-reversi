#pragma once

#include <iostream>
#include <algorithm>
#include <functional>
#include <limits>

#include "util.h"
#include "board.h"


bool greedy_move(BoardState *state, eval_func eval) {
  auto valid_moves = state->moves();

  if (valid_moves.size() == 0) {
    state->apply(PASS);
    return false;
  }

  int player = state->active_player;

  Point best_move;
  int best_score = std::numeric_limits<int>::min();

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
    state->apply(PASS);
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
    state->apply(PASS);
    return false;
  }

  while (true) {

    using namespace std;

    string cmd;
    cout << "Enter move: ";
    cin >> cmd;

    if (cin.eof()) exit(0);

    if (cmd.size() != 2) {
      cout << "Bad command" << endl;
      continue;
    }        

    int row = int(cmd[1] - '1');
    int col = int(cmd[0] - 'a');

    if (!BOUNDS(row, col)) {
      cout << "Out of bounds" << endl;
      continue;
    }

    Point move(row, col);

    if (std::find(valid_moves.begin(), valid_moves.end(), move) == valid_moves.end()) {
      cout << "Invalid move" << endl;
      continue;
    }

    state->apply(move);

    return true;
  }
}

template<typename T>
int rollout_game(const T move_policy_f, BoardState *start_state) {
  BoardState state(*start_state);

  bool passed = false;

  while (true) {
    bool pass = !move_policy_f(&state);

    if (pass && passed) break;
    passed = pass;
  }

  return state.winner();
}

int eval_pieces(BoardState *state, int player) {
  int s = 0;
  for (int i = 0; i < BOARD_H; ++i)
    for (int j = 0; j < BOARD_W; ++j)
      if (state->board[i][j] == player)
        ++s;
  return s;
}

int eval_inv_pieces(BoardState *state, int player) {
  int s = 0;
  for (int i = 0; i < BOARD_H; ++i)
    for (int j = 0; j < BOARD_W; ++j)
      if (state->board[i][j] == player)
        --s;
  return s;
}

int eval_sampling(BoardState *state, int player, int samples) {
  int s = 0;
  for (int i = 0; i < samples; ++i) {
    if (rollout_game(random_move, state) == player) {
      s++;
    }
  }
  return s;
}
