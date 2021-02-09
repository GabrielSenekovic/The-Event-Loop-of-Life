#ifndef WOLF_H
#define WOLF_H

#include "Animal.h"

class Wolf :public Animal
{
	EntityState renderState;
public:
	Wolf(const Vector2& position_in, const Vector2& health_in, const std::vector<Decal*>& sprites_in);
	std::vector<Entity*> targets;
	uint32_t target_handle; //handle
	Entity* target;
	void Sense(const Grid& grid, const EntityManager& entityManager) override;
	void Decide(Random& r, const IntVector2& dim) override;
	void Act(Random& r, Grid& grid, const float& deltaTime, const float& timeSpeed, std::vector<Entity*>& entities) override;

	void Render(TheEventLoopOfLife& game, Vector2 position) override;
};

#endif