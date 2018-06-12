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
#include "uct.h"
#include "ucb.h"

using namespace std;

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