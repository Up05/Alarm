// alarm.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "request_data.h"
#include "threads.h"
#include "date.h"

namespace server {
    void init();
}

bool shouldClose = false;


int main()
{
    std::cout << "Remember! Only the first char of the first argument is read! \n\"arg a bcdef 2\" is the same as \"a a bcdef 2\"\n";
    


    threads::init();
    io::init();
    server::init();
    // main thread WAITS for req after server::init()

}


void parse(char* input) {
    
    std::vector<std::string> args(0); { // splits input by space
        std::string temp_string(input);
        std::string arg;
        size_t pos = 0;
        while (
            (pos = temp_string.find(" ") ) != std::string::npos || 
            (pos = temp_string.find("\n")) != std::string::npos
        ) {
            args.push_back(temp_string.substr(0, pos));
            temp_string.erase(0, pos + 1);
        }
    }
    
    // ? if (args[0].length > 1) throw error

    LOG("main", "Request parsed.")
    // this should be moved elsewhere!
    switch (args[0][0]) { // ! <-- IMPORTANT
        case 'a': // add
            alarm::add(args);
            break;
        case 'r': // remove
            alarm::remove(args);
            break;
        case 'o': // offset
            break;
        case 'l': // list
            alarm::list(args);
            break;
    }


}




/*
    I think, most of my problems are coming from the priority queue data structure
    It might be a good idea to simply replace it with set or even array, in which case, 
    there would (likely) be no need for storing the creation_date.

    Safeguard against name collisions?

    Look into alarm.cpp 59!

*/