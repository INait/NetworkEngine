#include "network.h"
#include <boost/bind.hpp>

// ================================================================================================
// Client Session class
// ================================================================================================
uint64_t ClientSession::global_client_counter_ = 0;

ClientSession::ClientSession(Server& holder) : holder_(holder), socket_(holder.GetIoService())
{
	this->client_id_ = ++global_client_counter_;
}

ClientSession::~ClientSession()
{
	--global_client_counter_;
}

void ClientSession::OnRead(const boost::system::error_code& ec)
{
	if (ec)
		return;

	// do we have bytes on connection
	boost::system::error_code err;
	size_t avail_bytes = socket_.available(err);
	if (err || avail_bytes == 0)
		return;

	std::cout << "Client id = " << this->client_id_ << " msg: ";
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
	std::cout << std::endl;

	StartRead();
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

void Server::Accept()
{
	auto new_client_session = std::make_shared< ClientSession >(*this);

	acceptor_.async_accept(new_client_session->GetSocket(),
		[this, new_client_session](const boost::system::error_code& ec)
		{
			if (!ec)
			{
				new_client_session->StartRead();
				sessions_.insert(new_client_session.get());
				std::cout << "client connected, id = " << new_client_session->GetClientId() << std::endl;

			}
			Accept();
		}
	);
}

void Server::Listen(const std::string ip_addr, const int port)
{
	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address_v4::from_string(ip_addr), port);

	acceptor_.open(ep.protocol());
	acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
	acceptor_.bind(ep);
	acceptor_.listen();

	Accept();

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
	acceptor_.close();
}

// ================================================================================================
// Client class
// ================================================================================================

Client::Client() : socket_(io_service_), connected_(false)
{
}

Client::~Client()
{
}

void Client::Connect(const std::string& ip_addr, int port)
{
	boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address_v4::from_string(ip_addr), port);
	socket_.async_connect(ep, boost::bind(&Client::OnConnect, this, _1));

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

void Client::OnConnect(const boost::system::error_code& ec)
{
	if (!ec)
	{
		connected_ = true;

		std::cout << "connection to server success" << std::endl;

		StartRead();
		//socket_.async_write_some(boost::asio::buffer(msg, strlen(msg)), boost::bind(&Client::OnWrite, this, _1, _2));
	}
}

void Client::OnRead(const boost::system::error_code& ec)
{
	if (ec)
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
	std::cout << std::endl;

	StartRead();
}

void Client::StartRead()
{
	socket_.async_read_some(boost::asio::null_buffers(),
		std::bind(&Client::OnRead, shared_from_this(), std::placeholders::_1)
	);
}

void Client::OnWrite(const boost::system::error_code& ec, size_t bytes)
{
	// do nothing
}

void Client::Write(std::string message)
{
	socket_.write_some(boost::asio::buffer(message));
}

void Client::Stop()
{
	if (!connected_)
		return;

	connected_ = false;
	socket_.close();
}