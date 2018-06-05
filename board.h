#pragma once

#include <algorithm>

#include "util.h"

struct BoardState {

  int board[8][8];
  int next_move;

  BoardState() : next_move(BLACK) {
    std::fill(*board, board[7]+8, 0);

    board[3][3] = WHITE;
    board[3][4] = BLACK;
    board[4][3] = BLACK;
    board[4][4] = WHITE;
  }

  BoardState(const BoardState & other) {
    next_move = other.next_move;
    std::copy(*other.board, other.board[7]+8, *board);
  }

  void apply(Point move) {
    board[move.first][move.second] = next_move;

    for (auto adj_p : adjacent(move.first, move.second)) {
      int x = adj_p.second;
      int y = adj_p.first;

      if (board[y][x] == OTHER(next_move)) {

        int dx = x - move.second;
        int dy = y - move.first;

        while (true) {
          x += dx;
          y += dy;

          if (!BOUNDS(y, x)) {
            break;
          }

          if (board[y][x] == next_move) {
            while (true) {
              x -= dx;
              y -= dy;

              if (board[y][x] == next_move)
                break;

              board[y][x] = next_move;
            }
            break;
          }

          if (board[y][x] == EMPTY) {
            break;
          }
        }
      }
    }

    next_move = OTHER(next_move);
  }

  void pass() {
    next_move = OTHER(next_move);
  }

  std::vector<Point> moves() {
    std::vector<Point> m;
  
    for (int i = 0; i < 8; ++i) {
      for (int j = 0; j < 8; ++j) {
        if (board[i][j] == next_move) {
          for (auto adj_p : adjacent(i, j)) {
            int x = adj_p.second;
            int y = adj_p.first;

            if (board[y][x] == OTHER(next_move)) {

              int dx = x - j;
              int dy = y - i;

              while (true) {
                x += dx;
                y += dy;

                if (!BOUNDS(y, x)) {
                  break;
                }

                if (board[y][x] == next_move) {
                  break;
                }

                if (board[y][x] == EMPTY) {
                  m.emplace_back(y, x);
                  break;
                }
              }
            }
          }
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