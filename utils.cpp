#ifndef UTILS_CPP
#define UTILS_CPP

#include <iostream>
#include <vector>
using namespace std;
class Utils{
    public:
        static string REGISTER_REQUEST;
        static string LOGIN_REQUEST;
        static string CHANGE_PASSWORD_REQUEST;
        static string VIEW_LOCATIONS_REQUEST;
        static string SUBSCRIBE_LOCATION_REQUEST;
        static string UNSUBSCRIBE_LOCATION_REQUEST;
        static string LOCATION_MESSAGE_REQUEST;
        static string LIST_ONLINE_USERS_REQUEST;
        static string PRIVATE_MESSAGE_REQUEST;
        static string VIEW_MESSAGES_REQUEST;
        static string LOGOUT_REQUEST;

        static const int REGISTER = -1;
        static const int LOGIN = 0;
        static const int CHANGE_PASSWORD = 1;
        static const int VIEW_LOCATIONS = 2;
        static const int SUBSCRIBE_LOCATION = 3;
        static const int UNSUBSCRIBE_LOCATION = 4;
        static const int LOCATION_MESSAGE = 5;
        static const int LIST_ONLINE_USERS = 6;
        static const int PRIVATE_MESSAGE = 7;
        static const int VIEW_MESSAGES = 8;
        static const int LOGOUT = 9;

        Utils(){ 
        }

        static int requestType(string request){
            if(request.find(REGISTER_REQUEST) != string::npos) return REGISTER;
            else if(request.find(LOGIN_REQUEST) != string::npos) return LOGIN;
            else if(request.find(CHANGE_PASSWORD_REQUEST) != string::npos) return CHANGE_PASSWORD;
            else if(request.find(VIEW_LOCATIONS_REQUEST) != string::npos) return VIEW_LOCATIONS;
            else if(request.find(UNSUBSCRIBE_LOCATION_REQUEST) != string::npos) return UNSUBSCRIBE_LOCATION;
            else if(request.find(SUBSCRIBE_LOCATION_REQUEST) != string::npos) return SUBSCRIBE_LOCATION;
            else if(request.find(LOCATION_MESSAGE_REQUEST) != string::npos) return LOCATION_MESSAGE;
            else if(request.find(LIST_ONLINE_USERS_REQUEST) != string::npos) return LIST_ONLINE_USERS;
            else if(request.find(PRIVATE_MESSAGE_REQUEST) != string::npos) return PRIVATE_MESSAGE;
            else if(request.find(VIEW_MESSAGES_REQUEST) != string::npos) return VIEW_MESSAGES;
            else if (request.find(LOGOUT_REQUEST) != string::npos) return LOGOUT;
            else return -5;
        }

        static string getRequestString(int requestType){
            if(requestType == REGISTER) return REGISTER_REQUEST;
            else if(requestType == LOGIN) return LOGIN_REQUEST;
            else if(requestType == CHANGE_PASSWORD) return CHANGE_PASSWORD_REQUEST;
            else if(requestType == VIEW_LOCATIONS) return VIEW_LOCATIONS_REQUEST;
            else if(requestType == UNSUBSCRIBE_LOCATION) return UNSUBSCRIBE_LOCATION_REQUEST;
            else if(requestType == SUBSCRIBE_LOCATION) return SUBSCRIBE_LOCATION_REQUEST;
            else if(requestType == LOCATION_MESSAGE) return LOCATION_MESSAGE_REQUEST;
            else if(requestType == LIST_ONLINE_USERS) return LIST_ONLINE_USERS_REQUEST;
            else if(requestType == VIEW_MESSAGES) return VIEW_MESSAGES_REQUEST;
            else if(requestType == LOGOUT) return LOGOUT_REQUEST;
            else return "NONE";
        }

        static string parseRequestContent(string request, int requestType){
                return request.substr(request.find(getRequestString(requestType)) + getRequestString(requestType).length());
        }

        static vector<string> split(const string &str, char delimiter) {
            vector<string> tokens;
            size_t start = 0;
            size_t end = str.find(delimiter);
        
            while (end != std::string::npos) {
                tokens.push_back(str.substr(start, end - start));
                start = end + 1;
                end = str.find(delimiter, start);
            }
            
            tokens.push_back(str.substr(start));
        
            return tokens;
        }
};

string Utils::REGISTER_REQUEST = "register=";
string Utils::LOGIN_REQUEST = "login=";
string Utils::CHANGE_PASSWORD_REQUEST = "password-change=";
string Utils::VIEW_LOCATIONS_REQUEST = "view-locations";
string Utils::SUBSCRIBE_LOCATION_REQUEST = "subscribe=";
string Utils::UNSUBSCRIBE_LOCATION_REQUEST = "unsubscribe=";
string Utils::LOCATION_MESSAGE_REQUEST = "locations-message";
string Utils::LIST_ONLINE_USERS_REQUEST = "list-online-users";
string Utils::PRIVATE_MESSAGE_REQUEST = "private-message";
string Utils::VIEW_MESSAGES_REQUEST = "view-last-messages";
string Utils::LOGOUT_REQUEST = "logout";

#endif