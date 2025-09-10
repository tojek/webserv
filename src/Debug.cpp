#include "Debug.hpp"


namespace Debug {
	// use to display type and value
	void display1(std::string str1, std::string str2) {
        std::cout << PINK << str1 << ": " RESET << str2 << std::endl;
    }

	// display message containing info about the request
	void display_trace(std::map<std::string, std::string> tokens)
	{
		std::cout << PINK << "\n[REQUEST_TRACE] [method]: " << tokens["method"];
		std::cout << " [request uri]: " << tokens["request_uri"] << RESET << std::endl;
	}

	void debug()
	{
		std::cout << LY "debug point\n" RESET;
	}	
	
	void debug(std::string message)
	{
		std::cout << LY "debug point: " RESET << message << std::endl;
	}

}