#pragma once

#include <algorithm>

#include "util.h"

struct BoardState {

  int board[BOARD_H][BOARD_W];
  int active_player;
  bool passed;

  BoardState() : active_player(BLACK) {
    std::fill(*board, board[BOARD_H - 1]+BOARD_W, 0);
    passed = false;

    int mid_h = BOARD_H / 2 - 1;
    int mid_w = BOARD_W / 2 - 1;

    board[mid_h][mid_w] = WHITE;
    board[mid_h][mid_w+1] = BLACK;
    board[mid_h+1][mid_w] = BLACK;
    board[mid_h+1][mid_w+1] = WHITE;
  }

  BoardState(const BoardState & other) {
    active_player = other.active_player;
    passed = other.passed;
    std::copy(*other.board, other.board[BOARD_H - 1]+BOARD_W, *board);
  }

  inline int get(const int r, const int c) {
    return board[r][c];
  }

  inline void set(const int r, const int c, const int player) {
    board[r][c] = player;
  }

  void apply(const Point move) {
    if (move == PASS) {
      passed = true;
    } else {
      passed = false;
      assert(board[move.first][move.second] == EMPTY);
      board[move.first][move.second] = active_player;

      auto f = [&](int y, int x){
        if (board[y][x] == OTHER(active_player)) {

          const int dx = x - move.second;
          const int dy = y - move.first;

          while (true) {
            x += dx;
            y += dy;

            if (!BOUNDS(y, x)) break;

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

  std::vector<Point> moves() const {
    std::vector<Point> m;
    m.reserve(60);
  
    for (int i = 0; i < BOARD_H; ++i) {
      for (int j = 0; j < BOARD_W; ++j) {
        if (board[i][j] == active_player) {

          auto f = [&](int y, int x){
            if (board[y][x] == OTHER(active_player)) {

              const int dx = x - j;
              const int dy = y - i;

              while (true) {
                x += dx;
                y += dy;

                if (!BOUNDS(y, x)) break;

                if (board[y][x] == active_player) break;

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
    for (int i = 0; i < BOARD_W; ++i)
      printf("  %c", 'a' + i);
    printf("\n\n");

    for (int i = 0; i < BOARD_H; ++i) {
      printf("%d", i+1);
      for (int j = 0; j < BOARD_W; ++j) {
        if (board[i][j] == WHITE)
          printf("  \u25CB");
        else if (board[i][j] == BLACK)
          printf("  \u25CF");
        else {
          Point x(i, j);

          if (std::find(valid_moves.begin(), valid_moves.end(), x) != valid_moves.end())
            printf("  _");
          else 
            printf("  .");
        }
      }
      printf("\n\n");
    }
  }

  int winner() {
    assert(passed);

    int w_score = 0;
    int b_score = 0;

    for (int i = 0; i < BOARD_H; ++i) {
      for (int j = 0; j < BOARD_W; ++j) {
        if (board[i][j] == WHITE)
          w_score++;
        else if (board[i][j] == BLACK)
          b_score++;
      }
    }

    if (w_score > b_score) return WHITE;
    if (w_score < b_score) return BLACK;
    return EMPTY;
  }
};