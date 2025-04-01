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

struct User{
    string username;
    string password;
    string oldPassword;
    vector<string> locations;
    int socketNum;
    Inbox inbox;

    User(){}

    User(string newUsername, string newPassword, vector<string> newLocations){
        username = newUsername;
        password = newPassword;
        locations = newLocations;
    }

    string getLocations(){
        string result = "";
        if(locations.size() >= 1){
            for(int i = 0; i < locations.size() - 1; i++){
                result += locations.at(i) + ",";
            }
            result += locations.at(locations.size() - 1);
        }
        return result;
    }

};

#endif