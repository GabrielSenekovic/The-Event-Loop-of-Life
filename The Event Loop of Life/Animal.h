#ifndef ANIMAL_H
#define ANIMAL_H

#include "Entity.h"

class Animal : public Entity
{
public:
	Vector2 walkingPosition; //The renderedPosition
	Vector2 destination;

	Animal(const Vector2& position_in, const Vector2& health_in, const std::vector<Decal*>& sprites_in);

	Vector2 GetRandomAdjacentPosition(Random& r, const IntVector2& dim, const int& range, const std::vector<std::array<Entity*, 3>>& tileContent, const EntityType& type);

	void Move(const float& deltaTime, const float& timeSpeed);

	Vector2 GetPosition() override;
};

#endif