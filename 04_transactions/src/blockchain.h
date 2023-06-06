#pragma once
#include "block.h"
#include <vector>
#include <string>
#include <functional>
#include <algorithm>
#include <ranges>
#include <cstddef>
#include <cstdint>

class blockchain : private std::vector<block>
{
public:
	using std::vector<block>::operator[];
	using std::vector<block>::begin;
	using std::vector<block>::end;
	using std::vector<block>::clear;
	using std::vector<block>::back;
	using std::vector<block>::size;

	bool valid() const
	{
		for (std::size_t i = 0; i < size(); ++i)
		{
			if (!verify_hash(i))
				return false;
		}

		return true;
	}

	void add(const std::string& data, const std::uint32_t owner = 0)
	{
		emplace_back(data, owner, std::hash<std::string>{}(data + prev_hash(size())));
	}

	void transfer(const std::uint32_t sender, const std::uint32_t receiver, const std::string& data)
	{
		const auto it = std::ranges::find_if(*this | std::views::reverse, [&data](const block& block) { return block.data == data; });
		if (it != rend() && it->owner == sender)
			add(data, receiver);
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
