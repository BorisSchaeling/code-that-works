#include <blockchain.h>
#include <block.h>
#include <user.h>
#include <GUnit.h>
#include <botan/auto_rng.h>
#include <botan/rsa.h>
#include <boost/asio.hpp>
#include <boost/process.hpp>
#include <boost/algorithm/string.hpp>
#include <string>
#include <string_view>
#include <vector>
#include <algorithm>
#include <thread>
#include <chrono>
#include <cstdint>
#include <cstddef>
#include <signal.h>

GSTEPS("Blockchain Validation*")
{
	blockchain blchain;

	Given("an empty blockchain") = [&]()
	{
		blchain = blockchain{};
	};

	When("blocks are added", "data") = [&](const testing::Table& table)
	{
		Botan::AutoSeeded_RNG autoseeded_rng;
		const Botan::RSA_PrivateKey private_key{ autoseeded_rng, 2048 };
		for (const auto& row : table)
			blchain.add(block{ .data = row.at("data"), .owner = private_key });
	};

	When("data in block {i} is changed to {data}") = [&](const std::uint32_t i, const std::string& data)
	{
		blchain[i].data = data;
	};

	When("hash in block {i} is changed to {hash}") = [&](const std::uint32_t i, const std::size_t hash)
	{
		blchain[i].hash = hash;
	};

	Then("validation should pass") = [&]()
	{
		EXPECT_TRUE(blchain.valid());
	};

	Then("validation should fail") = [&]()
	{
		EXPECT_FALSE(blchain.valid());
	};
}

GSTEPS("Transaction Handling*")
{
	std::vector<user> users;

	Given("1 user with an empty blockchain") = [&]()
	{
		users = std::vector<user>(1);
	};

	Given("{n} users with an empty blockchain") = [&](const std::uint32_t n)
	{
		users = std::vector<user>(n);
	};

	When("user {sender} makes a transaction with the NFT {nft} to user {receiver}") = [&](const std::uint32_t sender, const std::string& nft, const std::uint32_t receiver)
	{
		ASSERT_LT(sender, users.size());
		ASSERT_LT(receiver, users.size());
		users.at(sender).send_transaction(nft, users.at(receiver));
	};

	Then("the blockchain of user {user} contains", "data") = [&](const std::uint32_t user, const testing::Table& table)
	{
		ASSERT_LT(user, users.size());
		std::uint32_t i{};
		for (const auto& row : table)
		{
			ASSERT_LT(i, users.at(user).get_blockchain().size());
			EXPECT_EQ(users.at(user).get_blockchain()[i].data, row.at("data"));
			EXPECT_TRUE(signed_by(users.at(user).get_blockchain()[i].data, users.at(user).get_blockchain()[i].signature, users.at(static_cast<int>(row.at("signedby"))).get_public_key()));
			EXPECT_EQ(users.at(user).get_blockchain()[i].owner.public_key_bits(), users.at(static_cast<int>(row.at("owner"))).get_public_key().public_key_bits());
			++i;
		}
		EXPECT_EQ(users.at(user).get_blockchain().size(), table.size());
	};
}

GSTEPS("Blockchain Consensus*")
{
	std::vector<user> users;

	Given("{n} users with an empty blockchain") = [&](const std::uint32_t n)
	{
		users = std::vector<user>(n);
	};

	When("blocks are added to the blockchain of user {i}", "data") = [&](const testing::Table& table, const std::uint32_t i)
	{
		for (const auto& row : table)
			users.at(i).get_blockchain().add(block{ .data = row.at("data"), .owner = users.at(i).get_private_key() });
	};

	When("user {i} and user {i} synchronize their blockchains") = [&](const std::uint32_t i, const std::uint32_t j)
	{
		synchronize(users.at(i), users.at(j));
	};

	Then("the blockchain of user {user} contains", "data") = [&](const std::uint32_t user, const testing::Table& table)
	{
		ASSERT_LT(user, users.size());
		std::uint32_t i{};
		for (const auto& row : table)
		{
			ASSERT_LT(i, users.at(user).get_blockchain().size());
			EXPECT_EQ(users.at(user).get_blockchain()[i].data, row.at("data"));
			++i;
		}
		EXPECT_EQ(users.at(user).get_blockchain().size(), table.size());
	};
}

GSTEPS("Integration*")
{
	boost::asio::io_context io_context;
	boost::process::child server;
	std::vector<boost::process::child> clients;
	std::vector<char> server_stdout(64);
	std::vector<std::vector<char>> client_stdouts;

	Given("a server with the blockchain", "data") = [&](const testing::Table& table)
	{
		std::string blockchain;
		for (const auto& row : table)
			blockchain += row.at("data") + ',';
		boost::algorithm::trim_right_if(blockchain, boost::algorithm::is_any_of(","));
		server = boost::process::child{ "../src/exe --server " + blockchain, boost::process::std_out > boost::asio::buffer(server_stdout), io_context };
		std::this_thread::sleep_for(std::chrono::seconds{1});
	};

	Given("{n} clients with the blockchain", "data") = [&](const std::uint32_t n, const testing::Table& table)
	{
		std::string blockchain;
		for (const auto& row : table)
			blockchain += row.at("data") + ',';
		boost::algorithm::trim_right_if(blockchain, boost::algorithm::is_any_of(","));
		client_stdouts.reserve(n);
		for (std::uint32_t i = 0; i < n; ++i)
		{
			client_stdouts.emplace_back(64);
			clients.emplace_back("../src/exe --client " + blockchain, boost::process::std_out > boost::asio::buffer(client_stdouts.back()), io_context);
		}
	};

	When("they sychronize") = [&]()
	{
		auto f = std::async(std::launch::async, [&clients, &server]() {
			std::ranges::for_each(clients, [](boost::process::child& c) { c.join(); });
			kill(server.id(), SIGTERM);
			});
		io_context.run();
		f.wait();
		server.join();
	};

	Then("the server blockchain contains", "data") = [&](const testing::Table& table)
	{
		std::string blockchain;
		for (const auto& row : table)
			blockchain += row.at("data") + ',';
		blockchain.back() = '\n';
		EXPECT_EQ(blockchain, std::string_view(server_stdout.data(), blockchain.size()));
	};

	Then("the client blockchains contain", "data") = [&](const testing::Table& table)
	{
		std::string blockchain;
		for (const auto& row : table)
			blockchain += row.at("data") + ',';
		blockchain.back() = '\n';
		std::ranges::for_each(client_stdouts, [&blockchain](const std::vector<char>& v) { EXPECT_EQ(blockchain, std::string_view(v.data(), blockchain.size())); });
	};
}
