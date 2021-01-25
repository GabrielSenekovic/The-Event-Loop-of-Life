#ifndef ENTITY_H
#define ENTITY_H

#include <cstdint>
#include "olcPixelGameEngine.h"
#include "Random.h"
#include <math.h>

#define Vector2 olc::vf2d
#define IntVector2 olc::vi2d

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
	Entity(const Vector2& position_in, const Vector2& health_in);
	Vector2 position;
	Vector2 walkingPosition; //The renderedPosition
	Vector2 destination;
	Vector2 health; //x = current health, y = max health
	EntityType entityType;
	EntityState state;
	uint8_t spaceOccupying; //0 = first, 1 = second
	bool dead; //flag for program to delete the entity
	virtual void Sense(const std::vector<std::array<Entity*, 3>>& grid, const IntVector2& dim);
	virtual void Decide(Random& r);
	virtual void Act(Random& r, const IntVector2& dim, const float& deltaTime, const float& timeSpeed, std::vector<std::array<Entity*, 3>>& tileContent, std::vector<Entity*>& entities);

	Vector2 GetRandomAdjacentPosition(Random& r, const IntVector2& dim);
	Vector2 GetRandomAdjacentPosition(Random& r, const IntVector2& dim, const std::vector<std::array<Entity*, 3>>& tileContent, const EntityType& type);

	void Move(const float& deltaTime, const float& timeSpeed);
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
	Grass(const Vector2& position_in, const Vector2& health_in);
	Maturity maturity;
	float maturityTimer;
	bool isBeingTrampled;
	bool isBeingEaten;
	void Sense(const std::vector<std::array<Entity*, 3>>& grid, const IntVector2& dim) override; //Am I being eaten or trampled by something?
	void Decide(Random& r) override; //Should I spread to an adjacent square, or should I grow?
	void Act(Random& r, const IntVector2& dim, const float& deltaTime, const float& timeSpeed, std::vector<std::array<Entity*, 3>>& tileContent, std::vector<Entity*>& entities); //Grow if not being trampled, wither if it has been mature for a while. Disappear if no HP
};
class Sheep :public Entity
{
	//Make 10 sheep on random positions
	//Sheep can share a square with a wolf or other sheep
	//They gradually move in a direction with a speed each tick
public:
	Sheep(const Vector2& position_in, const Vector2& health_in);
	std::vector<Entity> targets;
	Vector2 targetPosition;
	void Sense(const std::vector<std::array<Entity*, 3>>& grid, const IntVector2& dim) override; //Check if there are wolves too close. If there are, take several more wolves in a range into account.
	void Decide(Random& r) override; //Decide if should what state to be in
	void Act(Random& r, const IntVector2& dim, const float& deltaTime, const float& timeSpeed, std::vector<std::array<Entity*, 3>>& tileContent, std::vector<Entity*>& entities) override;
	//If there are wolves nearby, take all wolves into consideration when deciding where to go
	//If sheep is on a grassy square, reduce health of grass and increase owns health
	//If healthy, sheep spawns another sheep and loses health. The other sheep is also fairly weak.
	//If sheep can see grass and sheep is hungry, sheep will move toward that grass
	//If the sheep can see no grass, it randomly walks around
	//No matter what, each tic subtracts a little from HP
	//Die if HP reaches zero
};
class Wolf :public Entity
{
public:
	Wolf(const Vector2& position_in, const Vector2& health_in);
	Vector2 destination;
	std::vector<Entity> targets;
	void Sense(const std::vector<std::array<Entity*, 3>>& grid, const IntVector2& dim) override; //Check if there are sheep closeby
	void Decide(Random& r) override;
	void Act(Random& r, const IntVector2& dim, const float& deltaTime, const float& timeSpeed, std::vector<std::array<Entity*, 3>>& tileContent, std::vector<Entity*>& entities) override;
	//If a wolf is close enough to a sheep, eat the sheep. Sheep loses some of its health, and wolf gains it
	//If wolf is mature and healthy, wolf creates a new wolf in an adjacent square. Both have low health.
	//If wolf can see sheep nearby and is hungry, wolf will pursue it
	//If wolf can see no sheep, it randomly wanders around
	//Wolfs health keeps depleting over time
	//Die if HP reaches zero
};
class Crow : Entity
{

};
#endif