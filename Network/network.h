#include <iostream>
#include <set>
#include <string>

#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>

class Server;
class ClientSession : public std::enable_shared_from_this< ClientSession >
{
public:
	ClientSession(Server& holder);
	~ClientSession();

	void StartRead();
	void OnRead(const boost::system::error_code& ec);

	void StartWrite();
	void OnWrite();

	boost::asio::ip::tcp::socket& GetSocket() { return socket_;  }

private:
	Server&							holder_;
	boost::asio::ip::tcp::socket	socket_;

};

typedef std::shared_ptr< ClientSession > ClientSessionPtr;

/**
 * Class Server
 * @brief Contains acceptor socket, required to listen incoming connections.
 * Creates ClientSessions and stores them in its own pool
 */
class Server
{
public:
	Server();

	~Server();

	void Listen(const std::string ip_addr, const int port);
	void Stop();

	boost::asio::io_service& GetIoService() { return io_service_; }

protected:
	void StartAccept();
	void OnAccept(ClientSessionPtr client_session, const boost::system::error_code& ec);

private:
	boost::asio::io_service				io_service_;	///< Service to provide I/O socket activities
	boost::asio::ip::tcp::acceptor		acceptor_;		///< Socket, accepting incoming connections

	std::set< ClientSession* >			sessions_;		///< Clients, connected to the server

};


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