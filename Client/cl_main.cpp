#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>

class Client : public boost::enable_shared_from_this< Client >
{
public:
	Client();
	~Client();

	void Connect(const std::string& ip_addr, int port);
	void OnConnect(const boost::system::error_code& ec);

	void OnWrite(const boost::system::error_code& ec, size_t bytes);
	void Stop();

	boost::asio::io_service& GetIoService() { return io_service_; }

private:
	boost::asio::io_service			io_service_;
	boost::asio::ip::tcp::socket	socket_;

	bool							connected_;
};

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
}

void Client::OnConnect(const boost::system::error_code& ec)
{
	if (!ec)
	{
		connected_ = true;
		char write_buf[1024] = "Hello world from client";
		socket_.async_write_some(boost::asio::buffer(write_buf), boost::bind(&Client::OnWrite, this, _1, _2));
	}
}

void Client::OnWrite(const boost::system::error_code& ec, size_t bytes)
{
	// do nothing
}

void Client::Stop()
{
	if (!connected_)
		return;

	connected_ = false;
	socket_.close();
}

int main()
{
	std::string ip_addr = "127.0.0.1";
	int port = 34560;

	Client client;
	client.Connect(ip_addr, port);

	client.GetIoService().run();

	return 0;
}