#pragma once
#include "block.h"
#include <botan/rsa.h>
#include <botan/x509_key.h>
#include <vector>
#include <string>
#include <functional>
#include <cstddef>
#include <cstdint>

class blockchain : private std::vector<block>
{
public:
	using std::vector<block>::operator[];
	using std::vector<block>::begin;
	using std::vector<block>::end;
	using std::vector<block>::rbegin;
	using std::vector<block>::rend;
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

	void add(block bl)
	{
		bl.hash = make_hash(bl.data, bl.signature, bl.owner);
		push_back(bl);
	}

private:
	std::size_t make_hash(const std::string& data, const std::vector<std::uint8_t>& signature, const Botan::RSA_PublicKey& public_key) const
	{
		const std::string string_signature{signature.begin(), signature.end()};
		return std::hash<std::string>{}(data + string_signature + Botan::X509::PEM_encode(public_key) + prev_hash(size()));
	}

	bool verify_hash(const std::size_t i) const
	{
		const std::string string_signature{at(i).signature.begin(), at(i).signature.end()};
		return std::hash<std::string>{}(at(i).data + string_signature + Botan::X509::PEM_encode(at(i).owner) + prev_hash(i)) == at(i).hash;
	}

	std::string prev_hash(const std::size_t i) const
	{
		return i > 0 ? std::to_string(at(i - 1).hash) : "";
	}
};
