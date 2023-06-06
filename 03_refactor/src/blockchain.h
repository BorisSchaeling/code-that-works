#pragma once
#include "block.h"
#include <vector>
#include <string>
#include <functional>
#include <cstddef>

class blockchain : private std::vector<block>
{
public:
	using std::vector<block>::operator[];
	using std::vector<block>::clear;
	using std::vector<block>::back;

	bool valid() const
	{
		for (std::size_t i = 0; i < size(); ++i)
		{
			if (!verify_hash(i))
				return false;
		}

		return true;
	}

	void add(const std::string& data)
	{
		emplace_back(data, std::hash<std::string>{}(data + prev_hash(size())));
	}

private:
	bool verify_hash(const std::size_t i) const
	{
		return std::hash<std::string>{}(at(i).data + prev_hash(i)) == at(i).hash;
	}

	std::string prev_hash(const std::size_t i) const
	{
		return i > 0 ? std::to_string(at(i - 1).hash) : "";
	}
};
