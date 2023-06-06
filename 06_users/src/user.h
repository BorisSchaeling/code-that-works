#pragma once
#include "blockchain.h"
#include <botan/auto_rng.h>
#include <botan/rsa.h>
#include <botan/secmem.h>
#include <botan/pubkey.h>
#include <string>
#include <vector>
#include <algorithm>
#include <ranges>
#include <cstddef>
#include <cstdint>

inline std::vector<std::uint8_t> sign(const std::string& data, const Botan::RSA_PrivateKey& private_key)
{
	const Botan::secure_vector<std::uint8_t> data_vector{data.begin(), data.end()};
	Botan::AutoSeeded_RNG rng;
	Botan::PK_Signer signer(private_key, rng, "EMSA3(SHA-256)");
	return signer.sign_message(data_vector, rng);
}

inline bool signed_by(const std::string& data, const std::vector<std::uint8_t>& signature, const Botan::RSA_PublicKey& public_key)
{
	Botan::PK_Verifier verifier(public_key, "EMSA3(SHA-256)");
	const std::vector<std::uint8_t> data_vector{data.begin(), data.end()};
	return verifier.verify_message(data_vector, signature);
}

class user
{
public:
	user() : private_key{ make_private_key() }, public_key{ private_key } {}

	const Botan::RSA_PrivateKey& get_private_key() const
	{
		return private_key;
	}

	const Botan::RSA_PublicKey& get_public_key() const
	{
		return public_key;
	}

	const blockchain& get_blockchain() const
	{
		return blchain;
	}

	void send_transaction(const std::string& nft, user& receiver)
	{
		block bl{ nft, sign(nft, private_key), receiver.public_key };
		blchain.add(bl);
		if (public_key.public_key_bits() != receiver.public_key.public_key_bits())
			receiver.receive_transaction(bl);
	}

	void receive_transaction(const block& transaction)
	{
		const auto it = std::ranges::find_if(blchain | std::views::reverse, [&transaction](const block& block) { return block.data == transaction.data; });
		if (it == blchain.rend())
			blchain.add(transaction);
		else if (signed_by(transaction.data, transaction.signature, it->owner))
			blchain.add(transaction);
	}

private:
	static Botan::RSA_PrivateKey make_private_key()
	{
		Botan::AutoSeeded_RNG autoseeded_rng;
		return { autoseeded_rng, 2048 };
	}

	Botan::RSA_PrivateKey private_key;
	Botan::RSA_PublicKey public_key;
	blockchain blchain;
};
