#pragma once

#include <exception>
#include <string>
using namespace std;

class MessageException : public exception
{
private:
	string error_;

public:
	explicit MessageException(const string& error);

	const char* what() const override;
};

class Message
{
private:
	string from_;
	string to_;
	string text_;

public:
	Message(const string& from, const string& to, const string& text);
	~Message() = default;

	const string& getFrom() const;
	const string& getTo() const;

	friend ostream& operator<<(ostream& out, const Message& message);
};
