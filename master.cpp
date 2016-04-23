#include "master.hpp"

void Connection::start()
{
	send("hello fucker");
	do_read_header();
}

void Connection::send(const std::string& str)
{
	Message msg;

	msg.set_body_length(str.length());
	std::memcpy(msg.body(), str.c_str(), str.length());
	msg.encode_header();
	send(msg);
}

void Connection::send(const Message& msg)
{
	bool write_in_progress = !write_msgs_.empty();
	write_msgs_.push_back(msg);
	if (!write_in_progress)
	{
		do_write();
	}
}

void Connection::do_read_header()
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

void Connection::do_read_body()
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

void Connection::do_write()
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

void Master::start()
{
	static boost::asio::io_service io_service;

	static Master master(io_service);

	io_service.run();
}

void Master::do_accept()
{
	acceptor_.async_accept(socket_,
		[this](boost::system::error_code ec)
		{
			if (!ec)
			{
				std::make_shared<Connection>(std::move(socket_))->start();
			}

			do_accept();
		});
}

int main(int argc, char* argv[])
{
	try
	{
		Master::start();    
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}