#include "EntityManager.h"

EntityManager::EntityManager():nextHandle(0)
{
}

void EntityManager::Add(Entity* entity)
{
	entities.push_back(entity);
	entity->handle = nextHandle;
	nextHandle++;
}

Entity* EntityManager::GetEntity(const uint32_t& i)
{
	return entities[i];
}

Entity* EntityManager::GetEntityFromHandle(const uint32_t& i) const
{
	for (int j = 0; j < entities.size(); j++)
	{
		if (entities[j]->handle == i)
		{
			return entities[j];
		}
	}
	return nullptr;
}

void EntityManager::UpdateBoid(const int& i)
{
	olc::vf2d previousVelocity = { 0, -1 };
	int j = 0;
	int neighbors = 0;
	olc::vf2d averagePosition = { 0,0 };
	olc::vf2d averageVelocity = { 0,0 };
	olc::vf2d averageDistance = { 0,0 };
	olc::vf2d netSteeringForce = { 0,0 };
	olc::vf2d d;
	bool inView = false;

	for (j = 1; j < crows.size(); j++)
	{
		if (i != j)
		{
			d = crows[j].position - crows[i].position;

			inView = d.mag() < crows[i].cohesionRadius;

			if (inView)
			{
				if (crows[j].position.x != 0 && crows[j].position.y != 0)
				{
					averagePosition += crows[j].position == olc::vf2d{ 0, 0 } ? olc::vf2d{ 0, 0 } : crows[j].position;
				}
				if (d.mag() < crows[i].alignmentRadius)
				{
					if (crows[j].velocity.x != 0 && crows[j].velocity.y != 0)
					{
						averageVelocity += crows[j].velocity.norm();
					}
				}
				if (d.mag() <= crows[i].separationRadius)
				{
					averageDistance += d == olc::vf2d{ 0, 0 } ? olc::vf2d{ 0, 0 } : d.norm();
				}
				neighbors++;
			}
		}
	}
	if (neighbors > 0)
	{
		if (crows[i].cohesion)
		{
			//Cohesion
			olc::vf2d temp = averagePosition / neighbors;
			netSteeringForce += (temp - crows[i].position).norm();
		}
		if (crows[i].alignment)
		{
			//Alignment
			olc::vf2d temp = averageVelocity / neighbors;
			netSteeringForce += temp;
		}
		if (crows[i].separation)
		{
			//Separation
			olc::vf2d temp = averageDistance / neighbors * 60;
			netSteeringForce -= temp;
		}
	}
	crows[i].velocity += netSteeringForce;
	if (crows[i].velocity.x > 200)
	{
		crows[i].velocity.x = 200;
	}
	if (crows[i].velocity.y > 200)
	{
		crows[i].velocity.y = 200;
	}
	if (crows[i].velocity.x < -200)
	{
		crows[i].velocity.x = -200;
	}
	if (crows[i].velocity.y < -200)
	{
		crows[i].velocity.y = -200;
	}
	olc::vf2d newVelocity = crows[i].velocity.norm();
	float newAngle = atan2(previousVelocity.x * newVelocity.y - previousVelocity.y * newVelocity.x, previousVelocity.dot(newVelocity));
	crows[i].angle = newAngle;
}

size_t EntityManager::Size()
{
	return entities.size();
}
