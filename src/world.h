#pragma once

#include "SFML/Graphics.hpp"

#include <mutex>

class World
{
public:
	struct PheromoneCell
	{
	public:
		float getToBase();
		float getToFood();
		float getFood();
		void increaseToBase(float n);
		void decreaseToBase(float n);
		void increaseToFood(float n);
		void decreaseToFood(float n);
		void decreaseFood(float n);
		void setFood(float n);

		std::mutex mutex;
	private:
		float to_base = 0.0f;
		float to_food = 0.0f;
		float food = 0.0f;
	};
	struct WorldColumn
	{
		int x;
		World* world;
		WorldColumn(int x, World* world);
		PheromoneCell& operator[](int y);
	};
	World(unsigned int width, unsigned int height);
	~World();
	WorldColumn operator[](int x);
	int minX(); // Values are inclusive.
	int maxX();
	int minY();
	int maxY();
	void draw(sf::RenderWindow* window, bool draw_pheromones);
	void drawBase(sf::RenderWindow* window);
private:
	std::vector<sf::Vertex> render_vertices;
	std::vector<sf::Vertex> base_vertices;
	PheromoneCell* world_arr;
	unsigned int world_width;
	unsigned int world_height;
};
