#!/bin/bash

g++ bookstore.cpp -l sqlite3 -o store
g++ customer.cpp -l sqlite3 -o customer
