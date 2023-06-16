// alarm.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "Shell32.lib")
//#pragma comment(lib, "comctl32.lib")

#include <windows.h>
#include <shellapi.h>
#include <yaml-cpp\yaml.h>
//#include <commctrl.h>

#include "request_data.h"
#include "threads.h"
#include "date.h"



namespace server { void init(); }
namespace io { extern YAML::Node config; }

bool shouldClose = false;
static boolean window_visible = false;


int main()
{
    std::cout << "Remember! Only the first char of the first argument is read! \n\"arg a bcdef 2\" is the same as \"a a bcdef 2\"\n";
    
    threads::init();
    io::init();

    window_visible = io::config["show_console"].as<bool>();
    ShowWindow(GetConsoleWindow(), window_visible ? SW_SHOW : SW_HIDE);

    server::init(); // pause()
    // main thread WAITS for req after server::init()
    
}

// DEPRECATED
//namespace io { extern YAML::Node config; extern std::string appdata; }
//static void setup_window() {
//    if (!io::config["show_console"].as<bool>()) {
//
//        HWND console; {
//            char original_title[256], temp_title[] = "uniquetitletogetthefreakinghwndwhywasntthereabettermethodforawindowtoyouknow";
//            GetConsoleTitleA(original_title, 256);
//            SetConsoleTitleA(temp_title);
//            Sleep(40);
//
//            console = FindWindow(NULL, temp_title);
//            SetConsoleTitleA(original_title);
//        }
//
//
//        NOTIFYICONDATA data; 
//        ZeroMemory(&data, sizeof(NOTIFYICONDATA));
//        data.cbSize = sizeof(data);
//        data.hWnd = console;
//        data.uID = 52379527127251444 % UINT_MAX;
//        data.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
//        data.uCallbackMessage = WM_USER + 1;
//        data.uVersion = NOTIFYICON_VERSION_4;
//        std::string path = io::appdata + "\\Ult1\\Alarm\\tray.ico";
//        data.hIcon = (HICON) LoadImageA(NULL, path.c_str(), IMAGE_ICON, 48, 48, LR_LOADFROMFILE); // fails to set the tray icon!
//        LOG2("main", "Tray icon path: \"%s\"", path.c_str() );
//        Shell_NotifyIcon(NIM_ADD, &data);
//    }
//}

void toggle_window() {
    window_visible = !window_visible;
    ShowWindow(GetConsoleWindow(), window_visible ? SW_SHOW : SW_HIDE);
    
    if (window_visible) server::send("[SUCCESS] Window shown.");
    else                server::send("[SUCCESS] Window hidden.");
    server::send("[END]");
}

void parse(char* input) {
    
    std::vector<std::string> args(0); 

    
    std::string temp_string(input);
    
    if (temp_string[temp_string.size() - 1] == ' ') temp_string.resize(temp_string.size() - 1, '?');

    if(temp_string.find(" ") != std::string::npos) { // splits input by space
        std::string arg;
        size_t pos = 0;
        while ((pos = temp_string.find(" ") ) != std::string::npos) {
            args.push_back(temp_string.substr(0, pos));
            temp_string.erase(0, pos + 1);
        }
        args.push_back(temp_string.substr(0, temp_string.size()));

    } else {
        if (temp_string[temp_string.size() - 1] == '\n') {
            temp_string[temp_string.size() - 1] = '\0';
            temp_string.resize(temp_string.size() - 1, '?');
        }

        args.push_back(temp_string);
    }
    
    // ? if (args[0].length > 1) throw error

    LOG("main", "Request parsed.")
    // this should be moved elsewhere!
    switch (args[0][0]) { // ! <-- IMPORTANT
        case 'a': // add
            alarm::add(args);
            break;
        case 'r': // remove
            alarm::remove(args); // Only parsed r, didn't execute it. (args[0][0] != 'r')
            break;
        case 'o': // offset
            break;
        case 'l': // list
            alarm::list(args);
            break;
        case 's':
            toggle_window();
            break;
        default:
            LOG("main", (std::string("Request: \'") + args[0][0] + "\' command unknown!").c_str())
    }


}




/*
    Todo:
    All program branches should end in a server::send(...)!;
      e.g. Time not stated...


*/