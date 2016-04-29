#pragma once

#include <ctime>
#include <string>
#include <boost/asio.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <deque>
#include <iostream>

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

	void send(const std::string& str);
	void send(Message* msg);
	void do_read_header();
	void do_read_body();
	void do_write();

	tcp::socket socket;
	Message read_msg;
	MessageQueue write_msgs;
};

class Master
{
private:
	typedef boost::asio::ip::tcp tcp;

public:	

	Master(boost::asio::io_service& io_service);

	static void start();

private:
	void do_accept();

	tcp::acceptor acceptor;
	tcp::socket socket;
};
