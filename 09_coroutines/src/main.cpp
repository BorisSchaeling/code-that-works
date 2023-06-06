#include "user.h"
#include "blockchain.h"
#include "block.h"
#include <botan/auto_rng.h>
#include <botan/rsa.h>
#include <boost/asio.hpp>
#include <boost/algorithm/string.hpp>
#include <string>
#include <string_view>
#include <vector>
#include <istream>
#include <sstream>
#include <iostream>

Botan::AutoSeeded_RNG autoseeded_rng;
Botan::RSA_PrivateKey private_key{ autoseeded_rng, 2048 };
boost::asio::io_context io_context;

blockchain load_blockchain_from_string(std::string_view s)
{
	std::vector<std::string> v;
	boost::algorithm::split(v, boost::algorithm::trim_copy(s), boost::algorithm::is_any_of(","));
	blockchain blchain;
	for (const std::string& data : v)
		blchain.add(block{ .data = data, .owner = private_key });
	return blchain;
}

std::string save_blockchain_to_string(const blockchain& blchain)
{
	std::vector<std::string> v;
	for (const block& bl : blchain)
		v.push_back(bl.data);
	return boost::algorithm::join(v, ",");
}

void synchronize(boost::asio::ip::tcp::socket& sock, blockchain& bl)
{
	boost::asio::write(sock, boost::asio::buffer(save_blockchain_to_string(bl) + '\n'));
	boost::asio::streambuf streambuf;
	boost::system::error_code ec;
	boost::asio::read_until(sock, streambuf, '\n', ec);
	if (!ec)
	{
		std::istream is{&streambuf};
		std::stringstream ss;
		ss << is.rdbuf();
		const blockchain recv_bl = load_blockchain_from_string(ss.str());
		if (recv_bl.size() > bl.size())
			bl = recv_bl;
	}
}

boost::asio::awaitable<void> serve(blockchain& bl)
{
	auto executed = co_await boost::asio::this_coro::executor;
	boost::asio::ip::tcp::acceptor acceptor{io_context, boost::asio::ip::tcp::endpoint{boost::asio::ip::tcp::v4(), 4004}};
	for (;;)
	{
		boost::asio::ip::tcp::socket sock = co_await acceptor.async_accept(boost::asio::use_awaitable);
		synchronize(sock, bl);
	}
}

int main(int argc, char* argv[])
{
	if (argc < 2)
		return -1;

	user user;
	if (argc > 2)
		user.get_blockchain() = load_blockchain_from_string(argv[2]);

	if (std::string_view{argv[1]} == "--server")
	{
		boost::asio::signal_set signals{io_context, SIGTERM};
		signals.async_wait([&](auto, auto) { io_context.stop(); });
		boost::asio::co_spawn(io_context, serve(user.get_blockchain()), boost::asio::detached);
		io_context.run();
	}
	else if (std::string_view{argv[1]} == "--client")
	{
		boost::asio::ip::tcp::socket sock{io_context};
		boost::asio::ip::tcp::resolver resolver{io_context};
		boost::asio::connect(sock, resolver.resolve("localhost", "4004"));
		synchronize(sock, user.get_blockchain());
	}

	std::cout << save_blockchain_to_string(user.get_blockchain()) << '\n' << std::flush;
}
