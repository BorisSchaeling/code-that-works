#include <blockchain.h>
#include <block.h>
#include <GUnit.h>
#include <string>
#include <cstdint>

GSTEPS("Blockchain Validation*")
{
	blockchain blockchain;

	Given("an empty blockchain") = [&]()
	{
		blockchain.clear();
	};

	When("a block with a valid hash is added") = [&]()
	{
		blockchain.add("");
	};

	When("a block with an invalid hash is added") = [&]()
	{
		blockchain.add("");
		blockchain.back().hash = 0;
	};

	When("blocks are added", "data") = [&](const testing::Table& table)
	{
		for (const auto& row : table)
			blockchain.add(row.at("data"));
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

	Given("an empty blockchain") = [&]()
	{
		blockchain.clear();
	};

	Given("1 user") = [&]()
	{
	};

	Given("{n} users") = [&](const std::uint32_t n)
	{
	};

	When("user {i} adds the NFT {name}") = [&](const std::uint32_t i, const std::string& nft)
	{
		blockchain.add(nft, i);
	};

	When("user {sender} transfers the NFT {name} to user {receiver}") = [&](const std::uint32_t sender, const std::string& nft, const std::uint32_t receiver)
	{
		blockchain.transfer(sender, receiver, nft);
	};

	Then("the blocks exist", "data") = [&](const testing::Table& table)
	{
		std::uint32_t i{};
		for (const auto& row : table)
		{
			ASSERT_LT(i, blockchain.size());
			EXPECT_EQ(blockchain[i].data, row.at("data"));
			EXPECT_EQ(blockchain[i].owner, static_cast<std::uint32_t>(row.at("owner")));
			++i;
		}
	};
}
