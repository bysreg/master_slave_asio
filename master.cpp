#include <boost/thread/thread.hpp>
#include "master.hpp"

static const int read_msg_max_length = 1440000; 
static const int write_msg_max_length = 1000;

Connection::Connection(boost::asio::ip::tcp::socket socket_)
	: socket(std::move(socket_)), read_msg(read_msg_max_length)
{}

struct Test {
	char a;
	char b;
	char c;
	char d;
	Test() {
		a = 'h';
		b = 'i';
		c = 'l';
		d = 'm';
	}
};

void Connection::start()
{
	std::cout<<"a connection started"<<std::endl;

	Test test;
	send(test);

	//send("hello world");

	do_read_header();
}

void Connection::send(const std::string& str)
{
	// std::cout<<"trying to send a string"<<std::endl;

	Message* msg = new Message(str.length());

	msg->set_body_length(str.length());
	std::memcpy(msg->body(), str.c_str(), str.length());
	msg->encode_header();
	send(msg);
}

void Connection::send(Message* msg)
{
	bool write_in_progress = !write_msgs.empty();
	write_msgs.push_back(msg);
	if (!write_in_progress)
	{
		do_write();
	}
}

void Connection::do_write()
{
	// std::cout<<"trying to call write"<<std::endl;

	auto self(shared_from_this());
	boost::asio::async_write(socket,
		boost::asio::buffer(write_msgs.front()->data(),
		write_msgs.front()->length()),
		[this, self](boost::system::error_code ec, std::size_t /*length*/)
		{
			if (!ec)
			{
				//delete the recent sent message
				Message* sent = write_msgs.front();
				delete sent;

				write_msgs.pop_front();
				if (!write_msgs.empty())
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

void Connection::do_read_header()
{
	// std::cout<<"trying to call read header"<<std::endl;

	auto self(shared_from_this());
	boost::asio::async_read(socket,
		boost::asio::buffer(read_msg.data(), Message::header_length),
			[this, self](boost::system::error_code ec, std::size_t /*length*/)
		{
			if (!ec && read_msg.decode_header())
			{
				do_read_body();
			}
			else
			{
					// something is wrong
			}
		});
}

void Connection::do_read_body()
{
	// std::cout<<"trying to call read body"<<std::endl;
	auto self(shared_from_this());
	boost::asio::async_read(socket,
		boost::asio::buffer(read_msg.body(), read_msg.body_length()),
			[this, self](boost::system::error_code ec, std::size_t /*length*/)
		{
			if (!ec)
			{
				std::cout.write(read_msg.body(), read_msg.body_length());
				std::cout << "\n";

				do_read_header();
			}
			else
			{
					//something is wrong
			}
		});
}

Master::Master(boost::asio::io_service& io_service)
	: acceptor(io_service, tcp::endpoint(tcp::v4(), 50000)),
	socket(io_service)
{
	do_accept();
}

void Master::start()
{
	static boost::asio::io_service io_service;

	static Master master(io_service);

	std::cout<<"starting master..."<<std::endl;

	boost::thread t(boost::bind(&boost::asio::io_service::run,
		&io_service));
}

void Master::do_accept()
{
	acceptor.async_accept(socket,
		[this](boost::system::error_code ec)
		{
			if (!ec)
			{
				std::make_shared<Connection>(std::move(socket))->start();
			}

			do_accept();
		});
}

int main(int argc, char* argv[])
{
	try
	{
		Master::start();  

		while(true) {};

	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}