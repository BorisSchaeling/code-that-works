#pragma once
#include <botan/auto_rng.h>
#include <botan/rsa.h>
#include <botan/pubkey.h>
#include <string>
#include <vector>
#include <cstdint>

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

private:
	static Botan::RSA_PrivateKey make_private_key()
	{
		Botan::AutoSeeded_RNG autoseeded_rng;
		return { autoseeded_rng, 2048 };
	}

	Botan::RSA_PrivateKey private_key;
	Botan::RSA_PublicKey public_key;
};
