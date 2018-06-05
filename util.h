#pragma once

#include <vector>
#include <functional>

struct BoardState;

typedef std::pair<int,int> Point;

typedef std::function<int(BoardState*, int)> eval_func;

typedef std::function<bool(BoardState*)> move_func;

#define EMPTY 0
#define BLACK 1
#define WHITE 2

#define OTHER(p) ((p == WHITE) ? BLACK : WHITE)

#define BOUNDS(y, x) ((y >= 0 && y <= 7) && ((x >= 0 && x <= 7)))

std::vector<Point> adjacent(int y, int x) {
  std::vector<Point> adj;

  if (y > 1) {
    adj.emplace_back(y - 1, x);
    if (x > 1)
      adj.emplace_back(y - 1, x - 1);
    if (x < 7)
      adj.emplace_back(y - 1, x + 1);
  } 

  if (y < 7) {
    adj.emplace_back(y + 1, x);
    if (x > 1)
      adj.emplace_back(y + 1, x - 1);
    if (x < 7)
      adj.emplace_back(y + 1, x + 1);
  }

  if (x > 0)
    adj.emplace_back(y, x - 1);

  if (x < 7)
    adj.emplace_back(y, x + 1);

  return std::move(adj);
}