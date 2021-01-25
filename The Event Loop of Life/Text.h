#pragma once
#include "olcPixelGameEngine.h"
using namespace olc;

//Code taken from the Endless Runner I made in the first year and modified
class TheEventLoopOfLife;

struct Letter
{
	olc::Decal* sprites[26];
	Decal* m_sprite;
	vf2d m_trans; //Transform
	char m_value;
	Letter();
	Letter(const char c, const int x, const int y);
	void Update();
	void Render(TheEventLoopOfLife& game);
};

struct Word
{
	std::string word;
	std::vector<Letter> letters;
	Word();
	Word(const std::string word);
	Word(const std::string word, const int x, const int y);
	void Move(vf2d newPosition);
	void Render(TheEventLoopOfLife& game);
	void Reset();
};