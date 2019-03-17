//============================================================================
// Name        : endhiran.cpp
// Author      : Raghav
// Version     :
//============================================================================

#include "board.h"

using namespace std;

//#define NDEBUG

int main() {
	init_bitboards();
	Board board("/Users/raghav/wrk/endhiran/model/");

	board.init();
    board.readFen("2kr4/p7/1ppq3p/2p1b2P/4Pp2/1P1P3P/P1P3Q1/3N1RK1 b - -");

	board.display();
	char move[10];
	Move cmove;
	int count = 1;

	bool self = true;
	while (true) {

		if (!self) {
			Color c = board.turn();
			if (board.hasMoves()) {

				do {
					std::cout << "input: ";
					std::cin >> move;
				} while (!board.updateMove(move));
			}
			if (c != board.turn()) {
				board.display();
			} else {
				std::cout << COLORNAME[board.opponent()] << " wins!" << std::endl;
				return 0;
			}
		}

		int val = board.think();
		std::cout << val << " n";
		std::cout << board.nodes() << std::endl;
		cmove = board.bestMove();
		std::cout << "Turn:" << board.turn() << std::endl;
		std::cout << "moves: " << board.fiftymoves() << std::endl;
		std::cout << "hash: " << board.hashkey() << std::endl;
		std::cout << "reps: " << board.repeatCount() << std::endl;
		std::cout << count++ << ". " << cmove << std::endl;
		if (cmove.state) {
			board.makeMove(cmove);
		} else {
			if (val == VALUE_DRAW) {
				std::cerr << "Draw!" << std::endl;
			} else {
				std::cerr << COLORNAME[board.opponent()] << " wins!"
						<< std::endl;
			}

			return 0;
		}
		board.display();

  }

	return 0;
}
