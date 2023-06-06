#pragma once
#include "block.h"
#include <botan/auto_rng.h>
#include <botan/secmem.h>
#include <botan/pubkey.h>
#include <botan/rsa.h>
#include <botan/x509_key.h>
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

	void add(const Botan::RSA_PrivateKey& private_key, const Botan::RSA_PublicKey& public_key, const std::string& data)
	{
		const Botan::secure_vector<std::uint8_t> data_vector{data.begin(), data.end()};
		Botan::AutoSeeded_RNG rng;
		Botan::PK_Signer signer(private_key, rng, "EMSA3(SHA-256)");
		const std::vector<std::uint8_t> signature = signer.sign_message(data_vector, rng);
		const std::string string_signature{signature.begin(), signature.end()};
		emplace_back(data, signature, public_key, std::hash<std::string>{}(data + string_signature + Botan::X509::PEM_encode(public_key) + prev_hash(size())));
	}

	void transfer(const Botan::RSA_PrivateKey& private_key, const Botan::RSA_PublicKey& public_key, const std::string& data)
	{
		const auto it = std::ranges::find_if(*this | std::views::reverse, [&data](const block& block) { return block.data == data; });
		if (it != rend() && it->owner.public_key_bits() == static_cast<Botan::RSA_PublicKey>(private_key).public_key_bits())
			add(private_key, public_key, data);
	}

private:
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
