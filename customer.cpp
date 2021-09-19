#include <iostream>
#include <sqlite3.h>
#include <string>
#include <vector>

#if defined(__unix) || defined(__unix__) || defined(__linux__) || (defined(__APPLE__) && defined(__MACH__))
    #include <termios.h>
#endif

struct bookinfo {
    std::string name;
    int price;
};

bool isnum(std::string string) {
    for (int i = 0; i < string.size(); i++) {
        if (!(string[i] >= '0' && string[i] <= '9')) return false;
    }
    return true;
}

sqlite3* db;
sqlite3_stmt* statement;
std::vector<bookinfo> bought;
uintmax_t identifier; /* Who knows how many users a singular system may have at any given time? */
uintmax_t budget; /* Just in case Bill Gates wants to set his bank account to be his budget */
int menu();

void signup();
class login {
    private:
    int failedattempts;
    void passwordlogin(std::string username) {
        std::string password;

        sqlite3_prepare_v2(db, "select * from users where username=@username and password=@password", -1, &statement, NULL);
        sqlite3_bind_text(statement, sqlite3_bind_parameter_index(statement, "@username"), username.c_str(), username.size(), SQLITE_STATIC);

        termios def;
        tcgetattr(0, &def);
        termios n = def;
        n.c_lflag &= (~ECHO);

        std::cout << "Enter your password or type 's' to sign up:\n";

        tcsetattr(0, TCSANOW, &n);
        getline(std::cin, password);
        tcsetattr(0, TCSANOW, &def);
        if (password == "e") {
            sqlite3_close(db);
            return ;
        } else if (password  == "s") {
            sqlite3_reset(statement);
            sqlite3_clear_bindings(statement);

            system("clear");
            signup();
        } else {

            sqlite3_bind_text(statement, sqlite3_bind_parameter_index(statement, "@password"), password.c_str(), password.size(), SQLITE_STATIC);

            if (sqlite3_step(statement) != SQLITE_ROW) {
                failedattempts++;

                if (failedattempts == 3) {
                    std::cout << "Max amount of login attempts reached!\nPress enter to exit the program\n";
                    std::cin.get();

                    sqlite3_close(db);
                    return ;
                } else {
                    std::cout << "Incorrect Password!\nPress enter to try again\n";
                    std::cin.get();
                    passwordlogin(username);
                }
            } else {
                identifier = sqlite3_column_int(statement, 2);

                std::cout << "Login successful!\nPress enter to continue\n";
                std::cin.get();

                system("clear");
                std::string budgetstr;
                std::cout << "Enter your budget:\n$ ";
                std::cin >> budgetstr;

                if (isnum(budgetstr)) {
                    budget = stoi(budgetstr);
                } else {
                    std::cout << "Budget must be a numerical value\nPress enter to quit the program and try again\n";
                    std::cin.ignore();
                    std::cin.get();

                    sqlite3_close(db);
                    return ;
                }
                menu();
            }
        }
    }
    public:

    login() {
        failedattempts = 0;
    }

    void usernamelogin(bool stdignore = true) {
        system("clear");
        std::string username;

        sqlite3_prepare_v2(db, "select * from users where username=@username", -1, &statement, NULL);

        std::cout << "Enter your username or type 's' to sign up:\n";
        if (stdignore) { std::cin.ignore(); } /* Works fine on first time but ignores first character once function is repeated, creating false negatives */
        getline(std::cin, username);
        if (username == "e") {
            sqlite3_close(db);
            return ;
        } else if (username == "s") {
            sqlite3_reset(statement);
            sqlite3_clear_bindings(statement);

            system("clear");
            signup();
        } else {
            sqlite3_bind_text(statement, sqlite3_bind_parameter_index(statement, "@username"), username.c_str(), username.size(), SQLITE_STATIC);

            if (sqlite3_step(statement) == SQLITE_DONE) {
                sqlite3_reset(statement);
                sqlite3_clear_bindings(statement);

                std::cout << "User doesn't exist!\nPress enter to try again\n";
                std::cin.get();
                usernamelogin(false);
            } else {
                sqlite3_reset(statement);
                sqlite3_clear_bindings(statement);
                passwordlogin(username);
            }
        }
    }
};

void signup() {
    std::srand(time(nullptr));

    std::string username;
    std::string password;
    std::string confirm;

    int identifier_signup;
    int limiter = 999;
    int tries = 1;

    std::cout << "Enter a username:\n";
    std::cin.ignore();
    getline(std::cin, username);
    if (username == "e") { sqlite3_close(db); return ; }

    termios def;
    tcgetattr(0, &def);
    termios n = def;
    n.c_lflag &= (~ECHO);

    std::cout << "Enter a password:\n";
    tcsetattr(0, TCSANOW, &n);
    getline(std::cin, password);
    tcsetattr(0, TCSANOW, &def);
    if (password == "e") { sqlite3_close(db); return ; }

    std::cout << "Repeat the password:\n";
    tcsetattr(0, TCSANOW, &n);
    getline(std::cin, confirm);
    tcsetattr(0, TCSANOW, &def);
    if (confirm == "e") { sqlite3_close(db); return ; }

    while (confirm != password) {
        system("clear");
        std::cout << "Enter a password:\n";
        tcsetattr(0, TCSANOW, &n);
        getline(std::cin, password);
        tcsetattr(0, TCSANOW, &def);
        if (password == "e") { sqlite3_close(db); return ; }

        std::cout << "Repeat the password:\n";
        tcsetattr(0, TCSANOW, &n);
        getline(std::cin, confirm);
        tcsetattr(0, TCSANOW, &def);
        if (confirm == "e") { sqlite3_close(db); return ; }
    }

    identifier_signup = rand() % limiter;
    sqlite3_prepare_v2(db, "select * from users where identifier=@identifier", -1, &statement, NULL);
    sqlite3_bind_int(statement, sqlite3_bind_parameter_index(statement, "@identifier"), identifier_signup);

    while (sqlite3_step(statement) == SQLITE_ROW) {
        sqlite3_reset(statement);
        sqlite3_clear_bindings(statement);

        identifier_signup = rand() % limiter;
        tries++;

        if (tries > 999) {
            limiter += 1000;
            tries = 0;
        }

        sqlite3_prepare_v2(db, "select * from users where identifier=@identifier", -1, &statement, NULL);
        sqlite3_bind_int(statement, sqlite3_bind_parameter_index(statement, "@identifier"), identifier_signup);
    }
    sqlite3_reset(statement);
    sqlite3_clear_bindings(statement);

    sqlite3_prepare_v2(db, "insert into users values(@username, @password, @identifier)", -1, &statement, NULL);
    sqlite3_bind_text(statement, sqlite3_bind_parameter_index(statement, "@username"), username.c_str(), username.size(), SQLITE_STATIC);
    sqlite3_bind_text(statement, sqlite3_bind_parameter_index(statement, "@password"), password.c_str(), password.size(), SQLITE_STATIC);
    sqlite3_bind_int(statement, sqlite3_bind_parameter_index(statement, "@identifier"), identifier_signup);
    sqlite3_step(statement);

    sqlite3_reset(statement);
    sqlite3_clear_bindings(statement);

    identifier = identifier_signup;
    std::cout << "Account successfully created!\nPress enter to continue\n";
    std::cin.get();

    system("clear");
    std::string budgetstr;
    std::cout << "Enter your budget:\n$ ";
    std::cin >> budgetstr;

    if (isnum(budgetstr)) {
        budget = stoi(budgetstr);
    } else {
        std::cout << "Budget must be a numerical value\nPress enter to quit the program and try again\n";
        std::cin.ignore();
        std::cin.get();

        sqlite3_close(db);
        return ;
    }

    menu();
}

void b() {
    std::string name;
    int price;
    std::string file;

    std::cout << "Enter the book name:\n";
    std::cin.ignore();
    getline(std::cin, name);

    sqlite3_prepare_v2(db, "select * from books where name=@name", -1, &statement, NULL);
    sqlite3_bind_text(statement, sqlite3_bind_parameter_index(statement, "@name"), name.c_str(), name.size(), SQLITE_STATIC);
    if (sqlite3_step(statement) != SQLITE_ROW) {
        sqlite3_reset(statement);
        sqlite3_clear_bindings(statement);

        std::cout << "Book doesn't exist!\nPress enter to go back to the main menu\n";
        std::cin.get();
        menu();
    } else {
        price = sqlite3_column_int(statement, 1);
        file = (std::string)(char*)sqlite3_column_text(statement, 2);
    }
    sqlite3_reset(statement);
    sqlite3_clear_bindings(statement);

    if (!(budget - price < 0)) {
        budget -= price;
        bookinfo book = {name, price};
        bought.push_back(book);

        sqlite3_prepare_v2(db, "insert into customerpurchasehistory values(@name, @price, @identifier)", -1, &statement, NULL);
        sqlite3_bind_text(statement, sqlite3_bind_parameter_index(statement, "@name"), name.c_str(), name.size(), SQLITE_STATIC);
        sqlite3_bind_int(statement, sqlite3_bind_parameter_index(statement, "@price"), price);
        sqlite3_bind_int(statement, sqlite3_bind_parameter_index(statement, "@identifier"), identifier);
        sqlite3_step(statement);

        sqlite3_reset(statement);
        sqlite3_clear_bindings(statement);

        std::cout << "Book bought!\nPress enter to go back to the main menu\n";
    } else {
        std::cout << "You do not have enough money to buy this book!\nPress enter to go back to the main menu\n";
    }
    std::cin.get();
    menu();
}

void v() {
    sqlite3_prepare_v2(db, "select * from books", -1, &statement, NULL);
    sqlite3_bind_int(statement, sqlite3_bind_parameter_index(statement, "@identifier"), identifier);

    if (sqlite3_step(statement) != SQLITE_ROW) {
        std::cout << "No books to show\n";
    } else {
        std::cout << sqlite3_column_text(statement, 0) << ", $" << sqlite3_column_int(statement, 1) << "\n";

        while (sqlite3_step(statement) == SQLITE_ROW) {
            std::cout << sqlite3_column_text(statement, 0) << ", $" << sqlite3_column_int(statement, 1) << "\n";
        }
    }
    sqlite3_reset(statement);

    std::cout << "Press enter to go back to the main menu\n";
    std::cin.ignore();
    std::cin.get();
    menu();
}

void vb() {
    if (bought.size() == 0) {
        std::cout << "No books to show\n";
    } else {
        int totalcost = 0;
        for (int i = 0; i != bought.size(); i++) {

            if (totalcost == 0) {
                totalcost = bought[i].price;
            } else {
                totalcost += bought[i].price;
            }

            std::cout << bought[i].name << ", $" << bought[i].price << "\n";
        }

        std::cout << "Total number of books bought: " << bought.size() << "\n";
        std::cout << "Total cost: $" << totalcost << "\n";
    }
    std::cout << "Press enter to go back to the main menu\n";
    std::cin.ignore();
    std::cin.get();
    menu();
}

void vab() {
    sqlite3_prepare_v2(db, "select * from customerpurchasehistory where identifier=@identifier", -1, &statement, NULL);
    sqlite3_bind_int(statement, sqlite3_bind_parameter_index(statement, "@identifier"), identifier);

    if (sqlite3_step(statement) != SQLITE_ROW) {
        std::cout << "No books to show\nPress enter to go back to the main menu\n";
    } else {
        int bookamount = 1;
        int totalprice = sqlite3_column_int(statement, 1);
        std::cout << sqlite3_column_text(statement, 0) << ", $" << sqlite3_column_int(statement, 1) << "\n";

        while (sqlite3_step(statement) == SQLITE_ROW) {
            bookamount++;
            totalprice += sqlite3_column_int(statement, 1);
            std::cout << sqlite3_column_text(statement, 0) << ", $" << sqlite3_column_int(statement, 1) << "\n";
        }
        std::cout << "Total books: " << bookamount << "\n";
        std::cout << "Total cost: $" << totalprice << "\n";
        std::cout << "Press enter to go back to the main menu\n";
    }
    std::cin.ignore();
    std::cin.get();
    menu();
}

int menu() {
    system("clear");
    std::string in;

    if (budget < 0) {
        budget = 0;
    }

    std::cout << "Budget: $" << budget << "\n";
    std::cout << "========\n";
    std::cout << "Bookshop\n";
    std::cout << "========\n";
    std::cout << "Commands:\n";
    std::cout << "'b' to buy a book\n";
    std::cout << "'v' to view all of the books\n";
    std::cout << "'vb' to view your purchase history of this session\n";
    std::cout << "'vab' to view your full purchase history\n";
    std::cout << "'e' to exit the program\n";
    std::cin >> in;
    system("clear");

    if (in == "b") {
        b();
    } else if (in == "v") {
        v();
    } else if (in == "vb") {
        vb();
    }  else if (in == "vab") {
        vab();
    } else if (in == "e") {
        sqlite3_close(db);
        return 0;
    } else {
        std::cout << "Invalid input\nPress enter to go back to the main menu\n";
        std::cin.ignore();
        std::cin.get();
        menu();
    }

    sqlite3_close(db);
    return -1;
}

int main() {
    system("clear");

    #if !(__has_include(<termios.h>))
        std::cout << "Your Operating System is not compatible with this Software\n";
        return -1;
    #endif

    std::string in;
    sqlite3_open(".bookstore.db", &db);

    if (sqlite3_exec(db, "select count(*) from books", 0, NULL, NULL) != SQLITE_DONE && strcmp(sqlite3_errmsg(db), "no such table: books") == 0) {
        std::cout << "The bookshop has not been fully set up yet!\nPress enter to exit the program\n";
        std::cin.get();
        return -1;
    }

    if (sqlite3_exec(db, "select count(*) from customerpurchasehistory", 0, NULL, NULL) != SQLITE_OK && strcmp(sqlite3_errmsg(db), "no such table: customerpurchasehistory") == 0) {
        sqlite3_exec(db, "create table customerpurchasehistory(name text not null, price int not null, identifier int not null)", 0, NULL, NULL);
    }

    if (sqlite3_exec(db, "select count(*) from users", 0, NULL, NULL) != SQLITE_OK && strcmp(sqlite3_errmsg(db), "no such table: users") == 0) {
        sqlite3_exec(db, "create table users(username text not null, password text not null, identifier int not null)", 0, NULL, NULL);
    }

    login l;
    std::cout << "To use this software you must have an account\nTo log in type 'l' and to sign up, type 's'\n";
    std::cin >> in;
    system("clear");

    if (in == "e") {
        return 0;
    } else if (in == "l") {
        l.usernamelogin();
    } else if (in == "s") {
        signup();
    } else {
        while (in != "l" && in != "s") {
            std::cout << "Invalid input\nTo use this software you must have an account\nTo log in type 'l' and to sign up, type 's'\n";
            std::cin >> in;
            system("clear");
        }
        if (in == "l") {
            l.usernamelogin();
        } else {
            signup();
        }
    }
}
