#include "Grid.h"

Grid::Grid():
	tileContent(std::vector<std::array<Entity*, 3>>(grid.x* grid.y))
{

}

void Grid::Initialize(Random& r)
{
	for (size_t i = 0; i < grid.x * grid.y; i++)
	{
		//tileTraversibility.push_back(r.myRand() % 2); //0 == no resistance, 1 == -1 movement, 2 == rock
		//int probability = r.myRand() % 100;
		//tileTraversibility[i] = probability >= 85 ? 2 : tileTraversibility[i];
		//Pixel color = tileTraversibility[i] == 2 ? Pixel{ 60, 50, 30 } : Pixel{ 10, 170, 10 };
		Pixel color = Pixel{ 10, 170, 10 };
		tileColors.push_back(r.GetSimilarColor(color, 50));
	}
}

void Grid::PlaceEntityOnGrid(Entity* entity)
{
	if (tileContent[entity->position.x + grid.x * entity->position.y][0] == nullptr)
	{
		tileContent[entity->position.x + grid.x * entity->position.y][0] = entity;
		entity->spaceOccupying = 0;
	}
	else if (tileContent[entity->position.x + grid.x * entity->position.y][1] == nullptr)
	{
		tileContent[entity->position.x + grid.x * entity->position.y][1] = entity;
		entity->spaceOccupying = 1;
	}
	else
	{
		int oops = 0;
	}
}

bool Grid::TryOccupyNewPosition(Entity& entity, const int& target_i)
{
	int entity_i = entity.position.x + grid.x * entity.position.y;
	for (int i = 0; i < 2; i++)
	{
		if (tileContent[target_i][i] == nullptr)
		{
			tileContent[entity_i][entity.spaceOccupying] = nullptr;
			tileContent[target_i][i] = &entity;
			entity.spaceOccupying = i;
			return true;
		}
	}
	return false;
}

void Grid::ClampPositionToGrid(Vector2& vector)
{
	vector = Vector2(vector.x < 0 ? 0 : vector.x >= grid.x ? grid.x - 1 : vector.x,
					 vector.y < 0 ? 0 : vector.y >= grid.x ? grid.y - 1 : vector.y);
}

int Grid::GetTileIndexOfEntity(const int& x, const int& y, const Entity::EntityType& entity) const
{
	for (int i = 0; i < 2; i++)
	{
		if (tileContent[x + grid.x * y][i] != nullptr && tileContent[x + grid.x * y][i]->entityType == entity)
		{
			return i;
		}
	}
	return -1;
}
