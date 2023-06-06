#pragma once
#include "block.h"
#include <vector>
#include <string>
#include <functional>
#include <cstddef>

struct blockchain : private std::vector<block>
{
	using std::vector<block>::operator[];
	using std::vector<block>::clear;
	using std::vector<block>::back;

	bool valid() const
	{
		if (empty())
			return true;

		for (std::size_t i = size() - 1; i > 0; --i)
		{
			if (std::hash<std::string>{}(at(i).data + std::to_string(at(i - 1).hash)) != at(i).hash)
				return false;
		}

		return std::hash<std::string>{}(at(0).data) == at(0).hash;
	}

	void emplace_back(const std::string& data)
	{
		std::string to_hash = data;
		if (!empty())
			to_hash += std::to_string(back().hash);
		std::vector<block>::emplace_back(data, std::hash<std::string>{}(to_hash));
	}
};
