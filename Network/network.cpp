#include "network.h"
#include <boost/bind.hpp>

// ================================================================================================
// Client Session class
// ================================================================================================
ClientSession::ClientSession(Server& holder) : holder_(holder), socket_(holder.GetIoService())
{
}

ClientSession::~ClientSession()
{

}

void ClientSession::OnRead(const boost::system::error_code& ec)
{
	if (!ec)
		return;

	// do we have bytes on connection
	boost::system::error_code err;
	size_t avail_bytes = socket_.available(err);
	if (err || avail_bytes == 0)
		return;

	for (size_t read_bytes = 0; read_bytes < avail_bytes;)
	{
		char data[1024];
		size_t len = socket_.read_some(boost::asio::buffer(data), err);

		read_bytes += len;
		for (char* dataPtr = data; len; --len, ++dataPtr)
		{
			std::cout << *dataPtr;
			// TODO: call commands here
		}
	}
}

void ClientSession::StartRead()
{
	socket_.async_read_some(boost::asio::null_buffers(),
		std::bind(&ClientSession::OnRead, shared_from_this(), std::placeholders::_1)
	);
}

// ================================================================================================
// Server class
// ================================================================================================
Server::Server() : acceptor_(io_service_)
{
}

Server::~Server()
{
}

void Server::OnAccept(ClientSessionPtr client_session, const boost::system::error_code& ec)
{
	if (!ec)
	{
		client_session->StartRead();
		sessions_.insert(client_session.get());
	}

	StartAccept();
}

void Server::StartAccept()
{
	auto new_client_session = std::make_shared< ClientSession >(*this);

	acceptor_.async_accept(new_client_session->GetSocket(),
		std::bind(&Server::OnAccept, this, new_client_session, std::placeholders::_1)
	);
}

void Server::Listen(const std::string ip_addr, const int port)
{
	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address_v4::from_string(ip_addr), port);

	acceptor_.open(ep.protocol());
	acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
	acceptor_.bind(ep);
	acceptor_.listen();

	StartAccept();

	for (;;)
	{
		try{
			io_service_.run();
			break;
		}
		catch (const std::exception& ex)
		{
			std::cout << "io_service_ exception: " << ex.what() << std::endl;
		}
	}
}

void Server::Stop()
{
	GetIoService().stop();
}