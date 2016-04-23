#pragma once

class Message
{

public:
	static const int header_length = 4;
	static const int max_body_length = 200;

	Message() : body_length_(0)
	{}

	~Message()
	{

	}

	inline const char* data() const
	{
		return data_;
	}

	inline char* data()
	{
		return data_;
	}

	inline int length() const
	{
		return header_length + body_length_;
	}

	inline const char* body() const
	{
		return data_ + header_length;
	}

	inline char* body()
	{
		return data_ + header_length;
	}

	inline int body_length() const
	{
		return body_length_;
	}

	inline bool decode_header()
	{
		char header[header_length + 1] = "";
		std::strncat(header, data_, header_length);
		body_length_ = std::atoi(header);
		if (body_length_ > max_body_length)
		{
			body_length_ = 0;
			return false;
		}
		return true;
	}

	inline void encode_header()
	{
		char header[header_length + 1] = "";
		std::sprintf(header, "%4d", static_cast<int>(body_length_));
		std::memcpy(data_, header, header_length);
	}

	inline void set_body_length(int val)
	{
		body_length_ = val;
		if(body_length_ > val)
			body_length_ = val;
	}


private:
	char data_[header_length + max_body_length];
	int body_length_;

};	