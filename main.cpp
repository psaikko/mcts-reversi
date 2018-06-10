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

int winner(BoardState *terminal_state) {
  assert(terminal_state->passed);

  int w_score = 0;
  int b_score = 0;

  for (int i = 0; i < 8; ++i) {
    for (int j = 0; j < 8; ++j) {
      if (terminal_state->board[i][j] == WHITE)
        w_score++;
      else if (terminal_state->board[i][j] == BLACK)
        b_score++;
    }
  }

  if (w_score > b_score) return WHITE;
  if (w_score < b_score) return BLACK;
  return EMPTY;
}

int simulate_random_game(BoardState *start_state) {

  BoardState state(*start_state);

  bool passed = false;

  while (true) {
    bool pass = !random_move(&state);

    if (pass && passed) break;
    passed = pass;
  }

  return winner(&state);
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

  uct_node* pass_node;

  BoardState *state;

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
        return winner(state);
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
      res = simulate_random_game(node_children[j]->state);
    } else {
      j = -1;
      double max_val = -1;

      for (int i = 0; i < valid_moves.size(); ++i) {
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

  int player = state->active_player;

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

int main(int argc, char ** argv) {

  using namespace std::placeholders;

  srand(10101010);

  int w_wins = 0;
  int b_wins = 0;

  eval_func eval_sampling_1 = bind(eval_sampling, _1, _2, 1);
  eval_func eval_sampling_10 = bind(eval_sampling, _1, _2, 10);
  eval_func eval_sampling_100 = bind(eval_sampling, _1, _2, 100);
  eval_func eval_sampling_1000 = bind(eval_sampling, _1, _2, 1000);

  move_func greedy_sampling_10 = bind(greedy_move, _1, eval_sampling_10);
  move_func greedy_sampling_100 = bind(greedy_move, _1, eval_sampling_100);
  move_func greedy_sampling_1000 = bind(greedy_move, _1, eval_sampling_1000);

  move_func uct_move_10 = bind(uct_move, _1, 10);
  move_func uct_move_1000 = bind(uct_move, _1, 1000);

  move_func ucb1_move_10 = bind(ucb1_move, _1, 10);
  move_func ucb1_move_100 = bind(ucb1_move, _1, 100);
  move_func ucb1_move_1000 = bind(ucb1_move, _1, 1000);

  move_func greedy_pieces = bind(greedy_move, _1, eval_pieces);

  move_func minimax_sampling = bind(minimax_move, _1, eval_sampling_10, 3);
  move_func minimax_pieces = bind(minimax_move, _1, eval_pieces, 3);

  for (int i = 0; i < 1; ++i) {

    BoardState state;

    move_func player_a = uct_move_1000;   // black
    move_func player_b = minimax_sampling;

    bool passed = false;

    while (true) {

      state.print();

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