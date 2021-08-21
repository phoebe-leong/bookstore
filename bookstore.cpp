#include <iostream>
#include <string>
#include <sqlite3.h>

bool isnum(std::string string) {
    int amountofnumbers = 0;
    char numbers[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

    for (int i = 0; i < string.size(); i++) {
        for (int j = 0; j < sizeof(numbers) / sizeof(numbers[0]); j++) {
            if (string[i] == numbers[j]) {
                amountofnumbers++;
                break;
            }
        }
    }
    if (amountofnumbers == string.size()) {
        return true;
    }
    return false;
}

sqlite3* db;
sqlite3_stmt* statement;
std::string role;
int menu();

class login {
    private:
    int failedattempts;
    void passwordlogin(std::string username) {
        std::string password;

        sqlite3_prepare_v2(db, "select * from owners where username=@username and password=@password", -1, &statement, NULL);
        sqlite3_bind_text(statement, sqlite3_bind_parameter_index(statement, "@username"), username.c_str(), username.size(), SQLITE_STATIC);

        std::cout << "Enter your password:\n";
        getline(std::cin, password);
        if (password == "e") {
            sqlite3_close(db);
            return ; 
        }
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
            role = (std::string)(char*)sqlite3_column_text(statement, 2);

            std::cout << "Login successful!\nPress enter to continue\n";
            std::cin.get();
            menu();
        }
    }
    public:

    login() {
        failedattempts = 0;
    }

    void usernamelogin() {
        system("clear");
        std::string username;
 
        sqlite3_prepare_v2(db, "select * from owners where username=@username", -1, &statement, NULL);

        std::cout << "Enter your username:\n";
        getline(std::cin, username);
        if (username == "e") { 
            sqlite3_close(db);
            return ; 
        }
        sqlite3_bind_text(statement, sqlite3_bind_parameter_index(statement, "@username"), username.c_str(), username.size(), SQLITE_STATIC);

        if (sqlite3_step(statement) == SQLITE_DONE) {
            sqlite3_reset(statement);
            sqlite3_clear_bindings(statement);

            std::cout << "User doesn't exist!\nPress enter to try again\n";
            std::cin.get();
            usernamelogin();
        } else {
            sqlite3_reset(statement);
            sqlite3_clear_bindings(statement);
            passwordlogin(username);
        }
    }   
};

void a() {
    std::string name;
    std::string price;

    std::cout << "Enter the name of your book:\n";
    std::cin.ignore();
    getline(std::cin, name);

    sqlite3_prepare_v2(db, "select name from books where name=@name", -1, &statement, NULL);
    sqlite3_bind_text(statement, sqlite3_bind_parameter_index(statement, "@name"), name.c_str(), name.size(), SQLITE_STATIC);
    if (sqlite3_step(statement) == SQLITE_ROW) {
        sqlite3_reset(statement);
        sqlite3_clear_bindings(statement);

        std::cout << "Book already exists!\nPress enter to go back to the main menu\n";
        std::cin.get();
        menu();
    } else {
        sqlite3_reset(statement);
        sqlite3_clear_bindings(statement);

        std::cout << "Enter the price of your book:\n$ ";
        std::cin >> price;

        if (!isnum(price)) {
            std::cout << "Price must be a numeric value\nPress enter to go back to the main menu\n";
            std::cin.ignore();
            std::cin.get();
            menu();
        } else {
            sqlite3_prepare_v2(db, "insert into books values(@name, @price)", -1, &statement, NULL);
            sqlite3_bind_text(statement, sqlite3_bind_parameter_index(statement, "@name"), name.c_str(), name.size(), SQLITE_STATIC);
            sqlite3_bind_int(statement, sqlite3_bind_parameter_index(statement, "@price"), stoi(price));
            sqlite3_step(statement);

            sqlite3_reset(statement);
            sqlite3_clear_bindings(statement);

            std::cout << "Book added!\nPress enter to go back to the main menu\n";
            std::cin.ignore();
            std::cin.get();
            menu();
        }
    }
}

void r() {
    std::string name;

    std::cout << "Enter the name of your book:\n";
    std::cin.ignore();
    getline(std::cin, name);
    if (name == "e") {
        menu();
    } else {
        sqlite3_prepare_v2(db, "select * from books where name=@name", -1, &statement, NULL);
        sqlite3_bind_text(statement, sqlite3_bind_parameter_index(statement, "@name"), name.c_str(), name.size(), SQLITE_STATIC);
        if (sqlite3_step(statement) != SQLITE_ROW) {
            sqlite3_reset(statement);
            sqlite3_clear_bindings(statement);

            std::cout << "Book doesn't exist!\nPress enter to go back to the main menu\n";
            std::cin.ignore();
            std::cin.get();
            menu();
        }
        sqlite3_reset(statement);
        sqlite3_clear_bindings(statement);
    
        sqlite3_prepare_v2(db, "delete from books where name=@name", -1, &statement, NULL);
        sqlite3_bind_text(statement, sqlite3_bind_parameter_index(statement, "@name"), name.c_str(), name.size(), SQLITE_STATIC);
        sqlite3_step(statement);

        sqlite3_reset(statement);
        sqlite3_clear_bindings(statement);

        std::cout << "Book removed!\nPress enter to go back to the main menu\n";
        std::cin.get();
        menu();
    }
}

void p() {
    std::string name;
    std::string newprice;

    std::cout << "Enter the current name of your book:\n";
    std::cin.ignore();
    getline(std::cin, name);
    if (name == "e") {
        menu();
    } else {
        sqlite3_prepare_v2(db, "select * from books  where name=@name", -1, &statement, NULL);
        sqlite3_bind_text(statement, sqlite3_bind_parameter_index(statement, "@name"), name.c_str(), name.size(), SQLITE_STATIC);
        if (sqlite3_step(statement) != SQLITE_ROW) {
            sqlite3_reset(statement);
            sqlite3_clear_bindings(statement);

            std::cout << "Book doesn't exist!\nPress enter to go back to the main menu\n";
            std::cin.get();
            menu();
        }
        sqlite3_reset(statement);
        sqlite3_clear_bindings(statement);

        std::cout << "Enter the new price of your book:\n$ ";
        std::cin >> newprice;
        if (newprice == "e") {
            menu();
        }

        if (!isnum(newprice) && newprice != "e") {
            std::cout << "New price must be a numeric value\nPress enter to go back to the main menu\n";
            std::cin.ignore();
            std::cin.get();
            menu();
        } else if (newprice != "e") {

            sqlite3_prepare_v2(db, "update books set price=@newprice where name=@name", -1, &statement, NULL);
            sqlite3_bind_int(statement, sqlite3_bind_parameter_index(statement, "@newprice"), stoi(newprice));
            sqlite3_bind_text(statement, sqlite3_bind_parameter_index(statement, "@name"), name.c_str(), name.size(), SQLITE_STATIC);
            sqlite3_step(statement);

            sqlite3_reset(statement);
            sqlite3_clear_bindings(statement);

            std::cout << "Book price updated!\nPress enter to go back to the main menu\n";
            std::cin.ignore();
            std::cin.get();
            menu();
        }
    }
}

void n() {
    std::string name;
    std::string newname;

    std::cout << "Enter the current name of your book:\n";
    std::cin.ignore();
    getline(std::cin, name);
    if (name == "e") {
        menu();
    } else {

        sqlite3_prepare_v2(db, "select * from books where name=@name", -1, &statement, NULL);
        sqlite3_bind_text(statement, sqlite3_bind_parameter_index(statement, "@name"), name.c_str(), name.size(), SQLITE_STATIC);
        if (sqlite3_step(statement) != SQLITE_ROW) {
            sqlite3_reset(statement);
            sqlite3_clear_bindings(statement);

            std::cout << "Book doesn't exist!\nPress enter to go back to the main menu\n";
            std::cin.ignore();
            std::cin.get();
            menu();
        }
        sqlite3_reset(statement);
        sqlite3_clear_bindings(statement);

        std::cout << "Enter the new name of your book:\n";
        getline(std::cin, newname);
        if (newname == "e") {
            menu();
        } else {
            sqlite3_prepare_v2(db, "update books set name=@newname where name=@name", -1, &statement, NULL);
            sqlite3_bind_text(statement, sqlite3_bind_parameter_index(statement, "@newname"), newname.c_str(), newname.size(), SQLITE_STATIC);
            sqlite3_bind_text(statement, sqlite3_bind_parameter_index(statement, "@name"), name.c_str(), name.size(), SQLITE_STATIC);
            sqlite3_step(statement);

            sqlite3_reset(statement);
            sqlite3_clear_bindings(statement);

            std::cout << "Book name updated!\nPress enter to go back to the main menu\n";
            std::cin.get();
            menu();
        }
    }
}

void v() {
    sqlite3_prepare_v2(db, "select * from books", -1, &statement, NULL);

    if (sqlite3_step(statement) != SQLITE_ROW) {
        std::cout << "No books to display!\nPress enter to go back to the main menu\n";
        std::cin.ignore();
        std::cin.get();
        menu();
    } else {
        std::cout << sqlite3_column_text(statement, 0) << ", $" << sqlite3_column_int(statement, 1) << "\n";

        while (sqlite3_step(statement) == SQLITE_ROW) {
            std::cout << sqlite3_column_text(statement, 0) << ", $" << sqlite3_column_int(statement, 1) << "\n";
        }
        sqlite3_reset(statement);

        std::cout << "Press enter to go back to the main menu\n";
        std::cin.ignore();
        std::cin.get();
        menu();
    }
}

/* Manager only functions */
void w() {
    std::string yn;

    std::cout << "Are you sure you want to remove ALL of the books in the database? [y/n]\n";
    std::cin >> yn;

    if (yn == "y") {
        sqlite3_exec(db, "delete from books", 0, NULL, NULL);

        std::cout << "Books removed!\nPress enter to go back to the main menu\n";
        std::cin.ignore();
        std::cin.get();
        menu();
    } else if (yn == "n") {
        std::cout << "Press enter to go back to the main menu\n";
        std::cin.ignore();
        std::cin.get();
        menu();
    } else {
        std::cout << "Invalid input!\nPress enter to go back\n";
        std::cin.ignore();
        std::cin.get();
        w();
    }
}

void ao() {
    std::string username;
    std::string password;
    std::string assignedrole;

    sqlite3_prepare_v2(db, "select * from owners where username=@username and password=@password", -1, &statement, NULL);

    std::cout << "Enter a username:\n";
    std::cin.ignore();
    getline(std::cin, username);
    if (username == "e") {
        menu();
    } else {
        sqlite3_bind_text(statement, sqlite3_bind_parameter_index(statement, "@username"), username.c_str(), username.size(), SQLITE_STATIC);
        std::cout << "Enter a password:\n";
        getline(std::cin, password);
        sqlite3_bind_text(statement, sqlite3_bind_parameter_index(statement, "@password"), password.c_str(), password.size(), SQLITE_STATIC);

        if (sqlite3_step(statement) == SQLITE_ROW) {
            sqlite3_reset(statement);
            sqlite3_clear_bindings(statement);

            std::cout << "User already exists!\nPress enter to try again\n";
            std::cin.get();
            ao();
        } else {
            sqlite3_reset(statement);
            sqlite3_clear_bindings(statement);

            /* "e" works to exit mostly everywhere apart from the accounts role addition */
            std::cout << "Enter the accounts role (this will govern what the account can do and control):\n";
            std::cout << "Pick between 'Manager' and 'Employee'\n";
            std::cin >> assignedrole;

            if (islower(assignedrole[0]) == 0) {
                assignedrole[0] = toupper(assignedrole[0]);
            }

            while (assignedrole != "Manager" && assignedrole != "Employee") {
                std::cout << "Invalid role\n";
                std::cout << "Enter the accounts role (this will govern what the account can do and control):\n";
                std::cout << "Pick between 'Manager' and 'Employee'\n";
                std::cin >> assignedrole;

                if (islower(assignedrole[0]) == 0) {
                    assignedrole[0] = toupper(assignedrole[0]);
                }
                
            }
            sqlite3_prepare_v2(db, "insert into owners values(@username, @password, @role)", -1, &statement, NULL);
            sqlite3_bind_text(statement, sqlite3_bind_parameter_index(statement, "@username"), username.c_str(), username.size(), SQLITE_STATIC);
            sqlite3_bind_text(statement, sqlite3_bind_parameter_index(statement, "@password"), password.c_str(), password.size(), SQLITE_STATIC);
            sqlite3_bind_text(statement, sqlite3_bind_parameter_index(statement, "@role"), assignedrole.c_str(), assignedrole.size(), SQLITE_STATIC);
            sqlite3_step(statement);

            sqlite3_reset(statement);
            sqlite3_clear_bindings(statement);

            std::cout << "User added!\nPress enter to go back to the main menu\n";
            std::cin.ignore();
            std::cin.get();
            menu();
        }
    }
}

void ro(bool stdignore = true) {
    std::string username;
    sqlite3_prepare_v2(db, "select * from owners where username=@username", -1, &statement, NULL);

    std::cout << "Enter the username of the account you would like to remove:\n";
    if (stdignore) { std::cin.ignore(); }
    getline(std::cin, username);
    if (username == "e") {
        menu();
    } else {
        sqlite3_bind_text(statement, sqlite3_bind_parameter_index(statement, "@username"), username.c_str(), username.size(), SQLITE_STATIC);

        if (sqlite3_step(statement) == SQLITE_DONE) {
            sqlite3_reset(statement);
            sqlite3_clear_bindings(statement);

            std::cout << "User does not exist!\nPress enter to try again\n";
            std::cin.get();
            ro(false);
        } else {
            sqlite3_reset(statement);
            sqlite3_clear_bindings(statement);

            sqlite3_prepare_v2(db, "delete from owners where username=@username", -1, &statement, NULL);
            sqlite3_bind_text(statement, sqlite3_bind_parameter_index(statement, "@username"), username.c_str(), username.size(), SQLITE_STATIC);
            sqlite3_step(statement);

            sqlite3_reset(statement);
            sqlite3_clear_bindings(statement);

            std::cout << "User removed!\nPress enter to go back to the main menu\n";
            std::cin.get();
            menu();
        }
    }
}

void rao() {
    std::string yn;

    std::cout << "Are you sure you want to remove ALL of the owner accounts? [y/n] \n";
    std::cin >> yn;

    if (yn == "y") {
        sqlite3_exec(db, "delete from owners", 0, NULL, NULL);

        std::cout << "All owner accounts removed!\nPress enter to exit the program\n";
        std::cin.ignore();
        std::cin.get();

        sqlite3_close(db);
        return ;
    } else if (yn == "n") {
        std::cout << "Press enter to go back to the main menu\n";
        std::cin.get();
        menu();
    } else {
        std::cout << "Invalid input\nPress enter to try again\n";
        std::cin.get();
        rao();
    }
}


int menu() {
    system("clear");
    std::string in;

    std::cout << "===================\n";
    std::cout << "Bookshop Management\n";
    std::cout << "===================\n";
    std::cout << "Commands:\n";
    std::cout << "'a' to add a book\n";
    std::cout << "'r' to remove a book\n";
    std::cout << "'p' to change a book's price\n";
    std::cout << "'n' to change a book's name\n";
    std::cout << "'v' to view all the current books\n";

    if (role == "Manager") {
        std::cout << "\n'w' to remove all the books\n";
        std::cout << "'ao' to add an account\n";
        std::cout << "'ro' to remove an account\n";
        std::cout << "'rao' to remove all the owner accounts (including your own)\n";
    }

    std::cout << "'e' to exit the program\n";
    std::cin >> in;
    system("clear");

    if (in == "a") {
        a();
    } else if (in == "r") {
        r();
    } else if (in == "p") {
        p();
    } else if (in == "n") {
        n();
    } else if (in == "v") {
        v();
    } else if (role == "Manager" && in == "w") {
        w();
    } else if (role == "Manager" && in == "ao") {
        ao();
    } else if (role == "Manager" && in == "ro") {
        ro();
    } else if (role == "Manager" && in == "rao") {
        rao();
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
    sqlite3_open("bookstore.db", &db);
    system("clear");

    if (sqlite3_exec(db, "select count(*) from books", 0, NULL, NULL) != SQLITE_OK && strcmp(sqlite3_errmsg(db), "no such table: books") == 0) {
        sqlite3_exec(db, "create table books(name text not null, price int not null)", 0, NULL, NULL);
    }

    if (sqlite3_exec(db, "select count(*) from users", 0, NULL, NULL) != SQLITE_OK && strcmp(sqlite3_errmsg(db), "no such table: users") == 0) {
        sqlite3_exec(db, "create table users(username text not null, password text not null, identifier int not null)", 0, NULL, NULL);
    }

    if (sqlite3_exec(db, "select count(*) from owners", 0, NULL, NULL) != SQLITE_OK && strcmp(sqlite3_errmsg(db), "no such table: owners") == 0) {
        sqlite3_exec(db, "create table owners(username text not null, password text not null, role text not null)", 0, NULL, NULL);

        std::string username;
        std::string password;

        std::cout << "Press enter to create a manager account\n";
        std::cin.get();
        std::cout << "Enter a username:\n";
        getline(std::cin, username);
        std::cout << "Enter a password:\n";
        getline(std::cin, password);

        sqlite3_prepare_v2(db, "insert into owners values(@username, @password, \"Manager\")", -1, &statement, NULL);
        sqlite3_bind_text(statement, sqlite3_bind_parameter_index(statement, "@username"), username.c_str(), username.size(), SQLITE_STATIC);
        sqlite3_bind_text(statement, sqlite3_bind_parameter_index(statement, "@password"), password.c_str(), password.size(), SQLITE_STATIC);
        sqlite3_step(statement);
        sqlite3_reset(statement);

        role = "Manager";
        std::cout << "Account created!\nPress enter to continue\n";
        std::cin.get();
        menu();
    } else {
        sqlite3_prepare_v2(db, "select * from owners", -1, &statement, NULL);
        if (sqlite3_step(statement) == SQLITE_ROW) {
            sqlite3_reset(statement);

            login l;
            l.usernamelogin();
        } else {
            system("clear");
            sqlite3_reset(statement);

            std::string username;
            std::string password;

            sqlite3_prepare_v2(db, "insert into owners values(@username, @password, \"Manager\")", -1, &statement, NULL);

            std::cout << "Press enter to create a manager account\n";
            std::cin.get();
            std::cout << "Enter a username:\n";
            getline(std::cin, username);
            std::cout << "Enter a password:\n";
            getline(std::cin, password);

            sqlite3_bind_text(statement, sqlite3_bind_parameter_index(statement, "@username"), username.c_str(), username.size(), SQLITE_STATIC);
            sqlite3_bind_text(statement, sqlite3_bind_parameter_index(statement, "@password"), password.c_str(), password.size(), SQLITE_STATIC);
            sqlite3_step(statement);
            sqlite3_reset(statement);

            role = "Manager";
            std::cout << "Account created!\nPress enter to continue\n";
            std::cin.get();
            menu();
        }
    }
}