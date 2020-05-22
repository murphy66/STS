#pragma once

#include <random>

static std::random_device rd; // obtain a random number from hardware
static std::mt19937 eng(rd()); // seed the generator

int Rand(int to)
{
	std::uniform_int_distribution<> distr(1, to); // define the range
	return distr(eng);
}

int Rand(int from, int to)
{
	std::uniform_int_distribution<> distr(from, to); // define the range
	return distr(eng);
}