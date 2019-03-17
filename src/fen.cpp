/*
 * fen.cpp
 *
 *  Created on: 14-Dec-2011
 *      Author: raghav
 */

#include "board.h"

#include <iostream>
#include <stdio.h>
#include <cstring>
#include <string>
#include <sstream>

using namespace std;

void Board::readFen(const string& fen) {

	char token;
	std::istringstream ss(fen);
	Square sq = SQ_A8;
	string piecesstr = "PKNBRQpknbrq";
	Piece pieces[64];

	for (int i = 0; i < 64; i++) {
		pieces[i] = PIECE_NONE;
	}

	while (ss.get(token) && token != ' ') {
		if (piecesstr.find(token, 0) != string::npos) {
			pieces[sq] = charToPiece(token);
			sq++;
		} else if (isdigit(token))
			sq += Square(token - '0');
		else if (token == '/')
			sq -= Square(16);
	}

	if (!ss.get(token) || (token != 'w' && token != 'b'))
		;

	Color c = (token == 'w' ? WHITE : BLACK);

    if (!ss.get(token) || (token == ' '))
        ;


	Castling wcastle = CASTLING_NONE, bcastle = CASTLING_NONE;
	// 3. Castling availability
	while (ss.get(token) && token != ' ') {
		switch (token) {
		case 'K':
			wcastle = Castling(wcastle | CASTLING_OO);
			break;
		case 'Q':
			wcastle = Castling(wcastle | CASTLING_OOO);
			break;
		case 'k':
			bcastle = Castling(bcastle | CASTLING_OO);
			break;
		case 'q':
			bcastle = Castling(bcastle | CASTLING_OOO);
			break;
		default:
			break;
		}

	}

	char ep[2];
	Square epsquare = SQ_NONE;
	const string alnu = "abcdefghABCDEFGH0123456789";
	if (ss.get(token) && alnu.find(token, 0) != string::npos) {
		ep[0] = token;
		if (ss.get(token) && alnu.find(token, 0) != string::npos) {
			ep[1] = token;
			epsquare = an_square(ep);
			//cout<<epsquare;
		}
	}

	int fifty = 0;
	ss >> fifty;

	int full = 0;
	ss >> full;

	initFromSquares(pieces, c, fifty, wcastle, bcastle, epsquare);

}
