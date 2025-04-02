

#ifndef SERVER_UTILS
#define SERVER_UTILS
#include <WinSock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <fstream>
#include <vector>
#include "user.cpp"
#include "utils.cpp"
using namespace std;

class ServerUtils{
    public:
        static boolean initializeWSA(WSADATA wsaData);
        static int createServerSocket();
        static sockaddr_in defineServerAddress();
        static boolean bindServerSocket(int tcp_server_socket, sockaddr_in tcp_server_address);
        static boolean checkIfListening(int tcp_server_socket);
        static string saveUserInformation(User currUser);
        static vector<string> readUserVectorFromFile(string filename);
        static bool writeUserDataToFile(vector<string> userData, string filename);
        static bool checkUserDataForCredentials(string username, string password);
        static void setUpCurrUser(User *currUser, string username, string password);
        static bool appendUserData(string username, string password, string filename);
};

/**
 * Initializes the wsaData
 * @param WSADATA wsaData the WSADATA to initialize
 * @return bool representing if the wsaData was initialized or not
 */
boolean ServerUtils::initializeWSA(WSADATA wsaData){
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
 * @return boolean representing if the socket was bound properly
 */
boolean ServerUtils::bindServerSocket(int tcp_server_socket, sockaddr_in tcp_server_address){
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
boolean ServerUtils::checkIfListening(int tcp_server_socket){
    if(listen(tcp_server_socket, 5) < 0){
        cout << "Error listening for connections" << endl;
        closesocket(tcp_server_socket);
        return false;
    }
    cout << "[SERVER] - Listening on 127.0.0.1:39756" << endl;
    return true;
}

vector<string> ServerUtils::readUserVectorFromFile(string filename){
    vector<string> lines = vector<string>(0);

     //Open a file
     ifstream inFile(filename);
     if (!inFile.is_open()) {
         std::cerr << "Error: Unable to open file for reading." << std::endl;
         return lines;
     }

     //Pull the lines of data
     string line;
     int index = 0;
     while (getline(inFile, line)) { 
        lines.push_back(line);
     }
 
     inFile.close();
     if (index <= 0 || index > lines.size()) {
         std::cerr << "Error: Invalid line number." << std::endl;
         return vector<string>(0);
     }

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

    for(int i = 0; i < userData.size(); i++){
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
            currUser->inbox = Inbox();
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
}


#endif