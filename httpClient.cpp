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

bool login(){
    cout << "\nPlease input your username: ";
    string username;
    cin >> username;

    cout << "Logging " << username << " in..." << endl;

    //Verify with the server

    return false;
}

bool registerUser(){
    cout << "\nPlease input your preferred username: ";
    string username;
    cin >> username;

    cout << "Checking if " << username << " exists..." << endl;

    return false;
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
    serverAddress.sin_port = htons(39756); // Port 39756 for HTTP
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
            login();
            loggedIn = true;
        }
        if(userInput == "register" || userInput == "2"){
            registerUser();
        }
        if(userInput == "logout" || userInput == "3"){
            cout << "Logging out" << endl;
            loop = false;
        }

    }while(loop);


    do{
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
        cout << "  1. " << CHANGE_PASSWORD << endl;
        cout << "  2. " << VIEW_LOCATIONS << endl;
        cout << "  3. " << SUBSCRIBE_LOCATION << endl;
        cout << "  4. " << UNSUBSCRIBE_LOCATION << endl;
        cout << "  5. " << LOCATION_MESSAGE << endl;
        cout << "  6. " << LIST_ONLINE_USERS << endl;
        cout << "  7. " << PRIVATE_MESSAGE << endl;
        cout << "  8. " << VIEW_MESSAGES << endl;
        cout << "  9. " << LOGOUT << endl;
        cout << "Please type what you would like to do: ";
        string userInput;
        cin >> userInput;
        
        //Converts the string to lowercase
        for(int i = 0; i < userInput.length(); i++){
            userInput[i] = tolower(userInput[i]);
        }

        if(userInput == SUBSCRIBE_LOCATION || userInput == "1"){
        }
        if(userInput == UNSUBSCRIBE_LOCATION || userInput == "2"){
        }
        if(userInput == LIST_ONLINE_USERS || userInput == "3"){
        }
        if(userInput == PRIVATE_MESSAGE || userInput == "4"){
        }
        if(userInput == LOCATION_MESSAGE || userInput == "5"){
        }
        if(userInput == VIEW_LOCATIONS || userInput == "6"){
        }
        if(userInput == VIEW_MESSAGES || userInput == "7"){
        }
        if(userInput == CHANGE_PASSWORD || userInput == "8"){
        }
        if(userInput == LOGOUT || userInput == "9"){
        }

    }while(loggedIn);

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