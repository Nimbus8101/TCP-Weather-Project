/**
 * httpServer.cpp
 * @author Elijah Reyna
 * This file contains all the code to start a serer, as well as to receive and process responses from a client
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <unistd.h>
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include "user.cpp";
using namespace std;


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
     tcp_server_address.sin_port = htons(39756);             //Passing the port number - converting in right network byte order
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

/**
 * Processes the client request
 * @return string representing the server response to the request
 */
string processRequest(char* buffer){
    string failure = "HTTP/1.1 404 Not Found\nContent-Type:text/html\nContent-Length: 22\n\n<h1>404 Not Found</h1>";

    //Gets the information from the buffer
    stringstream ss(buffer);
    string requestString;
    getline(ss, requestString, '\n');

    if(requestString.find("GET") != string::npos){
        if(requestString.find("login=") != string::npos){
            int startingIndex = requestString.find("login=") + 6;
            int endingIndex = requestString.find("HTTP") - 1;

            string credentials = requestString.substr(startingIndex, requestString.length() - endingIndex - startingIndex);

            string username = credentials.substr(0, credentials.length() - credentials.find("+"));
            string password = credentials.substr(credentials.find("+") + 1);

            cout << "credentials: " << username << ", " << password << endl;

            //Check the credentials

            //Create a message that says the login was successful/unsuccessful
        }
    }else if(requestString.find("POST") != string::npos){
        if(requestString.find("register=") != string::npos){
            int startingIndex = requestString.find("register=") + 9;
            int endingIndex = requestString.find("HTTP") - 1;

            string credentials = requestString.substr(startingIndex, requestString.length() - endingIndex - startingIndex);

            string username = credentials.substr(0, credentials.length() - credentials.find("+"));
            string password = credentials.substr(credentials.find("+") + 1);

            cout << "credentials: " << username << ", " << password << endl;

            //Create a user
            //Add the User to the database (text file)

            //Create a message that says the registration was successful/unsuccessful
        }
    }else if(requestString.find("UPDATE") != string::npos){
        string PASSWORD_CHANGE = "passwordchange=";
        if(requestString.find(PASSWORD_CHANGE) != string::npos){
            int startingIndex = requestString.find(PASSWORD_CHANGE) + PASSWORD_CHANGE.length();
            int endingIndex = requestString.find("HTTP") - 1;

            string credentials = requestString.substr(startingIndex, requestString.length() - endingIndex - startingIndex);

            string username = credentials.substr(0, credentials.length() - credentials.find("+"));
            string newPassword = credentials.substr(credentials.find("+") + 1);

            cout << "Updating " << username << " to have password: " << newPassword << endl;

            //update the password in the User class

            //Create a message that says the change was successful/unsuccessful
        }else{
            return "HTTP/1.1 404 Not Found\nContent-Type:text/html\nContent-Length: 28\n\n<h1>404 Invalid Request</h1>";
        }
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
    }
    
    //close the socket
    closesocket(tcp_server_socket);
    WSACleanup();
    cout << "[SERVER] - Server about to close" << endl;
    return 0;
}