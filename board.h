#pragma once

#include <algorithm>

#include "util.h"

struct BoardState {

  int board[8][8];
  int active_player;
  bool passed;

  BoardState() : active_player(BLACK) {
    std::fill(*board, board[7]+8, 0);
    passed = false;

    board[3][3] = WHITE;
    board[3][4] = BLACK;
    board[4][3] = BLACK;
    board[4][4] = WHITE;
  }

  BoardState(const BoardState & other) {
    active_player = other.active_player;
    passed = other.passed;
    std::copy(*other.board, other.board[7]+8, *board);
  }

  void apply(Point move) {
    if (move == PASS) {
      passed = true;
    } else {
      passed = false;
      assert(board[move.first][move.second] == EMPTY);
      board[move.first][move.second] = active_player;

      auto f = [&](int y, int x){
        if (board[y][x] == OTHER(active_player)) {

          int dx = x - move.second;
          int dy = y - move.first;

          while (true) {
            x += dx;
            y += dy;

            if (!BOUNDS(y, x)) {
              break;
            }

            if (board[y][x] == active_player) {
              while (true) {
                x -= dx;
                y -= dy;

                if (board[y][x] == active_player)
                  break;

                board[y][x] = active_player;
              }
              break;
            }

            if (board[y][x] == EMPTY) {
              break;
            }
          }
        }
      };

      map_adjacent(move.first, move.second, f);
    }

    active_player = OTHER(active_player);
  }

  std::vector<Point> moves() {
    std::vector<Point> m;
    m.reserve(60);
  
    for (int i = 0; i < 8; ++i) {
      for (int j = 0; j < 8; ++j) {
        if (board[i][j] == active_player) {

          auto f = [&](int y, int x){
            if (board[y][x] == OTHER(active_player)) {

              int dx = x - j;
              int dy = y - i;

              while (true) {
                x += dx;
                y += dy;

                if (!BOUNDS(y, x)) {
                  break;
                }

                if (board[y][x] == active_player) {
                  break;
                }

                if (board[y][x] == EMPTY) {
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
  }

  void print() {

    auto valid_moves = moves();

    printf(" ");
    for (int i = 0; i < 8; ++i)
      printf(" %c", 'a' + i);
    printf("\n\n");

    for (int i = 0; i < 8; ++i) {
      printf("%d", i+1);
      for (int j = 0; j < 8; ++j) {
        if (board[i][j] == WHITE)
          printf(" \u25CB");
        else if (board[i][j] == BLACK)
          printf(" \u25CF");
        else {
          Point x(i, j);

          if (std::find(valid_moves.begin(), valid_moves.end(), x) != valid_moves.end())
            printf(" _");
          else 
            printf(" .");
        }
      }
      printf("\n\n");
    }
  }
};