#pragma once
#include <random>
#include "olcPixelGameEngine.h"
using namespace olc;

class Random
{
public:
	std::random_device rd;
	int magicNumber = 0;

	Random();
	uint32_t myRand();
	Pixel GetSimilarColor(Pixel color, int modulus);
};