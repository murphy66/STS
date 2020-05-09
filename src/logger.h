#pragma once
#include <iostream>
#include <string_view>

inline void Log(std::string_view msg)
{
	std::cout << msg << "\n";
}