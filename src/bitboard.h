/*
 Stockfish, a UCI chess playing engine derived from Glaurung 2.1
 Copyright (C) 2004-2008 Tord Romstad (Glaurung author)
 Copyright (C) 2008-2009 Marco Costalba
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

#ifndef BITBOARD_H_
#define BITBOARD_H_

#include "defs.h"

const Bitboard EmptyBoardBB = 0ULL;

extern Bitboard StepAttacksBB[16][64];
extern Bitboard SetMaskBB[65];
extern Bitboard ClearMaskBB[65];

extern const uint64_t RMult[64];
extern const int RShift[64];
extern Bitboard RMask[64];
extern int RAttackIndex[64];
extern Bitboard RAttacks[0x19000];

extern const uint64_t BMult[64];
extern const int BShift[64];
extern Bitboard BMask[64];
extern int BAttackIndex[64];
extern Bitboard BAttacks[0x1480];

extern Bitboard FileBB[8];
extern Bitboard RankBB[8];


void init_masks();
void init_step_attacks();
void init_sliding_attacks(Bitboard attacks[], int attackIndex[],
		Bitboard mask[], const int shift[], const Bitboard mult[],
		int deltas[][2]);
Bitboard sliding_attacks(int sq, Bitboard occupied, int deltas[][2],
                         int fmin, int fmax, int rmin, int rmax);
void init_bitboards();

unsigned int inline bitCnt(U64 bitmap);
unsigned int inline firstOne(U64 bitmap);
void print_bitboard(Bitboard b);
void displayBitmap(Bitboard in);

#if defined(USE_BSFQ)

inline unsigned int bitCnt(U64 bitmap) {
    unsigned long ret;
    __asm__("popcnt %1, %0" : "=r" (ret) : "r" (bitmap));
    return ret;
}
#else

inline unsigned int bitCnt(U64 bitmap) {

// MIT HAKMEM algorithm, see http://graphics.stanford.edu/~seander/bithacks.html

	static const U64 M1 = 0x5555555555555555; // 1 zero,  1 one ...
	static const U64 M2 = 0x3333333333333333; // 2 zeros,  2 ones ...
	static const U64 M4 = 0x0f0f0f0f0f0f0f0f; // 4 zeros,  4 ones ...
	static const U64 M8 = 0x00ff00ff00ff00ff; // 8 zeros,  8 ones ...
	static const U64 M16 = 0x0000ffff0000ffff; // 16 zeros, 16 ones ...
	static const U64 M32 = 0x00000000ffffffff; // 32 zeros, 32 ones

	bitmap = (bitmap & M1) + ((bitmap >> 1) & M1); //put count of each  2 bits into those  2 bits
	bitmap = (bitmap & M2) + ((bitmap >> 2) & M2); //put count of each  4 bits into those  4 bits
	bitmap = (bitmap & M4) + ((bitmap >> 4) & M4); //put count of each  8 bits into those  8 bits
	bitmap = (bitmap & M8) + ((bitmap >> 8) & M8); //put count of each 16 bits into those 16 bits
	bitmap = (bitmap & M16) + ((bitmap >> 16) & M16); //put count of each 32 bits into those 32 bits
	bitmap = (bitmap & M32) + ((bitmap >> 32) & M32); //put count of each 64 bits into those 64 bits
	return (int) bitmap;
}

#endif

inline Bitboard bit_is_set(Bitboard b, Square s) {
	return b & SetMaskBB[s];
}

inline void set_bit(Bitboard *b, Square s) {
	*b |= SetMaskBB[s];
}

inline void clear_bit(Bitboard *b, Square s) {
	*b &= ClearMaskBB[s];
}

inline Bitboard make_move_bb(Square from, Square to) {
  return SetMaskBB[from] | SetMaskBB[to];
}

inline void do_move_bb(Bitboard *b, Bitboard move_bb) {
  *b ^= move_bb;
}


/// Functions for computing sliding attack bitboards. rook_attacks_bb(),
/// bishop_attacks_bb() and queen_attacks_bb() all take a square and a
/// bitboard of occupied squares as input, and return a bitboard representing
/// all squares attacked by a rook, bishop or queen on the given square.

#if defined(IS_64BIT)

inline Bitboard rook_attacks_bb(Square s, Bitboard blockers) {
  Bitboard b = blockers & RMask[s];
  return RAttacks[RAttackIndex[s] + ((b * RMult[s]) >> RShift[s])];
}

inline Bitboard bishop_attacks_bb(Square s, Bitboard blockers) {
  Bitboard b = blockers & BMask[s];
  return BAttacks[BAttackIndex[s] + ((b * BMult[s]) >> BShift[s])];
}

#else // if !defined(IS_64BIT)

inline Bitboard rook_attacks_bb(Square s, Bitboard blockers) {
  Bitboard b = blockers & RMask[s];
  return RAttacks[RAttackIndex[s] +
        (unsigned(int(b) * int(RMult[s]) ^ int(b >> 32) * int(RMult[s] >> 32)) >> RShift[s])];
}

inline Bitboard bishop_attacks_bb(Square s, Bitboard blockers) {
  Bitboard b = blockers & BMask[s];
  return BAttacks[BAttackIndex[s] +
        (unsigned(int(b) * int(BMult[s]) ^ int(b >> 32) * int(BMult[s] >> 32)) >> BShift[s])];
}

#endif

inline Bitboard queen_attacks_bb(Square s, Bitboard blockers) {
  return rook_attacks_bb(s, blockers) | bishop_attacks_bb(s, blockers);
}


/// rank_bb() and file_bb() take a file or a square as input and return
/// a bitboard representing all squares on the given file or rank.

inline Bitboard rank_bb(Rank r) {
  return RankBB[r];
}

inline Bitboard rank_bb(Square s) {
  return RankBB[square_rank(s)];
}

inline Bitboard file_bb(File f) {
  return FileBB[f];
}

inline Bitboard file_bb(Square s) {
  return FileBB[square_file(s)];
}


/// first_1() finds the least significant nonzero bit in a nonzero bitboard.
/// pop_1st_bit() finds and clears the least significant nonzero bit in a
/// nonzero bitboard.

#if defined(USE_BSFQ)

#if defined(_MSC_VER) && !defined(__INTEL_COMPILER)

inline Square first_1(Bitboard b) {
	unsigned long index;
	_BitScanForward64(&index, b);
	return (Square) index;
}
#else

inline Square first_1(Bitboard b) { // Assembly code by Heinz van Saanen
	Bitboard dummy;
	__asm__("bsfq %1, %0": "=r"(dummy): "rm"(b) );
	return (Square) dummy;
}
#endif

inline Square pop_1st_bit(Bitboard* b) {
	const Square s = first_1(*b);
	*b &= ~(1ULL<<s);
	return s;
}

#else // if !defined(USE_BSFQ)
extern Square first_1(Bitboard b);
extern Square pop_1st_bit(Bitboard* b);

#endif

#endif /* BITBOARD_H_ */
