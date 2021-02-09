#ifndef GRID_H
#define GRID_H

#include "olcPixelGameEngine.h"
#include "Entity.h"
#include "Random.h"

using namespace olc;

class Grid
{
public:
	IntVector2 grid = { 20, 20 };
	std::vector<Pixel> tileColors;
	std::vector<int> tileTraversibility; //I like to make it a bit more intricate
	std::vector<std::array<Entity*, 3>> tileContent;

	Grid();
	void Initialize(Random& r);

	void PlaceEntityOnGrid(Entity* entity);

	int GetTileIndexOfEntity(const int& x, const int& y, const Entity::EntityType& entity) const;
};

#endif