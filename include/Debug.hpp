#pragma once

#define PINK "\033[1;38;5;205m"       // Bold Pink
#define LIGHT_PINK "\033[1;38;5;217m" // Bold Light Pink
#define LIGHT_BLUE "\033[1;38;5;153m" // Bold Light Blue
#define RESET "\033[0m"               // Reset to default color

#include <iostream>
#include <string>
#include <vector>
#include <map>

// If you want Debug to be a collection of utility functions, you don't need a class.
// You can use a namespace instead, which is more idiomatic in C++ for this purpose.

namespace Debug {
	void display1(std::string str1, std::string str2);
	void display_trace(std::map<std::string, std::string> tokens);
}