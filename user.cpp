/**
 * 
 * 
 * 
 */
#ifndef USER
#define USER
#include <iostream>
#include <string>
#include <vector>
#include "messageBox.cpp";
using namespace std;

class User{
    string username;
    vector<string> locations;
    int socketNum;
    MessageBox inbox;


};

#endif