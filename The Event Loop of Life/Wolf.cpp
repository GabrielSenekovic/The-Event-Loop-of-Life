#include "Wolf.h"
#include "TheEventLoopOfLife.h"

Wolf::Wolf(const Vector2& position_in, const Vector2& health_in, const std::vector<Decal*>& sprites_in) : Animal(position_in, health_in, sprites_in),
	renderState(EntityState::IDLE)
{
	entityType = EntityType::WOLF;
}

void Wolf::Sense(const Grid& grid, const EntityManager& entityManager)
{
	if (destination != Vector2(-1, -1)) { position = destination; }
	target = nullptr;
	//get target from handle
	if (target == nullptr)
	{
		int32_t index = position.x + grid.grid.x * position.y;
		int* constraints = getValidConstraints(index, 2, grid.grid);
		targets.clear();
		for (int y = constraints[3]; y < constraints[5]; y++)
		{
			for (int x = constraints[2]; x < constraints[4]; x++)
			{
				int i = grid.GetTileIndexOfEntity(x, y, EntityType::SHEEP);
				if (i != -1) 
				{ 
					targets.push_back(grid.tileContent[x + grid.grid.x * y][i]); 
				}
			}
		}
		delete constraints;
	}
}

void Wolf::Decide(Random& r, const IntVector2& dim)
{
	if (health.x <= 0)
	{
		//state = EntityState::DEATH;
		//renderState = EntityState::DEATH;
		//return;
	}
	//Breed if not being chased by wolves
	if (target != nullptr && position == target->position)
	{
		state = EntityState::EAT;
		renderState = EntityState::EAT;
	}
	else if (health.x > health.y * 0.75f) //If Sheep has more than 75% of health
	{
		state = EntityState::BREED;
		renderState = EntityState::BREED;
	}
	else if (targets.size() > 0 && health.x < health.y)
	{
		state = EntityState::PURSUE;
		renderState = EntityState::PURSUE;
		target = targets[r.myRand() % targets.size()];
	}
	else
	{
		state = EntityState::WANDER;
		renderState = EntityState::WANDER;
	}
}

void Wolf::Act(Random& r, Grid& grid, const float& deltaTime, const float& timeSpeed, std::vector<Entity*>& entities)
{
	switch (state)
	{
	case EntityState::BREED:
	{
		Vector2 breedingPlace = Entity::GetRandomAdjacentPosition(r, grid.grid, grid.tileContent, EntityType::WOLF);
		if (breedingPlace == Vector2{ -1,-1 }) { return; }
		else
		{
			health.x -= (health.y * 0.4f);
			Wolf* temp = new Wolf(position, { (health.y * 0.4f), health.y }, sprites);
			entities.push_back(temp);
			state = EntityState::IDLE;
		}
	}break;
	case EntityState::EAT:
	{
		int placementOfPrey = spaceOccupying == 0 ? 1 : 0;
		if (grid.tileContent[position.x + grid.grid.x * position.y][placementOfPrey] != nullptr)
		{
			health.x += 5;
			grid.tileContent[position.x + grid.grid.x * position.y][placementOfPrey]->health.x = 0;
		}
		state = EntityState::IDLE;
		target = nullptr;
	}
	break;
	case EntityState::PURSUE:
	{
		if (destination == Vector2{ -1, -1 } && target != nullptr && target->position != Vector2{ -1,-1 })
		{
			Vector2 movementDirection = target->position - position;
			if (movementDirection.mag() == 0)
			{
				state = EntityState::IDLE;
				return;
			}
			movementDirection = movementDirection.norm();
			movementDirection = Vector2(movementDirection.x < 0 ? floor(movementDirection.x) : ceil(movementDirection.x),
										movementDirection.y < 0 ? floor(movementDirection.y) : ceil(movementDirection.y));
			if (!TryPursue(destination, movementDirection, grid))
			{
				state = EntityState::IDLE;
				return;
			}
		}
		Move(deltaTime, timeSpeed);
	}break;
	case EntityState::WANDER:{Wander(r, grid, deltaTime, timeSpeed); }break;
	case EntityState::DEATH: {Die(); }break;
	default: break;
	}
	//health.x -= 1 / (timeSpeed / deltaTime);
}

void Wolf::Render(TheEventLoopOfLife& game, Vector2 renderPosition)
{
	Vector2 emotePosition = { renderPosition.x - 0.8f, renderPosition.y - 0.8f };
	switch (renderState)
	{
		case EntityState::IDLE:
		case EntityState::WANDER:
		{
			game.DrawDecal(renderPosition, sprites[0], { 1,1 }, olc::WHITE);
			game.DrawDecal(emotePosition, sprites[1], { 1,1 }, olc::WHITE);
			break;
		}
		case EntityState::PURSUE:
		{
			game.DrawDecal(renderPosition, sprites[0], { 1,1 }, olc::WHITE);
			game.DrawDecal(emotePosition, sprites[2], { 1,1 }, olc::RED);
			break;
		}
		case EntityState::EAT:
		{
			game.DrawDecal(renderPosition, sprites[0], { 1,1 }, olc::WHITE);
			break;
		}
		case EntityState::BREED:
		{
			game.DrawDecal(renderPosition, sprites[0], { 1,1 }, olc::WHITE);
			game.DrawDecal(emotePosition, sprites[3], { 1,1 }, olc::WHITE);
			break;
		}
		case EntityState::DEATH:
		{
			game.DrawDecal(renderPosition, sprites[0], { 1,1 }, olc::DARK_GREY);
			break;
		}
	}
}
