#include "Chat.h"
#define NOMINMAX
#include <windows.h>

// Colors for the console
const size_t colorWhite = 7;
const size_t colorLightBlue = 11;
const size_t colorLightGreen = 10;
const size_t colorYellow = 14;
const size_t colorDarkBlue = 19;

// Declare and initialize variables to change the color of text in the console
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
size_t concolColor = colorWhite;

// Reserve place for users in unordered_map
const size_t countChatUsers = 200;
// Reserve place for massedes in vector
const size_t countMessages = 1000;

Chat::Chat()
{
	isWork_ = true;
	users_.reserve(countChatUsers);
	messages_.reserve(countMessages);
	string salt;
	salt = createSalt();
	users_["all"] = make_shared<User>(User{ static_cast<string>("all"), createHashPassword(static_cast<string>("allPswd") + salt), salt });
	// Add two users for easy testing
	salt = createSalt();
	users_["test1"] = make_shared<User>(User{ static_cast<string>("test1"), createHashPassword(static_cast<string>("t1Pswd") + salt), salt, static_cast<string>("TEST1") });
	salt = createSalt();
	users_["test2"] = make_shared<User>(User{ static_cast<string>("test2"), createHashPassword(static_cast<string>("t2Pswd") + salt), salt, static_cast<string>("TEST2") });
}

bool Chat::getIsWork() const { return isWork_; }
bool Chat::getIsToolsMenu() const { return isToolsMenu_; }

shared_ptr<User> Chat::getPtrCurrentUser() const { return ptr_currentUser_; }
void Chat::setPtrCurrentUser(shared_ptr<User> ptr_currentUser) { ptr_currentUser_ = ptr_currentUser; }

shared_ptr<User> Chat::getPtrUserByLogin(const string& login) const
{
	auto result = users_.find(login);
	if (result != end(users_)) {
		return result->second;
	}
	return nullptr;
}

shared_ptr<User> Chat::getPtrUserByName(const string& name) const
{
	for (const auto& u : users_)
	{
		if (u.second->getName() == name)
			return u.second;
	}
	return nullptr;
}

bool Chat::isAlphaNumeric(const string &str)
{
	auto it = find_if_not(str.begin(), str.end(), ::isalnum);
	return it == str.end();
}

void Chat::checkNewLogin(const string& login)
{
	if (!isAlphaNumeric(login))
		throw UserException("The login must contain letters and numbers only!\n");

	auto ptrUser = getPtrUserByLogin(login);
	if (ptrUser)
		throw UserException("This login already exists!\n");

	if (login == "all")
		throw UserException("This login is reserved!\n");
}

void Chat::checkNewPassword(const string& psw)
{
	if (psw.length() < minPswLen)
		throw UserException("\nPassword must be equal to or more than 5 characters long!\n");

	if (psw.length() > maxPswLen)
		throw UserException("\nPassword must not exceed 20 characters long!\n");

	const char* pattern = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789"
		"`~!@#$%^&*()_-+=|/[]{}:;',.<>\? \"";

	if (psw.find_first_not_of(pattern) != string::npos)
		throw UserException("\nPassword contains invalid characters!\n");
}

void Chat::checkLogin(const string& login, const string& psw)
{

	auto ptrUser = getPtrUserByLogin(login);
	if (!ptrUser)
		throw UserException("Invalid login! Try again, please.\n");

	if (ptrUser->getHashPassword() != createHashPassword(psw + ptrUser->getSalt()))
		throw UserException("Invalid password! Try again, please.\n");

	setPtrCurrentUser(ptrUser);
}

string Chat::passwordField()
{
	const size_t size_password = 25;
	char password[size_password] = { 0 };
	char ch;
	size_t k = 0;
	while (k < size_password - 1)
	{
		ch = _getch();
		password[k] = ch;
		if (password[k] == 13) {
			password[k] = '\0';
			break;
		}
		else if (password[k] == 8) {
			cout << "\b \b";
			--k;
		}
		else {
			++k;
			cout << '*';
		}
	}

	return string(password);
}

uint Chat::createHashPassword(const string& psw)
{
	// Convert string to char*[]
	size_t lenPsw = psw.length();
	char* userPsw = new char[lenPsw];
	memcpy(userPsw, psw.c_str(), lenPsw);

	// Hash password (including salt)
	uint* userHashPsw = sha1(userPsw, lenPsw);

	// Clear
	delete[] userPsw;

	return *userHashPsw;
}

string Chat::createSalt()
{
	// Using the computer’s internal clock to generate the seed.
	const auto p = std::chrono::system_clock::now() - std::chrono::hours(24);
	auto seed = p.time_since_epoch().count();
	// Quickly generating pseudo random integers between 1 and 2147483646
	default_random_engine randomEngine(seed);
	// Converting those random integers into the range [33, 126] such that they’re uniformly distributed.
	uniform_int_distribution<int> distribution(0x21, 0x7E);
	auto randomReadableCharacter = [&]() { return distribution(randomEngine); };

	size_t size = 10;
	string salt;
	generate_n(back_inserter(salt), size, randomReadableCharacter);

	return salt;
}

void Chat::signUp()
{
	string userLogin;
	string userPassword;
	string userName;

	do
	{
		try
		{
			// User sign in attempt 
			cout << "Login: ";
			cin >> userLogin;
			checkNewLogin(userLogin);

			cout << endl << "Password: ";
			userPassword = passwordField();
			checkNewPassword(userPassword);

			cout << endl << "Name (optional): ";
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			getline(cin, userName);
			cout << endl;

			if (userName.length() == 0)
				userName = userLogin;

			string salt = createSalt();
			auto ptrNewUser = make_shared<User>(User{ userLogin, createHashPassword(userPassword + salt), salt, userName });
			users_.insert(make_pair( userLogin, ptrNewUser ));
			setPtrCurrentUser(ptrNewUser);
		}
		catch (UserException& e)
		{
			ptr_currentUser_ = nullptr;
			cerr << e.what();
		}
	} while (ptr_currentUser_ == nullptr);

	cout << *ptr_currentUser_ << ", you have successfully registered!\n" << endl;
}

void Chat::signIn()
{
	string userLogin;
	string userPassword;
	size_t cntAttemptsLogin = 0;

	do
	{
		cout << "Login: ";
		cin >> userLogin;
		cout << endl << "Password: ";
		userPassword = passwordField();
		cout << endl;

		try
		{
			// User sign in attempt 
			++cntAttemptsLogin;
			checkLogin(userLogin, userPassword);
		}
		catch (UserException& e)
		{
			cerr << e.what();
		}
	} while (ptr_currentUser_ == nullptr && (cntAttemptsLogin < cntAttempts));

	if (ptr_currentUser_ == nullptr)
		cout << "You have made " << cntAttempts << " attempts! Sign Up, please.\n";
}

void Chat::Exit()
{
	cout << "Goodby, " << *ptr_currentUser_ << "!" << endl;
	if (ptr_currentUser_)
		ptr_currentUser_ = nullptr;
}

void Chat::showAllUsers()
{
	// change the color of the text
	concolColor = colorDarkBlue;
	SetConsoleTextAttribute(hConsole, concolColor);
	// ----------
	cout << "*****   USERS   *****" << endl;
	// change the color of the text
	concolColor = colorWhite;
	SetConsoleTextAttribute(hConsole, concolColor);
	// ----------
	for (auto& u : users_)
	{
		cout << u.second->getName() << ((u.second == ptr_currentUser_) ? " (*)" : "") << endl;
	}
}

void Chat::showAllMessages()
{
	// change the color of the text
	concolColor = colorDarkBlue;
	SetConsoleTextAttribute(hConsole, concolColor);
	// ----------
	cout << "*****   MESSAGES   *****" << endl;
	// change the color of the text
	concolColor = colorWhite;
	SetConsoleTextAttribute(hConsole, concolColor);
	// ----------
	string userLogin;
	for (auto& msg : messages_)
	{
		userLogin = ptr_currentUser_->getLogin();
		if (msg->getTo() == userLogin || (msg->getTo() == static_cast<string>("all") && msg->getFrom() != userLogin))
			cout << "->  " << *msg << endl;
		if (msg->getFrom() == userLogin)
			cout << "<-  " << *msg << endl;
	}
}

void Chat::sendMessage()
{
	string to{};
	string text{};

	cout << "To (name or all): ";
	cin >> to;
	cout << endl << "Text: ";
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
	getline(cin, text);
	cout << endl;

	try
	{
		auto ptrUser = getPtrUserByName(to);
		if (!ptrUser && to != "all")
			throw MessageException("Invalid user name (to)! Choose another user, please.\n");

		messages_.push_back(make_shared<Message>(Message{ ptr_currentUser_->getLogin(), ptrUser->getLogin(), text }));
	}
	catch (MessageException& e)
	{
		cerr << e.what();
	}
}

void Chat::changePassword()
{
	string newPassword;

	cout << "Enter new password: ";
	newPassword = passwordField();
	cout << endl;

	try
	{
		checkNewPassword(newPassword);
	}
	catch (UserException& e)
	{
		cerr << e.what();
	}

	ptr_currentUser_->setHashPassword(createHashPassword(newPassword + ptr_currentUser_->getSalt()));
}

void Chat::changeName()
{
	string newName;

	cout << "Enter new nick name: ";
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
	getline(cin, newName);
	cout << endl;

	ptr_currentUser_->setName(newName);
}

void Chat::chatMenu()
{
	int choiceSign = 1;

	// change the color of the text
	concolColor = colorYellow;
	SetConsoleTextAttribute(hConsole, concolColor);
	// ----------
	cout << string(47, '-') << endl;
	cout << "| Sign In (1) || Sign Up (2) || Stop chat (0) |" << endl;
	cout << string(47, '-') << endl;
	// change the color of the text
	concolColor = colorWhite;
	SetConsoleTextAttribute(hConsole, concolColor);
	// ----------
	cin >> choiceSign;

	switch (choiceSign)
	{
	case 1:
		signIn();
		break;
	case 2:
		signUp();
		break;
	case 0:
		isWork_ = false;
		break;
	default:
		cout << "Wrong choice!" << endl << endl;
		break;
	}
}

void Chat::userMenu()
{
	int choice = 1;

	// change the color of the text
	concolColor = colorLightBlue;
	SetConsoleTextAttribute(hConsole, concolColor);
	// ----------
	cout << string(74, '-') << endl;
	cout << "| Send Message (1) || Messages (2) || Users (3) || Tools (4) || Exit (0) |" << endl;
	cout << string(74, '-') << endl;
	// change the color of the text
	concolColor = colorWhite;
	SetConsoleTextAttribute(hConsole, concolColor);
	// ----------
	cin >> choice;

	switch (choice)
	{
	case 1:
		sendMessage();
		break;
	case 2:
		showAllMessages();
		break;
	case 3:
		showAllUsers();
		break;
	case 4:
		isToolsMenu_ = true;
		break;
	case 0:
		Exit();
		break;
	default:
		cout << "Wrong choice!" << endl << endl;
		break;
	}
}

void Chat::toolsMenu()
{
	int choice = 1;

	// change the color of the text
	concolColor = colorLightGreen;
	SetConsoleTextAttribute(hConsole, concolColor);
	// ----------
	cout << string(56, '-') << endl;
	cout << "| Change Name (1) || Change password (2) || Return (0) |" << endl;
	cout << string(56, '-') << endl;
	// change the color of the text
	concolColor = colorWhite;
	SetConsoleTextAttribute(hConsole, concolColor);
	// ----------
	cin >> choice;

	switch (choice)
	{
	case 1:
		changeName();
		break;
	case 2:
		changePassword();
		break;
	case 0:
		isToolsMenu_ = false;
		break;
	default:
		cout << "Wrong choice!" << endl << endl;
		break;
	}
}
