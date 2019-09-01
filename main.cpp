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
using namespace std::placeholders;

// board evaluation functions
eval_func eval_sampling_10 = bind(eval_sampling, _1, _2, 10);
eval_func eval_sampling_100 = bind(eval_sampling, _1, _2, 100);
eval_func eval_sampling_1000 = bind(eval_sampling, _1, _2, 1000);
// eval_pieces
// eval_inv_pieces

move_func strategies[] = {
  io_move, // player
  random_move, // random
  bind(greedy_move, _1, eval_pieces), // greedy 
  bind(greedy_move, _1, eval_inv_pieces), // generous
  bind(greedy_move, _1, eval_sampling_10), // greedy with random sampling 
  bind(greedy_move, _1, eval_sampling_100),  
  bind(greedy_move, _1, eval_sampling_1000),
  bind(uct_move, _1, 10), // UCT with various amounts of sampling 
  bind(uct_move, _1, 100),
  bind(uct_move, _1, 1000),
  bind(ucb1_move, _1, 10), // UCB1 with various amounts of sampling 
  bind(ucb1_move, _1, 100),
  bind(ucb1_move, _1, 1000),
  bind(minimax_move, _1, eval_sampling_10, 3), // Minimax by sampling
  bind(minimax_move, _1, eval_pieces, 3) // Minimax by piece count
};

int main(int argc, char ** argv) {

  srand(10101010);

  int p1_strategy = 0;
  int p2_strategy = 1;
  int rounds = 1;
  bool print_states = false;

  if (argc > 1) p1_strategy = stoi(argv[1]);
  if (argc > 2) p2_strategy = stoi(argv[2]);
  if (argc > 3) rounds = stoi(argv[3]);
  if (argc > 4) print_states = string(argv[4]) != "0";

  int p1_wins = 0;
  int p2_wins = 0;

  for (int i = 0; i < rounds; ++i) {

    BoardState state;

    move_func player_1 = strategies[p1_strategy];   // black
    move_func player_2 = strategies[p2_strategy];

    bool passed = false;

    while (true) {

      if (print_states) state.print();

      bool pass = !player_1(&state);

      if (pass && passed) break;
      passed = pass;
      
      swap(player_1, player_2);
    }

    int w_score = eval_pieces(&state, WHITE);
    int b_score = eval_pieces(&state, BLACK);

    if (w_score > b_score) p1_wins++;
    if (w_score < b_score) p2_wins++;

    cout << "Player 1 " << b_score << "\nPlayer 2 " << w_score << endl;
  }

  if (rounds > 1) {
    cout << "1 " << p1_wins << endl;
    cout << "2 " << p2_wins << endl;
  }
}