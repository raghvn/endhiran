/*
 * ttable.cpp
 *
 *  Created on: 25-Dec-2011
 *      Author: raghav
 */

#include "table.h"
#include <cstring>

Ttable::Ttable(U32 sizemb) :
		size(sizemb * 1024 * 1024 / (sizeof(record))), occupied(0), collisions(
				0), hits(0) {
	table = new record[size];
}

Ttable::Ttable() :
		size(0), occupied(0), collisions(0), hits(0) {
	table = NULL;
}

Ttable::~Ttable() {
	delete[] table;
}

void Ttable::setsize(U32 size_) {
	size = size_ * 1024 * 1024 / (sizeof(record));
	delete[] table;
	table = new record[size];
	occupied = 0;
	hits = 0;

}

void Ttable::reset() {
	memset(table, 0, size * sizeof(record));
	occupied = 0;
	collisions = 0;
	hits = 0;
}

void Ttable::resize(U32 size_) {
	size = size_ * 1024 * 1024 / (sizeof(record));
	occupied = 0;
	collisions = 0;
	hits = 0;
	record* ner = new record[size * 1024 * 1024];memcpy
	(ner, table, sizeof(record));
	record* old = table;
	table = ner;
	delete[] old;
}

record* Ttable::operator[](U64 hash) {

	record* got = table + ((hash) % (size));
	if (got->key() == hash) {
		hits++;
		return got;
	}

	return NULL;
}

record* Ttable::operator[](U64 hash) const {
	record* got = table + ((hash) % (size));
	if (got->key() == hash) {
		return got;
	}


	return NULL;
}

bool Ttable::put(record& in) {
	record* got = table + ((in.key()) % (size));
	U64 gkey = got->key();

	if (in.type()>=got->type()) {
		*got = in;
		if (!gkey) {
			occupied++;
		} else if (gkey != in.key()) {
			collisions++;
			return false;
		}
		return true;
	}

	return false;

}

double Ttable::fillratio() const {
	return double(occupied) / size;
}

U32 Ttable::collision() const {
	return collisions;
}

std::ostream& operator<<(std::ostream& out, Ttable& tt) {
	return out << "Hash: " << tt.hits << " / " << tt.occupied << " / "
			<< tt.size << " / " << tt.fillratio() << " / " << tt.collisions;
}
