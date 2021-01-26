#include "TheEventLoopOfLife.h"

TheEventLoopOfLife::TheEventLoopOfLife(IntVector2 dim_in): dim(dim_in),
sheep(nullptr), tile(nullptr), wolf(nullptr), rock(nullptr)
{
}

bool TheEventLoopOfLife::OnUserCreate()
{
	tile = new Decal(new Sprite("./Assets/Tile.png"));
	rock = new Decal(new Sprite("./Assets/Rock.png"));
	sheep = new Decal(new Sprite("./Assets/Sheep.png"));
	sheep_eating = new Decal(new Sprite("./Assets/Sheep_Eating.png"));
	wolf = new Decal(new Sprite("./Assets/Wolf.png"));
	grass = new Decal(new Sprite("./Assets/Grass.png"));
	grass_growing = new Decal(new Sprite("./Assets/Grass_Growing.png"));
	wander = new Decal(new Sprite("./Assets/Spotted.png"));
	pursue = new Decal(new Sprite("./Assets/Discovered.png"));
	breed = new Decal(new Sprite("./Assets/Breed.png"));

	grid.Initialize(r);

	for (size_t i = 0; i < 50; i++)
	{
		Vector2 temp = { -1,-1 };
		while (temp == Vector2(-1, -1) || grid.tileContent[temp.x + grid.grid.x * temp.y][2] != nullptr || grid.tileTraversibility[temp.x + grid.grid.x * temp.y] == 2)
		{
			temp = { (float)(r.myRand() % grid.grid.x) ,(float)(r.myRand() % grid.grid.y) };
		}
		Grass* temp_grass = new Grass(temp, { (r.myRand() % 5) + 2.0f, 6.0f }, {grass, grass_growing});
		entities.push_back(temp_grass);
		grid.tileContent[temp.x + grid.grid.x * temp.y][2] = temp_grass;
	}
	for (unsigned int i = 0; i < 10; i++)
	{
		uint8_t width = 8;
		Sprite* temp = new Sprite("./Assets/Letters.png", olc::vi2d(width), olc::vi2d((width * 29) + (i * width), 0));
		digits[i] = new Decal(temp);
	}

	frameRate = HUDElement(Word("FRAMERATE", 10, 10), Number(00000, 10 + 8 * 10, 10, 1, 5, digits));
	amountOfTurns = HUDElement(Word("TURNS", 10, 20), Number(00000, 10 + 8 * 10, 20, 1, 5, digits));
	sheep_x = HUDElement(Word("X", 10, 30), Number(00000, 10 + 8 * 10, 30, 1, 5, digits));
	sheep_y = HUDElement(Word("Y", 10 + 8 * 20, 30), Number(00000, 10 + 8 * 30, 30, 1, 5, digits));
	sheep_target_x = HUDElement(Word("TARGETX", 10, 40), Number(00000, 10 + 8 * 10, 40, 1, 5, digits));
	sheep_target_y = HUDElement(Word("TARGETY", 10 + 8 * 20, 40), Number(00000, 10 + 8 * 30, 40, 1, 5, digits));
	sheep_HP = HUDElement(Word("HP", 10, 50), Number(00000, 10 + 8 * 10, 50, 1, 5, digits));
	amountOfGrass = HUDElement(Word("GRASS", 10, 100), Number(0000, 10 + 8 * 10, 100, 1, 4, digits));
	amountOfSheep = HUDElement(Word("SHEEP", 10, 110), Number(0000, 10 + 8 * 10, 110, 1, 4, digits));
	amountOfWolves = HUDElement(Word("WOLVES", 10, 120), Number(0000, 10 + 8 * 10, 120, 1, 4, digits));

	for (size_t i = 0; i < 1; i++)
	{
		Sheep* temp_sheep = new Sheep({ (float)(r.myRand() % grid.grid.x), (float)(r.myRand() % grid.grid.y) }, 20, {sheep, sheep_eating, wander, pursue, breed});
		entities.push_back(temp_sheep);
		sheepbitch = temp_sheep;
		grid.PlaceEntityOnGrid(temp_sheep);
	}

	Wolf* temp_wolf = new Wolf({ (float)(r.myRand() % grid.grid.x), (float)(r.myRand() % grid.grid.y) }, 20, {wolf, wander, pursue, breed});
	entities.push_back(temp_wolf);
	grid.PlaceEntityOnGrid(temp_wolf);

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

	for (int i = 0; i < entities.size(); i++)
	{
		if (entities[i]->entityType == Entity::EntityType::GRASS) { n_amountOfGrass++; }
		if (entities[i]->entityType == Entity::EntityType::SHEEP) { n_amountOfSheep++; }
		if (entities[i]->entityType == Entity::EntityType::WOLF) { n_amountOfWolves++; }

		if (timer == 0)
		{
			entities[i]->Sense(grid.tileContent, grid.grid);
			entities[i]->Decide(r);
		}
		entities[i]->Act(r, grid.grid, fElapsedTime, timeSpeed, grid.tileContent, entities);
		if (entities[i]->dead)
		{
			grid.tileContent[entities[i]->position.x + grid.grid.x * entities[i]->position.y][entities[i]->spaceOccupying] = nullptr;
			entities.erase(entities.begin() + i);
			i--;
		}
	}
	
	frameRate.number.Reset(); 
	frameRate.number.Add(fElapsedTime * 1000000);
	sheep_x.number.Reset(); sheep_x.number.Add(sheepbitch->position.x);
	sheep_y.number.Reset(); sheep_y.number.Add(sheepbitch->position.y);
	sheep_target_x.number.Reset(); sheep_target_x.number.Add(sheepbitch->targetPosition.x);
	sheep_target_y.number.Reset(); sheep_target_y.number.Add(sheepbitch->targetPosition.y);
	//sheep_HP.number.Reset(); sheep_HP.number.Add(sheepbitch->health.x);
	amountOfGrass.number.Reset(); amountOfGrass.number.Add(n_amountOfGrass);
	amountOfSheep.number.Reset(); amountOfSheep.number.Add(n_amountOfSheep);
	amountOfWolves.number.Reset(); amountOfWolves.number.Add(n_amountOfWolves);

	if (timer == 0) { timer = fElapsedTime - (1 - temp); }

	OnUserDraw();
	return true;
}

void TheEventLoopOfLife::OnUserDraw()
{
	vi2d centering = { grid.grid.x * tile->sprite->width / 2, grid.grid.y * tile->sprite->height / 2 };
	for (size_t i = 0; i < grid.grid.x * grid.grid.y; i++)
	{
		int x = i % grid.grid.x; int y = i / grid.grid.y;
		vi2d position = { tile->Width() * x + dim.x / 2 - centering.x, tile->Height() * y + dim.y / 2 - centering.y };
		bool hasGrass = grid.tileContent[i][2] != nullptr;
		Pixel color = hasGrass || grid.tileTraversibility[i] == 2 ? grid.tileColors[i] : Pixel( 40, 30, 20 );

		color = grid.tileContent[i][0] != nullptr && grid.tileContent[i][1] != nullptr ? Pixel(255, 0, 255) :
				grid.tileContent[i][0] != nullptr ? Pixel(255, 0, 0) :
				grid.tileContent[i][1] != nullptr ? Pixel(0, 0, 255) :
				color;

		/*if (sheepbitch->targets.size() > 0)
		{
			for (int j = 0; j < sheepbitch->targets.size(); j++)
			{
				if (Vector2(x, y) == sheepbitch->targets[j].position)
				{
					color = Pixel(255, 255, 0);
				}
			}
		}*/

		DrawDecal(position, tile, { 1,1 }, color);
		if (grid.tileTraversibility[i] == 2)
		{
			DrawDecal(position, rock, { 1,1 }, color);
		}
		if (hasGrass)
		{
			switch (dynamic_cast<Grass*>(grid.tileContent[i][2])->maturity)
			{
				case Grass::Maturity::GROWING:	DrawDecal(position, grass_growing, { 1,1 }, color); break;
				case Grass::Maturity::MATURE: DrawDecal(position, grass, { 1,1 }, color); break;
				case Grass::Maturity::WITHERING: DrawDecal(position, grass, { 1,1 }, Pixel(158, 124, 66)); break;
			}
		}
	}
	for (size_t i = 0; i < entities.size(); i++)
	{
		Vector2 position = { tile->Width() * entities[i]->walkingPosition.x + dim.x / 2 - centering.x, tile->Height() * entities[i]->walkingPosition.y + dim.y / 2 - centering.y };
		if (entities[i]->entityType != Entity::EntityType::GRASS)
		{
			entities[i]->Render(*this, position);
		}
	}
	frameRate.Render(*this);
	amountOfTurns.Render(*this);
	sheep_x.Render(*this);
	sheep_y.Render(*this);
	sheep_target_x.Render(*this);
	sheep_target_y.Render(*this);
	sheep_HP.Render(*this);
	amountOfGrass.Render(*this);
	amountOfSheep.Render(*this);
	amountOfWolves.Render(*this);
}

void TheEventLoopOfLife::Quit()
{
	DEL(tile);
	DEL(rock);
	DEL(sheep);
	DEL(wolf);
	DEL(grass);
	DEL(grass_growing);
	for (int i = 0; i < grid.tileContent.size(); i++)
	{
		DEL(grid.tileContent[i][0]);
		DEL(grid.tileContent[i][1]);
		DEL(grid.tileContent[i][2]);
	}
}
