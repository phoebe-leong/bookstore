# bookstore
Local bookstore management system. Includes both a program for the store itself and one for the customers.

## Overview & Functionality
### Book Management Program
With the book management program, users can add and remove books. They can also change the name and price of books at will.  
However, specific functionality such as removing all of the books, adding users, and removing users, are limited to the Managers.  
The Book Management Program permissions are split into two roles:

1. Employee
   - Users with this role will be able to add books, remove them, and change their data.
2. Manager
   - Users with this role will be able to do everything that the Employee can do, but they can also add users, remove users, and remove all of the books.  

When the Manager creates a new user, they can choose the user's username, password, and role.  
Any role inputted during this process that is not either 'Manager' or 'Employee' will not be accepted.  

### Bookstore Customer Program
With this piece of software, users can buy books from the store.  
At the start of the program, they are prompted to either sign up or log in with a pre-existing account.  
This is so that the user can view their full buying history if they desire.  

Each user is given a randomly generated identification code on sign up.  
The code is used as a differentiator to everyone else's buying history, as they are all stored in the same table.  

Users can buy books, view every book available, view their sessions's buying history, and view their full history if need be.  

### Similarities and links between the two
In both programs during most text input, users can enter 'e', which serves as a back button within the program.  
They both communicate to an SQL database, created by the Book Management Program if it does not exist.  
For reasons I need not express, I will advise users to not delete it.  

These programs are not perfect, please create an issue or a pull request if you find any bugs or feel the need to suggest any improvements.
