#pragma once

#include "User.h"
#include "Message.h"
#include "sha1.h"
#include <unordered_map>
#include <vector>
#include <memory>
#include <random>
#include <chrono>
#include <algorithm>
#include <conio.h>
using namespace std;

class Chat
{
private:
	const size_t cntAttempts = 3; // count of attempts to Sign In
	const size_t minPswLen = 5; // minimum password length
	const size_t maxPswLen = 20; // maximum password length

	bool isWork_{ false };
	bool isToolsMenu_{ false };
	unordered_map<string, shared_ptr<User>> users_; // key - login
	vector<shared_ptr<Message>> messages_;
	shared_ptr<User> ptr_currentUser_ = nullptr;

	void setPtrCurrentUser(shared_ptr<User> ptr_currentUser);

	shared_ptr<User> getPtrUserByLogin(const string& login) const;
	shared_ptr<User> getPtrUserByName(const string& name) const;

	bool isAlphaNumeric(const string &str);
	void checkNewLogin(const string& login);
	void checkNewPassword(const string& psw);
	void checkLogin(const string& login, const string& psw);

	string passwordField();

	uint createHashPassword(const string& psw);
	string createSalt();

	void signUp();
	void signIn();
	void Exit();

	void showAllUsers();
	void showAllMessages();
	void sendMessage();

	void changePassword();
	void changeName();

public:
	Chat();
	~Chat() = default;

	bool getIsWork() const;
	bool getIsToolsMenu() const;
	shared_ptr<User> getPtrCurrentUser() const;
	void chatMenu();
	void userMenu();
	void toolsMenu();
};


