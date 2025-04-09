/**
 * httpServer.cpp
 * @author Elijah Reyna
 * This file contains all the code to start a server, as well as to receive and process responses from a client
 */

#include <sstream>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <chrono>
#include <thread>
#include <cstring>
#include <unistd.h>
#include "user.cpp"
#include "utils.cpp"
#include "serverUtils.cpp"
using namespace std;

/**
 * Attempts to login the client using the credentials given
 * @param currUser *User A pointer to the currUser
 * @param credentials vector<string> The credentials to try logging in with
 * @returns string Message for the client
 */
string login(User *currUser, vector<string> credentials){
    //Sets the currUser variables
    currUser->username = credentials.at(0);
    currUser->password = credentials.at(1);

    cout << "[SERVER] - Checking credentials: " << currUser->username << "," << currUser->password  << endl;

    //Checks for the username and password in the user database (text file)
    bool matchFound = ServerUtils::checkUserDataForCredentials(currUser->username, currUser->password);

    if(matchFound){
        ServerUtils::setUpCurrUser(currUser, credentials.at(0), credentials.at(1));
        return "login-request: successful";
    }else{
        return "login-request: failure";
    }
}


/**
 * Attempts to register the client using the credentials given
 * @param currUser *User A pointer to the currUser
 * @param credentials vector<string> The credentials to try registering in with
 * @returns string Message for the client
 */
string registerUser(User *currUser, vector<string> credentials){
    string username = credentials.at(0);
    string password = credentials.at(1);

    cout << "[SERVER] - Attempting to register user: " << username << ", " << password << endl;

    bool matchFound = ServerUtils::checkUserDataForCredentials(username, password);

    if(matchFound){
        return "register-request: failure account_already_exists";
    }else{
        ServerUtils::appendUserData(username, password, "users.txt");
        return "register-request: success";
    }
}

/**
 * Attempts to change the password
 * @param currUser *User A pointer to the currUser
 * @param newPassword string The new password
 * @returns string Message for the client
 */
string changePassword(User *currUser, string newPassword){
        cout << "[SERVER] - Updating password for user " << currUser->username << ": " << currUser->password << " -> " << newPassword << endl;

        // Open the file for reading
        ifstream inFile("users.txt");
        if (!inFile.is_open()) {
            std::cerr << "Error: Unable to open file for reading." << std::endl;
            return "save-user: failure server_error";
        }

        //Pulls the lines of data and rewrites on of them
        vector<string> lines = vector<string>(0);
        string line;
        while (getline(inFile, line)) {
            vector data = Utils::split(line, ' ');
    
            if(currUser->username == data.at(0) && currUser->password == data.at(1)){
                line = currUser->username + " " + newPassword + " [" + currUser->getLocations() + "]";
                currUser->password = newPassword;
            }
            lines.push_back(line);
        }
        inFile.close();

        // Rewrites the file contents
        ofstream userData("users.txt");

        // Check if the file is open
        if (!userData.is_open()) {
            std::cerr << "Error: Unable to open file for rewriting" << std::endl;
            return "password-change-request: failure server_error";
        }

        //Write the new user data to the file
        for(int i = 0; i < (int) lines.size(); i++){
            userData << lines.at(i) << "\n";
            cout << lines.at(i) << endl;
        }
        userData.close();

        return "password-change-request: success";
        // <3
}


/**
 * Attempts to unsubscribe from a location
 * @param currUser *User A pointer to the currUser
 * @param newPassword string The location to remove
 * @returns string Message for the client
 */
string unsubscribe(User *currUser, string location){
    cout << "[SERVER] - Unsubscribing user " << currUser->username << " from " << location << endl;

    //Finds and removes the location give
    for(int i = 0; i < (int) currUser->locations.size(); i++){
        if(currUser->locations.at(i) == location){
            currUser->locations.erase(currUser->locations.begin() + i);
            break;
        }
    }

    //Saves the new currUser information
    string result = ServerUtils::saveUserInformation(*currUser);

    if(result == "save-user: success"){
        return "unsubscribe-request: success";
    }else if(result == "unsubscribe-request: failure server_error"){
        return "save-user: failure server_error";
    }else{
        return "unsubscribe-request: failure server_error";
    } 
}

/**
 * Attempts to subscribe to a location
 * @param currUser *User A pointer to the currUser
 * @param newPassword string The location to add
 * @returns string Message for the client
 */
string subscribe(User *currUser, string location){
    //FIXME Check if location already exists first!


    cout << "[SERVER] - Subscribing user " << currUser->username << " to " << location << endl;

    // Adds a location
    currUser->locations.push_back(location);

    // Saves the new currUser information
    string result = ServerUtils::saveUserInformation(*currUser);

    if(result == "save-user: success"){
        return "subscribe-request: success";
    }else if(result == "save-user: failure server_error"){
        return "subscribe-request: failure server_error";
    }else{
        return "subscribe-request: failure server_error";
    } 
}


/**
 * Pulls the locations the user is subscribed to
 * @param currUser *User A pointer to the currUsere
 * @returns string Message for the client
 */
string viewLocations(User *currUser){
    string result = "subscribed-locations=\n";
    for(int i = 0; i < (int) currUser->locations.size(); i++){
        result += " - " + currUser->locations.at(i) + "\n";
    }

    return result;
}


/**
 * Processes the client request
 * @return string representing the server response to the request
 */
string processRequest(char* buffer, User *currUser){
    //Gets the information from the buffer
    stringstream ss(buffer);
    string requestString;
    getline(ss, requestString, '\n');

    // Analyzes the type of request
    int requestType = Utils::requestType(requestString);

    // Performs actions depending on the request type
    if(requestType == Utils::LOGIN){
        return login(currUser, Utils::split(Utils::parseRequestContent(requestString, requestType), '+'));
    }
    else if(requestType == Utils::REGISTER){
        return registerUser(currUser, Utils::split(Utils::parseRequestContent(requestString, requestType), '+'));
    }
    else if(requestType == Utils::CHANGE_PASSWORD){
        return changePassword(currUser, Utils::parseRequestContent(requestString, requestType));
    }
    else if(requestType == Utils::UNSUBSCRIBE_LOCATION){
        return unsubscribe(currUser, Utils::parseRequestContent(requestString, requestType));
    }
    else if(requestType == Utils::SUBSCRIBE_LOCATION){
        return subscribe(currUser, Utils::parseRequestContent(requestString, requestType));
    }
    else if(requestType == Utils::VIEW_LOCATIONS){
        return viewLocations(currUser);
    }
    return "server_error";
}


void handle_client(int tcp_client_socket){
    User *currUser = new User();
    currUser->socketNum = tcp_client_socket;

    //Set initial status
    string status = "wait-read";\
    while(status == "wait-read"){
        //Set up the buffer
        char buffer[1024] = { 0 };
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(tcp_client_socket, buffer, sizeof(buffer), 0);
        if (bytesReceived < 0) {
            cout << "[SERVER] - Error receiving data" << endl;
            break;
        } else if (bytesReceived == 0) {
            cout << "[SERVER] - Client disconnected" << endl;
            break;
        }

        //Received a request
        cout << "[SERVER] - Received request" << endl;

        string response = processRequest(buffer, currUser);

        if(response == "Client requested disconnect. Disconnecting..."){
            cout << "[SERVER] - " << response << endl;
            status = "close";
        }
        
        send(tcp_client_socket, response.c_str(), response.length(), 0);
        memset(buffer, 0, sizeof(buffer));
    }
};


int main(){
    vector<User> users = vector<User>(0);

    //WSADATA setup
    #if _WIN32 
        WSADATA wsaData;
    if(!ServerUtils::initializeWSA(wsaData)){
        return -1;
    }

    #endif

    //Socket setup
    int tcp_server_socket = ServerUtils::createServerSocket();
    if(tcp_server_socket < 0){
        cout << "[SERVER] - Could not create socket" << endl;
        return -1;
    }

    //Server Address setup
    struct sockaddr_in tcp_server_address = ServerUtils::defineServerAddress();
    if(!ServerUtils::bindServerSocket(tcp_server_socket, tcp_server_address)){
        return -1;
    }

    if(!ServerUtils::checkIfListening(tcp_server_socket)){
        return -1;
    }
    
    // Receive and send data
    int tcp_client_socket;

    while (true) {
        printf("\n+++++++ Waiting for new connection ++++++++\n\n");

        //Accept the socket
        tcp_client_socket = accept(tcp_server_socket, NULL, NULL);
        while(tcp_client_socket < 0){
            tcp_client_socket = accept(tcp_server_socket, NULL, NULL);
        }

         // Create a new thread to handle the client
        std::thread client_thread(handle_client, tcp_client_socket);

         // Detach the thread to allow it to run independently
        client_thread.detach(); 
    }
    
    //Close the socket
    close(tcp_server_socket);
    
    #if WIN_32
        WSACleanup();
    #endif
    
    cout << "[SERVER] - Server about to close" << endl;
    return 0;
}