#include "Entity.h"
#include "TheEventLoopOfLife.h"

Entity::Entity(const Vector2& position_in, const Vector2& health_in, const std::vector<Decal*>& sprites_in) :
	position(position_in), health(health_in), walkingPosition(position_in),
	dead(false), state(EntityState::IDLE), destination(Vector2(-1,-1)), sprites(sprites_in)
{
}

void Entity::Sense(const std::vector<std::array<Entity*, 3>>& grid, const IntVector2& dim)
{
}

void Entity::Decide(Random& r)
{
}

void Entity::Act(Random& r, const IntVector2& dim, const float& deltaTime, const float& timeSpeed, std::vector<std::array<Entity*, 3>>& tileContent, std::vector<Entity*>& entities)
{
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
				if (position != Vector2(x,y) && (tileContent[x + dim.x * y][0] == nullptr || tileContent[x + dim.x * y][1] == nullptr))
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

void Entity::Move(const float& deltaTime, const float& timeSpeed)
{
	Vector2 movementVector = (destination - position) / (timeSpeed / deltaTime) * 2;
	walkingPosition = //Check that the distance to the walkingPosition isn't bigger than the distance to the destination
		std::abs(((walkingPosition + movementVector) - position).x) >= std::abs((destination - position).x) &&
		std::abs(((walkingPosition + movementVector) - position).y) >= std::abs((destination - position).y) ?
		destination : walkingPosition + movementVector;
	if (walkingPosition == destination)
	{
		position = destination;
		destination = { -1,-1 };
	}
}

void Entity::Render(TheEventLoopOfLife& game, Vector2 renderPosition){}