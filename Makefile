reversi: main.cpp minimax.h board.h util.h ucb.h uct.h basic.h
	g++ -std=c++14 -O3 -pedantic -Wall main.cpp -o reversi

test: test.cpp minimax.h board.h util.h ucb.h uct.h basic.h
	g++ -std=c++14 -O3 -pedantic -Wall test.cpp -o test