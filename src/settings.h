#pragma once

/* === MACROS USED BY THE WHOLE PROGRAM === */

#define VIEW_WIDTH 1080.0f // View should be a square, otherwise textures will be stretched
#define VIEW_HEIGHT 1080.0f // Height is constant, width depends on the aspect ratio of the window
#define DEFAULT_WINDOW_WIDTH 1280
#define DEFAULT_WINDOW_HEIGHT 720
#define WINDOW_NAME ("SFML Ant Simulation")
#define TEXTURE_PATH(x) ("assets/textures/" x)
#define FONTS_PATH(x) ("assets/fonts/" x)

#define ANT_POPULATION_COUNT Settings::ant_population_count
#define ANT_SIZE 5.0f // pixels
#define ANT_COLOR (sf::Color::Red)
#define ANT_SPEED Settings::ant_speed // Units (=pixels) per second
#define ANT_RANDOM_ROTATION_TIME_MIN 0.2f // seconds
#define ANT_RANDOM_ROTATION_TIME_MAX 1.5f
#define ANT_RANDOM_ROTATION_ANGLE_MAX 90.0f // degrees
#define ANT_PHEROMONE_SPAWN_INTERVAL Settings::ant_pheromone_spawn_interval // seconds
#define ANT_PHEROMONE_DETECTION_RADIUS Settings::ant_pheromone_detection_radius // Radii in world cells. Should be an even number
#define ANT_PHEROMONE_DETECTION_DEADZONE_RADIUS Settings::ant_pheromone_detection_deadzone_radius // Radius where a phereomone is too close to the ant to be detected. Should be an even number too
#define ANT_PHEROMONE_INCREASE_AMOUNT Settings::ant_pheromone_increase_amount
#define ANT_PHEROMONE_TOBASE_DECREASE_AMOUNT Settings::ant_pheromone_tobase_decrease_amount
#define ANT_PHEROMONE_TOFOOD_DECREASE_AMOUNT Settings::ant_pheromone_tofood_decrease_amount
#define ANT_FOOD_DETECTION_RADIUS 6 // cells
#define ANT_FOOD_COLLISION_RADIUS 2 // cells
#define ANT_HOME_DETECTION_RADIUS 100.0f
#define ANT_PHEROMONE_DECREASE_RADIUS 1 // cells
#define ANT_PHEROMONE_FOLLOW_RANDOMNESS Settings::ant_pheromone_follow_randomness
#define ANT_FOOD_CONSUMPTION_AMOUNT Settings::ant_food_consumption_amount // Amount removed to the food of a cell when consuming food
#define ANT_BOREDOM_PERCENTAGE Settings::ant_boredom_percentage
#define ANT_RANDOM_PULL_CENTER_FREEDOM (-0.1f) // 0 is normal, negative = more constrained, positive = less constrained

#define WORLD_GRID_CELL_SIZE 5.0f // width and height in units
#define WORLD_GRID_WIDTH Settings::world_grid_width // number of CELLS the world measures. Affects ram usage. Should be an even number
#define WORLD_GRID_HEIGHT Settings::world_grid_height // same.
#define WORLD_PHEROMONE_TOBASE_COLOR sf::Color::Blue
#define WORLD_PHEROMONE_TOFOOD_COLOR sf::Color::Red
#define WORLD_FOOD_COLOR sf::Color::Green
#define WORLD_FOOD_MAX_AMOUNT Settings::world_food_max_amount // number set on click of the mouse
#define WORLD_FOOD_MOUSE_RADIUS 8 // cells

#define BASE_SIZE 35.0f
#define BASE_COLOR (sf::Color::Yellow)

#define DEFAULT_TIMESCALE 1.0f
#define ZOOM_PER_SCROLL 0.10f // standard zoom is 1.0

#define UI_VALUE_COLOR (sf::Color::Blue)

#define ANT_TICK_THREADS_COUNT Settings::ant_tick_threads_count
/* ========================================= */

#include <string>

namespace Settings
{
	void load();
	extern int ant_population_count;
	extern int ant_tick_threads_count;
	extern float ant_speed;
	extern float ant_pheromone_spawn_interval;
	extern int ant_pheromone_detection_radius;
	extern int ant_pheromone_detection_deadzone_radius;
	extern float ant_pheromone_increase_amount;
	extern float ant_pheromone_tobase_decrease_amount;
	extern float ant_pheromone_tofood_decrease_amount;
	extern float ant_pheromone_follow_randomness;
	extern float ant_food_consumption_amount;
	extern float ant_boredom_percentage;
	extern int world_grid_width;
	extern int world_grid_height;
	extern float world_food_max_amount;
}

