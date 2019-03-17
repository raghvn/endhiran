/*
 * perft_main.cxx
 *
 *  Created on: 04-Nov-2018
 *      Author: raghav
 */

#include "board.h"

using namespace std;


int main(int argc, char** argv) {
	init_bitboards();
	Board board("model/");

	board.init();

    timer t;
    t.tic();
    U64 nodes = board.perft(4);
    uint64_t end = t.toc();
    
    std::cout<<"Perft "<<nodes<<" "<<end<<std::endl;


  return 0;
}
