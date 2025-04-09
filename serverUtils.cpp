

#ifndef SERVER_UTILS
#define SERVER_UTILS

#if WIN_32
#include <WinSock2.h>
#include <ws2tcpip.h>
#endif

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <iostream>
#include <fstream>
#include <vector>
#include "user.cpp"
#include "utils.cpp"
#include <unistd.h>
using namespace std;

class ServerUtils{
    public:
        static int createServerSocket();
        static sockaddr_in defineServerAddress();
        static bool bindServerSocket(int tcp_server_socket, sockaddr_in tcp_server_address);
        static bool checkIfListening(int tcp_server_socket);
        static string saveUserInformation(User currUser);
        static vector<string> readUserVectorFromFile(string filename);
        static bool writeUserDataToFile(vector<string> userData, string filename);
        static bool checkUserDataForCredentials(string username, string password);
        static void setUpCurrUser(User *currUser, string username, string password);
        static bool appendUserData(string username, string password, string filename);
};

/**
 * Creates the server socket
 * @return int representing the socket
 */
int ServerUtils::createServerSocket(){
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
sockaddr_in ServerUtils::defineServerAddress(){
     //define the server address
     struct sockaddr_in tcp_server_address;                  //declaring a structure for the address
     tcp_server_address.sin_family = AF_INET;                //Structure Fields' definition: Sets the address family of the address the client would connect to
     tcp_server_address.sin_port = htons(8080);             //Passing the port number - converting in right network byte order
     tcp_server_address.sin_addr.s_addr = inet_addr("127.0.0.1"); //INADDR_ANY;
    return tcp_server_address;
}

/**
 * Binds the server socket to the server address
 * @return bool representing if the socket was bound properly
 */
bool ServerUtils::bindServerSocket(int tcp_server_socket, sockaddr_in tcp_server_address){
    //Bind the server socket
    if(bind(tcp_server_socket, (struct sockaddr *)&tcp_server_address, sizeof(tcp_server_address)) != 0){
        cout << "[SERVER] - Could not bind socket" << endl;
        return false;
    }
    return true;
}

/**
 * Checks if the server socket is listening
 * @return bool representing if the socket is listening
 */
bool ServerUtils::checkIfListening(int tcp_server_socket){
    if(listen(tcp_server_socket, 5) < 0){
        cout << "Error listening for connections" << endl;
        close(tcp_server_socket);
        return false;
    }
    cout << "[SERVER] - Listening on 127.0.0.1:39756" << endl;
    return true;
}

vector<string> ServerUtils::readUserVectorFromFile(string filename){
    vector<string> lines = vector<string>(0);

     //Open a file
    ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open file for reading." << std::endl;
        return lines;
    }

     //Pull the lines of data
    string line;
    while (getline(file, line)) { 
        lines.push_back(line);
    }
    file.close();

    return lines;
}

bool ServerUtils::writeUserDataToFile(vector<string> userData, string filename){
    std::ofstream outFile(filename);
    if (!outFile.is_open()) {
        std::cerr << "Error: Unable to open file for writing." << std::endl;
        return false;
    }

    for (const auto& l : userData) {
        outFile << l << "\n";
    }
    outFile.close();  // Close the file after writing

    return true;
}


string ServerUtils::saveUserInformation(User currUser){
    vector<string> userData = readUserVectorFromFile("users.txt");

    for(int i = 0; i < (int) userData.size(); i++){
        if(userData.at(i).find(currUser.username) != string::npos && userData.at(i).find(currUser.password) != string::npos){
            userData.at(i) = currUser.username + " " + currUser.password + " [" + currUser.getLocations() + "]";
        }
    }

    bool success = writeUserDataToFile(userData, "users.txt");

    if(success) return "save-user: success";
    else return "save-user: failure";
}

bool ServerUtils::checkUserDataForCredentials(string username, string password){
    ifstream userData("users.txt");
    string line;
    bool matchFound = false;
    vector<string> data;
    while (getline(userData, line)) {    
        data = Utils::split(line, ' ');
        if(username == data.at(0) && password == data.at(1)){
            matchFound = true;
            break;
        }
    }
    userData.close();

    return matchFound;
}

void ServerUtils::setUpCurrUser(User *currUser, string username, string password){
    ifstream userData("users.txt");
    string line;
    while (getline(userData, line)) {    
        vector<string> data = Utils::split(line, ' ');
        if(username == data.at(0) && password == data.at(1)){
            currUser->username = data.at(0);
            currUser->password = data.at(1);
            //currUser->inbox = Inbox();
            currUser->socketNum = 0;

            string locationString = data.at(2).substr(1, data.at(2).length() - 2);
            if(locationString.length() == 0){
                currUser->locations = vector<string>(0);
            }else{
                currUser->locations = Utils::split(locationString, ',');
            }
        }
    }
    userData.close();
}

bool ServerUtils::appendUserData(string username, string password, string filename){
    ofstream userData("users.txt", ios::app);

    // Check if the file is open
    if (!userData.is_open()) {
        std::cerr << "Error: Unable to open file for appending." << std::endl;
        return "register-request: failure server_error";
    }

    //Write the new user data to the file
    userData << username << " " << password << " []" << "\n";
    userData.close();

    return true;
}


#endif