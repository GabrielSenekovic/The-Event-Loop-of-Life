#pragma once
#include "olcPixelGameEngine.h"
#include "Entity.h"
#include "Random.h"
#include "HUDElement.h"

using namespace olc;

#define TEA(val, comparison, ternary) (val = val == comparison ternary);
//Ternary Equivalence Assignment
//x = x==3? 1:2; turns into
//TEA(x, 3, ?1:2);
//Its only really useful for readability if x is reeeeaaaaally long
//It also means that you only have to write x once
#define DEL(val) {delete val; val = nullptr;};
//Simple and nice way to both delete and set to nullptr
//Only use when you want to do both
#define forall(i,a,b) for(size_t i=a;i<b;i++)
//Goes through all elements of a list
#define uint unsigned int
#define Vector2 vf2d
#define IntVector2 vi2d

class TheEventLoopOfLife : public olc::PixelGameEngine
{
	float timer = 0;
	float timeSpeed = 2;

	IntVector2 grid = { 20, 20 };
	std::vector<Pixel> tileColors;
	std::vector<int> tileTraversibility; //I like to make it a bit more intricate
	std::vector<std::array<Entity*, 3>> tileContent;
	Sheep* sheepbitch;

	olc::Decal* tile;
	olc::Decal* rock;
	olc::Decal* sheep;
	olc::Decal* wolf;
	olc::Decal* grass;
	olc::Decal* grass_growing;

	HUDElement frameRate;
	HUDElement amountOfTurns;
	HUDElement sheep_x;
	HUDElement sheep_y;
	HUDElement sheep_target_x;
	HUDElement sheep_target_y;
	HUDElement sheep_HP;

	std::vector<Entity*> entities;

	std::array<Decal*, 10> digits;

	Random r;

public:
	TheEventLoopOfLife(IntVector2 dim_in);

	bool OnUserCreate()override;
	bool OnUserUpdate(float fElepsedTime)override;
	void OnUserDraw();

	Pixel GetSimilarColor(Pixel color, int modulus);

	void Quit();

	IntVector2 dim; //Dimensions of the screen
};