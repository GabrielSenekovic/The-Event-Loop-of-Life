#include "TheEventLoopOfLife.h"

TheEventLoopOfLife::TheEventLoopOfLife(IntVector2 dim_in): dim(dim_in),
sheep(nullptr), tile(nullptr), wolf(nullptr), rock(nullptr),
//grass_grid(std::vector<Grass>(grid.x * grid.y)),
tileContent(std::vector<std::array<Entity*, 3>>(grid.x * grid.y))
{
}

bool TheEventLoopOfLife::OnUserCreate()
{
	tile = new Decal(new Sprite("./Assets/Tile.png"));
	rock = new Decal(new Sprite("./Assets/Rock.png"));
	sheep = new Decal(new Sprite("./Assets/Sheep.png"));
	wolf = new Decal(new Sprite("./Assets/Wolf.png"));
	grass = new Decal(new Sprite("./Assets/Grass.png"));
	grass_growing = new Decal(new Sprite("./Assets/Grass_Growing.png"));

	for (size_t i = 0; i < grid.x * grid.y; i++)
	{
		tileTraversibility.push_back(r.myRand() % 2); //0 == no resistance, 1 == -1 movement, 2 == rock
		int probability = r.myRand() % 100;
		tileTraversibility[i] = probability >= 85 ? 2 : tileTraversibility[i];
		Pixel color = tileTraversibility[i] == 2 ? Pixel{ 60, 50, 30 } : Pixel{ 10, 170, 10 };
		tileColors.push_back(GetSimilarColor(color, 50));
	}
	for (size_t i = 0; i < 50; i++)
	{
		Vector2 temp = { -1,-1 };
		while (temp == Vector2(-1, -1) || tileContent[temp.x + grid.x * temp.y][2] != nullptr || tileTraversibility[temp.x + grid.x * temp.y] == 2)
		{
			temp = { (float)(r.myRand() % grid.x) ,(float)(r.myRand() % grid.y) };
		}
		Grass* temp_grass = new Grass(temp, { (r.myRand() % 5) + 2.0f, 6.0f });
		entities.push_back(temp_grass);
		tileContent[temp.x + grid.x * temp.y][2] = temp_grass;
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

	for (size_t i = 0; i < 1; i++)
	{
		Sheep* temp_sheep = new Sheep({ (float)(r.myRand() % grid.x), (float)(r.myRand() % grid.y) }, 20);
		entities.push_back(temp_sheep);
		sheepbitch = temp_sheep;
		if (tileContent[temp_sheep->position.x + grid.x * temp_sheep->position.y][0] == nullptr)
		{
			tileContent[temp_sheep->position.x + grid.x * temp_sheep->position.y][0] = temp_sheep;
			temp_sheep->spaceOccupying = 0;
		}
		else if(tileContent[temp_sheep->position.x + grid.x * temp_sheep->position.y][1] == nullptr)
		{
			tileContent[temp_sheep->position.x + grid.x * temp_sheep->position.y][1] = temp_sheep;
			temp_sheep->spaceOccupying = 1;
		}
		else
		{
			int oops = 0;
		}
	}

	/*Wolf* temp_wolf = new Wolf({ (float)(r.myRand() % grid.x), (float)(r.myRand() % grid.y) }, 20);
	entities.push_back(temp_wolf);

	if (tileContent[temp_wolf->position.x + grid.x * temp_wolf->position.y][0] == nullptr)
	{
		tileContent[temp_wolf->position.x + grid.x * temp_wolf->position.y][0] = temp_wolf;
	}
	else if (tileContent[temp_wolf->position.x + grid.x * temp_wolf->position.y][1] == nullptr)
	{
		tileContent[temp_wolf->position.x + grid.x * temp_wolf->position.y][1] = temp_wolf;
	}
	else
	{
		int oops = 0;
	}*/

	return true;
}

bool TheEventLoopOfLife::OnUserUpdate(float fElapsedTime)
{
	float temp = timer;
	timer = (timer + fElapsedTime) * (timer <= timeSpeed);

	if(timer == 0){ amountOfTurns.number.Add(1); }

	for (int i = 0; i < entities.size(); i++)
	{
		if (timer == 0)
		{
			entities[i]->Sense(tileContent, grid);
			entities[i]->Decide(r);
		}
		entities[i]->Act(r, grid, fElapsedTime, timeSpeed, tileContent, entities);
		if (entities[i]->dead)
		{
			tileContent[entities[i]->position.x + grid.x * entities[i]->position.y][entities[i]->spaceOccupying] = nullptr;
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
	sheep_HP.number.Reset(); sheep_HP.number.Add(sheepbitch->health.x);

	if (timer == 0) { timer = fElapsedTime - (1 - temp); }

	OnUserDraw();
	return true;
}

void TheEventLoopOfLife::OnUserDraw()
{
	vi2d centering = { grid.x * tile->sprite->width / 2, grid.y * tile->sprite->height / 2 };
	for (size_t i = 0; i < grid.x * grid.y; i++)
	{
		int x = i % grid.x; int y = i / grid.y;
		vi2d position = { tile->Width() * x + dim.x / 2 - centering.x, tile->Height() * y + dim.y / 2 - centering.y };
		bool hasGrass = tileContent[i][2] != nullptr;
		Pixel color = hasGrass || tileTraversibility[i] == 2 ? tileColors[i] : Pixel( 40, 30, 20 );

		color = tileContent[i][0] != nullptr && tileContent[i][1] != nullptr ? Pixel(255, 0, 255) :
				tileContent[i][0] != nullptr ? Pixel(255, 0, 0) :
				tileContent[i][1] != nullptr ? Pixel(0, 0, 255) :
				color;

		if (sheepbitch->targets.size() > 0)
		{
			for (int j = 0; j < sheepbitch->targets.size(); j++)
			{
				if (Vector2(x, y) == sheepbitch->targets[j].position)
				{
					color = Pixel(255, 255, 0);
				}
			}
		}

		DrawDecal(position, tile, { 1,1 }, color);
		if (tileTraversibility[i] == 2)
		{
			DrawDecal(position, rock, { 1,1 }, color);
		}
		if (hasGrass)
		{
			switch (dynamic_cast<Grass*>(tileContent[i][2])->maturity)
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
		switch (entities[i]->entityType)
		{
			case Entity::EntityType::GRASS:
			{
				continue;
			}break;
			case Entity::EntityType::SHEEP:
			{
				DrawDecal(position, sheep, { 1,1 }, olc::WHITE);
			}break;
			case Entity::EntityType::WOLF:
			{
				DrawDecal(position, wolf, { 1,1 }, olc::WHITE);
			}
		}
	}
	frameRate.Render(*this);
	amountOfTurns.Render(*this);
	sheep_x.Render(*this);
	sheep_y.Render(*this);
	sheep_target_x.Render(*this);
	sheep_target_y.Render(*this);
	sheep_HP.Render(*this);
}

Pixel TheEventLoopOfLife::GetSimilarColor(Pixel color, int modulus)
{
	int biggestValue = std::max({ color.r, color.g, color.b });
	int redMod = biggestValue == color.r ? -((int)r.myRand() % modulus) : r.myRand() % modulus;
	int greenMod = biggestValue == color.g ? -((int)r.myRand() % modulus) : r.myRand() % modulus;
	int blueMod = biggestValue == color.b ? -((int)r.myRand() % modulus) : r.myRand() % modulus;
	Pixel value =
	{
		(uint8_t)(color.r + redMod),
		(uint8_t)(color.g + greenMod),
		(uint8_t)(color.b + blueMod),
		color.a
	};
	return value;
}

void TheEventLoopOfLife::Quit()
{
	DEL(tile);
	DEL(rock);
	DEL(sheep);
	DEL(wolf);
	DEL(grass);
	DEL(grass_growing);
	for (int i = 0; i < tileContent.size(); i++)
	{
		DEL(tileContent[i][0]);
		DEL(tileContent[i][1]);
		DEL(tileContent[i][2]);
	}
}
