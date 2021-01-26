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
	int* getValidConstraints(int i, olc::vi2d grid)
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
public:
	enum class EntityType
	{
		GRASS,
		SHEEP,
		WOLF
	};
	Entity(const Vector2& position_in, const Vector2& health_in, const std::vector<Decal*>& sprites_in);
	Vector2 position;
	Vector2 walkingPosition; //The renderedPosition
	Vector2 destination;
	Vector2 health; //x = current health, y = max health
	EntityType entityType;
	EntityState state;
	std::vector<Decal*> sprites;
	uint8_t spaceOccupying; //0 = first, 1 = second
	bool dead; //flag for program to delete the entity
	virtual void Sense(const std::vector<std::array<Entity*, 3>>& grid, const IntVector2& dim);
	virtual void Decide(Random& r);
	virtual void Act(Random& r, const IntVector2& dim, const float& deltaTime, const float& timeSpeed, std::vector<std::array<Entity*, 3>>& tileContent, std::vector<Entity*>& entities);

	Vector2 GetRandomAdjacentPosition(Random& r, const IntVector2& dim);
	Vector2 GetRandomAdjacentPosition(Random& r, const IntVector2& dim, const std::vector<std::array<Entity*, 3>>& tileContent, const EntityType& type);

	void Move(const float& deltaTime, const float& timeSpeed);

	virtual void Render(TheEventLoopOfLife& game, Vector2 position);
};
class Grass :public Entity
{
public:
	enum class Maturity
	{
		GROWING,
		MATURE,
		WITHERING
	};
	Grass();
	Grass(const Vector2& position_in, const Vector2& health_in, const std::vector<Decal*>& sprites_in);
	Maturity maturity;
	float maturityTimer;
	bool isBeingTrampled;
	bool isBeingEaten;
	void Sense(const std::vector<std::array<Entity*, 3>>& grid, const IntVector2& dim) override; //Am I being eaten or trampled by something?
	void Decide(Random& r) override; //Should I spread to an adjacent square, or should I grow?
	void Act(Random& r, const IntVector2& dim, const float& deltaTime, const float& timeSpeed, std::vector<std::array<Entity*, 3>>& tileContent, std::vector<Entity*>& entities); //Grow if not being trampled, wither if it has been mature for a while. Disappear if no HP

	void Render(TheEventLoopOfLife& game, Vector2 position) override;
};
class Sheep :public Entity
{
	EntityState renderState;
public:
	Sheep(const Vector2& position_in, const Vector2& health_in, const std::vector<Decal*>& sprites_in);
	std::vector<Entity> targets;
	Vector2 targetPosition;
	void Sense(const std::vector<std::array<Entity*, 3>>& grid, const IntVector2& dim) override; 
	void Decide(Random& r) override; 
	void Act(Random& r, const IntVector2& dim, const float& deltaTime, const float& timeSpeed, std::vector<std::array<Entity*, 3>>& tileContent, std::vector<Entity*>& entities) override;

	void Render(TheEventLoopOfLife& game, Vector2 position) override;
};
class Wolf :public Entity
{
	EntityState renderState;
public:
	Wolf(const Vector2& position_in, const Vector2& health_in, const std::vector<Decal*>& sprites_in);
	std::vector<Entity> targets;
	Vector2 targetPosition;
	void Sense(const std::vector<std::array<Entity*, 3>>& grid, const IntVector2& dim) override;
	void Decide(Random& r) override;
	void Act(Random& r, const IntVector2& dim, const float& deltaTime, const float& timeSpeed, std::vector<std::array<Entity*, 3>>& tileContent, std::vector<Entity*>& entities) override;

	void Render(TheEventLoopOfLife& game, Vector2 position) override;
};
class Crow : Entity
{

};
#endif