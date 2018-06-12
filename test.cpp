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

auto simple_adjacent = [](int y, int x) {
  std::vector<Point> adj;
  
  for (int x2 = x - 1; x2 <= x + 1; ++x2) {
    for (int y2 = y - 1; y2 <= y + 1; ++y2) {
      if (!(x2 == x && y2 == y) && BOUNDS(y2, x2)) {
        adj.emplace_back(y2, x2);
      }
    }
  }

  return std::move(adj);
};

auto simple_moves = [](BoardState* state) {
  std::vector<Point> m;
  m.reserve(60);

  for (int i = 0; i < BOARD_H; ++i) {
    for (int j = 0; j < BOARD_W; ++j) {
      if (state->get(i, j) == state->active_player) {

        auto f = [&](int y, int x){
          if (state->get(y, x) == OTHER(state->active_player)) {

            int dx = x - j;
            int dy = y - i;

            while (true) {
              x += dx;
              y += dy;

              if (!BOUNDS(y, x)) break;

              if (state->get(y, x) == state->active_player) break;

              if (state->get(y, x) == EMPTY) {
                m.emplace_back(y, x);
                break;
              }
            }
          }
        };

        map_adjacent(i, j, f);
      }
    }
  }

  return move(m);
};

auto simple_apply = [](BoardState *state, Point move) {
  if (move == PASS) {
    state->passed = true;
  } else {
    state->passed = false;
    assert(state->get(move.first, move.second) == EMPTY);
    state->set(move.first, move.second, state->active_player);

    auto f = [&](int y, int x){
      if (state->get(y, x) == OTHER(state->active_player)) {

        int dx = x - move.second;
        int dy = y - move.first;

        while (true) {
          x += dx;
          y += dy;

          if (!BOUNDS(y, x)) break;

          if (state->get(y, x) == state->active_player) {
            while (true) {
              x -= dx;
              y -= dy;

              if (state->get(y, x) == state->active_player)
                break;

              state->set(y, x, state->active_player);
            }
            break;
          }

          if (state->get(y, x) == EMPTY) {
            break;
          }
        }
      }
    };

    map_adjacent(move.first, move.second, f);
  }

  state->active_player = OTHER(state->active_player);
};

void assert_list_contents(vector<Point> v1, vector<Point> v2) {
  std::sort(v1.begin(), v1.end());
  std::sort(v2.begin(), v2.end());

  assert(v1.size() == v2.size());

  for (int i = 0; i < v1.size(); i++)
    assert(v1[i] == v2[i]);
}

void assert_board_state(BoardState *s1, BoardState *s2) {
  for (int i = 0; i < BOARD_H; ++i) {
    for (int j = 0; j < BOARD_W; ++j) {
      assert(s1->get(i, j) == s2->get(i, j));
    }
  }

  assert(s1->passed == s2->passed);

  assert(s1->active_player == s2->active_player);
}

void adjacent_unit() {

  for (int i = 0; i < BOARD_H; ++i) {
    for (int j = 0; j < BOARD_W; ++j) {
      vector<Point> check = simple_adjacent(i, j);

      vector<Point> adj_1 = adjacent(i, j);

      vector<Point> adj_2;
      map_adjacent(i, j, [&](int y, int x){
        adj_2.push_back({y, x});
      });

      assert_list_contents(check, adj_1);

      assert_list_contents(check, adj_2);
    }
  }

  printf("Adjacent squares ok\n");
}

void valid_moves_unit() {
  
  for (int i = 0; i < 100; ++i) {
    BoardState state;
    for (int j = 0; j < 10; ++j) {
      random_move(&state);
    }

    vector<Point> check = simple_moves(&state);

    vector<Point> mov1 = state.moves();

    assert_list_contents(check, mov1);
  }

  printf("Valid moves ok\n");
}

void apply_moves_unit() {

  for (int i = 0; i < 100; ++i) {
    BoardState state;
    for (int j = 0; j < 10; ++j) {
      random_move(&state);
    }

    for (auto move : simple_moves(&state)) {
      BoardState s1(state);
      BoardState s2(state);

      simple_apply(&s1, move);
      s2.apply(move);

      assert_board_state(&s1, &s2);
    }
  }

  printf("Apply move ok\n"); 

}

void random_game_perf() {
  BoardState state;

  for (int i = 0; i < 100000; ++i) {
    rollout_game(random_move, &state);
  }
}

int main(int argc, char ** argv) {

  adjacent_unit();

  valid_moves_unit();

  apply_moves_unit();

  random_game_perf();
}