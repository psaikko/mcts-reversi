#pragma once

#include <vector>
#include <functional>
#include <cassert>

struct BoardState;

typedef std::pair<int,int> Point;

typedef std::function<int(BoardState*, int)> eval_func;

typedef std::function<bool(BoardState*)> move_func;

const Point PASS = {-1,-1};

#define EMPTY 0
#define BLACK 1
#define WHITE 2

#define BOARD_W 8
#define BOARD_H 8

#define OTHER(p) (3 - (p))

#define BOUNDS(y, x) ((y >= 0 && y < BOARD_H) && ((x >= 0 && x < BOARD_W)))

std::vector<Point> adjacent(int y, int x) {

  if (y > 0 && y < (BOARD_H - 1) && x > 0 && x < (BOARD_W - 1))
    return { {y - 1, x}, {y - 1, x - 1}, {y - 1, x + 1},
             {y, x + 1}, {y, x - 1},
             {y + 1, x}, {y + 1, x - 1}, {y + 1, x + 1}, };

  if (y > 0 && y < (BOARD_H - 1) && x == 0)
    return { {y - 1, x}, {y - 1, x + 1},
             {y, x + 1}, 
             {y + 1, x}, {y + 1, x + 1} };

  if (y > 0 && y < (BOARD_H - 1) && x == (BOARD_W - 1))
    return { {y - 1, x}, {y - 1, x - 1}, 
             {y, x - 1},
             {y + 1, x}, {y + 1, x - 1} };

  if (y == 0 && x > 0 && x < (BOARD_W - 1))
    return { {y, x + 1}, {y, x - 1},
             {y + 1, x}, {y + 1, x - 1}, {y + 1, x + 1}, };

  if (y == (BOARD_H - 1) && x > 0 && x < (BOARD_W - 1))
    return { {y - 1, x}, {y - 1, x - 1}, {y - 1, x + 1},
             {y, x + 1}, {y, x - 1} };

  if (y == 0 && x == 0)
    return { {y + 1, x}, {y + 1, x + 1}, {y, x + 1} };

  if (y == 0 && x == (BOARD_W - 1))
    return { {y + 1, x}, {y + 1, x - 1}, {y, x - 1} };

  if (y == (BOARD_H - 1) && x == 0)
    return { {y - 1, x}, {y - 1, x + 1}, {y, x + 1} };

  assert(y == (BOARD_H - 1) && x == (BOARD_W - 1));
    return { {y - 1, x}, {y - 1, x - 1}, {y, x - 1} };

}

template<typename T>
inline void map_adjacent(const int y, const int x, const T f) {

  if (y > 0) {
    f(y - 1, x);
    if (x > 0) f(y - 1, x - 1);
    if (x < (BOARD_W - 1)) f(y - 1, x + 1);
  } 

  if (y < (BOARD_H - 1)) {
    f(y + 1, x);
    if (x > 0) f(y + 1, x - 1);
    if (x < (BOARD_W - 1)) f(y + 1, x + 1);
  }

  if (x > 0) f(y, x - 1);

  if (x < (BOARD_W - 1)) f(y, x + 1);
}
