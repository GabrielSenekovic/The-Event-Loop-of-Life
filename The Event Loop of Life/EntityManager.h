#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H

#include <vector>
#include "Entity.h"

class EntityManager
{
public:
	EntityManager();
	std::vector<Entity*> entities;
	void Add(Entity* entity);
	Entity* GetEntity(const uint32_t& i);
	Entity* GetEntityFromHandle(const uint32_t& i);
	uint32_t nextHandle;
	size_t Size();
};

#endif ENTITYMANAGER_H