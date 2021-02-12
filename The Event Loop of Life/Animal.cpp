#include "Animal.h"
#include "Grid.h"

Animal::Animal(const Vector2& position_in, const Vector2& health_in, const std::vector<Decal*>& sprites_in):
	Entity(position_in, health_in, sprites_in), 
	walkingPosition(position_in), destination(Vector2(-1, -1))
{
}

Vector2 Animal::GetRandomAdjacentPosition(Random& r, const Grid& grid, const int& range, const EntityType& type)
{
	int32_t index = position.x + grid.grid.x * position.y;
	int* constraints = grid.GetValidConstraints(index, range);
	std::vector<Vector2> possiblePositions = {};
	for (int y = constraints[3]; y < constraints[5]; y++)
	{
		for (int x = constraints[2]; x < constraints[4]; x++)
		{
			if (type == EntityType::SHEEP || type == EntityType::WOLF)
			{
				if (position != Vector2(x, y) && (grid.tileContent[x + grid.grid.x * y][0] == nullptr || grid.tileContent[x + grid.grid.x * y][1] == nullptr))
				{
					possiblePositions.push_back({ (float)x, (float)y });
				}
			}
			else
			{
				if (position != Vector2(x, y) && (grid.tileContent[x + grid.grid.x * y][0] == nullptr || grid.tileContent[x + grid.grid.x * y][1] == nullptr) && grid.tileContent[x + grid.grid.x * y][2] == nullptr)
				{
					possiblePositions.push_back({ (float)x, (float)y });
				}
			}
		}
	}
	if (possiblePositions.size() == 0) { return { -1,-1 }; }
	return possiblePositions[r.myRand() % possiblePositions.size()];
}

void Animal::Move(const float& deltaTime, const float& timeSpeed)
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

bool Animal::TryPursue(Vector2& destination, const Vector2& direction, Grid& grid)
{
	for (int i = 2; i > 0; i--)								//i signifies amount of steps forward
	{
		Vector2 temp = direction * i;						//multiply the direction by the amount of steps
		temp += position;									//add it to the position
		grid.ClampPositionToGrid(temp);
		int target_i = temp.x + grid.grid.x * temp.y;
		if (grid.TryOccupyNewPosition(*this, target_i))
		{
			destination = temp;
			return true;
		}
	}
	return false;
}

void Animal::Wander(Random& r, Grid& grid, const float& deltaTime, const float& timeSpeed)
{
	if (destination == Vector2{ -1, -1 })
	{
		destination = Entity::GetRandomAdjacentPosition(r, grid, entityType);
		if (destination == Vector2{ -1,-1 }) { return; }
		else
		{
			grid.tileContent[position.x + grid.grid.x * position.y][spaceOccupying] = nullptr;
			if (grid.tileContent[destination.x + grid.grid.x * destination.y][0] == nullptr) { grid.tileContent[destination.x + grid.grid.x * destination.y][0] = this; spaceOccupying = 0; }
			else { grid.tileContent[destination.x + grid.grid.x * destination.y][1] = this; spaceOccupying = 1; }
		}
	}
	Move(deltaTime, timeSpeed);
}

void Animal::Die()
{
	if (destination != Vector2(-1, -1))
	{
		position = destination;
	}
	dead = true;
}

Vector2 Animal::GetPosition()
{
	return walkingPosition;
}
