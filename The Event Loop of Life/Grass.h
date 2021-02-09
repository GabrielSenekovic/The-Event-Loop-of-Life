#ifndef GRASS_H
#define GRASS_H

#include "Entity.h"

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
	void Sense(const Grid& grid) override; //Am I being eaten or trampled by something?
	void Decide(Random& r, const IntVector2& dim) override; //Should I spread to an adjacent square, or should I grow?
	void Act(Random& r, Grid& grid, const float& deltaTime, const float& timeSpeed, std::vector<Entity*>& entities); //Grow if not being trampled, wither if it has been mature for a while. Disappear if no HP

	void Render(TheEventLoopOfLife& game, Vector2 position) override;
};

#endif