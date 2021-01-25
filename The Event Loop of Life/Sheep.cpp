#include "Entity.h"

Sheep::Sheep(const Vector2& position_in, const Vector2& health_in) : Entity(position_in, health_in),
{
	entityType = EntityType::SHEEP;
}

void Sheep::Sense(const std::vector<std::array<Entity*, 3>>& grid, const IntVector2& dim)
{
	if (destination != Vector2(-1, -1))
	{
		position = destination;
		destination = Vector2(-1, -1);
	}
	//Check if there are wolves too close. If there are, take several more wolves in a range into account.
	int32_t index = position.x + dim.x * position.y;
	int* constraints = getValidConstraints(index, dim);
	targets.clear();
	for (int y = constraints[3]; y < constraints[5]; y++)
	{
		for (int x = constraints[2]; x < constraints[4]; x++)
		{
			if (grid[x + dim.x * y][2] != nullptr) //index 2 is always grass
			{
				targets.push_back(*grid[x + dim.x * y][2]);
			}
		}
	}
	delete constraints;
}

void Sheep::Decide(Random& r)
{
	state = EntityState::PURSUE;
	//Breed if not being chased by wolves
	if (position == targetPosition && state == EntityState::PURSUE && health.x > 0)
	{
		state = EntityState::EAT;
	}
	else if (health.x > health.y * 0.75f) //If Sheep has more than 75% of health
	{
		state = EntityState::BREED;
	}
	else if (targets.size() > 0)
	{
		state = EntityState::PURSUE;
		targetPosition = targets[r.myRand() % targets.size()].position;
	}
	else if (health.x <= 0)
	{
		state = EntityState::DEATH;
	}
	else
	{
		state = EntityState::WANDER;
	}
}

void Sheep::Act(Random& r, const IntVector2& dim, const float& deltaTime, const float& timeSpeed, std::vector<std::array<Entity*, 3>>& tileContent, std::vector<Entity*>& entities)
{
	switch (state)
	{
		case EntityState::BREED:
		{
			Vector2 breedingPlace = GetRandomAdjacentPosition(r, dim, tileContent, EntityType::SHEEP);
			if (breedingPlace == Vector2{ -1,-1 }) { return; }
			else
			{
				health.x -= (health.y * 0.4f);
				Sheep* temp = new Sheep(position, { (health.y * 0.4f), health.y });
				if (tileContent[breedingPlace.x + dim.x * breedingPlace.y][0] == nullptr)
				{
					tileContent[breedingPlace.x + dim.x * breedingPlace.y][0] = temp; tileContent[breedingPlace.x + dim.x * breedingPlace.y][0]->spaceOccupying = 0;
				}
				else { tileContent[breedingPlace.x + dim.x * breedingPlace.y][1] = temp; tileContent[breedingPlace.x + dim.x * breedingPlace.y][1]->spaceOccupying = 1; }
				entities.push_back(temp);
				temp->destination = breedingPlace;
				state = EntityState::IDLE;
			}
		}break;
		case EntityState::EAT:
			health.x += 5;
			tileContent[position.x + dim.x * position.y][2]->health.x = 0;
			state = EntityState::IDLE;
			break;
		case EntityState::EVADE:
			//Go in a safe direction
			break;
		case EntityState::PURSUE:
		{
			if (destination == Vector2{ -1, -1 })
			{
				destination = targetPosition;
				if (destination == Vector2{ -1,-1 }) { return; }
				else
				{
					tileContent[position.x + dim.x * position.y][spaceOccupying] = nullptr;
					if (tileContent[destination.x + dim.x * destination.y][0] == nullptr) { tileContent[destination.x + dim.x * destination.y][0] = this; spaceOccupying = 0; }
					else { tileContent[destination.x + dim.x * destination.y][1] = this; spaceOccupying = 1; }
				}
			}
			Move(deltaTime, timeSpeed);
		}break;
		case EntityState::WANDER:
			//Go in a random direction
		{
			if (destination == Vector2{ -1, -1 })
			{
				destination = GetRandomAdjacentPosition(r, dim, tileContent, EntityType::SHEEP);
				if (destination == Vector2{ -1,-1 }) { return; }
				else
				{
					tileContent[position.x + dim.x * position.y][spaceOccupying] = nullptr;
					if (tileContent[destination.x + dim.x * destination.y][0] == nullptr) { tileContent[destination.x + dim.x * destination.y][0] = this; spaceOccupying = 0; }
					else { tileContent[destination.x + dim.x * destination.y][1] = this; spaceOccupying = 1; }
				}
			}
			Move(deltaTime, timeSpeed);
		}break;
		case EntityState::DEATH:
		{
			//play death animation
			if (destination != Vector2(-1, -1))
			{
				position = destination;
			}
			dead = true;
		}
	default: break;
	}
	health.x -= 1 / (timeSpeed / deltaTime);
}