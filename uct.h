#pragma once

#include <iostream>
#include <vector>
#include <cmath>
#include <cassert>

#include "board.h"
#include "util.h"
#include "basic.h"

using namespace std;

struct TreeNode {
  int n_visited;
  int n_moves;
  
  vector<double> T;
  vector<double> N;

  vector<Point> valid_moves;
  vector<TreeNode*> node_children;
  TreeNode* pass_node;
  BoardState *state;

  TreeNode(BoardState *state) : pass_node(NULL), state(state) {
    valid_moves = state->moves();
    n_moves = valid_moves.size();
    n_visited = 0;

    T.resize(n_moves);
    N.resize(n_moves);

    node_children.resize(n_moves);
  }

  ~TreeNode() {
    delete state;
    for (auto child : node_children)
      if (child) delete child;
    if (pass_node) delete pass_node;
  }

  Point select_best_move() {
    double best_score = -1;
    Point best_move;

    for (int i = 0; i < n_moves; ++i) {
      assert(N[i] != 0);
      if (T[i] / N[i] > best_score) {
        best_score = T[i] / N[i];
        best_move = valid_moves[i];
      }
    }

    assert(best_score != -1);
    return best_move;
  }

  int play() {
    int winner = -1;
    int next_move = -1;

    if (n_moves == 0) {
      if (state->passed) {
        return state->winner();
      }
      if (!pass_node) {
        BoardState *pass_state = new BoardState(*state);
        pass_state->apply(PASS);
        pass_node = new TreeNode(pass_state);
      }
      return pass_node->play();
    }

    if (n_visited < n_moves) {
      next_move = n_visited++;
      BoardState *next_state = new BoardState(*state);

      next_state->apply(valid_moves[next_move]);
      node_children[next_move] = new TreeNode(next_state);

      // rollout from next_move
      winner = rollout_game(random_move, node_children[next_move]->state);
    } else {
      double max_val = -1;

      for (unsigned i = 0; i < valid_moves.size(); ++i) {
        assert(N[i] != 0);

        double val = T[i] / N[i] + sqrt( ( 2 * log(n_visited) ) / N[i]);
        if (val > max_val) {
          max_val = val;
          next_move = i;
        }
      }

      assert(next_move != -1);
      // play from next_move
      winner = node_children[next_move]->play();
    }

    // update statistics
    N[next_move]++;
    T[next_move] += (winner == state->active_player);

    return winner;
  }
};

bool uct_move(BoardState *state, int n_trials) {
  auto valid_moves = state->moves();

  BoardState * root_state = new BoardState(*state);
  TreeNode root_node(root_state);

  if (root_node.n_moves) {
    for (int i = 0; i < n_trials * root_node.n_moves; ++i) {
      root_node.play();
    }
    Point move = root_node.select_best_move();
    state->apply(move);
  } else {
    state->apply(PASS);
    return false;
  }

  return true;
}
