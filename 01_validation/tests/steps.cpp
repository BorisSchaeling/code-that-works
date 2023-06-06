#include <blockchain.h>
#include <block.h>
#include <GUnit.h>

GSTEPS("Blockchain Validation*")
{
	Given("an empty blockchain") = [&]()
	{
		blockchain blockchain;

		When("a block with a valid hash is added") = [&]()
		{
			blockchain.emplace_back("valid");
		};

		When("a block with an invalid hash is added") = [&]()
		{
			blockchain.emplace_back("invalid");
		};

		Then("validation should pass") = [&]()
		{
			EXPECT_TRUE(blockchain.valid());
		};

		Then("validation should fail") = [&]()
		{
			EXPECT_FALSE(blockchain.valid());
		};
	};
}
