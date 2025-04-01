/**
 * httpServer.cpp
 * @author Elijah Reyna
 * This file contains all the code to start a serer, as well as to receive and process responses from a client
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <thread>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include "user.cpp"
using namespace std;

#define PORT 8080

/**
 * Initializes the wsaData
 * @param WSADATA wsaData the WSADATA to initialize
 * @return bool representing if the wsaData was initialized or not
 */
boolean initializeWSA(WSADATA wsaData){
    if(WSAStartup(MAKEWORD(2, 2), &wsaData) != 0){
        cout << "[SERVER] - Could not initialize" << endl;
        return false;
    }
    return true;
}

/**
 * Creates the server socket
 * @return int representing the socket
 */
int createServerSocket(){
    int tcp_server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(tcp_server_socket < 0){
        cout << "[SERVER] - Could not create socket" << endl;
        return false;
    }
    return tcp_server_socket;
}

/**
 * Defines the Server Address
 * @return sockaddr_in representing the socket address info
 */
sockaddr_in defineServerAddress(){
     //define the server address
     struct sockaddr_in tcp_server_address;                  //declaring a structure for the address
     tcp_server_address.sin_family = AF_INET;                //Structure Fields' definition: Sets the address family of the address the client would connect to
     tcp_server_address.sin_port = htons(PORT);             //Passing the port number - converting in right network byte order
     tcp_server_address.sin_addr.s_addr = inet_addr("127.0.0.1"); //INADDR_ANY;
    return tcp_server_address;
}

/**
 * Binds the server socket to the server address
 * @return boolean representing if the socket was bound properly
 */
boolean bindServerSocket(int tcp_server_socket, sockaddr_in tcp_server_address){
    //Bind the server socket
    if(bind(tcp_server_socket, (struct sockaddr *)&tcp_server_address, sizeof(tcp_server_address)) != 0){
        cout << "[SERVER] - Could not bind socket" << endl;
        return false;
    }
    return true;
}

/**
 * Checks if the server socket is listening
 * @return boolean representing if the socket is listening
 */
boolean checkIfListening(int tcp_server_socket){
    if(listen(tcp_server_socket, 5) < 0){
        cout << "Error listening for connections" << endl;
        closesocket(tcp_server_socket);
        return false;
    }
    cout << "[SERVER] - Listening on 127.0.0.1:39756" << endl;
    return true;
}

/**
 * Parses the request
 * @return string representing the request
 */
string parseRequest(string httpReceived){
    string temp = httpReceived.substr(5);
    int length = temp.find("HTTP") - 1;
    return temp.substr(0, length);
}

/**
 * Generates the html page given the html content
 * @return an html page so it can be sent
 */
string generateHTMLPage(string htmlContent){
    string newPage = "HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length: ";
    newPage += to_string(htmlContent.length()) + "\n\n";
    newPage += htmlContent;
    return newPage;
}

vector<string> split(const string &str, char delimiter) {
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

/**
 * Processes the client request
 * @return string representing the server response to the request
 */
string processRequest(char* buffer){
    //Gets the information from the buffer
    stringstream ss(buffer);
    string requestString;
    getline(ss, requestString, '\n');

    //Potential Requests
    string LOGIN = "login=";
    string REGISTER = "register=";
    string PASSWORD_CHANGE = "passwordchange=";

    if(requestString.find(LOGIN) != string::npos){
        // Parses the username and password based on the '+' character, after removing the "login=" portion of the request string
        vector credentials = split(requestString.substr(requestString.find(LOGIN) + LOGIN.length()), '+');

        string username = credentials.at(0);
        string password = credentials.at(1);

        cout << "credentials: " << username << ", " << password << endl;

        // Verifies that the user exists
        ifstream userData("users.txt");
        string line;
        bool matchFound = false;
        while (getline(userData, line)) {    
            vector data = split(line, ' ');

            if(username == data.at(0) && password == data.at(1)){
                matchFound = true;
                break;
            }
        }
        userData.close();

        if(matchFound){
            // FIXME Initialize a User object for the session
            return "login-request: successful";
        }else{
            return "login-request: failure";
        }
    }
    if(requestString.find(REGISTER) != string::npos){
        // Parses the username and password based on the '+' character, after removing the "register=" portion of the request string
        vector credentials = split(requestString.substr(requestString.find(REGISTER) + REGISTER.length()), '+');

        string username = credentials.at(0);
        string password = credentials.at(1);

        cout << "credentials: " << username << ", " << password << endl;

        // Check if the user exists
        // Verifies that the user exists
        ifstream userData("users.txt");
        string line;
        bool matchFound = false;
        while (getline(userData, line)) {    
            vector data = split(line, ' ');

            if(username == data.at(0) && password == data.at(1)){
                matchFound = true;
                break;
            }
        }
        userData.close();

        if(matchFound){
            // Tell the client the request failed
            return "register-request: failure account_already_exists";
        }else{
            ofstream userData("users.txt", ios::app);

            // Check if the file is open
            if (!userData.is_open()) {
                //Tell the client the request failed
                std::cerr << "Error: Unable to open file for appending." << std::endl;
                return "register-request: failure server_error";
            }

            //Write the new user data to the file
            userData << username << " " << password << " " << "[]\n";
            userData.close();

            return "register-request: success";
        }
    }
    
    if(requestString.find(PASSWORD_CHANGE) != string::npos){
        string newPassword = requestString.substr(requestString.find(PASSWORD_CHANGE) + PASSWORD_CHANGE.length());

        cout << "Updating password: " << newPassword << endl;

        string username = "TEST_USER"; //FIXME pull the actual username from the current user
        string password = "TEST_PASSWORD"; //FIXME pull the actual password from the current user

        //update the password in the User class

        //Update the information in the database (text file)
        ifstream inFile("users.txt");  // Open the file for reading
        if (!inFile.is_open()) {
            std::cerr << "Error: Unable to open file for reading." << std::endl;
            return "password-change-request: failure server_error";
        }

        vector<string> lines;
        string line;
        int index = 0;
        while (getline(inFile, line)) {
            vector data = split(line, ' ');

            if(username == data.at(0) && password == data.at(1)){
                line = username + " " + newPassword + " " + data[2];
                index = lines.size() - 1;
            }

            lines.push_back(line);
        }
        inFile.close();

        if (index <= 0 || index > lines.size()) {
            std::cerr << "Error: Invalid line number." << std::endl;
            return "password-change-request: failure server_error";
        }

        std::ofstream outFile("users.txt");  // Open the file for writing
        if (!outFile.is_open()) {
            std::cerr << "Error: Unable to open file for writing." << std::endl;
            return "password-change-request: failure server_error";
        }

        for (const auto& l : lines) {
            outFile << l << "\n";
        }

        outFile.close();  // Close the file after writing

        return "password-change-request: success";
        //Create a message that says the change was successful/unsuccessful
    }else{
        return "HTTP/1.1 404 Not Found\nContent-Type:text/html\nContent-Length: 28\n\n<h1>404 Invalid Request</h1>";
    }
    /** 
    if(requestString.substr(0, 3) == "GET"){
        string request = parseRequest(requestString);
        
        //If it is requesting a file
        if(request.find(".") != string::npos){
            //Open a file stream
            ifstream fileStream(request);
            if (!fileStream.is_open()) {
                return "HTTP/1.1 404 Not Found\nContent-Type:text/html\nContent-Length: 22\n\n<h1>404 Not Found</h1>";
            }else{
                //Create and send the html
                stringstream response;
                response << fileStream.rdbuf();
                string htmlContent = response.str();

                return generateHTMLPage(htmlContent);
            } 
        }else if(request.substr(0, 7) == "message"){
            return "echo - \"" + request.substr(8) + "\"";
        }else if(request == "close"){
            return "Client requested disconnect. Disconnecting...";
        }else{
            return "HTTP/1.1 404 Not Found\nContent-Type:text/html\nContent-Length: 28\n\n<h1>404 Invalid Request</h1>";
        }
    }else{
        return "HTTP/1.1 404 Not Found\nContent-Type:text/html\nContent-Length: 28\n\n<h1>404 Invalid Request</h1>";
    }
    */
}

void handle_client(int tcp_client_socket){
    cout << "Handle Client function" << endl;

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

        string response = processRequest(buffer);
        
        /**
         * This code was to prevent the client's buffer from being overfilled, and i havent set up a way to send multiple bits of data yet
            if(response.length() > 1024){
            response = "Too many characters in response, try again later";
        }*/

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
    WSADATA wsaData;
    if(!initializeWSA(wsaData)){
        return -1;
    }

    //Socket setup
    int tcp_server_socket = createServerSocket();
    if(tcp_server_socket < 0){
        cout << "[SERVER] - Could not create socket" << endl;
        return -1;
    }

    //Server Address setup
    struct sockaddr_in tcp_server_address = defineServerAddress();
    if(!bindServerSocket(tcp_server_socket, tcp_server_address)){
        return -1;
    }

    if(!checkIfListening(tcp_server_socket)){
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
    closesocket(tcp_server_socket);
    WSACleanup();
    cout << "[SERVER] - Server about to close" << endl;
    return 0;
}