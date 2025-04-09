/**
 * httpClient.cpp
 * @author Elijah Reyna
 * httpClient.cpp controls the client functionality
 * It has some helper functions, and performs tcp protocol
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
using namespace std;

#define SERVER_IP "127.0.0.1"
#define PORT 8080

string sendRequest(string request, int connectSocket){
    string response = "";
    const char* message = request.c_str();
    send(connectSocket, message, strlen(message), 0);

    //Waits for a response
    char recvbuf[1024];
    int result = recv(connectSocket, recvbuf, sizeof(recvbuf), 0);
    if(result > 0) {   //If a response was received, display it
        recvbuf[result] = 0;
        string result = recvbuf;
        //cout << "[CLIENT] - Received response:" << endl;
        //cout << result << endl;

        response = result;
    }else if(result == 0) {
        cout << "[CLIENT] - Connection closed\n";
        response = "connection_closed";
    }else{
        #if WIN_32 
            cout << "[CLIENT] - recv failed: " << WSAGetLastError() << endl;
        #endif

        response = "receive_failed";
    }
    return response;
}

/**
 * Logs the user in
 * @returns bool representing if the login was successful
 */
bool login(int connectSocket){
    cout << "\nPlease input your username: ";
    string username;
    cin >> username;

    cout << "Please input your password: ";
    string password;
    cin >> password;

    string userRequest = "login=" + username + "+" + password;

    string response = sendRequest(userRequest, connectSocket);

    if(response == "login-request: successful"){
        cout << "Login successful" << endl;
        return true;
    }else if(response == "login-request: failure"){
        cout << "Login unsuccessful" << endl;
        return false;
    }else{
        cout << "[CLIENT] - An error ocurred" << endl;
        return false;
    }
}

/**
 * Registers a new user
 * @returns bool representing if the user already exists
 */
bool registerUser(int connectSocket){
    cout << "\nPlease input your desired username: ";
    string username;
    cin >> username;

    cout << "Please input your desired password: ";
    string password;
    cin >> password;

    // Input sanitization would occur here

    string userRequest = "register=" + username + "+" + password;

    string response = sendRequest(userRequest, connectSocket);

    if(response == "register-request: success"){
        cout << "Registration successful" << endl;
        return true;
    }else if(response == "register-request: failure account_already_exists"){
        cout << "Account already exists" << endl;
        return false;
    }else if(response == "register-request: failure server_error"){
        cout << "Registration unsuccessful, try again later" << endl;
        return false;
    }else{
        cout << "[CLIENT] - An error ocurred" << endl;
        return false;
    }
}


bool changePassword(int connectSocket){
    cout << "New password: ";
    string newPassword;
    cin >> newPassword;

    cout << "Trying to change to " << newPassword << endl;

    string userRequest = "password-change=" + newPassword;

    string response = sendRequest(userRequest, connectSocket);

    if(response == "password-change-request: success"){
        cout << "Password change successful" << endl;
        return true;
    }else if(response == "password-change-request: failure server_error"){
        cout << "Password unsuccessful, try again later" << endl;
        return false;
    }else{
        cout << "[CLIENT] - An error ocurred" << endl;
        return false;
    }
}


bool subscribeToLocation(int connectSocket){
    cout << "Type the location you would like to subscribe to: ";
    string location;
    cin >> location;

    cout << "Received: " << location << endl;

    string userRequest = "subscribe=" + location;

    string response = sendRequest(userRequest, connectSocket);

    if(response == "subscribe-request: success"){
        cout << "Location subscription successful" << endl;
        return true;
    }else if(response == "subscribe-request: failure server_error"){
        cout << "Subscription request failed due to a server error" << endl;
        return false;
    }else{
        cout << "Subscription request failed due to a server error" << endl;
        return false;
    } 
}

bool unsubscribeFromLocation(int connectSocket){
    cout << "Type the location you would like to unsubscribe from: ";
    string location;
    cin >> location;

    cout << "Received: " << location << endl;

    string userRequest = "unsubscribe=" + location;

    string response = sendRequest(userRequest, connectSocket);

    if(response == "unsubscribe-request: success"){
        cout << "Location subscription successful" << endl;
        return true;
    }else if(response == "unsubscribe-request: failure server_error"){
        cout << "Unsubscription request failed due to a server error" << endl;
        return false;
    }else{
        cout << "Unsubscription request failed due to a server error" << endl;
        return false;
    } 
}

bool viewLocations(int connectSocket){
    string userRequest = "view-locations";

    string response = sendRequest(userRequest, connectSocket);

    response = response.substr(22);

    cout << "Subscribed Locations: \n";
    cout << response << endl;

    return true;
}


/**
 * Main function
 */
int main() {
    int sockfd;
    struct sockaddr_in serv_addr;

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    // Set up server address
    memset(&serv_addr, '0', sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080); // Server port
    serv_addr.sin_addr.s_addr = INADDR_ANY; // Or use inet_pton to convert IP address string

    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        exit(1);
    }


    bool loop = true;
    bool loggedIn = false;
    do{
        cout << "\nYou may:" << endl;
        cout << "  1. Login" << endl;
        cout << "  2. Register" << endl;
        cout << "  3. Logout" << endl;
        cout << "Please type what you would like to do: ";
        string userInput;
        cin >> userInput;
        
        //Converts the string to lowercase
        for(int i = 0; i < (int) userInput.length(); i++){
            userInput[i] = tolower(userInput[i]);
        }

        if(userInput == "login" || userInput == "1"){
            loggedIn = login(sockfd);
            if(loggedIn){
                loop = false;
            }
        }
        if(userInput == "register" || userInput == "2"){
            registerUser(sockfd);
        }
        if(userInput == "logout" || userInput == "3"){
            cout << "Logging out" << endl;
            loop = false;
        }

    }while(loop);


    while(loggedIn){
        string CHANGE_PASSWORD = "Change password";
        string VIEW_LOCATIONS = "View subscribed locations";
        string SUBSCRIBE_LOCATION = "Subscribe to a location";
        string UNSUBSCRIBE_LOCATION = "Unsubscribe to a location";
        string LOCATION_MESSAGE = "Send a message to a location";
        string LIST_ONLINE_USERS = "See all online users";
        string PRIVATE_MESSAGE = "Send a private message";
        string VIEW_MESSAGES = "See my last 10 messages";
        string LOGOUT = "Logout";

        cout << "\nYou may:" << endl;
        cout << "  1. " << SUBSCRIBE_LOCATION << endl;
        cout << "  2. " << UNSUBSCRIBE_LOCATION << endl;
        cout << "  3. " << LIST_ONLINE_USERS << endl;
        cout << "  4. " << PRIVATE_MESSAGE << endl;
        cout << "  5. " << LOCATION_MESSAGE << endl;
        cout << "  6. " << VIEW_LOCATIONS << endl;
        cout << "  7. " << VIEW_MESSAGES << endl;
        cout << "  8. " << CHANGE_PASSWORD << endl;
        cout << "  9. " << LOGOUT << endl;
        cout << "Please type what you would like to do: ";
        string userInput;
        cin >> userInput;
        
        //Converts the string to lowercase
        for(int i = 0; i < (int) userInput.length(); i++){
            userInput[i] = tolower(userInput[i]);
        }

        if(userInput == SUBSCRIBE_LOCATION || userInput == "1"){
            subscribeToLocation(sockfd);
        }
        if(userInput == UNSUBSCRIBE_LOCATION || userInput == "2"){
            unsubscribeFromLocation(sockfd);
        }
        if(userInput == LIST_ONLINE_USERS || userInput == "3"){
        }
        if(userInput == PRIVATE_MESSAGE || userInput == "4"){
        }
        if(userInput == LOCATION_MESSAGE || userInput == "5"){
        }
        if(userInput == VIEW_LOCATIONS || userInput == "6"){
            viewLocations(sockfd);
        }
        if(userInput == VIEW_MESSAGES || userInput == "7"){
        }
        if(userInput == CHANGE_PASSWORD || userInput == "8"){
            changePassword(sockfd);
        }
        if(userInput == LOGOUT || userInput == "9"){
            loggedIn = false;
        }
    }

    //Closes the program when the user enters "close"
    cout << "[CLIENT] - Closing client program..." << endl;
    close(sockfd);

    return 0;
}