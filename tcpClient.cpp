/**
 * httpClient.cpp
 * @author Elijah Reyna
 * httpClient.cpp controls the client functionality
 * It has some helper functions, and performs tcp protocol
 */

#include <string>
#include <sstream>
#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <chrono>
#include <thread>
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
        cout << "[CLIENT] - recv failed: " << WSAGetLastError() << endl;
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
 * Parses the information from the request
 * @param argc int representing number of arguments given from the command line
 * @param argv pointer array representing the arguments to process
 * @returns string representing the content to process
 */
string parseContent(int argc, char* argv[]){
    string arg = argv[2];
    if(argc == 3){      //For the default usage
        return argv[2];
    }else if(arg == "message"){     //For sending a message
        string content = "";
        for(int i = 3; i < argc - 1; i++){
            string temp = argv[i];
            if(temp == "end"){
                break;
            }
            content += temp + " ";
        }
        return content;
    }else{   //If the inputs don't match any valid inputs
        return "Incorrect Usage";
    }
}


/**
 * Main function
 */
int main() {
    //Sets the address port and the content
    string addressPort = "1000";
    
    //WSADATA setup
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cout << "[CLIENT] - WSAStartup failed." << endl;
        return 1;
    }
    
    //Socket setup
    SOCKET connectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (connectSocket == INVALID_SOCKET) {
        cout << "[CLIENT] - Error creating socket: " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    //Server info setup
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    if (inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr) <= 0) {
        cout << "[CLIENT] - Invalid address/ Address not supported" << endl;
        closesocket(connectSocket);
        WSACleanup();
        return 1;
    }

    //Socket connection
    if (connect(connectSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        cout << "[CLIENT] - Connect failed with error: " << WSAGetLastError() << endl;
        closesocket(connectSocket);
        WSACleanup();
        return 1;
    }

    boolean loop = true;
    boolean loggedIn = false;
    do{
        cout << "\nYou may:" << endl;
        cout << "  1. Login" << endl;
        cout << "  2. Register" << endl;
        cout << "  3. Logout" << endl;
        cout << "Please type what you would like to do: ";
        string userInput;
        cin >> userInput;
        
        //Converts the string to lowercase
        for(int i = 0; i < userInput.length(); i++){
            userInput[i] = tolower(userInput[i]);
        }

        if(userInput == "login" || userInput == "1"){
            loggedIn = login(connectSocket);
            if(loggedIn){
                loop = false;
            }
        }
        if(userInput == "register" || userInput == "2"){
            registerUser(connectSocket);
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
        for(int i = 0; i < userInput.length(); i++){
            userInput[i] = tolower(userInput[i]);
        }

        if(userInput == SUBSCRIBE_LOCATION || userInput == "1"){
            subscribeToLocation(connectSocket);
        }
        if(userInput == UNSUBSCRIBE_LOCATION || userInput == "2"){
            unsubscribeFromLocation(connectSocket);
        }
        if(userInput == LIST_ONLINE_USERS || userInput == "3"){
        }
        if(userInput == PRIVATE_MESSAGE || userInput == "4"){
        }
        if(userInput == LOCATION_MESSAGE || userInput == "5"){
        }
        if(userInput == VIEW_LOCATIONS || userInput == "6"){
            viewLocations(connectSocket);
        }
        if(userInput == VIEW_MESSAGES || userInput == "7"){
        }
        if(userInput == CHANGE_PASSWORD || userInput == "8"){
            changePassword(connectSocket);
        }
        if(userInput == LOGOUT || userInput == "9"){
            loggedIn = false;
        }
    }

    /** 
    //Loop to repeatedly collect userinput
    boolean firstLoop = true;
    do{
        //The first time the program reaches here, it already has some inputs, and skips this line accordingly
        if(!firstLoop){
            cout << "Please input the file you would like to retrieve from the server \nOr the message you would like to send: ";
            cout.flush();
            getline(cin, userRequest, '\n');
        }else{
            firstLoop = false;
        }

        //Creates a GET request from the userRequest and sends it
        string request = "GET /" + userRequest + " HTTP/1.1\r\nHost: 127.0.0.1:80\r\nConnection: keep-alive\r\n\r\n";
        const char* message = request.c_str();
        send(connectSocket, message, strlen(message), 0);

        //Waits for a response
        char recvbuf[1024];
        int result = recv(connectSocket, recvbuf, sizeof(recvbuf), 0);
        if(result > 0) {   //If a response was received, display it
            recvbuf[result] = 0;
            string result = recvbuf;
            cout << "[CLIENT] - Received response:" << endl;
            cout << result << endl;
        }else if(result == 0) {
            cout << "[CLIENT] - Connection closed\n";
            break;
        }else{
            cout << "[CLIENT] - recv failed: " << WSAGetLastError() << endl;
            break;
        }
    }while(userRequest != "close");

    */
    //Closes the program when the user enters "close"
    cout << "[CLIENT] - Closing client program..." << endl;
    closesocket(connectSocket);
    WSACleanup();

    return 0;
}