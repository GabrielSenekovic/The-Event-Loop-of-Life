#ifndef SHEEP_H
#define SHEEP_H

#include "Animal.h"

class Sheep :public Animal
{
	EntityState renderState;
public:
	std::vector<Entity> targets;
	std::vector<Entity*> threats;
	Vector2 targetPosition;

	Sheep(const Vector2& position_in, const Vector2& health_in, const std::vector<Decal*>& sprites_in);

	void Sense(const Grid& grid, const EntityManager& entityManager) override;
	void Decide(Random& r, const Grid& grid) override;
	void Act(Random& r, Grid& grid, const float& deltaTime, const float& timeSpeed, EntityManager& entities) override;

	void Render(TheEventLoopOfLife& game, Vector2 position) override;
};

#endif