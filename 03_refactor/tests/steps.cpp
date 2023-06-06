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
