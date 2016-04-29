#pragma once

#include <ctime>
#include <string>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <deque>
#include <iostream>
#include <set>

#include "message.hpp"

class Connection
: public std::enable_shared_from_this<Connection>
{
private:
	typedef boost::asio::ip::tcp tcp;
	typedef std::deque<Message*> MessageQueue;

public:
	Connection(tcp::socket socket);	

	void start();
private:

	template<typename T>
	void send(const T& value) {
		Message* msg = new Message(sizeof(T));

		msg->set_body_length(sizeof(T));
		std::memcpy(msg->body(), &value, sizeof(T));
		msg->encode_header();
		send(msg);
	}

	void send(const std::string& str);
	void send(Message* msg);
	void do_read_header();
	void do_read_body();
	void do_write();

	tcp::socket socket;
	Message read_msg;
	MessageQueue write_msgs;
};

typedef std::shared_ptr<Connection> ConnectionPtr;

class Master
{
private:
	typedef boost::asio::ip::tcp tcp;

public:	

	Master(boost::asio::io_service& io_service);

	static void start();

	// void send_all(const std::string& str);
	// void send_all(Message* msg);

private:
	void do_accept();

	tcp::acceptor acceptor;
	tcp::socket socket;

	std::set<ConnectionPtr> connections;
};
