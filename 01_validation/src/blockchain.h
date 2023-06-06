#pragma once
#include "block.h"
#include <vector>
#include <algorithm>

struct blockchain : private std::vector<block>
{
	using std::vector<block>::emplace_back;
	using std::vector<block>::begin;
	using std::vector<block>::end;

	bool valid() const
	{
		return std::ranges::all_of(*this, [](const block& block) { return block.hash == "valid"; });
	}
};
