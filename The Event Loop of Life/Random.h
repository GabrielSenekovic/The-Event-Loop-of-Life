#pragma once
#include <random>

class Random
{
public:
	std::random_device rd;
	int magicNumber = 0;

	Random();
	uint32_t myRand();
};