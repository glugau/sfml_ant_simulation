#pragma once


#include <stdexcept>
#include <thread>
#include <algorithm>

#include "SFML/Graphics.hpp"
#include "ant.h"
#include "world.h"
#include "settings.h"

class Application
{
public:
	Application();
	~Application();
	void run();

private:
	void init();
	void mainLoop(float dt, float realtime_dt);
	void handleEvents();
	void resizeWindow();
	void drawObjects();
	void terminate();
	void startTickingThreads();
	void threaded_tickAnts(int part_index);
	void spawnFood();
	void zoomView(const sf::Event& ev);
	void moveView(const sf::Event& ev);
	void drawUI();
	void changeTimescale();

	bool draw_pheromones = true;
	bool block_clicks = false;
	sf::Text slow_down_text;
	sf::Text speed_up_text;
	World* world;
	sf::RenderWindow* window;
	sf::View* view;
	sf::View* UIview;
	sf::Vector2f view_size; // Before aspect ratio of window applied (dimensions should always be 1:1, see constants)
	float time_scale = 1.0f;
	std::vector<std::thread> ant_tick_threads;
	std::vector<sf::Vector2i> food_spawn_offsets;
	Ant* ant_population;
	sf::CircleShape base_shape;
	sf::Font font;
};

