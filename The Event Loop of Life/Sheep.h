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

	void Sense(const std::vector<std::array<Entity*, 3>>& grid, const IntVector2& dim) override;
	void Decide(Random& r, const IntVector2& dim) override;
	void Act(Random& r, const IntVector2& dim, const float& deltaTime, const float& timeSpeed, std::vector<std::array<Entity*, 3>>& tileContent, std::vector<Entity*>& entities) override;

	void Render(TheEventLoopOfLife& game, Vector2 position) override;
};

#endif