#pragma once

#include <ctime>
#include <string>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <deque>
#include <iostream>

#include "message.hpp"

class Connection 
	: public boost::enable_shared_from_this<Connection> 
{	
private:
	typedef boost::asio::ip::tcp tcp;
	typedef std::deque<Message> MessageQueue;

public:

	Connection(tcp::socket socket)
		: socket_(std::move(socket)) 
	{}

	void start()
	{
		send("hello world");
		do_read_header();	
	}

private:

	void send(const std::string& str)
	{
		Message msg;

		msg.set_body_length(str.length());
		std::memcpy(msg.body(), str.c_str(), str.length());
		msg.encode_header();
		send(msg);
	}

	void send(const Message& msg)
	{
		bool write_in_progress = !write_msgs_.empty();
		write_msgs_.push_back(msg);
		if (!write_in_progress)
		{
			do_write();
		}
	}

	void do_read_header()
	{
		auto self(shared_from_this());
		boost::asio::async_read(socket_,
			boost::asio::buffer(read_msg_.data(), Message::header_length),
				[this, self](boost::system::error_code ec, std::size_t /*length*/)
			{
				if (!ec && read_msg_.decode_header())
				{
					do_read_body();
				}
				else
				{
						// something is wrong
				}
			});
	}

	void do_read_body()
	{
		auto self(shared_from_this());
		boost::asio::async_read(socket_,
			boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
				[this, self](boost::system::error_code ec, std::size_t /*length*/)
			{
				if (!ec)
				{
					std::cout.write(read_msg_.body(), read_msg_.body_length());
					std::cout << "\n";

					do_read_header();
				}
				else
				{
						//something is wrong
				}
			});
	}

	void do_write()
	{
		auto self(shared_from_this());
		boost::asio::async_write(socket_,
			boost::asio::buffer(write_msgs_.front().data(),
				write_msgs_.front().length()),
				[this, self](boost::system::error_code ec, std::size_t /*length*/)
			{
				if (!ec)
				{
					write_msgs_.pop_front();
					if (!write_msgs_.empty())
					{
						do_write();
					}
				}
				else
				{
						//something is wrong
				}
			});
	}

	tcp::socket socket_;
	Message read_msg_;
	MessageQueue write_msgs_;
};

// class Master
// {

// private:
// 	typedef boost::asio::ip::tcp tcp;

// public:	

// 	// FIXME : should be private
// 	Master(boost::asio::io_service& io_service) 
// 		: acceptor(io_service, tcp::endpoint(tcp::v4(), 50000)), // 1030 is the port number
// 		  socket(io_service)
// 	{}	

// 	// create a new thread to run the master tcp async server
// 	static void start();

// 	// stop master tcp async server
// 	static void stop();

// 	static const char ACK = 1;

// private:		

// 	tcp::acceptor acceptor;
// 	tcp::socket socket;

// 	void run();
// 	void start_accept();
// 	void handle_accept(const boost::system::error_code& error);
// 	// void send_complete(const boost::system::error_code& /*error*/,
//  //      size_t /*bytes_transferred*/);

// 	// communications
// 	// void send_ack(Connection::pointer);
// 	// void send_helloworld(Connection::pointer);
	
// 	// void send(Connection::pointer, char code);
// 	// void send(Connection::pointer, const std::string& s);
// 	// void send(Connection::pointer, const Message& msg);

// 	static std::string make_daytime_string()
// 	{
// 	  using namespace std; // For time_t, time and ctime;
// 	  time_t now = time(0);
// 	  return ctime(&now);
// 	}
// };
