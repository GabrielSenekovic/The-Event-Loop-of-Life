#include "Entity.h"
#include "TheEventLoopOfLife.h"

Sheep::Sheep(const Vector2& position_in, const Vector2& health_in, const std::vector<Decal*>& sprites_in) : Animal(position_in, health_in, sprites_in),
	renderState(EntityState::IDLE)
{
	entityType = EntityType::SHEEP;
}

void Sheep::Sense(const std::vector<std::array<Entity*, 3>>& grid, const IntVector2& dim)
{
	if (destination != Vector2(-1, -1))
	{
		position = destination;
		//destination = Vector2(-1, -1);
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
	constraints = getValidConstraints(index, 2, dim);
	threats.clear();
	for (int y = constraints[3]; y < constraints[5]; y++)
	{
		for (int x = constraints[2]; x < constraints[4]; x++)
		{
			if ((grid[x + dim.x * y][0] != nullptr && grid[x + dim.x * y][0]->entityType == EntityType::WOLF))
			{
				threats.push_back(grid[x + dim.x * y][0]);
			}
			if ((grid[x + dim.x * y][1] != nullptr && grid[x + dim.x * y][1]->entityType == EntityType::WOLF))
			{
				threats.push_back(grid[x + dim.x * y][1]);
			}
		}
	}
	delete constraints;
}

void Sheep::Decide(Random& r, const IntVector2& dim)
{
	if (health.x <= 0) 
	{ 
		state = EntityState::DEATH; 
		renderState = EntityState::DEATH;
		return; 
	}
	if (threats.size() > 0)
	{
		state = EntityState::EVADE;
		renderState = EntityState::EVADE;
		Vector2 averageDirection;
		for (int i = 0; i < threats.size(); i++)
		{
			averageDirection += position - threats[i]->position;
		}
		averageDirection /= threats.size();
		//averageDirection *= -1; //This only gives us the direction to move in, not the position to move to
		if (averageDirection.mag() == 0)
		{
			state = EntityState::IDLE;
			return;
		}
		averageDirection = averageDirection.norm();
		averageDirection = Vector2(averageDirection.x < 0 ? floor(averageDirection.x) : ceil(averageDirection.x), 
								   averageDirection.y < 0 ? floor(averageDirection.y) : ceil(averageDirection.y));
		targetPosition = position + averageDirection;
		targetPosition = Vector2(targetPosition.x < 0 ? 0 : targetPosition.x >= dim.x ? dim.x -1 : targetPosition.x,
								 targetPosition.y < 0 ? 0 : targetPosition.y >= dim.x ? dim.y -1: targetPosition.y);
	}
	else if (position == destination && state == EntityState::PURSUE)
	{
		state = EntityState::EAT;
		renderState = EntityState::EAT;
	}
	/*else if (health.x > health.y * 0.75f) //If Sheep has more than 75% of health
	{
		state = EntityState::BREED;
		renderState = EntityState::BREED;
	}*/
	else if (targets.size() > 0 && health.x < health.y)
	{
		state = EntityState::PURSUE;
		renderState = EntityState::PURSUE;
		targetPosition = targets[r.myRand() % targets.size()].position;
	}
	else
	{
		state = EntityState::WANDER;
		renderState = EntityState::WANDER;
	}
}

void Sheep::Act(Random& r, const IntVector2& dim, const float& deltaTime, const float& timeSpeed, std::vector<std::array<Entity*, 3>>& tileContent, std::vector<Entity*>& entities)
{
	switch (state)
	{
		case EntityState::BREED:
		{
			Vector2 breedingPlace = Entity::GetRandomAdjacentPosition(r, dim, tileContent, EntityType::SHEEP);
			if (breedingPlace == Vector2{ -1,-1 }) { return; }
			else
			{
				health.x -= (health.y * 0.5f);
				Sheep* temp = new Sheep(position, { (health.y * 0.3f), health.y }, sprites);
				entities.push_back(temp);
				state = EntityState::IDLE;
			}
		}break;
		case EntityState::EAT:
			if (tileContent[position.x + dim.x * position.y][2] != nullptr) //Because sometimes the sheep might try to eat grass that another sheep just ate
			{
				health.x += 5;
				tileContent[position.x + dim.x * position.y][2]->health.x = 0;
			}
			state = EntityState::IDLE;
			break;
		case EntityState::EVADE:
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
					else if (tileContent[destination.x + dim.x * destination.y][1] == nullptr) { tileContent[destination.x + dim.x * destination.y][1] = this; spaceOccupying = 1; }
					else
					{
						tileContent[position.x + dim.x * position.y][spaceOccupying] = this;
						destination = Vector2(-1, -1);
						state = EntityState::IDLE;
						return;
					}
				}
			}
			Move(deltaTime, timeSpeed);
		}break;
		case EntityState::WANDER:
			//Go in a random direction
		{
			if (destination == Vector2{ -1, -1 })
			{
				destination = Entity::GetRandomAdjacentPosition(r, dim, tileContent, EntityType::SHEEP);
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
	//health.x -= 1 / (timeSpeed / deltaTime);
}

void Sheep::Render(TheEventLoopOfLife& game, Vector2 renderPosition)
{
	Vector2 emotePosition = { renderPosition.x - 0.8f, renderPosition.y - 0.8f };
	switch (renderState)
	{
		case EntityState::IDLE:
		case EntityState::WANDER:
		{
			game.DrawDecal(renderPosition, sprites[0], { 1,1 }, olc::WHITE);
			game.DrawDecal(emotePosition, sprites[2], { 1,1 }, olc::WHITE);
			break;
		}
		case EntityState::PURSUE:
		{
			game.DrawDecal(renderPosition, sprites[0], { 1,1 }, olc::WHITE);
			game.DrawDecal(emotePosition, sprites[3], { 1,1 }, olc::RED);
			break;
		}
		case EntityState::EAT:
		{
			game.DrawDecal(renderPosition, sprites[1], { 1,1 }, olc::WHITE);
			break;
		}
		case EntityState::BREED:
		{
			game.DrawDecal(renderPosition, sprites[0], { 1,1 }, olc::WHITE);
			game.DrawDecal(emotePosition, sprites[4], { 1,1 }, olc::WHITE);
			break;
		}
		case EntityState::DEATH:
		{
			game.DrawDecal(renderPosition, sprites[0], { 1,1 }, olc::DARK_GREY);
			break;
		}
		case EntityState::EVADE:
		{
			game.DrawDecal(renderPosition, sprites[0], { 1,1 }, olc::WHITE);
			game.DrawDecal(emotePosition, sprites[3], { 1,1 }, olc::YELLOW);
			break;
		}
	}
}
