/*
 * perft_main.cxx
 *
 *  Created on: 04-Nov-2018
 *      Author: raghav
 */

#include "board.h"

using namespace std;

#define NDEBUG

extern bool execute_uci_command(const string& cmd);

int main(int argc, char** argv) {
	init_bitboards();
	string cmd;
  while (getline(cin, cmd) && execute_uci_command(cmd)) {}

  return 0;
}
