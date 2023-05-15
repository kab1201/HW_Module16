#pragma once

#include <exception>
#include <string>
#include <iostream>
using namespace std;

typedef unsigned int uint;

class UserException : public exception
{
private:
	string error_;

public:
	explicit UserException(const string& error);

	const char* what() const override;
};

class User
{
private:
	const string login_;
	uint hashPassword_;
	const string salt_;
	string name_;

public:
	User(const string& login, const uint& hashPassword, const string& salt);
	User(const string& login, const uint& hashPassword, const string& salt, const string& name);

	~User() = default;

	const string& getLogin() const;
	const string& getSalt() const;

	const uint& getHashPassword() const;
	void setHashPassword(const uint& newHashPassword);

	const string& getName() const;
	void setName(const string& newName);

	friend ostream& operator<<(ostream& out, const User& user);
};



