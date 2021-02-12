#include "TheEventLoopOfLife.h"

TheEventLoopOfLife::TheEventLoopOfLife(IntVector2 dim_in): dim(dim_in),
tile(nullptr)//, rock(nullptr)
{
}

bool TheEventLoopOfLife::OnUserCreate()
{
	tile = new Decal(new Sprite("./Assets/Tile.png"));
	//rock = new Decal(new Sprite("./Assets/Rock.png"));

	grid.Initialize(r);
	entityManager.Initialize(r, grid);

	for (unsigned int i = 0; i < 10; i++)
	{
		uint8_t width = 8;
		Sprite* temp = new Sprite("./Assets/Letters.png", olc::vi2d(width), olc::vi2d((width * 29) + (i * width), 0));
		digits[i] = new Decal(temp);
	}

	frameRate = HUDElement(Word("FRAMERATE", 10, 10), Number(00000, 10 + 8 * 10, 10, 1, 5, digits));
	amountOfTurns = HUDElement(Word("TURNS", 10, 20), Number(00000, 10 + 8 * 10, 20, 1, 5, digits));
	amountOfGrass = HUDElement(Word("GRASS", 10, 100), Number(0000, 10 + 8 * 10, 100, 1, 4, digits));
	amountOfSheep = HUDElement(Word("SHEEP", 10, 110), Number(0000, 10 + 8 * 10, 110, 1, 4, digits));
	amountOfWolves = HUDElement(Word("WOLVES", 10, 120), Number(0000, 10 + 8 * 10, 120, 1, 4, digits));

	return true;
}

bool TheEventLoopOfLife::OnUserUpdate(float fElapsedTime)
{
	float temp = timer;
	timer = (timer + fElapsedTime) * (timer <= timeSpeed);

	if(timer == 0){ amountOfTurns.number.Add(1); }

	int n_amountOfGrass = 0;
	int n_amountOfSheep = 0;
	int n_amountOfWolves = 0;

	for (int i = 0; i < entityManager.Size(); i++)
	{
		if (entityManager.GetEntity(i)->entityType == Entity::EntityType::GRASS) { n_amountOfGrass++; }
		if (entityManager.GetEntity(i)->entityType == Entity::EntityType::SHEEP) { n_amountOfSheep++; }
		if (entityManager.GetEntity(i)->entityType == Entity::EntityType::WOLF) { n_amountOfWolves++; }

		if (timer == 0)
		{
			entityManager.GetEntity(i)->Sense(grid, entityManager);
			entityManager.GetEntity(i)->Decide(r, grid);
		}
		entityManager.GetEntity(i)->Act(r, grid, fElapsedTime, timeSpeed, entityManager);
		if (entityManager.GetEntity(i)->dead)
		{
			grid.tileContent[entityManager.GetEntity(i)->position.x + grid.grid.x * entityManager.GetEntity(i)->position.y][entityManager.GetEntity(i)->spaceOccupying] = nullptr;
			DEL(entityManager.entities[i]);
			entityManager.entities.erase(entityManager.entities.begin() + i);
			i--;
		}
	}

	for (int i = 0; i < entityManager.crows.size(); i++)
	{
		entityManager.UpdateBoid(i);
		entityManager.crows[i].Move(fElapsedTime, grid.grid);
	}
	
	frameRate.number.Reset(); 
	frameRate.number.Add(fElapsedTime * 1000000);
	amountOfGrass.number.Reset(); amountOfGrass.number.Add(n_amountOfGrass);
	amountOfSheep.number.Reset(); amountOfSheep.number.Add(n_amountOfSheep);
	amountOfWolves.number.Reset(); amountOfWolves.number.Add(n_amountOfWolves);

	if (timer == 0) { timer = fElapsedTime - (1 - temp); }

	OnUserDraw();
	return true;
}

void TheEventLoopOfLife::OnUserDraw()
{
	//Refactor this
	vi2d centering = { grid.grid.x * tile->sprite->width / 2, grid.grid.y * tile->sprite->height / 2 };
	for (size_t i = 0; i < grid.grid.x * grid.grid.y; i++)
	{
		int x = i % grid.grid.x; int y = i / grid.grid.y;
		vi2d position = { tile->Width() * x + dim.x / 2 - centering.x, tile->Height() * y + dim.y / 2 - centering.y };
		bool hasGrass = grid.tileContent[i][2] != nullptr;
		
		//Pixel color = hasGrass || grid.tileTraversibility[i] == 2 ? grid.tileColors[i] : Pixel( 40, 30, 20 );
		Pixel color = hasGrass ? grid.tileColors[i] : Pixel(40, 30, 20);

		/*color = grid.tileContent[i][0] != nullptr && grid.tileContent[i][1] != nullptr ? Pixel(255, 0, 255) :
				grid.tileContent[i][0] != nullptr ? Pixel(255, 0, 0) :
				grid.tileContent[i][1] != nullptr ? Pixel(0, 0, 255) :
				color;*/ //Debugging tool to see what tiles are currently occupied

		DrawDecal(position, tile, { 1,1 }, color);
		/*if (grid.tileTraversibility[i] == 2)
		{
			DrawDecal(position, rock, { 1,1 }, color);
		}*/
		if (hasGrass){dynamic_cast<Grass*>(grid.tileContent[i][2])->Render(*this, position, color);}
	}
	for (size_t i = 0; i < entityManager.Size(); i++)
	{
		Vector2 position = { tile->Width() * entityManager.GetEntity(i)->GetPosition().x + dim.x / 2 - centering.x, tile->Height() * entityManager.GetEntity(i)->GetPosition().y + dim.y / 2 - centering.y };
		if (entityManager.GetEntity(i)->entityType != Entity::EntityType::GRASS)
		{
			entityManager.GetEntity(i)->Render(*this, position);
		}
	}
	for (size_t i = 0; i < entityManager.crows.size(); i++)
	{
		Vector2 position = { tile->Width() * entityManager.crows[i].position.x + dim.x / 2 - centering.x, tile->Height() * entityManager.crows[i].position.y + dim.y / 2 - centering.y };
		entityManager.crows[i].Render(*this, entityManager.crow, position);
	}
	frameRate.Render(*this);
	amountOfTurns.Render(*this);
	amountOfGrass.Render(*this);
	amountOfSheep.Render(*this);
	amountOfWolves.Render(*this);
}

void TheEventLoopOfLife::Quit()
{
	DEL(tile);
	//DEL(rock);
	
	for (int i = 0; i < grid.tileContent.size(); i++)
	{
		DEL(grid.tileContent[i][0]);
		DEL(grid.tileContent[i][1]);
		DEL(grid.tileContent[i][2]);
	}
}
