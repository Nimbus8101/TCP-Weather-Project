/**
 * 
 * 
 */

#ifndef MESSAGE_BOX
#define MESSAGE_BOX
#include <iostream>
#include <string>
#include <vector>
using namespace std;

class MessageBox{
    public:
        int MAX_MESSAGE_SIZE = 500;
        MessageBox(){

        }

        void addMessage(string message){
            messages.push_back(message);
        }

        vector<string> getLastNMessages(int n = 10){
            vector<string> lastMessages = vector<string>(0);

            for(int i = 1; i <= n; i++){
                lastMessages.push_back(messages.at(messages.size() - i));
            }

            return lastMessages;
        }
    
        private:
            vector<string> messages = vector<string>(0);

};

#endif