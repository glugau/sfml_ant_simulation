#include "world.h"

#include "application.h"

#include <iostream>
#include <algorithm>

World::World(unsigned int width, unsigned int height)
	: world_width{width}, world_height{height}
{
	world_arr = new PheromoneCell[(unsigned long long)width * (unsigned long long)height]; // casts to remove warnings
	for (int y = minY(); y < maxY(); ++y)
	{
		for (int x = minX(); x <= maxX(); ++x)
		{
			float world_min_x = x * WORLD_GRID_CELL_SIZE;
			float world_min_y = y * WORLD_GRID_CELL_SIZE;
			float world_max_x = world_min_x + WORLD_GRID_CELL_SIZE;
			float world_max_y = world_min_y + WORLD_GRID_CELL_SIZE;

			sf::Vertex vertex1, vertex2, vertex3, vertex4;
			vertex1.position = sf::Vector2f(world_min_x, world_min_y);
			vertex2.position = sf::Vector2f(world_max_x, world_min_y);
			vertex3.position = sf::Vector2f(world_max_x, world_max_y);
			vertex4.position = sf::Vector2f(world_min_x, world_max_y);

			render_vertices.push_back(vertex1);
			render_vertices.push_back(vertex2);
			render_vertices.push_back(vertex3);
			render_vertices.push_back(vertex4);
		}
	}
}

World::~World()
{
	delete[] world_arr;
}

float World::PheromoneCell::getToBase()
{
	return to_base;
}

float World::PheromoneCell::getToFood()
{
	return to_food;
}

float World::PheromoneCell::getFood()
{
	return food;
}

void World::PheromoneCell::increaseToBase(float n)
{
	to_base += n;
}

void World::PheromoneCell::decreaseToBase(float n)
{
	to_base = std::max(0.0f, to_base - n);
}

void World::PheromoneCell::increaseToFood(float n)
{
	to_food += n;
}

void World::PheromoneCell::decreaseToFood(float n)
{
	to_food = std::max(0.0f, to_food - n);
}

void World::PheromoneCell::decreaseFood(float n)
{
	food = std::max(0.0f, food - n);
}

void World::PheromoneCell::setFood(float n)
{
	food = n;
}

World::WorldColumn::WorldColumn(int x, World* world)
	: x{x}, world{world}
{}

World::WorldColumn World::operator[](int x){
	return WorldColumn(x, this);
}

World::PheromoneCell& World::WorldColumn::operator[](int y)
{
	int arr_x = x + world->world_width / 2;
	int arr_y = y + world->world_height / 2;
	return world->world_arr[arr_y * world->world_width + arr_x];
}

int World::minX()
{ 
	return -(int)world_width / 2;
}

int World::maxX()
{
	return world_width / 2;
}

int World::minY()
{
	return -(int)world_height / 2;
}

int World::maxY()
{
	return world_height / 2;
}

static sf::Color getCellColor(float max_to_base, float max_to_food, float max_food, float to_base, float to_food, float food)
{
	sf::Color color;
	color.r = std::min(WORLD_PHEROMONE_TOBASE_COLOR.r * (to_base / max_to_base) + WORLD_PHEROMONE_TOFOOD_COLOR.r * (to_food / max_to_food) + WORLD_FOOD_COLOR.r * (food / max_food), 255.0f);
	color.g = std::min(WORLD_PHEROMONE_TOBASE_COLOR.g * (to_base / max_to_base) + WORLD_PHEROMONE_TOFOOD_COLOR.g * (to_food / max_to_food) + WORLD_FOOD_COLOR.g * (food / max_food), 255.0f);
	color.b = std::min(WORLD_PHEROMONE_TOBASE_COLOR.b * (to_base / max_to_base) + WORLD_PHEROMONE_TOFOOD_COLOR.b * (to_food / max_to_food) + WORLD_FOOD_COLOR.b * (food / max_food), 255.0f);
	color.a = 255;
	return color;
}

void World::draw(sf::RenderWindow* window, bool draw_pheromones)
{
	float max_base = 1.0f;
	float max_food = 1.0f;

	for (int y = minY(); y < maxY(); ++y)
	{
		for (int x = minX(); x <= maxX(); ++x)
		{
			float to_base = (*this)[x][y].getToBase();
			float to_food = (*this)[x][y].getToFood();
			
			max_base = std::max(to_base, max_base);
			max_food = std::max(to_food, max_food);
		}
	}

	long long index = 0;
	for (int y = minY(); y < maxY(); ++y)
	{
		for (int x = minX(); x <= maxX(); ++x)
		{
			sf::Color color;
			if(draw_pheromones)
				 color = getCellColor(max_base, max_food, WORLD_FOOD_MAX_AMOUNT, (*this)[x][y].getToBase(), (*this)[x][y].getToFood(), (*this)[x][y].getFood());
			else
				color = getCellColor(max_base, max_food, WORLD_FOOD_MAX_AMOUNT, 0.0f, 0.0f, (*this)[x][y].getFood());
			render_vertices[index].color = color;
			render_vertices[index + 1ll].color = color;
			render_vertices[index + 2ll].color = color;
			render_vertices[index + 3ll].color = color;

			index += 4;
		}
	}

	window->draw(&render_vertices[0], render_vertices.size(), sf::Quads);
}