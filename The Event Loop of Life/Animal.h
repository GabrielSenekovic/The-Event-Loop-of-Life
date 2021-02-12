#ifndef ANIMAL_H
#define ANIMAL_H

#include "Entity.h"

class Animal : public Entity
{
public:
	Vector2 walkingPosition; //The renderedPosition
	Vector2 destination;

	Animal(const Vector2& position_in, const Vector2& health_in, const std::vector<Decal*>& sprites_in);

	Vector2 GetRandomAdjacentPosition(Random& r, const Grid& grid, const int& range, const EntityType& type);

	void Move(const float& deltaTime, const float& timeSpeed);
	bool TryPursue(Vector2& destination, const Vector2& direction, Grid& grid);
	void Wander(Random& r, Grid& grid, const float& deltaTime, const float& timeSpeed);
	void Die();

	Vector2 GetPosition() override;
};

#endif