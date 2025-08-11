#pragma once

#include <utility>

#include "Types.hpp"

struct IndexPair {
	uint32 a;
	uint32 b;

	IndexPair(uint32 i1, uint32 i2) {
		a = std::min(i1, i2);
		b = std::max(i1, i2);
	}

	bool operator<(const IndexPair& other) const {
		return (a != other.a ? a < other.a : b < other.b);
	}
};
