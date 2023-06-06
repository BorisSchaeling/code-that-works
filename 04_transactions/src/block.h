#pragma once
#include <string>
#include <cstdint>
#include <cstddef>

struct block
{
	std::string data;
	std::uint32_t owner;
	std::size_t hash;
};
