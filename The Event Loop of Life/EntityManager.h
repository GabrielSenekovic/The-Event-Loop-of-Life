#ifndef ENTITYMANAGER_H
#define ENTITYMANAGER_H

#include <vector>
#include "Entity.h"

//The EntityManager was made because when an entity was deleted because it was dead, the target pointer in the wolf class would contain garbage valuyes
//Therefore, I had to make a class that contains a function that can return an entity based on a handle value

class EntityManager
{
public:
	EntityManager();
	std::vector<Entity*> entities;
	std::vector<Crow> crows;
	void Add(Entity* entity);
	Entity* GetEntity(const uint32_t& i);
	Entity* GetEntityFromHandle(const uint32_t& i) const;
	void UpdateBoid(const int& i);
	uint32_t nextHandle;
	size_t Size();
};

#endif ENTITYMANAGER_H