#pragma once

#include <iostream>
#include <vector>
#include <cmath>
#include <cassert>

#include "board.h"
#include "util.h"
#include "basic.h"

using namespace std;

int uct_nodes;
int uct_visits;
int uct_rollouts;

struct uct_node {

  int n_visited;
  int n_moves;
  
  vector<double> T;
  vector<double> N;

  vector<Point> valid_moves;
  vector<uct_node*> node_children;

  BoardState *state;

  uct_node* pass_node;

  uct_node(BoardState *state) : state(state), pass_node(NULL) {

    ++ uct_nodes;

    valid_moves = state->moves();
    n_moves = valid_moves.size();
    n_visited = 0;

    T.resize(n_moves);
    N.resize(n_moves);

    node_children.resize(n_moves);
  }

  ~uct_node() {
    delete state;
    for (auto child : node_children)
      if (child) delete child;
    if (pass_node) delete pass_node;
  }

  Point get_best_move() {

    double best_score = -1;
    Point best_move;

    printf("UCT select");

    for (int i = 0; i < n_moves; ++i) {
      assert(N[i] != 0);
      printf(" %.2f", T[i] / N[i]);
      if (T[i] / N[i] > best_score) {
        best_score = T[i] / N[i];
        best_move = valid_moves[i];
      }
    }
    printf("\n");

    assert(best_score != -1);
    return best_move;
  }

  int play() {

    ++ uct_visits;

    int res = -1;
    int j = -1;

    if (n_moves == 0) {
      if (state->passed) {
        return state->winner();
      }

      if (!pass_node) {
        BoardState *pass_state = new BoardState(*state);
        pass_state->apply(PASS);
        pass_node = new uct_node(pass_state);
      }

      return pass_node->play();
    }

    if (n_visited < n_moves) {
      j = n_visited++;

      BoardState *j_state = new BoardState(*state);
      j_state->apply(valid_moves[j]);
      node_children[j] = new uct_node(j_state);

      // rollout from j
      ++ uct_rollouts;
      res = rollout_game(random_move, node_children[j]->state);
    } else {
      j = -1;
      double max_val = -1;

      for (unsigned i = 0; i < valid_moves.size(); ++i) {
        assert(N[i] != 0);

        double c = 1;
        double val = T[i] / N[i] + c * sqrt( ( 2 * log(n_visited) ) / N[i]);

        if (val > max_val) {
          max_val = val;
          j = i;
        }
      }

      assert(j != -1);
      // play from j
      res = node_children[j]->play();
    }

    // update statistics
    N[j]++;
    T[j] += (res == state->active_player);

    return res;
  }

};

bool uct_move(BoardState *state, int n_trials) {

  uct_rollouts = 0;
  uct_nodes = 0;
  uct_visits = 0;

  auto valid_moves = state->moves();

  if (valid_moves.size() == 0) {
    printf("pass\n");
    state->apply(PASS);
    return false;
  }

  BoardState * root_state = new BoardState(*state);
  uct_node root_node(root_state);

  if (root_node.n_moves) {

    for (int i = 0; i < n_trials * root_node.n_moves; ++i)
      root_node.play();

    Point move = root_node.get_best_move();
    state->apply(move);
  } else {
    state->apply(PASS);
  }

  printf("UCT nodes %d visits %d rollouts %d\n", uct_nodes, uct_visits, uct_rollouts);

  return true;
}
