#include <blockchain.h>
#include <block.h>
#include <user.h>
#include <GUnit.h>
#include <string>
#include <vector>
#include <cstdint>

GSTEPS("Blockchain Validation*")
{
	blockchain blockchain;
	user user;

	Given("an empty blockchain") = [&]()
	{
		blockchain.clear();
	};

	When("a block with a valid hash is added") = [&]()
	{
		blockchain.add(user.get_private_key(), user.get_public_key(), "");
	};

	When("a block with an invalid hash is added") = [&]()
	{
		blockchain.add(user.get_private_key(), user.get_public_key(), "");
		blockchain.back().hash = 0;
	};

	When("blocks are added", "data") = [&](const testing::Table& table)
	{
		for (const auto& row : table)
			blockchain.add(user.get_private_key(), user.get_public_key(), row.at("data"));
	};

	When("data in block {n} is changed to {data}") = [&](const std::uint32_t i, const std::string& data)
	{
		blockchain[i].data = data;
	};

	Then("validation should pass") = [&]()
	{
		EXPECT_TRUE(blockchain.valid());
	};

	Then("validation should fail") = [&]()
	{
		EXPECT_FALSE(blockchain.valid());
	};
}

GSTEPS("Transaction Handling*")
{
	blockchain blockchain;
	std::vector<user> users;

	Given("an empty blockchain") = [&]()
	{
		blockchain.clear();
	};

	Given("1 user") = [&]()
	{
		users.assign(1, {});
	};

	Given("{n} users") = [&](const std::uint32_t n)
	{
		users.assign(n, {});
	};

	When("user {i} adds the NFT {name}") = [&](const std::uint32_t i, const std::string& nft)
	{
		blockchain.add(users.at(i).get_private_key(), users.at(i).get_public_key(), nft);
	};

	When("user {sender} transfers the NFT {name} to user {receiver}") = [&](const std::uint32_t sender, const std::string& nft, const std::uint32_t receiver)
	{
		blockchain.transfer(users.at(sender).get_private_key(), users.at(receiver).get_public_key(), nft);
	};

	Then("the blocks exist", "data") = [&](const testing::Table& table)
	{
		std::uint32_t i{};
		for (const auto& row : table)
		{
			ASSERT_LT(i, blockchain.size());
			EXPECT_EQ(blockchain[i].data, row.at("data"));
			EXPECT_TRUE(signed_by(blockchain[i].data, blockchain[i].signature, users.at(static_cast<int>(row.at("signedby"))).get_public_key()));
			EXPECT_EQ(blockchain[i].owner.public_key_bits(), users.at(static_cast<int>(row.at("owner"))).get_public_key().public_key_bits());
			++i;
		}
	};
}
