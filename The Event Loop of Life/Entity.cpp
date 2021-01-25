#include "Entity.h"

Entity::Entity(const Vector2& position_in, const Vector2& health_in) :
	position(position_in), health(health_in), walkingPosition(position_in),
	dead(false), state(EntityState::IDLE), destination(Vector2(-1,-1))
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

Grass::Grass():Entity({-1,-1},{0,0})
{ 
	entityType = EntityType::GRASS; 
	maturity = Maturity::GROWING;
	maturityTimer = 0;
	spaceOccupying = 2;
}
Grass::Grass(const Vector2& position_in, const Vector2& health_in) : Entity(position_in, health_in) 
{ 
	entityType = EntityType::GRASS; 
	if (health.x == health.y)
	{
		maturity = Maturity::MATURE;
	}
	else
	{
		maturity = Maturity::GROWING;
	}
	maturityTimer = 0;
	spaceOccupying = 2;
}

void Grass::Sense(const std::vector<std::array<Entity*, 3>>& grid, const IntVector2& dim)
{
	//Am I being eaten or trampled?
	isBeingTrampled = (grid[position.x + dim.x * position.y][0] != nullptr || grid[position.x + dim.x * position.y][1] != nullptr);
	isBeingEaten = ((grid[position.x + dim.x * position.y][0] != nullptr && grid[position.x + dim.x * position.y][0]->entityType == EntityType::SHEEP && grid[position.x + dim.x * position.y][0]->state == EntityState::EAT) ||
				    (grid[position.x + dim.x * position.y][1] != nullptr && grid[position.x + dim.x * position.y][1]->entityType == EntityType::SHEEP && grid[position.x + dim.x * position.y][1]->state == EntityState::EAT));
	if (maturity == Maturity::GROWING && health.x >= health.y)
	{
		maturity = Maturity::MATURE;
	}
	else if (maturity == Maturity::MATURE && maturityTimer >= 1)
	{
		maturity = Maturity::WITHERING;
	}
}

void Grass::Decide(Random& r)
{
	//Should I spread to an adjacent square, or should I grow?
	if (health.x <= 0) 
	{ 
		state = EntityState::DEATH; 
	}
	else if (isBeingTrampled || isBeingEaten) { state = EntityState::IDLE; }
	else if (maturity == Maturity::MATURE)
	{ 
		state = EntityState::BREED; 
	}
}

void Grass::Act(Random& r, const IntVector2& dim, const float& deltaTime, const float& timeSpeed, std::vector<std::array<Entity*, 3>>& tileContent, std::vector<Entity*>& entities)
{
	//Grow if not being trampled, wither if it has been mature for a while. Disappear if no HP
	if (state == EntityState::BREED)
	{
		Vector2 whereToGrow = { -1,-1 };
		if (whereToGrow == Vector2{ -1, -1 })
		{
			whereToGrow = GetRandomAdjacentPosition(r, dim, tileContent, EntityType::GRASS);
			if (whereToGrow == Vector2{ -1,-1 }) { return; }
			else
			{
				Grass* temp = new Grass(whereToGrow, { (r.myRand() % 5) + 2.0f, 6.0f });
				tileContent[whereToGrow.x + dim.x * whereToGrow.y][2] = temp;
				entities.push_back(temp);
			}
		}
		state = EntityState::IDLE;
	}
	if(state == EntityState::DEATH)
	{
		dead = true;
	}
	if (maturity == Maturity::WITHERING)
	{
		health.x -= 1 / (timeSpeed / deltaTime);
	}
	else if (maturity == Maturity::GROWING)
	{
		health.x += 1 / (timeSpeed / deltaTime);
	}
	else if (maturity == Maturity::MATURE)
	{
		maturityTimer += 1 / (timeSpeed / deltaTime);
	}
}

Wolf::Wolf(const Vector2& position_in, const Vector2& health_in): Entity(position_in, health_in),
destination(-1, -1)
{
	entityType = EntityType::WOLF;
}

void Wolf::Sense(const std::vector<std::array<Entity*, 3>>& grid, const IntVector2& dim)
{
	//Check if there are wolves too close. If there are, take several more wolves in a range into account.
	int32_t index = position.x + dim.x * position.y;
	int* constraints = getValidConstraints(index, dim);
	targets.clear();
	for (int y = constraints[3]; y < constraints[5]; y++)
	{
		for (int x = constraints[2]; x < constraints[4]; x++)
		{
			//Look for grass and wolves
			if ((grid[x + dim.x * y][0] != nullptr && grid[x + dim.x * y][0]->entityType == EntityType::SHEEP))
			{
				targets.push_back(*grid[x + dim.x * y][0]);
			}
			if ((grid[x + dim.x * y][1] != nullptr && grid[x + dim.x * y][1]->entityType == EntityType::SHEEP))
			{
				targets.push_back(*grid[x + dim.x * y][1]);
			}
		}
	}
	delete constraints;
}

void Wolf::Decide(Random& r)
{
	if (position == destination && state == EntityState::PURSUE)
	{
		state == EntityState::EAT;
	}
	else if (health.x > health.y * 0.75f) //If Wolf has more than 75% of health
	{
		state = EntityState::BREED;
	}
	else if (targets.size() > 0)
	{
		state = EntityState::PURSUE;
		destination = targets[r.myRand() % targets.size()].position;
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

void Wolf::Act(Random& r, const IntVector2& dim, const float& deltaTime, const float& timeSpeed, std::vector<std::array<Entity*, 3>>& tileContent, std::vector<Entity*>& entities)
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
			Wolf* temp = new Wolf(position, { (health.y * 0.4f), health.y });
			if (tileContent[breedingPlace.x + dim.x * breedingPlace.y][0] == nullptr)
			{
				tileContent[breedingPlace.x + dim.x * breedingPlace.y][0] = temp; tileContent[breedingPlace.x + dim.x * breedingPlace.y][0]->spaceOccupying = 0;
			}
			else { tileContent[breedingPlace.x + dim.x * breedingPlace.y][1] = temp; tileContent[breedingPlace.x + dim.x * breedingPlace.y][1]->spaceOccupying = 1; }
			entities.push_back(temp);
			temp->destination = breedingPlace;
			state = EntityState::IDLE;
		}
	}
	break;
	case EntityState::EAT:
		health.x += 5;
		destination = Vector2(-1, -1);
		//tileContent[position.x + dim.x * position.y][2]->health.x = 0;
		state = EntityState::IDLE;
		break;
	case EntityState::EVADE:
		//Go in a safe direction
		break;
	case EntityState::PURSUE:
		{
			Vector2 movementVector = (destination - position) / (timeSpeed / deltaTime) * 2;
			walkingPosition = //Check that the distance to the walkingPosition isn't bigger than the distance to the destination
				std::abs(((walkingPosition + movementVector) - position).x) >= std::abs((destination - position).x) &&
				std::abs(((walkingPosition + movementVector) - position).y) >= std::abs((destination - position).y) ?
				destination : walkingPosition + movementVector;
			if (walkingPosition == destination)
			{
				position = destination;
			}
		}
		break;
	case EntityState::WANDER:
		//Go in a random direction
	{
		if (destination == Vector2{ -1, -1 })
		{
			destination = GetRandomAdjacentPosition(r, dim, tileContent, EntityType::WOLF);
			if (destination == Vector2{ -1,-1 }) { return; }
			else
			{
				tileContent[position.x + dim.x * position.y][spaceOccupying] = nullptr;
				if (tileContent[destination.x + dim.x * destination.y][0] == nullptr) { tileContent[destination.x + dim.x * destination.y][0] = this; spaceOccupying = 0; }
				else { tileContent[destination.x + dim.x * destination.y][1] = this; spaceOccupying = 1; }
			}
		}
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
