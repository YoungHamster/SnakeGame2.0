#pragma once
#include <Windows.h>

class Console
{
	// Singleton template
public:
    static Console& Instance()
    {
		// according to standard this code is lazy and threadsafe
        static Console s;
        return s;
    }

  private:
	  Console() {  }  // constructor is not available
    ~Console() {  } // and destructor

    // it is also necessary to prohibit copying
	Console(Console const&); // doesn't need realization
	Console& operator= (Console const&);  // and here

private:
	HANDLE out;// = GetStdHandle(STD_OUTPUT_HANDLE);
public:

};

Console& console = Console::Instance();