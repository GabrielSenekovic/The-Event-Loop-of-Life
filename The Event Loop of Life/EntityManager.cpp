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

size_t EntityManager::Size()
{
	return entities.size();
}
