#include "Message.h"

MessageException::MessageException(const string& error) : error_(error) {};

const char* MessageException::what() const
{
	return error_.c_str();
}

Message::Message(const string& from, const string& to, const string& text) : from_(from), to_(to), text_(text) {}

const string& Message::getFrom() const { return from_; }
const string& Message::getTo() const { return to_; }

ostream& operator<<(ostream& out, const Message& message)
{
	size_t ln = message.from_.length() + message.to_.length() + 15;
	out << "From: " << message.from_ << " To: " << message.to_ << endl << message.text_ << endl << string(ln, '-');

	return out;
}
