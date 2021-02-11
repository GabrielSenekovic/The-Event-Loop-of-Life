#include "EntityManager.h"

EntityManager::EntityManager():nextHandle(0)
{
}

void EntityManager::Initialize(Random& r, Grid& grid)
{
	sheep = new Decal(new Sprite("./Assets/Sheep.png"));
	sheep_eating = new Decal(new Sprite("./Assets/Sheep_Eating.png"));
	wolf = new Decal(new Sprite("./Assets/Wolf.png"));
	grass = new Decal(new Sprite("./Assets/Grass.png"));
	grass_growing = new Decal(new Sprite("./Assets/Grass_Growing.png"));
	wander = new Decal(new Sprite("./Assets/Spotted.png"));
	pursue = new Decal(new Sprite("./Assets/Discovered.png"));
	breed = new Decal(new Sprite("./Assets/Breed.png"));
	crow = new Decal(new Sprite("./Assets/Crow.png"));

	for (size_t i = 0; i < 50; i++)
	{
		Vector2 temp = { -1,-1 };
		while (temp == Vector2(-1, -1) || grid.tileContent[temp.x + grid.grid.x * temp.y][2] != nullptr /*|| grid.tileTraversibility[temp.x + grid.grid.x * temp.y] == 2*/)
		{
			temp = { (float)(r.myRand() % grid.grid.x) ,(float)(r.myRand() % grid.grid.y) };
		}
		Grass* temp_grass = new Grass(temp, { (r.myRand() % 5) + 2.0f, 6.0f }, { grass, grass_growing });
		Add(temp_grass);
		grid.tileContent[temp.x + grid.grid.x * temp.y][2] = temp_grass;
	}
	for (size_t i = 0; i < 10; i++)
	{
		Sheep* temp_sheep = new Sheep({ (float)(r.myRand() % grid.grid.x), (float)(r.myRand() % grid.grid.y) }, 20, { sheep, sheep_eating, wander, pursue, breed });
		Add(temp_sheep);
		grid.PlaceEntityOnGrid(temp_sheep);
		Wolf* temp_wolf = new Wolf({ (float)(r.myRand() % grid.grid.x), (float)(r.myRand() % grid.grid.y) }, 20, { wolf, wander, pursue, breed });
		Add(temp_wolf);
		grid.PlaceEntityOnGrid(temp_wolf);
	}
	for (int i = 0; i < 50; i++)
	{
		crows.push_back(Crow(grid.grid));
	}
}

EntityManager::~EntityManager()
{
	DEL(sheep);
	DEL(wander);
	DEL(pursue);
	DEL(breed);
	DEL(sheep_eating);
	DEL(wolf);
	DEL(grass);
	DEL(grass_growing);
	DEL(crow);
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
	Vector2 previousVelocity = { 0, -1 };
	int neighbors = 0;
	Vector2 averagePosition = { 0,0 };
	Vector2 averageVelocity = { 0,0 };
	Vector2 averageDistance = { 0,0 };
	Vector2 netSteeringForce = { 0,0 };
	Vector2 d;
	bool inView = false;

	for (int j = 1; j < crows.size(); j++)
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
			Vector2 temp = averagePosition / neighbors;
			netSteeringForce += (temp - crows[i].position).norm();
		}
		if (crows[i].alignment)
		{
			//Alignment
			Vector2 temp = averageVelocity / neighbors;
			netSteeringForce += temp;
		}
		if (crows[i].separation)
		{
			//Separation
			Vector2 temp = averageDistance / neighbors * 60;
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
	Vector2 newVelocity = crows[i].velocity.norm();
	float newAngle = atan2(previousVelocity.x * newVelocity.y - previousVelocity.y * newVelocity.x, previousVelocity.dot(newVelocity));
	crows[i].angle = newAngle;
}

size_t EntityManager::Size()
{
	return entities.size();
}
