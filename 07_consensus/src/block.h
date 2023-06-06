#pragma once
#include <botan/rsa.h>
#include <string>
#include <vector>
#include <cstdint>
#include <cstddef>

struct block
{
	std::string data;
	std::vector<std::uint8_t> signature;
	Botan::RSA_PublicKey owner;
	std::size_t hash;
};
