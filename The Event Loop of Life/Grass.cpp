#include "Grass.h"
#include "Grid.h"

Grass::Grass() :Entity({ -1,-1 }, { 0,0 }, {})
{
	entityType = EntityType::GRASS;
	maturity = Maturity::GROWING;
	maturityTimer = 0;
	spaceOccupying = 2;
}
Grass::Grass(const Vector2& position_in, const Vector2& health_in, const std::vector<Decal*>& sprites_in) : Entity(position_in, health_in, sprites_in)
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

void Grass::Sense(const Grid& grid)
{
	//Am I being eaten or trampled?
	isBeingTrampled = (grid.tileContent[position.x + grid.grid.x * position.y][0] != nullptr || grid.tileContent[position.x + grid.grid.x * position.y][1] != nullptr);
	isBeingEaten = ((grid.tileContent[position.x + grid.grid.x * position.y][0] != nullptr && grid.tileContent[position.x + grid.grid.x * position.y][0]->entityType == EntityType::SHEEP && grid.tileContent[position.x + grid.grid.x * position.y][0]->state == EntityState::EAT) ||
		(grid.tileContent[position.x + grid.grid.x * position.y][1] != nullptr && grid.tileContent[position.x + grid.grid.x * position.y][1]->entityType == EntityType::SHEEP && grid.tileContent[position.x + grid.grid.x * position.y][1]->state == EntityState::EAT));
	if (maturity == Maturity::GROWING && health.x >= health.y)
	{
		maturity = Maturity::MATURE;
	}
	else if (maturity == Maturity::MATURE && maturityTimer >= 1)
	{
		maturity = Maturity::WITHERING;
	}
}

void Grass::Decide(Random& r, const IntVector2& dim)
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

void Grass::Act(Random& r, Grid& grid, const float& deltaTime, const float& timeSpeed, std::vector<Entity*>& entities)
{
	//Grow if not being trampled, wither if it has been mature for a while. Disappear if no HP
	if (state == EntityState::BREED)
	{
		Vector2 whereToGrow = { -1,-1 };
		if (whereToGrow == Vector2{ -1, -1 })
		{
			whereToGrow = GetRandomAdjacentPosition(r, grid.grid, grid.tileContent, EntityType::GRASS);
			if (whereToGrow == Vector2{ -1,-1 }) { return; }
			else
			{
				Grass* temp = new Grass(whereToGrow, { (r.myRand() % 5) + 2.0f, 6.0f }, sprites);
				grid.tileContent[whereToGrow.x + grid.grid.x * whereToGrow.y][2] = temp;
				entities.push_back(temp);
			}
		}
		state = EntityState::IDLE;
	}
	if (state == EntityState::DEATH)
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

void Grass::Render(TheEventLoopOfLife& game, Vector2 renderPosition)
{
}