/**
 * 
 * 
 * 
 */
#ifndef USER_CPP
#define USER_CPP
#include <iostream>
#include <string>
#include <vector>
#include "Inbox.cpp"
using namespace std;

class User{
    string username;
    vector<string> locations;
    int socketNum;
    Inbox inbox;


};

#endif