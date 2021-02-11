#ifndef GRID_H
#define GRID_H

#include "olcPixelGameEngine.h"
#include "Entity.h"
#include "Random.h"

using namespace olc;

class Grid
{
public:
	IntVector2 grid = { 50, 50 };
	std::vector<Pixel> tileColors;
	//std::vector<int> tileTraversibility; //For A*. I'm prepared
	std::vector<std::array<Entity*, 3>> tileContent;

	Grid();
	void Initialize(Random& r);

	void PlaceEntityOnGrid(Entity* entity);
	bool TryOccupyNewPosition(Entity& entity, const int& target_i);

	void ClampPositionToGrid(Vector2& vector);

	int GetTileIndexOfEntity(const int& x, const int& y, const Entity::EntityType& entity) const;
};

#endif