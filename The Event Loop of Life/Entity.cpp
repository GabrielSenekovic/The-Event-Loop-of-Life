#include "Entity.h"
#include "TheEventLoopOfLife.h"
#include "Grid.h"
#include "EntityManager.h"

Entity::Entity(const Vector2& position_in, const Vector2& health_in, const std::vector<Decal*>& sprites_in) :
	position(position_in), health(health_in),
	dead(false), state(EntityState::IDLE), sprites(sprites_in)
{
}
int* Entity::getValidConstraints(int i, olc::vi2d grid)
{
	int targY = (i / grid.x);
	int startY = (targY - 1) % grid.y; //! POS - 1 
	int targX = i % grid.x;
	if (startY < 0) { startY = 0; }
	int startX = (targX - 1) % grid.x; //! POS - 1
	if (startX < 0) { startX = 0; }
	int yLimit = targY + 2;
	int xLimit = targX + 2;

	if (xLimit > grid.x) { xLimit = grid.x; }
	if (yLimit > grid.y) { yLimit = grid.y; }

	return new int[6]{ targX, targY, startX, startY, xLimit, yLimit };
}

int* Entity::getValidConstraints(int i, int range, olc::vi2d grid)
{
	int targY = (i / grid.x);
	int startY = (targY - range) % grid.y; //! POS - 1 
	int targX = i % grid.x;
	if (startY < 0) { startY = 0; }
	int startX = (targX - range) % grid.x; //! POS - 1
	if (startX < 0) { startX = 0; }
	int yLimit = targY + range + 1;
	int xLimit = targX + range + 1;

	if (xLimit > grid.x) { xLimit = grid.x; }
	if (yLimit > grid.y) { yLimit = grid.y; }

	return new int[6]{ targX, targY, startX, startY, xLimit, yLimit };
}

Vector2 Entity::GetRandomAdjacentPosition(Random& r, const IntVector2& dim)
{
	int32_t index = position.x + dim.x * position.y;
	int* constraints = getValidConstraints(index, dim);
	std::vector<Vector2> possiblePositions = {};
	for (int y = constraints[3]; y < constraints[5]; y++)
	{
		for (int x = constraints[2]; x < constraints[4]; x++)
		{
			if (position != Vector2(x, y))
			{
				possiblePositions.push_back({ (float)x, (float)y });
			}
		}
	}
	return possiblePositions[r.myRand() % possiblePositions.size()];
}

Vector2 Entity::GetRandomAdjacentPosition(Random& r, const IntVector2& dim, const std::vector<std::array<Entity*, 3>>& tileContent, const EntityType& type)
{
	int32_t index = position.x + dim.x * position.y;
	int* constraints = getValidConstraints(index, dim);
	std::vector<Vector2> possiblePositions = {};
	for (int y = constraints[3]; y < constraints[5]; y++)
	{
		for (int x = constraints[2]; x < constraints[4]; x++)
		{
			if (type == EntityType::SHEEP || type == EntityType::WOLF)
			{
				if (position != Vector2(x, y) && (tileContent[x + dim.x * y][0] == nullptr || tileContent[x + dim.x * y][1] == nullptr))
				{
					possiblePositions.push_back({ (float)x, (float)y });
				}
			}
			else
			{
				if (position != Vector2(x, y) && (tileContent[x + dim.x * y][0] == nullptr || tileContent[x + dim.x * y][1] == nullptr) && tileContent[x + dim.x * y][2] == nullptr)
				{
					possiblePositions.push_back({ (float)x, (float)y });
				}
			}
		}
	}
	if (possiblePositions.size() == 0) { return { -1,-1 }; }
	return possiblePositions[r.myRand() % possiblePositions.size()];
}


void Entity::Sense(const Grid& grid, const EntityManager& entityManager)
{
}

void Entity::Decide(Random& r, const IntVector2& dim)
{
}

void Entity::Act(Random& r, Grid& grid, const float& deltaTime, const float& timeSpeed, EntityManager& entities)
{
}

Vector2 Entity::GetPosition()
{
	return position;
}

void Entity::Render(TheEventLoopOfLife& game, Vector2 renderPosition){}
