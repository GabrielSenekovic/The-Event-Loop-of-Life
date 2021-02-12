#ifndef ENTITY_H
#define ENTITY_H

#include <cstdint>
#include "olcPixelGameEngine.h"
using namespace olc;

#include "Random.h"
#include <math.h>

#define Vector2 vf2d
#define IntVector2 vi2d

class TheEventLoopOfLife;
class EntityManager;
class Grid;

class Entity
{
protected:
	enum class EntityState
	{
		EVADE,
		EAT,
		BREED,
		PURSUE,
		WANDER,
		DEATH,
		IDLE //used only for example, when sheep have bred once to prevent them from breeding eternally
	};
public:
	enum class EntityType
	{
		GRASS,
		SHEEP,
		WOLF
	};
	Entity(const Vector2& position_in, const Vector2& health_in, const std::vector<Decal*>& sprites_in);
	Vector2 position;
	Vector2 health; //x = current health, y = max health
	EntityType entityType;
	EntityState state;
	std::vector<Decal*> sprites;
	uint8_t spaceOccupying; //0 = first, 1 = second
	uint32_t handle;
	bool dead; //flag for program to delete the entity

	Vector2 GetRandomAdjacentPosition(Random& r, const Grid& grid);
	Vector2 GetRandomAdjacentPosition(Random& r, const Grid& grid, const EntityType& type);

	virtual void Sense(const Grid& grid, const EntityManager& entityManager);
	virtual void Decide(Random& r, const Grid& grid);
	virtual void Act(Random& r, Grid& grid, const float& deltaTime, const float& timeSpeed, EntityManager& entities);

	virtual Vector2 GetPosition();

	virtual void Render(TheEventLoopOfLife& game, Vector2 position);
};
#endif