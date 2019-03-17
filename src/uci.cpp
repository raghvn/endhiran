/*
  Stockfish, a UCI chess playing engine derived from Glaurung 2.1
  Copyright (C) 2004-2008 Tord Romstad (Glaurung author)
  Copyright (C) 2008-2010 Marco Costalba, Joona Kiiski, Tord Romstad
  Stockfish is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  Stockfish is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "board.h"

#include <cassert>
#include <cstring>
#include <iomanip>
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "move.h"

using std::string;

const string move_to_uci(Move m) {

	Square from = m.from();
	Square to = m.to();
	string promotion;

	if (m.key() == 0)
		return "0000";

	if (m.special() == PROMOTION_MOVE)
		promotion = char(tolower(PIECENAMES[m.promotion()]));

	string move =  string(SQCHAR[from]) + string(SQCHAR[to]) + promotion;
	std::transform(move.begin(), move.end(), move.begin(), ::tolower);

	return move;
}

using namespace std;


// FEN string for the initial position
const string StartPositionFEN =
		"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

// UCIParser is a class for parsing UCI input. The class
// is actually a string stream built on a given input string.
typedef istringstream UCIParser;

void set_option(UCIParser& up);
void set_position(Board& b, UCIParser& up);
bool go(Board& b, UCIParser& up);

bool execute_uci_command(const string& cmd) {

	static Board b("model/"); // The root position

	UCIParser up(cmd);
	string token;

	up >> token;

	if (token == "quit")
		return false;

	if (token == "go")
		return go(b, up);

	if (token == "ucinewgame")
		b.readFen(StartPositionFEN);

	else if (token == "isready")
		cout << "readyok" << endl;

	else if (token == "position")
		set_position(b, up);

	//else if (token == "setoption")
	//    set_option(up);

	//else if (token == "perft")
	//    perft(b, up);

	//else if (token == "d")
	//	b.display();

	// else if (token == "flip")
	//     b.flip();

	else if (token == "uci")
		cout << "id name " << "Endhiran" << "\nid author " << "Raghav"
				<< "\nuciok" << endl;
	else
		cout << "Unknown command: " << cmd << endl;

	return true;
}


// set_position() is called when engine receives the "position" UCI
// command. The function sets up the position described in the given
// fen string ("fen") or the starting position ("startpos") and then
// makes the moves given in the following move list ("moves").

void set_position(Board& pos, UCIParser& up) {

	string token, fen;

	up >> token; // operator>>() skips any whitespace

	if (token == "startpos") {
		pos.readFen(StartPositionFEN);
		up >> token; // Consume "moves" token if any
	} else if (token == "fen") {
		while (up >> token && token != "moves")
			fen += token + " ";

		pos.readFen(fen);
	} else
		return;

	// Parse move list (if any)
	while (up >> token)
		pos.updateMove(token.c_str());
}

// set_option() is called when engine receives the "setoption" UCI
// command. The function updates the corresponding UCI option ("name")
// to the given value ("value").

/*void set_option(UCIParser& up) {

 string token, name;
 string value = "true"; // UCI buttons don't have a "value" field

 up >> token; // Consume "name" token
 up >> name;  // Read option name

 // Handle names with included spaces
 while (up >> token && token != "value")
 name += " " + token;

 up >> value; // Read option value

 // Handle values with included spaces
 while (up >> token)
 value += " " + token;

 if (Options.find(name) != Options.end())
 Options[name].set_value(value);
 else
 cout << "No such option: " << name << endl;
 }*/

// go() is called when engine receives the "go" UCI command. The
// function sets the thinking time and other parameters from the input
// string, and then calls think(). Returns false if a quit command
// is received while thinking, true otherwise.
bool go(Board& pos, UCIParser& up) {

	pos.think();
	cout << "bestmove "<<move_to_uci(pos.bestMove())<<endl;
	return true;
}
