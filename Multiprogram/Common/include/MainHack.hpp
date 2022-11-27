#pragma once

// This is a trick that I picked up from the SDL2 library. Due to the way that
// the C/C++ preprocessor works, you can replace the user main fuction with
// another function name, and then add a new main function that calls the other
// main function

#include <stdio.h>

// Forward declare the user main, since we need the macro to redefine the user's
// main to appear after our replacement for the main function
int _user_main(int argc, char** argv);

int main(int argc, char** argv)
{
    // Debugging messages
    printf("Entering overriden main. argc=%d:\n", argc);
    for(int i = 0; i < argc; i++) {
        printf("        argv[i]: \"%s\"\n", argv[i]);
    }

    int  slideNumber    = -1;
    if(argc >= 2) {
        printf("    Getting slide to return to: ");
        slideNumber = std::stoi(argv[1]);

        printf("%d\n", slideNumber);
    }

    // Enter the user's main program
    
    printf("    Entering user's main program\n");
    _user_main(argc, argv);
    printf("    Done with user's main program\n");

    if(slideNumber >= 0) {
        printf("Returning to slide #%d\n", slideNumber);
    }
    else {
        printf("Halting program\n");
    }
    
    SleepThread();
}

// Redefine the main function after this so that we can inject our main function
// before it
#define main(...) _user_main(__VA_ARGS__)